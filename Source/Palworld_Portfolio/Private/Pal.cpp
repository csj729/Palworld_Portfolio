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
    Exponent = 2.1f;

    // Pal은 인벤토리 크기 1
    Inven->Capacity = 1;
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

    RecalculateStats();
    UpdateColor();
    CurrentHP = FinalMaxHP;
}

// Called every frame
void APal::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

const TMap<EPalTraitType, FPalTrait>& APal::GetTraitMap()
{
    static TMap<EPalTraitType, FPalTrait> TraitMap = {
        { EPalTraitType::Brave,   FPalTrait(TEXT("Brave"),   1.0f, 1.3f, 1.0f, 1.2f) },
        { EPalTraitType::Sturdy,  FPalTrait(TEXT("Sturdy"),  1.2f, 0.9f, 1.3f, 1.15f) },
        { EPalTraitType::Swift,   FPalTrait(TEXT("Swift"),   0.9f, 1.2f, 0.8f, 1.1f) },
        { EPalTraitType::Artisan, FPalTrait(TEXT("Artisan"), 1.1f, 1.1f, 1.1f, 1.25f) },
        { EPalTraitType::Coward,  FPalTrait(TEXT("Coward"),  0.9f, 0.7f, 0.8f, 0.75f) },
        { EPalTraitType::Frail,   FPalTrait(TEXT("Frail"),   0.7f, 1.0f, 0.7f, 0.7f) },
        { EPalTraitType::Lazy,    FPalTrait(TEXT("Lazy"),    0.9f, 0.85f, 0.9f, 0.8f) },
        { EPalTraitType::Glutton, FPalTrait(TEXT("Glutton"), 1.3f, 0.8f, 0.8f, 0.85f) },
        { EPalTraitType::Moody,   FPalTrait(TEXT("Moody"),   1.1f, 0.7f, 1.2f, 0.95f) },
        { EPalTraitType::Normal,  FPalTrait(TEXT("Normal"),  1.0f, 1.0f, 1.0f, 1.0f) }
    };
    return TraitMap;
}

APal* APal::Breed(UObject* WorldContextObject, APal* ParentA, APal* ParentB, FVector SpawnLocation)
{
    if (!WorldContextObject || !ParentA || !ParentB)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid parameters for Breed"));
        return nullptr;
    }

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World) return nullptr;

    // SpawnParameters 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // Child Spawn
    APal* Child = World->SpawnActor<APal>(ParentA->GetClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    if (!Child) return nullptr;

    // --- 능력치 설정 ---
    Child->Level = 1;
    const float MinAttack = 10.0f, MaxAttack = 200.0f;
    const float MinDefense = 10.0f, MaxDefense = 200.0f;
    const float StatMutation = 10.0f;

    float AttackPotential = FMath::Lerp(ParentA->BaseAttack, ParentB->BaseAttack, FMath::FRand());
    Child->BaseAttack = FMath::Clamp(AttackPotential + FMath::RandRange(-StatMutation, StatMutation), MinAttack, MaxAttack);

    float DefensePotential = FMath::Lerp(ParentA->BaseDefense, ParentB->BaseDefense, FMath::FRand());
    Child->BaseDefense = FMath::Clamp(DefensePotential + FMath::RandRange(-StatMutation, StatMutation), MinDefense, MaxDefense);

    // --- Traits 설정 ---
    Child->Traits.Empty();
    const float InheritanceChance = 0.6f;
    const float MutationChance = 0.2f;

    if (ParentA->Traits.Num() > 0 && FMath::FRand() < InheritanceChance)
        Child->Traits.AddUnique(ParentA->Traits[FMath::RandRange(0, ParentA->Traits.Num() - 1)]);

    if (ParentB->Traits.Num() > 0 && FMath::FRand() < InheritanceChance)
        Child->Traits.AddUnique(ParentB->Traits[FMath::RandRange(0, ParentB->Traits.Num() - 1)]);

    TArray<FPalTrait> AllTraits;
    GetTraitMap().GenerateValueArray(AllTraits);
    if (AllTraits.Num() > 0 && FMath::FRand() < MutationChance)
        Child->Traits.AddUnique(AllTraits[FMath::RandRange(0, AllTraits.Num() - 1)]);

    // --- StrengthTier 및 색상 계산 ---
    float AttackTier = (Child->BaseAttack - MinAttack) / (MaxAttack - MinAttack);
    float DefenseTier = (Child->BaseDefense - MinDefense) / (MaxDefense - MinDefense);
    float BaseStatTier = (AttackTier + DefenseTier) / 2.0f;

    float TotalTraitMultiplier = 1.0f;
    for (const FPalTrait& trait : Child->Traits)
        TotalTraitMultiplier *= trait.TraitScore;

    float StrengthTier = FMath::Clamp(BaseStatTier * TotalTraitMultiplier, 0.0f, 1.0f);

    float MixRatio = FMath::FRand();
    FLinearColor BaseColor = FMath::Lerp(ParentA->BodyColor, ParentB->BodyColor, MixRatio);
    Child->BodyColor = FMath::Lerp(BaseColor, FLinearColor::White, StrengthTier);

    // --- AI Controller를 수동으로 생성하고 빙의 ---
    if (ParentA->PalAIControllerClass)
    {
        // 1. AIController를 직접 생성
        AAIController* NewAIController = World->SpawnActor<AAIController>(
            ParentA->PalAIControllerClass,
            SpawnLocation,
            FRotator::ZeroRotator,
            SpawnParams
        );

        // 2. 생성된 컨트롤러가 Child 액터에 빙의하도록 명령
        if (NewAIController)
        {
            NewAIController->Possess(Child);
        }
    }
    // 색상 적용
    Child->UpdateColor();

    // 로그 출력
    UE_LOG(LogTemp, Warning, TEXT("New Pal Born! Strength Tier: %.2f / Trait Count: %d"),
        StrengthTier, Child->Traits.Num());

    return Child;
}

