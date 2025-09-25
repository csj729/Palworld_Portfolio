// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pal.h"
#include "InventoryComponent.h"
#include "GameFramework/Actor.h"
#include "ItemData.h"
#include "Components/StaticMeshComponent.h"
#include "BuildingDataAsset.h" // UBuildingDataAsset 헤더를 포함합니다.
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

	// ABuilding 인스턴스가 어떤 데이터 에셋을 기반으로 생성되었는지 가리킵니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	class UBuildingDataAsset* BuildingData;

	// 1. 현재 체력 (Current Health)만 남깁니다. MaxHealth는 데이터 에셋에서 가져옵니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHealth;

	// Static Mesh Component는 그대로 유지합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BuildingMesh;

	// 4. 인벤토리 컴포넌트 (Inventory Component)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UInventoryComponent* Inven;

	// 5. 할당된 팰 (Assigned Pals)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assigned Pals")
	TArray<APal*> AssignedPals;

    // 건설 완료 시 호출되는 함수
    UFUNCTION(BlueprintCallable, Category = "Building")
    void OnBuildComplete();

    // 해체 시 호출되는 함수
    UFUNCTION(BlueprintCallable, Category = "Building")
    void OnDismantle();

};
	