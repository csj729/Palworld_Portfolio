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
    // ���� (����)
    Brave               UMETA(DisplayName = "Brave"),           // �밨
    Ferocious           UMETA(DisplayName = "Ferocious"),       // �糪��
    HardSkin            UMETA(DisplayName = "Hard Skin"),       // �ܴ��� �Ǻ�
    ToughBody           UMETA(DisplayName = "Tough Body"),      // �＾ ��ü
    DiamondBody         UMETA(DisplayName = "Diamond Body"),    // ���̾Ƹ�� ����
    Impetuous           UMETA(DisplayName = "Impetuous"),       // ������
    LevelHeaded         UMETA(DisplayName = "Level-Headed"),    // ��ö��
    Demon               UMETA(DisplayName = "Demon"),           // �ͽ�
    Legendary           UMETA(DisplayName = "Legendary"),       // ����
    Vampire             UMETA(DisplayName = "Vampire"),         // ������

    // ���� (�̵�)
    Swift               UMETA(DisplayName = "Swift"),           // �����
    Runner              UMETA(DisplayName = "Runner"),          // �޸��� ��
    Nimble              UMETA(DisplayName = "Nimble"),          // �ż�
    HealthyConstitution UMETA(DisplayName = "Healthy Constitution"),// �ǰ��� �췮��
    AbundantEnergy      UMETA(DisplayName = "Abundant Energy"), // ���ĳ��� ���
    PerpetualMotion     UMETA(DisplayName = "Perpetual Motion"),// �������

    // ���� (����)
    Serious             UMETA(DisplayName = "Serious"),         // ������
    Artisan             UMETA(DisplayName = "Artisan"),         // ���� ����
    MasterTechnician    UMETA(DisplayName = "Master Technician"),// �����ⱳ
    WorkSlave           UMETA(DisplayName = "Work Slave"),      // �� �뿹
    DietExpert          UMETA(DisplayName = "Diet Expert"),     // �Ľ����� ����
    Abstinence          UMETA(DisplayName = "Abstinence"),      // ������ ����
    PositiveThinker     UMETA(DisplayName = "Positive Thinker"),// �������� ���
    Workaholic          UMETA(DisplayName = "Workaholic"),      // �� �ߵ�
    UnwaveringHeart     UMETA(DisplayName = "Unwavering Heart"),// �ε������ ����
    LoggingForeman      UMETA(DisplayName = "Logging Foreman"), // ���� ������
    MiningForeman       UMETA(DisplayName = "Mining Foreman"),  // ���� ������
    Motivation          UMETA(DisplayName = "Motivation"),      // ���� �ο�
    Philanthropist      UMETA(DisplayName = "Philanthropist"),  // �ھ�������
    Nocturnal           UMETA(DisplayName = "Nocturnal"),       // ���༺
    Rare                UMETA(DisplayName = "Rare"),            // ���

    // ���� (�����)
    Slacker             UMETA(DisplayName = "Slacker"),         // ����ġ�� ����
    WeakGrip            UMETA(DisplayName = "Weak Grip"),       // ���� ����
    BrittleBones        UMETA(DisplayName = "Brittle Bones"),   // �ν��� ��
    Coward              UMETA(DisplayName = "Coward"),          // ������
    Pacifist            UMETA(DisplayName = "Pacifist"),        // ��������
    Bully               UMETA(DisplayName = "Bully"),           // �ҷ���
    Sadist              UMETA(DisplayName = "Sadist"),          // ���Ʈ
    Masochist           UMETA(DisplayName = "Masochist"),       // ��������Ʈ
    Hooligan            UMETA(DisplayName = "Hooligan"),        // ������
    Musclehead          UMETA(DisplayName = "Musclehead"),      // � �ٺ�
    Clumsy              UMETA(DisplayName = "Clumsy"),          // ������
    Conceited           UMETA(DisplayName = "Conceited"),       // �ڸ���
    Glutton             UMETA(DisplayName = "Glutton"),         // �Ժ�
    BlackHoleStomach    UMETA(DisplayName = "Black Hole Stomach"),// ��Ȧ ����
    Unstable            UMETA(DisplayName = "Unstable"),        // ���� �Ҿ���
    Destructive         UMETA(DisplayName = "Destructive"),     // �ĸ� ���

    // �÷��̾� ����
    AssaultCommander    UMETA(DisplayName = "Assault Commander"),// ���� ������
    BulwarkStrategist   UMETA(DisplayName = "Bulwark Strategist") // ö�˼��� å��
};

