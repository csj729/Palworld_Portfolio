// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct FItemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UItemDataAsset* ItemAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity; 
    
    FItemData()
        : ItemAsset(nullptr), Quantity(0)
    {
    }

    FItemData(UItemDataAsset* InItem, int32 InQuantity)
        : ItemAsset(InItem), Quantity(InQuantity)
    {
    }

    bool IsEmpty() const { return ItemAsset == nullptr || Quantity <= 0; }
};