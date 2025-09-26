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
	// �̵� ȸ�� ����
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	PrimaryActorTick.bCanEverTick = true;
	// Inventory ����
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

	// Pal�� �κ��丮 ũ�� 1
	Inven->Capacity = 1;

	// bIsBred ������ �⺻���� false�� �����մϴ�.
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

	// �ڿ� ������ ��쿡�� ����
	if (!bIsBred)
	{
		// ======NEW! �ڿ� ���� Pal �ʱ�ȭ ����======

		// 1. ���� ���� ����
		FLinearColor HSV;
		HSV.R = FMath::FRandRange(0.0f, 360.0f); // H (����)
		HSV.G = 1.0f;                            // S (ä��)
		HSV.B = 1.0f;                            // V (��)
		this->BodyColor = FLinearColor::MakeFromHSV8(HSV.R, HSV.G * 255, HSV.B * 255);

		// 2. ���� ���� ���� (1 ~ 5)  // <<-- �� �κ��� �߰��Ǿ����ϴ�!
		Level = FMath::RandRange(1, 5);

		// 3. ��ü��(IV) ���� ���� (0 ~ 100)
		HP_IV = FMath::FRandRange(0.0f, 100.0f);
		Attack_IV = FMath::FRandRange(0.0f, 100.0f);
		Defense_IV = FMath::FRandRange(0.0f, 100.0f);

		// 4. Ư�� ���� �ο� (0 ~ 4��)
		Traits.Empty(); // Ȥ�� �𸣴� �ʱ�ȭ
		const TMap<EPalTraitType, FPalTrait>& TraitMapRef = GetTraitMap();
		if (TraitMapRef.Num() > 0)
		{
			TArray<EPalTraitType> AllTraitTypes;
			TraitMapRef.GenerateKeyArray(AllTraitTypes);
			Algo::RandomShuffle(AllTraitTypes); // ��� Ư���� �������� ����

			int32 NumTraitsToAssign = FMath::RandRange(0, 4);
			for (int32 i = 0; i < NumTraitsToAssign && i < AllTraitTypes.Num(); ++i)
			{
				Traits.Add(AllTraitTypes[i]); // ���� ��Ͽ��� ������� N�� �߰�
			}
		}
	}

	RecalculateStats();
	CurrentHP = FinalMaxHP;

	// �������Ʈ�� ������ OnUpdateColor �̺�Ʈ�� ȣ���Ͽ� ���� ���� ��ȣ�� ����
	OnUpdateColor();
}