// Trait (Ư��) ����ü
USTRUCT(BlueprintType)
struct FPalTrait
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    // --- ���� ���� ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HPScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AttackScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float DefenseScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MovementSpeedScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxStaminaScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float WorkSpeedScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HungerDepletionScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SanityDepletionScale;

    // --- �߰� ȿ�� ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SkillCooldownReduction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerAttackBuff;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerDefenseBuff;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerLoggingEfficiency;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerMiningEfficiency;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerWorkSpeedBuff;

    // --- Ư�� �ɷ� ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasLifesteal;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bWorksAtNight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIncreasesEggProduction;

    // --- ���� ���� ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float TraitScore;

    /**
     * @brief �⺻ ������. ��� ���� ȿ�� ���� �⺻ ���·� �ʱ�ȭ�մϴ�.
     */
    FPalTrait()
        : Name(TEXT("Default")), HPScale(1.0f), AttackScale(1.0f), DefenseScale(1.0f), MovementSpeedScale(1.0f),
        MaxStaminaScale(1.0f), WorkSpeedScale(1.0f), HungerDepletionScale(1.0f),
        SanityDepletionScale(1.0f), SkillCooldownReduction(0.0f), PlayerAttackBuff(0.0f),
        PlayerDefenseBuff(0.0f), PlayerLoggingEfficiency(0.0f), PlayerMiningEfficiency(0.0f),
        PlayerWorkSpeedBuff(0.0f), bHasLifesteal(false), bWorksAtNight(false),
        bIncreasesEggProduction(false), TraitScore(1.0f)
    {
    }

    /**
     * @brief GetTraitMap()���� ����ϴ� �� �ʱ�ȭ�� ���� �������Դϴ�.
     */
    FPalTrait(float InAtk, float InDef, float InMove, float InStamina, float InWork, float InHunger, float InSanity, float InCooldown,
        float InPlayerAtk, float InPlayerDef, float InPlayerLog, float InPlayerMine, float InPlayerWork,
        bool InLifesteal, bool InNightWork, bool InEgg, float InScore)
        : Name(TEXT("")), HPScale(1.0f), AttackScale(InAtk), DefenseScale(InDef), MovementSpeedScale(InMove),
        MaxStaminaScale(InStamina), WorkSpeedScale(InWork), HungerDepletionScale(InHunger),
        SanityDepletionScale(InSanity), SkillCooldownReduction(InCooldown), PlayerAttackBuff(InPlayerAtk),
        PlayerDefenseBuff(InPlayerDef), PlayerLoggingEfficiency(InPlayerLog), PlayerMiningEfficiency(InPlayerMine),
        PlayerWorkSpeedBuff(InPlayerWork), bHasLifesteal(InLifesteal), bWorksAtNight(InNightWork),
        bIncreasesEggProduction(InEgg), TraitScore(InScore)
    {
        // Name�� GetTraitMap���� �����ǹǷ� ����Ӵϴ�. HPScale�� �⺻�� 1.0f�� ����մϴ�.
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
    TArray<EPalTraitType> Traits;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|State")
    EPalState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Appearance")
    FLinearColor BodyColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|State")
    bool bIsAttacking = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|State")
    bool bIsBred;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pal|State")
    bool bIsCaptured;

    // ====== �⺻ ���� ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseMaxHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseDefense;

    // �ڿ� ���� �� �����ϰ� �����Ǵ� ���ʽ� �����Դϴ�.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
    float HP_IV;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
    float Attack_IV;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
    float Defense_IV;

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
    UFUNCTION(BlueprintImplementableEvent, Category = "Pal | Visuals")
    void OnUpdateColor();

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

