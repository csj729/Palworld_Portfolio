// Fill out your copyright notice in the Description page of Project Settings.

#include "Pal.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Algo/RandomShuffle.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

APal::APal()
{
	// 이동 회전 관련
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	PrimaryActorTick.bCanEverTick = true;
	// Inventory 생성
	Inven = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	PalName = TEXT("DefaultPal");
	Level = 1;
	CurrentXP = 0.0f;
	CurrentState = EPalState::Idle;

	BaseMaxHP = 500.0f;
	BaseAttack = 10.0f;
	BaseDefense = 5.0f;
	CurrentHP = BaseMaxHP;

	BaseXP = 100.0f;
	BaseXP = 100.0f;
	Exponent = 2.1f;

	// Pal은 인벤토리 크기 1
	Inven->Capacity = 1;

	// bIsBred 변수의 기본값을 false로 설정합니다.
	bIsBred = false;
}

// Called when the game starts or when spawned
void APal::BeginPlay()
{
	Super::BeginPlay();

	AController* Ctrl = GetController();
	if (Ctrl)
	{
		UE_LOG(LogTemp, Log, TEXT("%s possessed by %s"), *GetName(), *Ctrl->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s has NO Controller!"), *GetName());
	}

	// 자연 스폰일 경우에만 실행
	if (!bIsBred)
	{
		// ======NEW! 자연 스폰 Pal 초기화 로직======

		// 1. 랜덤 색상 생성
		FLinearColor HSV;
		HSV.R = FMath::FRandRange(0.0f, 360.0f); // H (색조)
		HSV.G = 1.0f;                            // S (채도)
		HSV.B = 1.0f;                            // V (명도)
		this->BodyColor = FLinearColor::MakeFromHSV8(HSV.R, HSV.G * 255, HSV.B * 255);

		// 2. 레벨 랜덤 설정 (1 ~ 5)  // <<-- 이 부분이 추가되었습니다!
		Level = FMath::RandRange(1, 5);

		// 3. 개체값(IV) 랜덤 설정 (0 ~ 100)
		HP_IV = FMath::FRandRange(0.0f, 100.0f);
		Attack_IV = FMath::FRandRange(0.0f, 100.0f);
		Defense_IV = FMath::FRandRange(0.0f, 100.0f);

		// 4. 특성 랜덤 부여 (0 ~ 4개)
		Traits.Empty(); // 혹시 모르니 초기화
		const TMap<EPalTraitType, FPalTrait>& TraitMapRef = GetTraitMap();
		if (TraitMapRef.Num() > 0)
		{
			TArray<EPalTraitType> AllTraitTypes;
			TraitMapRef.GenerateKeyArray(AllTraitTypes);
			Algo::RandomShuffle(AllTraitTypes); // 모든 특성을 무작위로 섞음

			int32 NumTraitsToAssign = FMath::RandRange(0, 4);
			for (int32 i = 0; i < NumTraitsToAssign && i < AllTraitTypes.Num(); ++i)
			{
				Traits.Add(AllTraitTypes[i]); // 섞인 목록에서 순서대로 N개 추가
			}
		}
	}

	RecalculateStats();
	CurrentHP = FinalMaxHP;

	// 블루프린트에 구현된 OnUpdateColor 이벤트를 호출하여 색상 적용 신호를 보냄
	OnUpdateColor();
}

// Called every frame
void APal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const TMap<EPalTraitType, FPalTrait>& APal::GetTraitMap()
{
	// 생성자 파라미터 순서 (총 17개)
	// Atk, Def, Move, Stamina, Work, Hunger, Sanity, Cooldown,
	// PlayerAtk, PlayerDef, PlayerLog, PlayerMine, PlayerWork,
	// Lifesteal, NightWork, Egg,
	// Score
	static TMap<EPalTraitType, FPalTrait> TraitMap = {
		// 긍정 (전투)
		{ EPalTraitType::Brave, FPalTrait(1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::Ferocious, FPalTrait(1.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::HardSkin, FPalTrait(1.0f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::ToughBody, FPalTrait(1.0f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::DiamondBody, FPalTrait(1.0f, 1.3f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 3.0f) },
		{ EPalTraitType::Impetuous, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.15f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::LevelHeaded, FPalTrait(1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 4.0f) },
		{ EPalTraitType::Demon, FPalTrait(1.3f, 1.05f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 5.0f) },
		{ EPalTraitType::Legendary, FPalTrait(1.2f, 1.2f, 1.15f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 6.0f) },
		{ EPalTraitType::Vampire, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, true, true, false, 3.0f) },

		// 긍정 (이동)
		{ EPalTraitType::Swift, FPalTrait(1.0f, 1.0f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::Runner, FPalTrait(1.0f, 1.0f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::Nimble, FPalTrait(1.0f, 1.0f, 1.3f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 3.0f) },
		{ EPalTraitType::HealthyConstitution, FPalTrait(1.0f, 1.0f, 1.0f, 1.25f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::AbundantEnergy, FPalTrait(1.0f, 1.0f, 1.0f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::PerpetualMotion, FPalTrait(1.0f, 1.0f, 1.0f, 1.75f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 3.0f) },

		// 긍정 (거점)
		{ EPalTraitType::Serious, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::Artisan, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::MasterTechnician, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.75f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 3.0f) },
		{ EPalTraitType::WorkSlave, FPalTrait(0.7f, 1.0f, 1.0f, 1.0f, 1.3f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 0.0f) },
		{ EPalTraitType::DietExpert, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::Abstinence, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.85f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::PositiveThinker, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::Workaholic, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.85f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::UnwaveringHeart, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 3.0f) },
		{ EPalTraitType::LoggingForeman, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.25f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::MiningForeman, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.25f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::Motivation, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.25f, false, false, false, 1.0f) },
		{ EPalTraitType::Philanthropist, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, true, 1.0f) },
		{ EPalTraitType::Nocturnal, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, true, false, 1.0f) },
		{ EPalTraitType::Rare, FPalTrait(1.15f, 1.0f, 1.0f, 1.0f, 1.15f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },

		// 부정 (디버프)
		{ EPalTraitType::Slacker, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -3.0f) },
		{ EPalTraitType::WeakGrip, FPalTrait(1.0f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -1.0f) },
		{ EPalTraitType::BrittleBones, FPalTrait(1.0f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -2.0f) },
		{ EPalTraitType::Coward, FPalTrait(0.9f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -1.0f) },
		{ EPalTraitType::Pacifist, FPalTrait(0.8f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -2.0f) },
		{ EPalTraitType::Bully, FPalTrait(1.1f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 0.0f) },
		{ EPalTraitType::Sadist, FPalTrait(1.15f, 0.85f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 0.0f) },
		{ EPalTraitType::Masochist, FPalTrait(0.85f, 1.15f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 0.0f) },
		{ EPalTraitType::Hooligan, FPalTrait(1.15f, 1.0f, 1.0f, 1.0f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -1.0f) },
		{ EPalTraitType::Musclehead, FPalTrait(1.3f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -2.0f) },
		{ EPalTraitType::Clumsy, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -1.0f) },
		{ EPalTraitType::Conceited, FPalTrait(1.0f, 0.9f, 1.0f, 1.0f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 0.0f) },
		{ EPalTraitType::Glutton, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -1.0f) },
		{ EPalTraitType::BlackHoleStomach, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.15f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -2.0f) },
		{ EPalTraitType::Unstable, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -1.0f) },
		{ EPalTraitType::Destructive, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.15f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, -2.0f) },

		// 플레이어 버프
		{ EPalTraitType::AssaultCommander, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::BulwarkStrategist, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.1f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) }
	};

	// Name을 설정해주는 로직을 추가할 수 있습니다.
	// 예시: for (auto& Elem : TraitMap) { Elem.Value.Name = UEnum::GetValueAsString(Elem.Key); }

	return TraitMap;
}


