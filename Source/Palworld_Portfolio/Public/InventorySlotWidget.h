#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemData.h"
#include "InventorySlotWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PALWORLD_PORTFOLIO_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 이 슬롯이 참조하는 Inventory의 인덱스 */
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    int32 SlotIndex;

    /** 슬롯 초기화 (인덱스 + 아이템 데이터) */
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
    void InitItemSlot(const FItemData& ItemData);

    /** 슬롯 초기화 (인덱스 + 펠 데이터) */
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
    void InitPalSlot(const FItemData& ItemData);

    /** C++에서 인덱스 설정용 */
    void SetSlotIndex(int32 Index) { SlotIndex = Index; }

};