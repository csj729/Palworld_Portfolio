// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Palworld_PortfolioCharacter.h"
#include "PlayerSystem.generated.h"

// 기본 스탯 구조체
USTRUCT(BlueprintType)
struct FCharacterStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int MaxHP = 500;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int Attack = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int Defense = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int MaxStamina = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float WorkSpeed = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int WeightCapacity = 500;
};

// 기술 도면 구조체 (나중에 DataAsset으로 확장 가능)
USTRUCT(BlueprintType)
struct FTechData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TechID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TechPointCost = 1;
};

UCLASS()
class PALWORLD_PORTFOLIO_API APlayerSystem : public APalworld_PortfolioCharacter
{
	GENERATED_BODY()
	
public:
    // ================= 기본 진행도 =================
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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tech")
    TArray<FName> UnlockedTechs;

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
