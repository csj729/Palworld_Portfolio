// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.h"
#include "BuildingDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PALWORLD_PORTFOLIO_API UBuildingDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
    // �� ������ ������ ���� ID (���α׷��� �ĺ���)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    FName BuildingID;

    // �� �ǹ��� ���� �� ������ ���� �ǹ� Ŭ���� (ABuilding �ڽ� �������Ʈ)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    TSubclassOf<class ABuilding> BuildingClass;

    // �ǹ� �ִ�ü��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    float MaxHP;

    // UI�� ǥ�õ� �̸�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    FText DisplayName;

    // �Ǽ��� �ʿ��� �ڿ�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    TArray<FItemData> RequiredResources;

    // �Ǽ��� �ʿ��� �� �۾���
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    float WorkRequired;

    // �ǹ��� �̸����� �޽� (���� ����)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    class UStaticMesh* PreviewMesh;
};
