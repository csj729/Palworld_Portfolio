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

    // while ������ ����� ���� �� ������ �� �� �ֵ��� ó��
    while (CurrentXP >= NextLevelXP)
    {
        CurrentXP -= NextLevelXP; // ���� ���� ����ġ�� ���� ���� ����ġ�� ����
        LevelUp(); // LevelUp �Լ������� �� �̻� CurrentXP = 0 �ڵ尡 �ʿ� ����
    }
}

void APlayerSystem::LevelUp()
{
    Level++;
    CurrentXP = 0;

    // ����ġ � ����: BaseXP * Level^Exponent
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
        return nullptr; // �̹� ��ȯ ���̸� ����
    }
    if (!PalInven || !PalInven->StoredPals.IsValidIndex(Index))
        return nullptr;

    APal* PalToSummon = PalInven->StoredPals[Index];
    if (!PalToSummon) return nullptr;

    // ��ȯ �α�
    UE_LOG(LogTemp, Warning, TEXT("Summoned Pal at Index %d : %s"), Index, *PalToSummon->GetName());

    // ��ġ/ȸ�� ����
    FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 200.f;
    FRotator SpawnRotation = GetActorRotation();
    PalToSummon->SetActorLocation(SpawnLocation);
    PalToSummon->SetActorRotation(SpawnRotation);

    // ���� ���� + Ȱ��ȭ
    PalToSummon->SetActorHiddenInGame(false);
    PalToSummon->SetActorEnableCollision(true);
    PalToSummon->SetActorTickEnabled(true);

    // ������ ����
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
        // �α�
        UE_LOG(LogTemp, Warning, TEXT("Unsummoned Pal at Index %d : %s"), Palindex, *PalToHide->GetName());

        // ��Ȱ��ȭ + ���� ó��
        PalToHide->SetActorHiddenInGame(true);
        PalToHide->SetActorEnableCollision(false);
        PalToHide->SetActorTickEnabled(false);

        // �����ϰ� "���� ��ġ"�� �̵����ѵ�
        static const FVector StorageLocation(0.f, 0.f, -10000.f);
        PalToHide->SetActorLocation(StorageLocation);
    }

    // ���� �ʱ�ȭ
    bIsSummoned = false;
}