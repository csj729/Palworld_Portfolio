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
    // 이 맵에 모든 건물 데이터 에셋을 채워 넣습니다.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    TMap<FName, UBuildingDataAsset*> AllBuildingData;

    virtual void Init() override;

    UFUNCTION(BlueprintCallable, Category = "Building Data")
    UBuildingDataAsset* GetBuildingData(FName BuildingID) const;

    // BuildMode에서 선택용 배열
    UPROPERTY(Transient)
    TArray<UBuildingDataAsset*> AllBuildingDataArray;

    void GenerateArrayFromMap()
    {
        AllBuildingData.GenerateValueArray(AllBuildingDataArray);
    }
};
