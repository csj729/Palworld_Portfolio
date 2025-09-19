// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Consumable UMETA(DisplayName = "Consumable"),
    Material   UMETA(DisplayName = "Material"),
    Equipment  UMETA(DisplayName = "Equipment"),
    Misc       UMETA(DisplayName = "Misc")
};

UCLASS()
class PALWORLD_PORTFOLIO_API UItemDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 MaxStackSize = 99;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float Weight = 0.0f;
};
