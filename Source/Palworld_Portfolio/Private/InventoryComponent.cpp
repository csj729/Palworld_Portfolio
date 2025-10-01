// InventoryComponent.cpp

#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false; // 매 프레임 틱 불필요
    Slots.SetNum(Capacity); // 고정 슬롯 개수 확보
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{

}

// ================== 아이템 관련 ==================

bool UInventoryComponent::AddItem(UItemDataAsset* ItemAsset, int32 Quantity)
{
    if (!ItemAsset || Quantity <= 0) return false;

    int32 MaxStack = ItemAsset->MaxStackSize;

    // 1. 같은 아이템이 들어있는 슬롯에 우선 추가
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

    // 2. 빈 슬롯에 새로 추가
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

    // 다 추가 못했으면 실패
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
        // 다 빼면 슬롯 초기화
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
    check(Slots.IsValidIndex(SlotIndex)); // 잘못된 인덱스면 Crash → 디버그에 도움
    return Slots[SlotIndex];
}

bool UInventoryComponent::SwapItems(int32 FirstIndex, int32 SecondIndex)
{
    // 인덱스 유효성 체크
    if (!Slots.IsValidIndex(FirstIndex) || !Slots.IsValidIndex(SecondIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("SwapItems: Invalid indices (%d, %d)"), FirstIndex, SecondIndex);
        return false;
    }

    // 같은 슬롯이면 교환할 필요 없음
    if (FirstIndex == SecondIndex)
    {
        return false;
    }

    // 두 슬롯 교환
    Slots.Swap(FirstIndex, SecondIndex);

    return true;
}

bool UInventoryComponent::SwapPals(int32 FirstIndex, int32 SecondIndex)
{
    // 인덱스 유효성 체크
    if (!StoredPals.IsValidIndex(FirstIndex) || !StoredPals.IsValidIndex(SecondIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("SwapItems: Invalid indices (%d, %d)"), FirstIndex, SecondIndex);
        return false;
    }

    // 같은 슬롯이면 교환할 필요 없음
    if (FirstIndex == SecondIndex)
    {
        return false;
    }

    // 두 슬롯 교환
    StoredPals.Swap(FirstIndex, SecondIndex);

    return true;
}

// ================== Pal 관련 ==================

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