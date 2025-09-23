// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BuildingDataAsset.h"
#include "PalWorldGameInstance.generated.h"

UCLASS()
class PALWORLD_PORTFOLIO_API UPalWorldGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // �� �ʿ� ��� �ǹ� ������ ������ ä�� �ֽ��ϴ�.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    TMap<FName, UBuildingDataAsset*> AllBuildingData;
};
