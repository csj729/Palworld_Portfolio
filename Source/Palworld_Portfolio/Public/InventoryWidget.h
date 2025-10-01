#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;
class UInventorySlotWidget; 
class UInventoryComponent;

UCLASS()
class PALWORLD_PORTFOLIO_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 슬롯 위젯 클래스 (BP에서 지정 가능) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

    /** 슬롯이 붙을 그리드 (디자이너가 BP에서 Bind) */
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* SlotGrid;
    
    /** 인벤토리 갱신 함수 (C++ 호출 → BP UI 업데이트) */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RebuildItemInventory(UInventoryComponent* Inventory);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RebuildPalInventory(UInventoryComponent* Inventory);

protected:
    /** 열 개수 (디자이너가 BP에서 조정 가능) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 NumColumns = 5;
};