// Pal.cpp 파일에서 Breed 함수만 이 내용으로 교체하세요.

APal* APal::Breed(UObject* WorldContextObject, APal* ParentA, APal* ParentB, FVector SpawnLocation)
{
	if (!WorldContextObject || !ParentA || !ParentB)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters for Breed"));
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 자식은 부모 A와 같은 클래스(종족)로 스폰됩니다.
	APal* Child = World->SpawnActor<APal>(ParentA->GetClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (!Child) return nullptr;

	Child->bIsBred = true;
	Child->Level = 1;

	// ★★★★★ 1. 기본 스탯 고정 ★★★★★
	// 더 이상 부모 스탯을 섞거나 랜덤 변이를 주지 않습니다.
	// 자식은 부모 종족의 기본 스탯을 그대로 물려받습니다.
	Child->BaseMaxHP = ParentA->BaseMaxHP;
	Child->BaseAttack = ParentA->BaseAttack;
	Child->BaseDefense = ParentA->BaseDefense;

	// 각 스탯에 대해 30% 확률로 부모 A, 30% 확률로 부모 B, 40% 확률로 랜덤 개체값을 부여받습니다.

	// HP 개체값 유전
	float HPRoll = FMath::FRand();
	if (HPRoll < 0.3f) Child->HP_IV = ParentA->HP_IV;
	else if (HPRoll < 0.6f) Child->HP_IV = ParentB->HP_IV;
	else Child->HP_IV = FMath::FRandRange(0.0f, 100.0f);

	// Attack 개체값 유전
	float AttackRoll = FMath::FRand();
	if (AttackRoll < 0.3f) Child->Attack_IV = ParentA->Attack_IV;
	else if (AttackRoll < 0.6f) Child->Attack_IV = ParentB->Attack_IV;
	else Child->Attack_IV = FMath::FRandRange(0.0f, 100.0f);

	// Defense 개체값 유전
	float DefenseRoll = FMath::FRand();
	if (DefenseRoll < 0.3f) Child->Defense_IV = ParentA->Defense_IV;
	else if (DefenseRoll < 0.6f) Child->Defense_IV = ParentB->Defense_IV;
	else Child->Defense_IV = FMath::FRandRange(0.0f, 100.0f);

	// --- Traits 설정 (이 로직은 그대로 유지) ---
	// 특성은 여전히 부모에게 유전되거나 돌연변이로 발생합니다.
	Child->Traits.Empty();
	const float InheritanceChance = 0.6f;
	const float MutationChance = 0.2f;

	if (ParentA->Traits.Num() > 0 && FMath::FRand() < InheritanceChance)
		Child->Traits.AddUnique(ParentA->Traits[FMath::RandRange(0, ParentA->Traits.Num() - 1)]);

	if (ParentB->Traits.Num() > 0 && FMath::FRand() < InheritanceChance)
		Child->Traits.AddUnique(ParentB->Traits[FMath::RandRange(0, ParentB->Traits.Num() - 1)]);

	if (FMath::FRand() < MutationChance)
	{
		TArray<EPalTraitType> AllTraitTypes;
		GetTraitMap().GenerateKeyArray(AllTraitTypes);
		if (AllTraitTypes.Num() > 0)
		{
			Child->Traits.AddUnique(AllTraitTypes[FMath::RandRange(0, AllTraitTypes.Num() - 1)]);
		}
	}

	// '개체값 평균 점수'와 '특성 등급 점수'를 먼저 합산하여 최종 총점을 계산합니다.

	// 4-1. 개체값 점수 계산 (0 ~ 100)
	float AverageIV_Score = (Child->HP_IV + Child->Attack_IV + Child->Defense_IV) / 3.0f;

	// 4-2. 특성 등급 점수 계산 (TraitScore 합계)
	float TotalTrait_Score = 0.0f;
	const TMap<EPalTraitType, FPalTrait>& TraitMap = GetTraitMap();
	for (const EPalTraitType& TraitType : Child->Traits)
	{
		if (const FPalTrait* TraitData = TraitMap.Find(TraitType))
		{
			TotalTrait_Score += TraitData->TraitScore; // 모든 특성 점수를 더함
		}
	}

	// 4-3. 최종 총점 계산
	// 낮은 IV를 가진 Pal이 특성으로 얻는 보너스가 더 크게 느껴지도록, 점수를 바로 더합니다.
	float FinalScore = AverageIV_Score + TotalTrait_Score;

	// 4-4. 최종 총점을 0.0 ~ 1.0 사이의 등급으로 변환
	// 총점의 예상 범위를 설정합니다. (예: 최악 IV 0 + 최악 특성 -10 = -10점 / 최상 IV 100 + 최상 특성 20 = 120점)
	const float MinFinalScore = -10.0f;
	const float MaxFinalScore = 120.0f;
	float StrengthTier = FMath::GetMappedRangeValueClamped(
		FVector2D(MinFinalScore, MaxFinalScore), // 입력값 범위 (최종 총점)
		FVector2D(0.0f, 1.0f),                   // 출력값 범위 (0~1 등급)
		FinalScore                               // 실제 최종 총점
	);

	// ★★★★★ 5. 색상 계산 로직 (HSV 공간에서 혼합) ★★★★★
	float MixRatio = FMath::FRand();

	// 1. 부모 A와 B의 색상을 각각 HSV로 변환합니다.
	FLinearColor ParentA_HSV = ParentA->BodyColor.LinearRGBToHSV();
	FLinearColor ParentB_HSV = ParentB->BodyColor.LinearRGBToHSV();

	// 2. 자식의 HSV 색상을 새로 만듭니다.
	FLinearColor Child_HSV;

	// 3. 색조(H)는 부모로부터 Lerp(선형 보간)하여 물려받습니다.
	//    ParentA_HSV.R이 Hue 값을 가지고 있습니다.
	Child_HSV.R = FMath::Lerp(ParentA_HSV.R, ParentB_HSV.R, MixRatio);

	// 4. 채도(S)와 명도(V)는 1.0으로 고정하여 가장 밝고 쨍한 색으로 만듭니다.
	Child_HSV.G = 1.0f; // Saturation
	Child_HSV.B = 1.0f; // Value

	// 5. 계산된 자식의 HSV 색상을 다시 RGB로 변환합니다.
	FLinearColor SaturatedColor = FLinearColor::MakeFromHSV8(Child_HSV.R, Child_HSV.G * 255, Child_HSV.B * 255);

	// 6. 최종 등급(StrengthTier)에 따라 흰색을 섞습니다.
	//    이 부분은 기존 로직과 동일합니다.
	float ColorBlendAlpha = FMath::Pow(StrengthTier, 3.0f);
	FLinearColor FinalColor = FMath::Lerp(SaturatedColor, FLinearColor::White, ColorBlendAlpha);
	Child->BodyColor = FinalColor;

	// --- AI Controller 및 최종 처리 ---
	if (ParentA->PalAIControllerClass)
	{
		AAIController* NewAIController = World->SpawnActor<AAIController>(
			ParentA->PalAIControllerClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		if (NewAIController)
		{
			NewAIController->Possess(Child);
		}
	}

	Child->OnUpdateColor();

	UE_LOG(LogTemp, Warning, TEXT("New Pal Born! Trait-Based Tier: %.2f / Trait Count: %d"),
		StrengthTier, Child->Traits.Num());

	return Child;
}

float APal::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	AController* MyController = GetController();
	if (CurrentState == EPalState::Dead)
	{
		return 0.f;
	}

	if (ActualDamage > 0.f)
	{
		CurrentHP -= ActualDamage;
		UE_LOG(LogTemp, Warning, TEXT("%s's HP: %f"), *GetName(), CurrentHP);

		if (CurrentHP <= 0)
		{
			USkeletalMeshComponent* MeshComp = GetMesh();
			if (MeshComp)
			{
				MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
				MeshComp->SetSimulatePhysics(true);
				MeshComp->WakeAllRigidBodies();
				MeshComp->bBlendPhysics = true;
				CurrentState = EPalState::Dead;
			}

			if (MyController)
			{
				MyController->UnPossess();
			}
		}
		else
		{
			if (CurrentState == EPalState::Idle)
			{
				CurrentState = EPalState::Battle;
				UE_LOG(LogTemp, Warning, TEXT("%s enters Battle State!"), *GetName());
			}
		}
	}
	return ActualDamage;
}

float APal::GetXPToNextLevel() const
{
	return BaseXP * FMath::Pow(Level, Exponent);
}

void APal::GainExperience(float Amount)
{
	CurrentXP += Amount;
	CheckLevelUp();
}

void APal::CheckLevelUp()
{
	while (CurrentXP >= GetXPToNextLevel())
	{
		CurrentXP -= GetXPToNextLevel();
		LevelUp();
	}
}

void APal::LevelUp()
{
	Level++;

	float HPIncrease = 1.0f;
	float AttackIncrease = 2.0f;
	float DefenseIncrease = 1.0f;

	BaseMaxHP += HPIncrease;
	BaseAttack += AttackIncrease;
	BaseDefense += DefenseIncrease;

	RecalculateStats();

	CurrentHP = FinalMaxHP;

	UE_LOG(LogTemp, Log, TEXT("%s has leveled up! Level: %d"), *PalName, Level);
	UE_LOG(LogTemp, Log, TEXT("HP: %.1f | ATK: %.1f | DEF: %.1f"),
		FinalMaxHP, FinalAttack, FinalDefense);
}

void APal::RecalculateStats()
{
	float HPScale_Total = 1.0f;
	float AttackScale_Total = 1.0f;
	float DefenseScale_Total = 1.0f;
	// ... 다른 스탯 변수들도 추가 ...

	const TMap<EPalTraitType, FPalTrait>& TraitMap = GetTraitMap();

	for (const EPalTraitType& TraitType : Traits)
	{
		if (const FPalTrait* TraitData = TraitMap.Find(TraitType))
		{
			HPScale_Total *= TraitData->HPScale;
			AttackScale_Total *= TraitData->AttackScale;
			DefenseScale_Total *= TraitData->DefenseScale;
			// ... 다른 스탯들도 여기에 곱셈 누적 ...
		}
	}

	// (기본 스탯 + 개체값) * 특성 배율
	FinalMaxHP = (BaseMaxHP + HP_IV) * HPScale_Total;
	FinalAttack = (BaseAttack + Attack_IV) * AttackScale_Total;
	FinalDefense = (BaseDefense + Defense_IV) * DefenseScale_Total;
	// ... 다른 최종 스탯들도 계산 ...

	CurrentHP = FMath::Min(CurrentHP, FinalMaxHP);
}

