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
    UInventoryComponent(); // <<-- ���⿡ ������ '����' �߰�

protected:
    // Called when the game starts
    virtual void BeginPlay() override; // <<-- ���⿡ BeginPlay '����' �߰�

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Capacity = 20;

    /** ���� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<FItemData> Items;

    // ====== Pal ���� ======
    // ���� ���� ���� ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Pals")
    TArray<APal*> StoredPals;

    /** ������ �߰� */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual bool AddItem(UItemDataAsset* ItemAsset, int32 Quantity);

    /** ������ ���� */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(UItemDataAsset* ItemAsset, int32 Quantity);

    /** Ư�� ������ ���� Ȯ�� */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetItemCount(UItemDataAsset* ItemAsset) const;

    // Pal �߰�
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    bool AddPal(APal* NewPal);
    // Pal ����
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    bool RemovePal(APal* PalToRemove);

    // Pal ��������
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    const TArray<APal*>& GetAllPals() const { return StoredPals; }
};