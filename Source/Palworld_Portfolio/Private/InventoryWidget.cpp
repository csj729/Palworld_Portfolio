// InventoryWidget.cpp
#include "InventoryWidget.h"
#include "InventorySlotWidget.h"
#include "InventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UInventoryWidget::RebuildItemInventory(UInventoryComponent* Inventory)
{
    if (!Inventory || !SlotGrid || !SlotWidgetClass) return;

    SlotGrid->ClearChildren();

    int32 Capacity = Inventory->Capacity;
    int32 NumRows = FMath::CeilToInt((float)Capacity / NumColumns);

    for (int32 i = 0; i < Capacity; i++)
    {
        UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), SlotWidgetClass);
        if (!SlotWidget) continue;

        // �ε��� ����
        SlotWidget->SetSlotIndex(i);

        // ���� ������ ������ ������ �ʱ�ȭ
        if (Inventory->Slots.IsValidIndex(i))
        {
            SlotWidget->InitItemSlot(Inventory->Slots[i]); // BPExpose �Լ�
        }

        // Grid�� ��ġ
        int32 Row = i / NumColumns;
        int32 Col = i % NumColumns;
        UUniformGridSlot* GridSlot = SlotGrid->AddChildToUniformGrid(SlotWidget, Row, Col);

        // ���� ũ�� ����� BP���� SizeBox Override�� ����
    }
}

void UInventoryWidget::RebuildPalInventory(UInventoryComponent* Inventory)
{
    if (!Inventory || !SlotGrid || !SlotWidgetClass) return;

    SlotGrid->ClearChildren();

    int32 Capacity = Inventory->StoredPals.Num();
    int32 NumRows = FMath::CeilToInt((float)Capacity / NumColumns);

    for (int32 i = 0; i < Capacity; i++)
    {
        UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), SlotWidgetClass);
        if (!SlotWidget) continue;

        // �ε��� ����
        SlotWidget->SetSlotIndex(i);

        // ���� ������ ������ ������ �ʱ�ȭ
        if (Inventory->Slots.IsValidIndex(i))
        {
            SlotWidget->InitPalSlot(Inventory->Slots[i]); // BPExpose �Լ�
        }

        // Grid�� ��ġ
        int32 Row = i / NumColumns;
        int32 Col = i % NumColumns;
        UUniformGridSlot* GridSlot = SlotGrid->AddChildToUniformGrid(SlotWidget, Row, Col);

        // ���� ũ�� ����� BP���� SizeBox Override�� ����
    }
}
