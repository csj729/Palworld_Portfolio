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
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
    void InitSlot(const FItemData& ItemData);

    // (C++에서도 직접 Icon/Text를 제어하고 싶다면 BindWidget 사용 가능)
    UPROPERTY(meta = (BindWidgetOptional))
    UImage* ItemIcon;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* QuantityText;
};