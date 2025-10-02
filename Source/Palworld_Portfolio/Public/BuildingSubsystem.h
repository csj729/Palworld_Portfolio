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
    void BeginBuilding();

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

    /** Preview 메시를 특정 회전값만큼 회전시킴 (Mouse Wheel Input용) */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void AddPreviewRotation(float AxisValue);

    // === Preview 업데이트 ===
    void UpdatePreview();

    // === DismantleMode 업데이트 ===
    void UpdateDismantleHighlight();

    // BuildMode에서 건물 선택
    UFUNCTION(BlueprintCallable, Category = "Building")
    void NextBuilding();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void PrevBuilding();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void SetCurrentBuilding(UBuildingDataAsset* NewBuilding);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
    float PreviewYaw = 0.f; // 현재 회전값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
    float PreviewRotationSpeed = 15.f; // 마우스 휠 1틱당 회전 각도

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

    UPROPERTY()
    bool bCanBuild = true;
    // Preview용
    UPROPERTY()
    ABuilding* PreviewBuilding = nullptr;

    UPROPERTY()
    UMaterialInterface* PreviewMaterial = nullptr;

    // Preview Material Instance들
    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> PreviewMIDs;

    UPROPERTY()
    ABuilding* HighlightedBuilding = nullptr;

    TMap<ABuilding*, TArray<UMaterialInterface*>> OriginalMaterials;

    // 현재 선택된 건물 인덱스 (GameInstance의 TArray 기준)
    int32 CurrentBuildIndex = 0;

    // BuildMode 토글 시 선택 가능한 건물 배열
    UPROPERTY()
    TArray<UBuildingDataAsset*> AvailableBuildings;
};