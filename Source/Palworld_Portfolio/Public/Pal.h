// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventoryComponent.h"
#include "Pal.generated.h"

// Trait (특성) 구조체
USTRUCT(BlueprintType)
struct FPalTrait
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    // 스탯 보정 배율 (예: 1.2 = 20% 증가)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HPScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DefenseScale;

    FPalTrait()
        : Name(TEXT("Default")),
        HPScale(1.0f), AttackScale(1.0f), DefenseScale(1.0f) {
    }
};

// ====== Pal 상태 ======
UENUM(BlueprintType)
enum class EPalState : uint8
{
    Idle       UMETA(DisplayName = "Idle"),
    Following  UMETA(DisplayName = "Following"),
    Working    UMETA(DisplayName = "Working"),
    Combat     UMETA(DisplayName = "Combat")
};


UCLASS()
class PALWORLD_PORTFOLIO_API APal : public ACharacter
{
	GENERATED_BODY()

public:
    APal();
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

public:
    // ====== 인벤토리 ======
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UInventoryComponent* Inven;

    // ====== Pal 기본 정보 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Info")
    FString PalName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Info")
    int32 Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Info")
    float CurrentXP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Info")
    TArray<FPalTrait> Traits;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|State")
    EPalState State;

    // ====== 기본 스탯 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseMaxHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseDefense;

    // 최종 스탯 (Trait 적용 후 값)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
    float FinalMaxHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
    float FinalAttack;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
    float FinalDefense;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
    float CurrentHP;

    // 경험치 곡선
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Level")
    float BaseXP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Level")
    float Exponent;

    // 소속
    ACharacter* OwnerCharacter; // 어느 캐릭터의 것인지
    //ABaseBuilding* OwnerBase; // 어떤 기지에 속해있는지

    // 계산된 스탯 (Traits 적용)
    float GetMaxHP() const;
    float GetAttack() const;
    float GetDefense() const;

    // 번식
    static APal* Breed(APal* ParentA, APal* ParentB);

    // 스탯 재계산 함수
    void RecalculateStats();

    // 경험치/레벨업
    float GetXPToNextLevel() const;
    void GainExperience(float Amount);
    void CheckLevelUp();
    void LevelUp();

};
