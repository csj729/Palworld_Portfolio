// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemData.h"
#include "InventoryComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PALWORLD_PORTFOLIO_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UInventoryComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Capacity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<FItemData> Items;

    /** 아이템 추가 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(UItemDataAsset* ItemAsset, int32 Quantity);

    /** 아이템 제거 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(UItemDataAsset* ItemAsset, int32 Quantity);

    /** 특정 아이템 개수 확인 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetItemCount(UItemDataAsset* ItemAsset) const;


};
