// InventoryComponent.cpp

#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false; // �� ������ ƽ ���ʿ�
    Slots.SetNum(Capacity); // ���� ���� ���� Ȯ��
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{

}

// ================== ������ ���� ==================

bool UInventoryComponent::AddItem(UItemDataAsset* ItemAsset, int32 Quantity)
{
    if (!ItemAsset || Quantity <= 0) return false;

    int32 MaxStack = ItemAsset->MaxStackSize;

    // 1. ���� �������� ����ִ� ���Կ� �켱 �߰�
    for (FItemData& Slot : Slots)
    {
        if (Slot.ItemAsset == ItemAsset && Slot.Quantity < MaxStack)
        {
            int32 SpaceLeft = MaxStack - Slot.Quantity;
            int32 ToAdd = FMath::Min(SpaceLeft, Quantity);

            Slot.Quantity += ToAdd;
            Quantity -= ToAdd;

            if (Quantity <= 0) return true;
        }
    }

    // 2. �� ���Կ� ���� �߰�
    for (FItemData& Slot : Slots)
    {
        if (Slot.ItemAsset == nullptr)
        {
            int32 ToAdd = FMath::Min(MaxStack, Quantity);

            Slot.ItemAsset = ItemAsset;
            Slot.Quantity = ToAdd;

            Quantity -= ToAdd;
            if (Quantity <= 0) return true;
        }
    }

    // �� �߰� �������� ����
    return false;
}

bool UInventoryComponent::RemoveItemBySlot(int32 SlotIndex, int32 Quantity)
{
    if (!Slots.IsValidIndex(SlotIndex) || Quantity <= 0) return false;

    FItemData& Slot = Slots[SlotIndex];
    if (!Slot.ItemAsset || Slot.Quantity <= 0) return false;

    if (Slot.Quantity > Quantity)
    {
        Slot.Quantity -= Quantity;
    }
    else
    {
        // �� ���� ���� �ʱ�ȭ
        Slot.ItemAsset = nullptr;
        Slot.Quantity = 0;
    }

    return true;
}

bool UInventoryComponent::RemoveItemByAsset(UItemDataAsset* ItemAsset, int32 Quantity)
{
    if (!ItemAsset || Quantity <= 0) return false;

    for (int32 i = 0; i < Slots.Num(); i++)
    {
        if (Slots[i].ItemAsset == ItemAsset)
        {
            if (Slots[i].Quantity > Quantity)
            {
                Slots[i].Quantity -= Quantity;
                return true;
            }
            else if (Slots[i].Quantity == Quantity)
            {
                Slots.RemoveAt(i);
                return true;
            }
            else
            {
                Quantity -= Slots[i].Quantity;
                Slots.RemoveAt(i);
                i--;
            }
        }
    }

    return Quantity == 0;
}

int32 UInventoryComponent::GetItemCount(UItemDataAsset* ItemAsset) const
{
    if (!ItemAsset) return 0;

    int32 Count = 0;
    for (const FItemData& Slot : Slots)
    {
        if (Slot.ItemAsset == ItemAsset)
        {
            Count += Slot.Quantity;
        }
    }
    return Count;
}

const FItemData& UInventoryComponent::GetSlot(int32 SlotIndex) const
{
    check(Slots.IsValidIndex(SlotIndex)); // �߸��� �ε����� Crash �� ����׿� ����
    return Slots[SlotIndex];
}

bool UInventoryComponent::SwapItems(int32 FirstIndex, int32 SecondIndex)
{
    // �ε��� ��ȿ�� üũ
    if (!Slots.IsValidIndex(FirstIndex) || !Slots.IsValidIndex(SecondIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("SwapItems: Invalid indices (%d, %d)"), FirstIndex, SecondIndex);
        return false;
    }

    // ���� �����̸� ��ȯ�� �ʿ� ����
    if (FirstIndex == SecondIndex)
    {
        return false;
    }

    // �� ���� ��ȯ
    Slots.Swap(FirstIndex, SecondIndex);

    return true;
}

bool UInventoryComponent::SwapPals(int32 FirstIndex, int32 SecondIndex)
{
    // �ε��� ��ȿ�� üũ
    if (!StoredPals.IsValidIndex(FirstIndex) || !StoredPals.IsValidIndex(SecondIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("SwapItems: Invalid indices (%d, %d)"), FirstIndex, SecondIndex);
        return false;
    }

    // ���� �����̸� ��ȯ�� �ʿ� ����
    if (FirstIndex == SecondIndex)
    {
        return false;
    }

    // �� ���� ��ȯ
    StoredPals.Swap(FirstIndex, SecondIndex);

    return true;
}

// ================== Pal ���� ==================

bool UInventoryComponent::AddPal(APal* NewPal)
{
    if (NewPal)
    {
        StoredPals.Add(NewPal);
        return true;
    }
    return false;
}

bool UInventoryComponent::RemovePal(APal* PalToRemove)
{
    return StoredPals.Remove(PalToRemove) > 0;
}