#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Building.h"
#include "BuildingDataAsset.h"
#include "BuildingSubsystem.generated.h"

UENUM(BlueprintType)
enum class EClickType : uint8
{
    LeftClick,
    RightClick
};

UCLASS(Blueprintable)
class PALWORLD_PORTFOLIO_API UBuildingSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    // === 건설 ===
    UFUNCTION(BlueprintCallable, Category = "Building")
    void BeginBuilding(UBuildingDataAsset* BuildingDataAsset);

    UFUNCTION(BlueprintCallable, Category = "Building")
    void FinishBuilding(EClickType ClickType);

    UFUNCTION(BlueprintCallable, Category = "Building")
    void EndBuildingMode();

    // === 해체 ===
    UFUNCTION(BlueprintCallable, Category = "Building")
    void BeginDismantling();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void DismantleAt(EClickType ClickType);

    // === 상태 체크 ===
    UFUNCTION(BlueprintCallable, Category = "Building")
    bool IsBuildingMode() const { return bIsBuildingMode; }

    UFUNCTION(BlueprintCallable, Category = "Building")
    bool IsDismantlingMode() const { return bIsDismantlingMode; }

    // === Preview 업데이트 ===
    void UpdatePreview();

    /** 매 프레임 실행할 Tick */
    virtual void Tick(float DeltaTime) override;

    /** 필수: Tick 등록용 StatId */
    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UBuildingSubsystem, STATGROUP_Tickables);
    }

private:
    // 현재 건설 중인 건물의 Data
    UPROPERTY()
    UBuildingDataAsset* CurrentBuildingData;

    UPROPERTY()
    bool bIsBuildingMode = false;

    UPROPERTY()
    bool bIsDismantlingMode = false;

    // Preview용
    UPROPERTY()
    ABuilding* PreviewBuilding = nullptr;

    UPROPERTY()
    UMaterialInterface* PreviewMaterial = nullptr;

    // Preview Material Instance들
    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> PreviewMIDs;
};