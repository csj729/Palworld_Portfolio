#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemData.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PALWORLD_PORTFOLIO_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UInventoryComponent(); // <<-- 여기에 생성자 '목차' 추가

protected:
    // Called when the game starts
    virtual void BeginPlay() override; // <<-- 여기에 BeginPlay '목차' 추가

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Capacity = 20;

    /** 기존 아이템 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<FItemData> Items;

    // ====== Pal 저장 ======
    // 실제 월드 액터 포인터 저장
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Pals")
    TArray<APal*> StoredPals;

    /** 아이템 추가 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual bool AddItem(UItemDataAsset* ItemAsset, int32 Quantity);

    /** 아이템 제거 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(UItemDataAsset* ItemAsset, int32 Quantity);

    /** 특정 아이템 개수 확인 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetItemCount(UItemDataAsset* ItemAsset) const;

    // Pal 추가
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    bool AddPal(APal* NewPal);
    // Pal 제거
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    bool RemovePal(APal* PalToRemove);

    // Pal 가져오기
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    const TArray<APal*>& GetAllPals() const { return StoredPals; }
};