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
    if (bIsSummoned)
    {
        UE_LOG(LogTemp, Warning, TEXT("SummonPal blocked: already summoned"));
        return nullptr; // 이미 소환 중이면 중지
    }
    if (!PalInven || !PalInven->StoredPals.IsValidIndex(Index))
        return nullptr;

    APal* PalToSummon = PalInven->StoredPals[Index];
    if (!PalToSummon) return nullptr;

    // 소환 로그
    UE_LOG(LogTemp, Warning, TEXT("Summoned Pal at Index %d : %s"), Index, *PalToSummon->GetName());

    // 위치/회전 갱신
    FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 200.f;
    FRotator SpawnRotation = GetActorRotation();
    PalToSummon->SetActorLocation(SpawnLocation);
    PalToSummon->SetActorRotation(SpawnRotation);

    // 숨김 해제 + 활성화
    PalToSummon->SetActorHiddenInGame(false);
    PalToSummon->SetActorEnableCollision(true);
    PalToSummon->SetActorTickEnabled(true);

    // 소유자 갱신
    PalToSummon->OwnerCharacter = this;

    Palindex = Index;
    bIsSummoned = true;

    return PalToSummon;
}

void APlayerSystem::UnsummonPal(APal* SummonedPal)
{
    if (!bIsSummoned || !PalInven || !PalInven->StoredPals.IsValidIndex(Palindex))
        return;

    APal* PalToHide = SummonedPal;
    if (PalToHide)
    {
        // 로그
        UE_LOG(LogTemp, Warning, TEXT("Unsummoned Pal at Index %d : %s"), Palindex, *PalToHide->GetName());

        // 비활성화 + 숨김 처리
        PalToHide->SetActorHiddenInGame(true);
        PalToHide->SetActorEnableCollision(false);
        PalToHide->SetActorTickEnabled(false);

        // 안전하게 "저장 위치"로 이동시켜둠
        static const FVector StorageLocation(0.f, 0.f, -10000.f);
        PalToHide->SetActorLocation(StorageLocation);
    }

    // 상태 초기화
    bIsSummoned = false;
}