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

    // PreviewMaterial 1ȸ �ε�
    if (!PreviewMaterial)
    {
        PreviewMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Build/M_BuildingPreview.M_BuildingPreview"));
    }

    // ������ Actor ����
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

        // ��� StaticMeshComponent�� Dynamic Material Instance ����
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
                    PreviewMIDs.Add(MID); // Tick���� ����/���� ������Ʈ��
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

    // �޽� ũ��� ������ ���
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

    // ��ġ ���� ���� üũ
    bool bCanPlace = !World->OverlapAnyTestByChannel(Location, FQuat::Identity, ECC_WorldStatic,
        FCollisionShape::MakeBox(BoxExtent));

    // Dynamic Material Instance ����/���� ������Ʈ
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

    // ���� �ǹ� ����
    ABuilding* NewBuilding = World->SpawnActor<ABuilding>(
        CurrentBuildingData->BuildingClass,
        SpawnTransform
    );

    if (NewBuilding)
        UE_LOG(LogTemp, Log, TEXT("Building placed: %s"), *NewBuilding->GetName());



    // LeftClick �� ����, RightClick �� ��� ��ġ ����
    if (ClickType == EClickType::LeftClick)
    {
        // ������ ����
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
        return; // ��ü ��尡 �ƴ�
    }

    UE_LOG(LogTemp, Warning, TEXT("Dismantle Enter"));
    UWorld* World = GetWorld();
    if (!World) return;

    // �÷��̾� ��Ʈ�ѷ��� �� ��������
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn) return;

    FVector Start = PlayerPawn->GetActorLocation();
    FVector Forward = PlayerPawn->GetActorForwardVector();

    // forward �������� 500 ���� ������ ��������
    FVector End = Start + Forward * 500.0f;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(PlayerPawn); // �ڱ� �ڽ� ����

    if (World->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, Params))
    {
        ABuilding* BuildingToDismantle = Cast<ABuilding>(HitResult.GetActor());
        if (BuildingToDismantle)
        {
            UE_LOG(LogTemp, Warning, TEXT("Dismantle Building"));
            BuildingToDismantle->OnDismantle();
        }
    }

    // ��� ����
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
        // �� ������ ������ ������Ʈ
        UpdatePreview();
    }
}
