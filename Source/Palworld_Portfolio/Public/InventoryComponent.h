#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemData.h"
#include "InventoryComponent.generated.h"

class APal;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PALWORLD_PORTFOLIO_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // 생성자
    UInventoryComponent();

protected:
    // 게임 시작 시 호출
    virtual void BeginPlay() override;

public:
    /** 인벤토리 슬롯 개수 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Capacity = 20;

    /** 고정 슬롯 배열 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<FItemData> Slots;

    // ================= 아이템 관련 =================
    /** 아이템 추가 (빈 슬롯 또는 기존 스택에 추가) */   
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(UItemDataAsset* ItemAsset, int32 Quantity);

    /** 슬롯 인덱스로 아이템 제거 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItemBySlot(int32 SlotIndex, int32 Quantity);

    /** 아이템 에셋으로 아이템 제거 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItemByAsset(UItemDataAsset* ItemAsset, int32 Quantity);

    /** 특정 아이템 전체 개수 확인 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetItemCount(UItemDataAsset* ItemAsset) const;

    /** 슬롯 가져오기 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    const FItemData& GetSlot(int32 SlotIndex) const;

    /** 전체 슬롯 가져오기 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    const TArray<FItemData>& GetAllSlots() const { return Slots; }

    /** 두 슬롯의 아이템을 교환 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool SwapItems(int32 FirstIndex, int32 SecondIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool SwapPals(int32 FirstIndex, int32 SecondIndex);
    // ================= Pal 관련 =================
    /** Pal 추가 */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    bool AddPal(APal* NewPal);

    /** Pal 제거 */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    bool RemovePal(APal* PalToRemove);

    /** 전체 Pal 가져오기 */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    const TArray<APal*>& GetAllPals() const { return StoredPals; }

    /** Pal 저장 공간 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Pals")
    TArray<APal*> StoredPals;
};