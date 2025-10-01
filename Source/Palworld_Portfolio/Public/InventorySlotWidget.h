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
    /** �� ������ �����ϴ� Inventory�� �ε��� */
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    int32 SlotIndex;

    /** ���� �ʱ�ȭ (�ε��� + ������ ������) */
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
    void InitItemSlot(const FItemData& ItemData);

    /** ���� �ʱ�ȭ (�ε��� + �� ������) */
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
    void InitPalSlot(const FItemData& ItemData);

    /** C++���� �ε��� ������ */
    void SetSlotIndex(int32 Index) { SlotIndex = Index; }

};