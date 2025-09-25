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
    // === �Ǽ� ===
    UFUNCTION(BlueprintCallable, Category = "Building")
    void BeginBuilding(UBuildingDataAsset* BuildingDataAsset);

    UFUNCTION(BlueprintCallable, Category = "Building")
    void FinishBuilding(EClickType ClickType);

    UFUNCTION(BlueprintCallable, Category = "Building")
    void EndBuildingMode();

    // === ��ü ===
    UFUNCTION(BlueprintCallable, Category = "Building")
    void BeginDismantling();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void DismantleAt(EClickType ClickType);

    // === ���� üũ ===
    UFUNCTION(BlueprintCallable, Category = "Building")
    bool IsBuildingMode() const { return bIsBuildingMode; }

    UFUNCTION(BlueprintCallable, Category = "Building")
    bool IsDismantlingMode() const { return bIsDismantlingMode; }

    /** Preview �޽ø� Ư�� ȸ������ŭ ȸ����Ŵ (Mouse Wheel Input��) */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void AddPreviewRotation(float AxisValue);

    // === Preview ������Ʈ ===
    void UpdatePreview();

    // === DismantleMode ������Ʈ ===
    void UpdateDismantleHighlight();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
    float PreviewYaw = 0.f; // ���� ȸ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
    float PreviewRotationSpeed = 15.f; // ���콺 �� 1ƽ�� ȸ�� ����

    /** �� ������ ������ Tick */
    virtual void Tick(float DeltaTime) override;

    /** �ʼ�: Tick ��Ͽ� StatId */
    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UBuildingSubsystem, STATGROUP_Tickables);
    }

private:
    // ���� �Ǽ� ���� �ǹ��� Data
    UPROPERTY()
    UBuildingDataAsset* CurrentBuildingData;

    UPROPERTY()
    bool bIsBuildingMode = false;

    UPROPERTY()
    bool bIsDismantlingMode = false;

    UPROPERTY()
    bool bCanBuild = true;
    // Preview��
    UPROPERTY()
    ABuilding* PreviewBuilding = nullptr;

    UPROPERTY()
    UMaterialInterface* PreviewMaterial = nullptr;

    // Preview Material Instance��
    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> PreviewMIDs;

    UPROPERTY()
    ABuilding* HighlightedBuilding = nullptr;

    TMap<ABuilding*, TArray<UMaterialInterface*>> OriginalMaterials;
};