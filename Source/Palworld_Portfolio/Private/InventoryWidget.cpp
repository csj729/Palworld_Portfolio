// InventoryWidget.cpp
#include "InventoryWidget.h"
#include "InventorySlotWidget.h"
#include "InventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UInventoryWidget::RebuildInventory(UInventoryComponent* Inventory)
{
    if (!Inventory || !SlotGrid || !SlotWidgetClass) return;

    SlotGrid->ClearChildren();

    int32 Capacity = Inventory->Capacity;
    int32 NumRows = FMath::CeilToInt((float)Capacity / NumColumns);

    for (int32 i = 0; i < Capacity; i++)
    {
        UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), SlotWidgetClass);
        if (!SlotWidget) continue;

        // 슬롯 위젯에 아이템 데이터 초기화
        if (Inventory->Slots.IsValidIndex(i))
        {
            SlotWidget->InitSlot(Inventory->Slots[i]); // BPExpose 함수
        }

        // Grid에 배치
        int32 Row = i / NumColumns;
        int32 Col = i % NumColumns;
        UUniformGridSlot* GridSlot = SlotGrid->AddChildToUniformGrid(SlotWidget, Row, Col);

        // 슬롯 크기 계산은 BP에서 SizeBox Override로 조절
    }
}