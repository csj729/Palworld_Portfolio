// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventoryComponent.h"
#include "AIController.h"
#include "Pal.generated.h"

// Pal Trait ���
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

// Trait (Ư��) ����ü
USTRUCT(BlueprintType)
struct FPalTrait
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    // ���� ���� ���� (��: 1.2 = 20% ����)
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

// ====== Pal ���� ======
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
    // ====== �κ��丮 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UInventoryComponent* Inven;

    // ====== Pal �⺻ ���� ======
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

    // ====== �⺻ ���� ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseMaxHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseDefense;

    // ���� ���� (Trait ���� �� ��)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float FinalMaxHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float FinalAttack;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float FinalDefense;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float CurrentHP;

    // ����ġ �
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Level")
    float BaseXP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Level")
    float Exponent;

    // Pal Controller
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|AI")
    TSubclassOf<AAIController> PalAIControllerClass;

    // �Ҽ�
    ACharacter* OwnerCharacter; // ��� ĳ������ ������
    //ABaseBuilding* OwnerBase; // � ������ �����ִ���

    // ���� ���� (Traits ����)
    UFUNCTION(BlueprintCallable)
    float GetMaxHP() const { return FinalMaxHP; } 
    UFUNCTION(BlueprintCallable)
    float GetAttack() const { return FinalAttack; }
    UFUNCTION(BlueprintCallable)
    float GetDefense() const { return FinalDefense; }
    
    // Trait ��� ��������
    UFUNCTION(BlueprintCallable)
    static const TMap<EPalTraitType, FPalTrait>& GetTraitMap();

    // ����
    UFUNCTION(BlueprintCallable, Category = "Pal Breeding", meta = (WorldContext = "WorldContextObject"))
    static APal* Breed(UObject* WorldContextObject, APal* ParentA, APal* ParentB, FVector SpawnLocation);

    // ���� ���� �Լ�
    UFUNCTION(BlueprintCallable)
    void RecalculateStats();

    // Pal ���� �� Pal ������ ����
    UFUNCTION(BlueprintCallable)
    void UpdateColor();

    // ������ �ޱ�
    UFUNCTION(BlueprintCallable)
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // ����ġ/������
    UFUNCTION(BlueprintCallable)
    float GetXPToNextLevel() const;
    UFUNCTION(BlueprintCallable)
    void GainExperience(float Amount);
    UFUNCTION(BlueprintCallable)
    void CheckLevelUp();
    UFUNCTION(BlueprintCallable)
    void LevelUp();

};