/// <summary>
///  MAT_PalBody 이름 이용해서 동적 메터리얼 생성하려고 했으나 안됨. 그래서 일단 0번 메터리얼을 동적 메터리얼로 지정해서 변경해야됨. 수정필수
/// </summary>
void APal::UpdateColor()
{
    USkeletalMeshComponent* SkeletalMesh = GetMesh();
    if (!SkeletalMesh) return;
    // MAT_PalBody를 동적 메터리얼로 지정
    if (!SkeletalMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetMesh error"));
        return;
    }

    int32 MaterialIndex = SkeletalMesh->GetMaterialIndex(FName("MAT_PalBody"));
    if (MaterialIndex == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("MAT_PalBody error"));
        return;
    }

    UMaterialInterface* CurrentMaterial = SkeletalMesh->GetMaterial(MaterialIndex);
    if (!CurrentMaterial) return;

    UMaterialInstanceDynamic* DynMaterial = Cast<UMaterialInstanceDynamic>(CurrentMaterial);

    if (!DynMaterial)
    {
        DynMaterial = SkeletalMesh->CreateAndSetMaterialInstanceDynamic(MaterialIndex);
    }

    if (DynMaterial)
    {
        DynMaterial->SetVectorParameterValue(FName("BodyColor"), BodyColor);
    }
}

float APal::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 부모 클래스(ACharacter)의 TakeDamage 함수를 먼저 호출하여 기본적인 데미지 처리(예: 방어력 계산)를 수행합니다.
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    AController* MyController = GetController();
    // 이미 죽은 상태라면 데미지를 받지 않고 0을 반환합니다.
    if (CurrentState == EPalState::Dead)
    {
        return 0.f;
    }

    // 실제 들어온 데미지가 0보다 크다면
    if (ActualDamage > 0.f)
    {
        // 현재 체력에서 데미지를 차감합니다.
        CurrentHP -= ActualDamage;
        // 로그를 출력하여 현재 체력을 확인합니다. (예: Pal_0's HP: 80.0)
        UE_LOG(LogTemp, Warning, TEXT("%s's HP: %f"), *GetName(), CurrentHP);

        // 체력이 0 이하로 떨어졌다면
        if(CurrentHP <= 0)
        {
            USkeletalMeshComponent* MeshComp = GetMesh();
            if (MeshComp)
            {
                MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));  // 물리에 맞는 충돌 채널 적용
                MeshComp->SetSimulatePhysics(true);                  // 물리 시뮬레이션 켜기
                MeshComp->WakeAllRigidBodies();                      // 물리 깨우기
                MeshComp->bBlendPhysics = true;                      // 애니메이션 → 물리 전환 허용
                CurrentState = EPalState::Dead;
            }

            // 컨트롤러 입력 차단 (플레이어인 경우)
            if (MyController)
            {
                MyController->UnPossess();
            }
        }

        else
        {   
            // 죽지 않았고, 현재 상태가 '대기' 상태였다면
            if (CurrentState == EPalState::Idle)
            {
                // 상태를 '전투(Battle)'로 변경합니다.
                CurrentState = EPalState::Battle;
                UE_LOG(LogTemp, Warning, TEXT("%s enters Battle State!"), *GetName());
            }
        }
    }
    // 최종적으로 적용된 데미지 양을 반환합니다.
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

    // 기본 증가치
    float HPIncrease = 10.0f;
    float AttackIncrease = 2.0f;
    float DefenseIncrease = 1.0f;

    // 1단계: 기본 스탯 증가치 적용
    BaseMaxHP += HPIncrease;
    BaseAttack += AttackIncrease;
    BaseDefense += DefenseIncrease;

    // 2단계: Trait 포함 최종 계산
    RecalculateStats();

    // 3단계: 현재 HP를 새 최대 HP로 갱신
    CurrentHP = FinalMaxHP;

    UE_LOG(LogTemp, Log, TEXT("%s has leveled up! Level: %d"), *PalName, Level);
    UE_LOG(LogTemp, Log, TEXT("HP: %.1f | ATK: %.1f | DEF: %.1f"),
        FinalMaxHP, FinalAttack, FinalDefense);
}

void APal::RecalculateStats()
{
    float HPScale = 1.0f;
    float AttackScale = 1.0f;
    float DefenseScale = 1.0f;

    for (const FPalTrait& Trait : Traits)
    {
        HPScale *= Trait.HPScale;
        AttackScale *= Trait.AttackScale;
        DefenseScale *= Trait.DefenseScale;
    }

    FinalMaxHP = BaseMaxHP * HPScale;
    FinalAttack = BaseAttack * AttackScale;
    FinalDefense = BaseDefense * DefenseScale;

    // HP가 초과되지 않게 보정
    CurrentHP = FMath::Min(CurrentHP, FinalMaxHP);
}