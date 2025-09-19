// Fill out your copyright notice in the Description page of Project Settings.


#include "Pal.h"
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
    State = EPalState::Idle;

    BaseMaxHP = 100.0f;
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
    UE_LOG(LogTemp, Warning, TEXT("APal::BeginPlay Called"));
    AController* Ctrl = GetController();
    if (Ctrl)
    {
        UE_LOG(LogTemp, Log, TEXT("%s possessed by %s"), *GetName(), *Ctrl->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s has NO Controller!"), *GetName());
    }

}

// Called every frame
void APal::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

float APal::GetMaxHP() const
{
    return FinalMaxHP;
}

float APal::GetAttack() const
{
    return FinalAttack;
}

float APal::GetDefense() const
{
    return FinalDefense;
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