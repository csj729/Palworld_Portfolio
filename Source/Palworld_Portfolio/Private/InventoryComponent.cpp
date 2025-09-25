// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...

}

bool UInventoryComponent::AddItem(UItemDataAsset* ItemAsset, int32 Quantity)
{
    if (!ItemAsset || Quantity <= 0) return false;

    // 기존 슬롯 찾기
    for (FItemData& Slot : Items)
    {
        if (Slot.ItemAsset == ItemAsset)
        {
            int32 MaxStack = ItemAsset->MaxStackSize;
            int32 SpaceLeft = MaxStack - Slot.Quantity;

            if (SpaceLeft > 0)
            {
                int32 ToAdd = FMath::Min(SpaceLeft, Quantity);
                Slot.Quantity += ToAdd;
                Quantity -= ToAdd;
            }
        }
    }

    // 남은 수량이 있으면 새 슬롯 추가
    while (Quantity > 0 && Items.Num() < Capacity)
    {
        int32 ToAdd = FMath::Min(ItemAsset->MaxStackSize, Quantity);
        Items.Add(FItemData(ItemAsset, ToAdd));
        Quantity -= ToAdd;
    }

    return Quantity == 0; // 전부 추가됐으면 true
}

bool UInventoryComponent::RemoveItem(UItemDataAsset* ItemAsset, int32 Quantity)
{
    if (!ItemAsset || Quantity <= 0) return false;

    for (int32 i = 0; i < Items.Num(); i++)
    {
        if (Items[i].ItemAsset == ItemAsset)
        {
            if (Items[i].Quantity > Quantity)
            {
                Items[i].Quantity -= Quantity;
                return true;
            }
            else if (Items[i].Quantity == Quantity)
            {
                Items.RemoveAt(i);
                return true;
            }
            else
            {
                Quantity -= Items[i].Quantity;
                Items.RemoveAt(i);
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
    for (const FItemData& Slot : Items)
    {
        if (Slot.ItemAsset == ItemAsset)
        {
            Count += Slot.Quantity;
        }
    }
    return Count;
}

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