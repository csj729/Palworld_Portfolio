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
    // 이 데이터 에셋의 고유 ID (프로그래밍 식별용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    FName BuildingID;

    // 이 건물을 지을 때 스폰될 실제 건물 클래스 (ABuilding 자식 블루프린트)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    TSubclassOf<class ABuilding> BuildingClass;

    // 건물 최대체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    float MaxHP;

    // UI에 표시될 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    FText DisplayName;

    // 건설에 필요한 자원
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    TArray<FItemData> RequiredResources;

    // 건설에 필요한 총 작업량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    float WorkRequired;

    // 건물의 미리보기 메시 (선택 사항)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Data")
    class UStaticMesh* PreviewMesh;
};
