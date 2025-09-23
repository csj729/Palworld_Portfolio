// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Building.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/GameplayStatics.h"

void UBuildingSubsystem::BeginBuilding(UBuildingDataAsset* BuildingDataAsset)
{
    if (!BuildingDataAsset || !BuildingDataAsset->BuildingClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid BuildingDataAsset or BuildingClass"));
        return;
    }

    CurrentBuildingData = BuildingDataAsset;
    bIsBuildingMode = true;
    bIsDismantlingMode = false;

    UWorld* World = GetWorld();
    if (!World) return;

    if (PreviewBuilding)
    {
        PreviewBuilding->Destroy();
        PreviewBuilding = nullptr;
    }

    // PreviewMaterial 1회 로드
    if (!PreviewMaterial)
    {
        PreviewMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Build/M_BuildingPreview.M_BuildingPreview"));
    }

    // 프리뷰 Actor 생성
    PreviewBuilding = World->SpawnActorDeferred<ABuilding>(
        CurrentBuildingData->BuildingClass,
        FTransform::Identity,
        nullptr,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

    if (PreviewBuilding)
    {
        UE_LOG(LogTemp, Warning, TEXT("PreviewBuilding enter"));
        PreviewBuilding->SetActorEnableCollision(false);
        PreviewBuilding->FinishSpawning(FTransform::Identity);

        // 모든 StaticMeshComponent에 Dynamic Material Instance 적용
        PreviewMIDs.Empty();
        TArray<UStaticMeshComponent*> MeshComps;
        PreviewBuilding->GetComponents<UStaticMeshComponent>(MeshComps);

        for (UStaticMeshComponent* MeshComp : MeshComps)
        {
            int32 NumMats = MeshComp->GetNumMaterials();
            for (int32 i = 0; i < NumMats; i++)
            {
                UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(PreviewMaterial, this);
                if (MID)
                {
                    MeshComp->SetMaterial(i, MID);
                    PreviewMIDs.Add(MID); // Tick에서 색상/투명도 업데이트용
                    UE_LOG(LogTemp, Warning, TEXT("MeshNum"));
                }
            }
            MeshComp->SetCastShadow(false);
        }
    }
}

void UBuildingSubsystem::UpdatePreview()
{
    if (!bIsBuildingMode || !PreviewBuilding || !CurrentBuildingData) return;

    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;
    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn) return;

    FVector PlayerLocation = PlayerPawn->GetActorLocation();
    FVector Forward = PlayerPawn->GetActorForwardVector();

    // 메쉬 크기로 오프셋 계산
    ABuilding* DefaultBuilding = CurrentBuildingData->BuildingClass->GetDefaultObject<ABuilding>();
    UStaticMeshComponent* MeshComp = DefaultBuilding->FindComponentByClass<UStaticMeshComponent>();
    float ForwardSize = 100.f;
    FVector BoxExtent(50.f);
    if (MeshComp && MeshComp->GetStaticMesh())
    {
        BoxExtent = MeshComp->GetStaticMesh()->GetBounds().BoxExtent;
        ForwardSize = BoxExtent.X * 2.0f + 50.0f;
    }

    FVector Location = PlayerLocation + Forward * ForwardSize;
    PreviewBuilding->SetActorLocation(Location);

    // 설치 가능 여부 체크
    bool bCanPlace = !World->OverlapAnyTestByChannel(Location, FQuat::Identity, ECC_WorldStatic,
        FCollisionShape::MakeBox(BoxExtent));

    // Dynamic Material Instance 색상/투명도 업데이트
    FLinearColor Color = bCanPlace ? FLinearColor::White : FLinearColor::Red;
    float Opacity = 0.5f;

    for (UMaterialInstanceDynamic* MID : PreviewMIDs)
    {
        if (MID)
        {
            MID->SetVectorParameterValue(FName("Color"), Color);
            MID->SetScalarParameterValue(FName("Opacity"), Opacity);
        }
    }
}

void UBuildingSubsystem::FinishBuilding(EClickType ClickType)
{
    if (!bIsBuildingMode || !CurrentBuildingData || !CurrentBuildingData->BuildingClass) return;
    if (!PreviewBuilding) return;

    UWorld* World = GetWorld();
    if (!World) return;

    FTransform SpawnTransform = PreviewBuilding->GetActorTransform();

    // 실제 건물 스폰
    ABuilding* NewBuilding = World->SpawnActor<ABuilding>(
        CurrentBuildingData->BuildingClass,
        SpawnTransform
    );

    if (NewBuilding)
        UE_LOG(LogTemp, Log, TEXT("Building placed: %s"), *NewBuilding->GetName());



    // LeftClick → 종료, RightClick → 계속 설치 가능
    if (ClickType == EClickType::LeftClick)
    {
        // 프리뷰 제거
        PreviewBuilding->Destroy();
        PreviewBuilding = nullptr;

        bIsBuildingMode = false;
        bIsDismantlingMode = false;
        CurrentBuildingData = nullptr;
    }
}

void UBuildingSubsystem::BeginDismantling()
{
    bIsDismantlingMode = true;
    bIsBuildingMode = false;
    UE_LOG(LogTemp, Warning, TEXT("Destruction mode on"));
}

void UBuildingSubsystem::DismantleAt(EClickType ClickType)
{
    if (!bIsDismantlingMode)
    {
        return; // 해체 모드가 아님
    }

    UE_LOG(LogTemp, Warning, TEXT("Dismantle Enter"));
    UWorld* World = GetWorld();
    if (!World) return;

    // 플레이어 컨트롤러와 폰 가져오기
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn) return;

    FVector Start = PlayerPawn->GetActorLocation();
    FVector Forward = PlayerPawn->GetActorForwardVector();

    // forward 방향으로 500 유닛 떨어진 지점까지
    FVector End = Start + Forward * 500.0f;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(PlayerPawn); // 자기 자신 무시

    if (World->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, Params))
    {
        ABuilding* BuildingToDismantle = Cast<ABuilding>(HitResult.GetActor());
        if (BuildingToDismantle)
        {
            UE_LOG(LogTemp, Warning, TEXT("Dismantle Building"));
            BuildingToDismantle->OnDismantle();
        }
    }

    // 모드 종료
    if (ClickType == EClickType::LeftClick)
    {
        bIsDismantlingMode = false;
    }
}

void UBuildingSubsystem::EndBuildingMode()
{
    if (PreviewBuilding)
    {
        PreviewBuilding->Destroy();
        PreviewBuilding = nullptr;
    }

    bIsBuildingMode = false;
    bIsDismantlingMode = false;
    CurrentBuildingData = nullptr;
}

void UBuildingSubsystem::Tick(float DeltaTime)
{
    if (bIsBuildingMode && CurrentBuildingData)
    {
        // 매 프레임 프리뷰 업데이트
        UpdatePreview();
    }
}
