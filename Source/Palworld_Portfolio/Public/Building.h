// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pal.h"
#include "InventoryComponent.h"
#include "GameFramework/Actor.h"
#include "ItemData.h"
#include "Components/StaticMeshComponent.h"
#include "BuildingDataAsset.h" // UBuildingDataAsset ����� �����մϴ�.
#include "Building.generated.h"

#define ECC_Building ECC_GameTraceChannel1

UCLASS()
class PALWORLD_PORTFOLIO_API ABuilding : public AActor
{
    GENERATED_BODY()

public:
    ABuilding();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // ������ ����
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
    class UBuildingDataAsset* BuildingData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CurrentHealth;

    // --- ��Ʈ �� ������Ʈ �߰� (�̰� ��Ʈ�� ���)
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    USceneComponent* SceneRoot;

    // Static Mesh (���� SceneRoot�� �ڽ�)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* BuildingMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UInventoryComponent* Inven;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assigned Pals")
    TArray<APal*> AssignedPals;

    UFUNCTION(BlueprintCallable, Category = "Building")
    void OnBuildComplete();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void OnDismantle();
};