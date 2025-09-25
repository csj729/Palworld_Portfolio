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
    // 긍정 (전투)
    Brave               UMETA(DisplayName = "Brave"),           // 용감
    Ferocious           UMETA(DisplayName = "Ferocious"),       // 사나움
    HardSkin            UMETA(DisplayName = "Hard Skin"),       // 단단한 피부
    ToughBody           UMETA(DisplayName = "Tough Body"),      // 억센 육체
    DiamondBody         UMETA(DisplayName = "Diamond Body"),    // 다이아몬드 보디
    Impetuous           UMETA(DisplayName = "Impetuous"),       // 성급함
    LevelHeaded         UMETA(DisplayName = "Level-Headed"),    // 냉철함
    Demon               UMETA(DisplayName = "Demon"),           // 귀신
    Legendary           UMETA(DisplayName = "Legendary"),       // 전설
    Vampire             UMETA(DisplayName = "Vampire"),         // 흡혈귀

    // 긍정 (이동)
    Swift               UMETA(DisplayName = "Swift"),           // 재빠름
    Runner              UMETA(DisplayName = "Runner"),          // 달리기 왕
    Nimble              UMETA(DisplayName = "Nimble"),          // 신속
    HealthyConstitution UMETA(DisplayName = "Healthy Constitution"),// 건강한 우량아
    AbundantEnergy      UMETA(DisplayName = "Abundant Energy"), // 넘쳐나는 기력
    PerpetualMotion     UMETA(DisplayName = "Perpetual Motion"),// 영구기관

    // 긍정 (거점)
    Serious             UMETA(DisplayName = "Serious"),         // 성실함
    Artisan             UMETA(DisplayName = "Artisan"),         // 장인 기질
    MasterTechnician    UMETA(DisplayName = "Master Technician"),// 초절기교
    WorkSlave           UMETA(DisplayName = "Work Slave"),      // 일 노예
    DietExpert          UMETA(DisplayName = "Diet Expert"),     // 식습관의 달인
    Abstinence          UMETA(DisplayName = "Abstinence"),      // 절식의 극의
    PositiveThinker     UMETA(DisplayName = "Positive Thinker"),// 긍정적인 사고
    Workaholic          UMETA(DisplayName = "Workaholic"),      // 일 중독
    UnwaveringHeart     UMETA(DisplayName = "Unwavering Heart"),// 부동명왕의 마음
    LoggingForeman      UMETA(DisplayName = "Logging Foreman"), // 벌목 관리자
    MiningForeman       UMETA(DisplayName = "Mining Foreman"),  // 광산 관리자
    Motivation          UMETA(DisplayName = "Motivation"),      // 동기 부여
    Philanthropist      UMETA(DisplayName = "Philanthropist"),  // 박애주의자
    Nocturnal           UMETA(DisplayName = "Nocturnal"),       // 야행성
    Rare                UMETA(DisplayName = "Rare"),            // 희귀

    // 부정 (디버프)
    Slacker             UMETA(DisplayName = "Slacker"),         // 농땡이치는 버릇
    WeakGrip            UMETA(DisplayName = "Weak Grip"),       // 약한 맷집
    BrittleBones        UMETA(DisplayName = "Brittle Bones"),   // 부실한 뼈
    Coward              UMETA(DisplayName = "Coward"),          // 겁쟁이
    Pacifist            UMETA(DisplayName = "Pacifist"),        // 무사주의
    Bully               UMETA(DisplayName = "Bully"),           // 불량배
    Sadist              UMETA(DisplayName = "Sadist"),          // 사디스트
    Masochist           UMETA(DisplayName = "Masochist"),       // 마조히스트
    Hooligan            UMETA(DisplayName = "Hooligan"),        // 난폭함
    Musclehead          UMETA(DisplayName = "Musclehead"),      // 운동 바보
    Clumsy              UMETA(DisplayName = "Clumsy"),          // 서투름
    Conceited           UMETA(DisplayName = "Conceited"),       // 자만심
    Glutton             UMETA(DisplayName = "Glutton"),         // 먹보
    BlackHoleStomach    UMETA(DisplayName = "Black Hole Stomach"),// 블랙홀 위장
    Unstable            UMETA(DisplayName = "Unstable"),        // 정신 불안정
    Destructive         UMETA(DisplayName = "Destructive"),     // 파멸 기원

    // 플레이어 버프
    AssaultCommander    UMETA(DisplayName = "Assault Commander"),// 돌격 지휘자
    BulwarkStrategist   UMETA(DisplayName = "Bulwark Strategist") // 철옹성의 책사
};

// Trait (특성) 구조체
USTRUCT(BlueprintType)
struct FPalTrait
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    // --- 스탯 배율 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HPScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AttackScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float DefenseScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MovementSpeedScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxStaminaScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float WorkSpeedScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HungerDepletionScale;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SanityDepletionScale;

    // --- 추가 효과 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SkillCooldownReduction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerAttackBuff;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerDefenseBuff;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerLoggingEfficiency;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerMiningEfficiency;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerWorkSpeedBuff;

    // --- 특수 능력 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasLifesteal;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bWorksAtNight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIncreasesEggProduction;

    // --- 내부 계산용 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float TraitScore;

    /**
     * @brief 기본 생성자. 모든 값을 효과 없는 기본 상태로 초기화합니다.
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
     * @brief GetTraitMap()에서 사용하는 상세 초기화를 위한 생성자입니다.
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
        // Name은 GetTraitMap에서 설정되므로 비워둡니다. HPScale은 기본값 1.0f를 사용합니다.
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

    // ====== 기본 스탯 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseMaxHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal|Stats")
    float BaseDefense;

    // 자연 스폰 시 랜덤하게 결정되는 보너스 스탯입니다.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
    float HP_IV;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
    float Attack_IV;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
    float Defense_IV;

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
    UFUNCTION(BlueprintImplementableEvent, Category = "Pal | Visuals")
    void OnUpdateColor();

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

