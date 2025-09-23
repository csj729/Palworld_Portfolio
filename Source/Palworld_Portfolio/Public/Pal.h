// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventoryComponent.h"
#include "AIController.h"
#include "Pal.generated.h"

// Pal Trait 목록
UENUM(BlueprintType)
enum class EPalTraitType : uint8
{
    Brave       UMETA(DisplayName = "Brave"),
    Sturdy      UMETA(DisplayName = "Sturdy"),
    Swift       UMETA(DisplayName = "Swift"),
    Artisan     UMETA(DisplayName = "Artisan"),
    Coward      UMETA(DisplayName = "Coward"),
    Frail       UMETA(DisplayName = "Frail"),
    Lazy        UMETA(DisplayName = "Lazy"),
    Glutton     UMETA(DisplayName = "Glutton"),
    Moody       UMETA(DisplayName = "Moody"),
    Normal      UMETA(DisplayName = "Normal")
};

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TraitScore;

    FPalTrait()
        : Name(TEXT("Default")),
        HPScale(1.0f), AttackScale(1.0f), DefenseScale(1.0f), TraitScore(1.0f) {
    }

    FPalTrait(const FString& InName, float InHP, float InAtk, float InDef, float InScore)
        : Name(InName), HPScale(InHP), AttackScale(InAtk), DefenseScale(InDef), TraitScore(InScore)
    {
    }

    bool operator==(const FPalTrait& Other) const { return Name == Other.Name; }
};

// ====== Pal 상태 ======
UENUM(BlueprintType)
enum class EPalState : uint8
{
    Idle       UMETA(DisplayName = "Idle"),
    Following  UMETA(DisplayName = "Following"),
    Working    UMETA(DisplayName = "Working"),
    Battle     UMETA(DisplayName = "Battle"),
    Dead       UMETA(DisplayName = "Dead")
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Info")
    bool IsWild = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|State")
    EPalState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Appearance")
    FLinearColor BodyColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|State")
    bool bIsAttacking = false;

    // ====== 기본 스탯 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseMaxHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseDefense;

    // 최종 스탯 (Trait 적용 후 값)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float FinalMaxHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float FinalAttack;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float FinalDefense;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float CurrentHP;

    // 경험치 곡선
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Level")
    float BaseXP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Level")
    float Exponent;

    // Pal Controller
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|AI")
    TSubclassOf<AAIController> PalAIControllerClass;

    // 소속
    ACharacter* OwnerCharacter; // 어느 캐릭터의 것인지
    //ABaseBuilding* OwnerBase; // 어떤 기지에 속해있는지

    // 계산된 스탯 (Traits 적용)
    UFUNCTION(BlueprintCallable)
    float GetMaxHP() const { return FinalMaxHP; } 
    UFUNCTION(BlueprintCallable)
    float GetAttack() const { return FinalAttack; }
    UFUNCTION(BlueprintCallable)
    float GetDefense() const { return FinalDefense; }
    
    // Trait 목록 가져오기
    UFUNCTION(BlueprintCallable)
    static const TMap<EPalTraitType, FPalTrait>& GetTraitMap();

    // 번식
    UFUNCTION(BlueprintCallable, Category = "Pal Breeding", meta = (WorldContext = "WorldContextObject"))
    static APal* Breed(UObject* WorldContextObject, APal* ParentA, APal* ParentB, FVector SpawnLocation);

    // 스탯 재계산 함수
    UFUNCTION(BlueprintCallable)
    void RecalculateStats();

    // Pal 교배 시 Pal 몸색깔 결정
    UFUNCTION(BlueprintCallable)
    void UpdateColor();

    // 데미지 받기
    UFUNCTION(BlueprintCallable)
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // 경험치/레벨업
    UFUNCTION(BlueprintCallable)
    float GetXPToNextLevel() const;
    UFUNCTION(BlueprintCallable)
    void GainExperience(float Amount);
    UFUNCTION(BlueprintCallable)
    void CheckLevelUp();
    UFUNCTION(BlueprintCallable)
    void LevelUp();

};