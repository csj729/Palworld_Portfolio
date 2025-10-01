#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemData.h"
#include "InventoryComponent.generated.h"

class APal;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PALWORLD_PORTFOLIO_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // ������
    UInventoryComponent();

protected:
    // ���� ���� �� ȣ��
    virtual void BeginPlay() override;

public:
    /** �κ��丮 ���� ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Capacity = 20;

    /** ���� ���� �迭 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<FItemData> Slots;

    // ================= ������ ���� =================
    /** ������ �߰� (�� ���� �Ǵ� ���� ���ÿ� �߰�) */   
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(UItemDataAsset* ItemAsset, int32 Quantity);

    /** ���� �ε����� ������ ���� */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItemBySlot(int32 SlotIndex, int32 Quantity);

    /** ������ �������� ������ ���� */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItemByAsset(UItemDataAsset* ItemAsset, int32 Quantity);

    /** Ư�� ������ ��ü ���� Ȯ�� */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetItemCount(UItemDataAsset* ItemAsset) const;

    /** ���� �������� */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    const FItemData& GetSlot(int32 SlotIndex) const;

    /** ��ü ���� �������� */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    const TArray<FItemData>& GetAllSlots() const { return Slots; }

    /** �� ������ �������� ��ȯ */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool SwapItems(int32 FirstIndex, int32 SecondIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool SwapPals(int32 FirstIndex, int32 SecondIndex);
    // ================= Pal ���� =================
    /** Pal �߰� */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    bool AddPal(APal* NewPal);

    /** Pal ���� */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    bool RemovePal(APal* PalToRemove);

    /** ��ü Pal �������� */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Pals")
    const TArray<APal*>& GetAllPals() const { return StoredPals; }

    /** Pal ���� ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Pals")
    TArray<APal*> StoredPals;
};