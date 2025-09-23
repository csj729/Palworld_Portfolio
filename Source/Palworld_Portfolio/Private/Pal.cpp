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
    Exponent = 2.1f;

    // Pal�� �κ��丮 ũ�� 1
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

    // SpawnParameters ����
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // Child Spawn
    APal* Child = World->SpawnActor<APal>(ParentA->GetClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    if (!Child) return nullptr;

    // --- �ɷ�ġ ���� ---
    Child->Level = 1;
    const float MinAttack = 10.0f, MaxAttack = 200.0f;
    const float MinDefense = 10.0f, MaxDefense = 200.0f;
    const float StatMutation = 10.0f;

    float AttackPotential = FMath::Lerp(ParentA->BaseAttack, ParentB->BaseAttack, FMath::FRand());
    Child->BaseAttack = FMath::Clamp(AttackPotential + FMath::RandRange(-StatMutation, StatMutation), MinAttack, MaxAttack);

    float DefensePotential = FMath::Lerp(ParentA->BaseDefense, ParentB->BaseDefense, FMath::FRand());
    Child->BaseDefense = FMath::Clamp(DefensePotential + FMath::RandRange(-StatMutation, StatMutation), MinDefense, MaxDefense);

    // --- Traits ���� ---
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

    // --- StrengthTier �� ���� ��� ---
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

    // --- AI Controller�� �������� �����ϰ� ���� ---
    if (ParentA->PalAIControllerClass)
    {
        // 1. AIController�� ���� ����
        AAIController* NewAIController = World->SpawnActor<AAIController>(
            ParentA->PalAIControllerClass,
            SpawnLocation,
            FRotator::ZeroRotator,
            SpawnParams
        );

        // 2. ������ ��Ʈ�ѷ��� Child ���Ϳ� �����ϵ��� ���
        if (NewAIController)
        {
            NewAIController->Possess(Child);
        }
    }
    // ���� ����
    Child->UpdateColor();

    // �α� ���
    UE_LOG(LogTemp, Warning, TEXT("New Pal Born! Strength Tier: %.2f / Trait Count: %d"),
        StrengthTier, Child->Traits.Num());

    return Child;
}

/// <summary>
///  MAT_PalBody �̸� �̿��ؼ� ���� ���͸��� �����Ϸ��� ������ �ȵ�. �׷��� �ϴ� 0�� ���͸����� ���� ���͸���� �����ؼ� �����ؾߵ�. �����ʼ�
/// </summary>
void APal::UpdateColor()
{
    USkeletalMeshComponent* SkeletalMesh = GetMesh();
    if (!SkeletalMesh) return;
    // MAT_PalBody�� ���� ���͸���� ����
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
    // �θ� Ŭ����(ACharacter)�� TakeDamage �Լ��� ���� ȣ���Ͽ� �⺻���� ������ ó��(��: ���� ���)�� �����մϴ�.
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    AController* MyController = GetController();
    // �̹� ���� ���¶�� �������� ���� �ʰ� 0�� ��ȯ�մϴ�.
    if (CurrentState == EPalState::Dead)
    {
        return 0.f;
    }

    // ���� ���� �������� 0���� ũ�ٸ�
    if (ActualDamage > 0.f)
    {
        // ���� ü�¿��� �������� �����մϴ�.
        CurrentHP -= ActualDamage;
        // �α׸� ����Ͽ� ���� ü���� Ȯ���մϴ�. (��: Pal_0's HP: 80.0)
        UE_LOG(LogTemp, Warning, TEXT("%s's HP: %f"), *GetName(), CurrentHP);

        // ü���� 0 ���Ϸ� �������ٸ�
        if(CurrentHP <= 0)
        {
            USkeletalMeshComponent* MeshComp = GetMesh();
            if (MeshComp)
            {
                MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));  // ������ �´� �浹 ä�� ����
                MeshComp->SetSimulatePhysics(true);                  // ���� �ùķ��̼� �ѱ�
                MeshComp->WakeAllRigidBodies();                      // ���� �����
                MeshComp->bBlendPhysics = true;                      // �ִϸ��̼� �� ���� ��ȯ ���
                CurrentState = EPalState::Dead;
            }

            // ��Ʈ�ѷ� �Է� ���� (�÷��̾��� ���)
            if (MyController)
            {
                MyController->UnPossess();
            }
        }

        else
        {   
            // ���� �ʾҰ�, ���� ���°� '���' ���¿��ٸ�
            if (CurrentState == EPalState::Idle)
            {
                // ���¸� '����(Battle)'�� �����մϴ�.
                CurrentState = EPalState::Battle;
                UE_LOG(LogTemp, Warning, TEXT("%s enters Battle State!"), *GetName());
            }
        }
    }
    // ���������� ����� ������ ���� ��ȯ�մϴ�.
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

    // �⺻ ����ġ
    float HPIncrease = 10.0f;
    float AttackIncrease = 2.0f;
    float DefenseIncrease = 1.0f;

    // 1�ܰ�: �⺻ ���� ����ġ ����
    BaseMaxHP += HPIncrease;
    BaseAttack += AttackIncrease;
    BaseDefense += DefenseIncrease;

    // 2�ܰ�: Trait ���� ���� ���
    RecalculateStats();

    // 3�ܰ�: ���� HP�� �� �ִ� HP�� ����
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

    // HP�� �ʰ����� �ʰ� ����
    CurrentHP = FMath::Min(CurrentHP, FinalMaxHP);
}