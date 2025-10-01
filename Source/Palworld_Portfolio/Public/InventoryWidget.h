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
    /** ���� ���� Ŭ���� (BP���� ���� ����) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

    /** ������ ���� �׸��� (�����̳ʰ� BP���� Bind) */
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* SlotGrid;
    
    /** �κ��丮 ���� �Լ� (C++ ȣ�� �� BP UI ������Ʈ) */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RebuildItemInventory(UInventoryComponent* Inventory);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RebuildPalInventory(UInventoryComponent* Inventory);

protected:
    /** �� ���� (�����̳ʰ� BP���� ���� ����) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 NumColumns = 5;
};