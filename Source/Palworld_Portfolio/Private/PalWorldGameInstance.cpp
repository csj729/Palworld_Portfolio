// Fill out your copyright notice in the Description page of Project Settings.

#include "PalWorldGameInstance.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"

void UPalWorldGameInstance::Init()
{
    Super::Init();
    UE_LOG(LogTemp, Log, TEXT("PalWorldGameInstance::Init()"));

    if (!FModuleManager::Get().IsModuleLoaded("AssetRegistry"))
    {
        UE_LOG(LogTemp, Warning, TEXT("AssetRegistry"));
    }

    // Building DataAsset 자동 등록
    TArray<FAssetData> AssetList;
    FARFilter Filter;
    Filter.ClassNames.Add(UBuildingDataAsset::StaticClass()->GetFName());
    Filter.PackagePaths.Add(FName("/Game/Build/DataAsset")); //  폴더 경로 (원하는 폴더로 변경)
    Filter.bRecursivePaths = true; // 하위 폴더까지 포함

    FAssetRegistryModule& AssetRegistryModule =
        FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

    AssetRegistryModule.Get().GetAssets(Filter, AssetList);

    for (const FAssetData& Data : AssetList)
    {
        if (UBuildingDataAsset* Asset = Cast<UBuildingDataAsset>(Data.GetAsset()))
        {
            AllBuildingData.Add(Asset->GetFName(), Asset);
        }
    }

    UE_LOG(LogTemp, Log, TEXT(" %d BuildingDataAsset Resist Complete"), AllBuildingData.Num());

    for (const auto& Pair : AllBuildingData)
    {
        if (Pair.Value)
        {
            UE_LOG(LogTemp, Warning, TEXT("ID: %s, Name: %s"),
                *Pair.Key.ToString(), *Pair.Value->GetName());
        }
    }

    GenerateArrayFromMap();
    UE_LOG(LogTemp, Warning, TEXT("AllBuildingDataArray Num=%d"), AllBuildingDataArray.Num());
}

UBuildingDataAsset* UPalWorldGameInstance::GetBuildingData(FName BuildingID) const
{
    if (UBuildingDataAsset* const* Found = AllBuildingData.Find(BuildingID))
    {
        return *Found;
    }
    return nullptr;
}