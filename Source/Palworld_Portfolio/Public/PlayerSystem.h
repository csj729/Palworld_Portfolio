// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Palworld_PortfolioCharacter.h"
#include "Building.h"
#include "PlayerSystem.generated.h"

// 기본 스탯 구조체
USTRUCT(BlueprintType)
struct FCharacterStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP = 500;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Attack = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Defense = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxStamina = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float WorkSpeed = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float WeightCapacity = 500;
};

// 기술 도면 구조체 (나중에 DataAsset으로 확장 가능)
USTRUCT(BlueprintType)
struct FTechData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tech")
    FName TechID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tech")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tech")
    int32 RequiredLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tech")
    int32 TechPointCost = 1;
};

UCLASS()
class PALWORLD_PORTFOLIO_API APlayerSystem : public APalworld_PortfolioCharacter
{
	GENERATED_BODY()
	
public:
    APlayerSystem();
    // ================= 기본 진행도(레벨, 기술도면) =================
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int32 CurrentXP = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int32 NextLevelXP = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int32 StatPoints = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int32 TechPoints = 0;

    // ================= 기본 스탯 =================
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FCharacterStats BaseStats;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CurrentHP;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CurrentStamina;
    // ================= 해금된 기술 목록 =================
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tech")
    TArray<FName> UnlockedTechs;

    // ================= Getter / Setter =================
    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetMaxHP() const { return BaseStats.MaxHP; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float SetMaxHP(float NewHP)
    {
        BaseStats.MaxHP = NewHP;
        return BaseStats.MaxHP;
    }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetAttack() const { return BaseStats.Attack; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float SetAttack(float NewAttack) { return BaseStats.Attack = NewAttack; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetDefense() const { return BaseStats.Defense; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float SetDefense(float NewDefense) { return BaseStats.Defense = NewDefense; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetMaxStamina() const { return BaseStats.MaxStamina; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float SetMaxStamina(float NewStamina) { return BaseStats.MaxStamina = NewStamina; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetWorkSpeed() const { return BaseStats.WorkSpeed; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float SetWorkSpeed(float NewSpeed) { return BaseStats.WorkSpeed = NewSpeed; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetWeightCapacity() const { return BaseStats.WeightCapacity; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float SetWeightCapacity(float NewCapacity) { return BaseStats.WeightCapacity = NewCapacity; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetCurrentHP() const { return CurrentHP; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float SetCurrentHP(float NewHP) { return CurrentHP = NewHP; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetCurrentStamina() const { return CurrentStamina; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float SetCurrentStamina(float NewStamina) { return CurrentStamina = NewStamina; }

    // ================= 함수 =================
    UFUNCTION(BlueprintCallable, Category = "Progression")
    void AddXP(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void LevelUp();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void UpgradeStat(FName StatName);

    UFUNCTION(BlueprintCallable, Category = "Tech")
    bool UnlockTech(const FTechData& Tech);


};