// Called every frame
void APal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const TMap<EPalTraitType, FPalTrait>& APal::GetTraitMap()
{
	// ������ �Ķ���� ���� (�� 17��)
	// Atk, Def, Move, Stamina, Work, Hunger, Sanity, Cooldown,
	// PlayerAtk, PlayerDef, PlayerLog, PlayerMine, PlayerWork,
	// Lifesteal, NightWork, Egg,
	// Score
	static TMap<EPalTraitType, FPalTrait> TraitMap = {
		// ���� (����)
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

		// ���� (�̵�)
		{ EPalTraitType::Swift, FPalTrait(1.0f, 1.0f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::Runner, FPalTrait(1.0f, 1.0f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::Nimble, FPalTrait(1.0f, 1.0f, 1.3f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 3.0f) },
		{ EPalTraitType::HealthyConstitution, FPalTrait(1.0f, 1.0f, 1.0f, 1.25f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 1.0f) },
		{ EPalTraitType::AbundantEnergy, FPalTrait(1.0f, 1.0f, 1.0f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::PerpetualMotion, FPalTrait(1.0f, 1.0f, 1.0f, 1.75f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 3.0f) },

		// ���� (����)
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

		// ���� (�����)
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

		// �÷��̾� ����
		{ EPalTraitType::AssaultCommander, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) },
		{ EPalTraitType::BulwarkStrategist, FPalTrait(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.1f, 1.0f, 1.0f, 1.0f, false, false, false, 2.0f) }
	};

	// Name�� �������ִ� ������ �߰��� �� �ֽ��ϴ�.
	// ����: for (auto& Elem : TraitMap) { Elem.Value.Name = UEnum::GetValueAsString(Elem.Key); }

	return TraitMap;
}


// Pal.cpp ���Ͽ��� Breed �Լ��� �� �������� ��ü�ϼ���.

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

	// �ڽ��� �θ� A�� ���� Ŭ����(����)�� �����˴ϴ�.
	APal* Child = World->SpawnActor<APal>(ParentA->GetClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (!Child) return nullptr;

	Child->bIsBred = true;
	Child->Level = 1;

	// �ڡڡڡڡ� 1. �⺻ ���� ���� �ڡڡڡڡ�
	// �� �̻� �θ� ������ ���ų� ���� ���̸� ���� �ʽ��ϴ�.
	// �ڽ��� �θ� ������ �⺻ ������ �״�� �����޽��ϴ�.
	Child->BaseMaxHP = ParentA->BaseMaxHP;
	Child->BaseAttack = ParentA->BaseAttack;
	Child->BaseDefense = ParentA->BaseDefense;

	// �� ���ȿ� ���� 30% Ȯ���� �θ� A, 30% Ȯ���� �θ� B, 40% Ȯ���� ���� ��ü���� �ο��޽��ϴ�.

	// HP ��ü�� ����
	float HPRoll = FMath::FRand();
	if (HPRoll < 0.3f) Child->HP_IV = ParentA->HP_IV;
	else if (HPRoll < 0.6f) Child->HP_IV = ParentB->HP_IV;
	else Child->HP_IV = FMath::FRandRange(0.0f, 100.0f);

	// Attack ��ü�� ����
	float AttackRoll = FMath::FRand();
	if (AttackRoll < 0.3f) Child->Attack_IV = ParentA->Attack_IV;
	else if (AttackRoll < 0.6f) Child->Attack_IV = ParentB->Attack_IV;
	else Child->Attack_IV = FMath::FRandRange(0.0f, 100.0f);

	// Defense ��ü�� ����
	float DefenseRoll = FMath::FRand();
	if (DefenseRoll < 0.3f) Child->Defense_IV = ParentA->Defense_IV;
	else if (DefenseRoll < 0.6f) Child->Defense_IV = ParentB->Defense_IV;
	else Child->Defense_IV = FMath::FRandRange(0.0f, 100.0f);

	// --- Traits ���� (�� ������ �״�� ����) ---
	// Ư���� ������ �θ𿡰� �����ǰų� �������̷� �߻��մϴ�.
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

	// '��ü�� ��� ����'�� 'Ư�� ��� ����'�� ���� �ջ��Ͽ� ���� ������ ����մϴ�.

	// 4-1. ��ü�� ���� ��� (0 ~ 100)
	float AverageIV_Score = (Child->HP_IV + Child->Attack_IV + Child->Defense_IV) / 3.0f;

	// 4-2. Ư�� ��� ���� ��� (TraitScore �հ�)
	float TotalTrait_Score = 0.0f;
	const TMap<EPalTraitType, FPalTrait>& TraitMap = GetTraitMap();
	for (const EPalTraitType& TraitType : Child->Traits)
	{
		if (const FPalTrait* TraitData = TraitMap.Find(TraitType))
		{
			TotalTrait_Score += TraitData->TraitScore; // ��� Ư�� ������ ����
		}
	}

	// 4-3. ���� ���� ���
	// ���� IV�� ���� Pal�� Ư������ ��� ���ʽ��� �� ũ�� ����������, ������ �ٷ� ���մϴ�.
	float FinalScore = AverageIV_Score + TotalTrait_Score;

	// 4-4. ���� ������ 0.0 ~ 1.0 ������ ������� ��ȯ
	// ������ ���� ������ �����մϴ�. (��: �־� IV 0 + �־� Ư�� -10 = -10�� / �ֻ� IV 100 + �ֻ� Ư�� 20 = 120��)
	const float MinFinalScore = -10.0f;
	const float MaxFinalScore = 120.0f;
	float StrengthTier = FMath::GetMappedRangeValueClamped(
		FVector2D(MinFinalScore, MaxFinalScore), // �Է°� ���� (���� ����)
		FVector2D(0.0f, 1.0f),                   // ��°� ���� (0~1 ���)
		FinalScore                               // ���� ���� ����
	);

	// �ڡڡڡڡ� 5. ���� ��� ���� (HSV �������� ȥ��) �ڡڡڡڡ�
	float MixRatio = FMath::FRand();

	// 1. �θ� A�� B�� ������ ���� HSV�� ��ȯ�մϴ�.
	FLinearColor ParentA_HSV = ParentA->BodyColor.LinearRGBToHSV();
	FLinearColor ParentB_HSV = ParentB->BodyColor.LinearRGBToHSV();

	// 2. �ڽ��� HSV ������ ���� ����ϴ�.
	FLinearColor Child_HSV;

	// 3. ����(H)�� �θ�κ��� Lerp(���� ����)�Ͽ� �����޽��ϴ�.
	//    ParentA_HSV.R�� Hue ���� ������ �ֽ��ϴ�.
	Child_HSV.R = FMath::Lerp(ParentA_HSV.R, ParentB_HSV.R, MixRatio);

	// 4. ä��(S)�� ��(V)�� 1.0���� �����Ͽ� ���� ��� ¸�� ������ ����ϴ�.
	Child_HSV.G = 1.0f; // Saturation
	Child_HSV.B = 1.0f; // Value

	// 5. ���� �ڽ��� HSV ������ �ٽ� RGB�� ��ȯ�մϴ�.
	FLinearColor SaturatedColor = FLinearColor::MakeFromHSV8(Child_HSV.R, Child_HSV.G * 255, Child_HSV.B * 255);

	// 6. ���� ���(StrengthTier)�� ���� ����� �����ϴ�.
	//    �� �κ��� ���� ������ �����մϴ�.
	float ColorBlendAlpha = FMath::Pow(StrengthTier, 3.0f);
	FLinearColor FinalColor = FMath::Lerp(SaturatedColor, FLinearColor::White, ColorBlendAlpha);
	Child->BodyColor = FinalColor;

	// --- AI Controller �� ���� ó�� ---
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
	// ... �ٸ� ���� �����鵵 �߰� ...

	const TMap<EPalTraitType, FPalTrait>& TraitMap = GetTraitMap();

	for (const EPalTraitType& TraitType : Traits)
	{
		if (const FPalTrait* TraitData = TraitMap.Find(TraitType))
		{
			HPScale_Total *= TraitData->HPScale;
			AttackScale_Total *= TraitData->AttackScale;
			DefenseScale_Total *= TraitData->DefenseScale;
			// ... �ٸ� ���ȵ鵵 ���⿡ ���� ���� ...
		}
	}

	// (�⺻ ���� + ��ü��) * Ư�� ����
	FinalMaxHP = (BaseMaxHP + HP_IV) * HPScale_Total;
	FinalAttack = (BaseAttack + Attack_IV) * AttackScale_Total;
	FinalDefense = (BaseDefense + Defense_IV) * DefenseScale_Total;
	// ... �ٸ� ���� ���ȵ鵵 ��� ...

	CurrentHP = FMath::Min(CurrentHP, FinalMaxHP);
}

