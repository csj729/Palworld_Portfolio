// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSystem.h"

APlayerSystem::APlayerSystem()
{
    CurrentHP = BaseStats.MaxHP;
    CurrentStamina = BaseStats.MaxStamina;
}

void APlayerSystem::AddXP(int32 Amount)
{
    CurrentXP += Amount;
    UE_LOG(LogTemp, Log, TEXT("Gained XP: %d / %d"), CurrentXP, NextLevelXP);

    // while 루프를 사용해 여러 번 레벨업 할 수 있도록 처리
    while (CurrentXP >= NextLevelXP)
    {
        CurrentXP -= NextLevelXP; // 다음 레벨 경험치를 빼고 남은 경험치는 유지
        LevelUp(); // LevelUp 함수에서는 더 이상 CurrentXP = 0 코드가 필요 없음
    }
}

void APlayerSystem::LevelUp()
{
    Level++;
    CurrentXP = 0;

    // 경험치 곡선 적용: BaseXP * Level^Exponent
    NextLevelXP = 100 * FMath::Pow(Level, 2.1f);

    StatPoints += 1;
    TechPoints += 4;

    UE_LOG(LogTemp, Log, TEXT("LEVEL UP! Now Level %d"), Level);
    UE_LOG(LogTemp, Log, TEXT("StatPoints: %d | TechPoints: %d"), StatPoints, TechPoints);
}

void APlayerSystem::UpgradeStat(FName StatName)
{
    if (StatPoints <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No StatPoints left!"));
        return;
    }

    if (StatName == "HP") BaseStats.MaxHP += 100.f;
    else if (StatName == "Attack") BaseStats.Attack += 10.f;
    else if (StatName == "Defense") BaseStats.Defense += 10.f;
    else if (StatName == "WorkSpeed") BaseStats.WorkSpeed += 0.1f;
    else if (StatName == "Weight") BaseStats.WeightCapacity += 50.f;
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Stat Name!"));
        return;
    }

    StatPoints--;
    UE_LOG(LogTemp, Log, TEXT("Upgraded %s! Remaining StatPoints: %d"), *StatName.ToString(), StatPoints);
}

bool APlayerSystem::UnlockTech(const FTechData& Tech)
{
    if (Level < Tech.RequiredLevel)
    {
        UE_LOG(LogTemp, Warning, TEXT("Level too low for Tech %s"), *Tech.TechID.ToString());
        return false;
    }

    if (TechPoints < Tech.TechPointCost)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough TechPoints for %s"), *Tech.TechID.ToString());
        return false;
    }

    TechPoints -= Tech.TechPointCost;
    UnlockedTechs.Add(Tech.TechID);

    UE_LOG(LogTemp, Log, TEXT("Unlocked Tech: %s"), *Tech.TechID.ToString());
    return true;
}

APal* APlayerSystem::SummonPal(int32 Index)
{
    if (bIsSummoned) return nullptr; // 이미 소환되어 있으면 중지
    if (!PalInven || !PalInven->StoredPals.IsValidIndex(Index)) return nullptr;

    APal* PalToSummon = PalInven->StoredPals[Index];
    if (!PalToSummon) return nullptr;

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;

    FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 200.f;
    FRotator SpawnRotation = GetActorRotation();

    APal* SpawnedPal = World->SpawnActor<APal>(PalToSummon->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
    if (SpawnedPal)
    {
        SpawnedPal->OwnerCharacter = this;
        Palindex = Index;
        PalInven->StoredPals[Index] = SpawnedPal; // 인벤토리 참조 업데이트
        bIsSummoned = true;
    }

    return SpawnedPal;
}

void APlayerSystem::UnsummonPal()
{
    if (!bIsSummoned || !PalInven) return;

    if (!PalInven->StoredPals.IsValidIndex(Palindex)) return;

    APal* SpawnedPal = PalInven->StoredPals[Palindex];
    if (SpawnedPal && SpawnedPal->GetWorld() == GetWorld())
    {
        SpawnedPal->Destroy();
    }

    bIsSummoned = false;
}