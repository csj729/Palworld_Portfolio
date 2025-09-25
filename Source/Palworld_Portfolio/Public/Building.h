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

	// ABuilding �ν��Ͻ��� � ������ ������ ������� �����Ǿ����� ����ŵ�ϴ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	class UBuildingDataAsset* BuildingData;

	// 1. ���� ü�� (Current Health)�� ����ϴ�. MaxHealth�� ������ ���¿��� �����ɴϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHealth;

	// Static Mesh Component�� �״�� �����մϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BuildingMesh;

	// 4. �κ��丮 ������Ʈ (Inventory Component)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UInventoryComponent* Inven;

	// 5. �Ҵ�� �� (Assigned Pals)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assigned Pals")
	TArray<APal*> AssignedPals;

    // �Ǽ� �Ϸ� �� ȣ��Ǵ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Building")
    void OnBuildComplete();

    // ��ü �� ȣ��Ǵ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Building")
    void OnDismantle();

};
	