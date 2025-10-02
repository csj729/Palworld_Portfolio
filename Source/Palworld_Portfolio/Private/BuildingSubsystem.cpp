// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Building.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "EngineUtils.h"
#include "PalWorldGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UBuildingSubsystem::BeginBuilding()
{
    if (UPalWorldGameInstance* GI = Cast<UPalWorldGameInstance>(GetOuter()))
    {
        GI->AllBuildingData.GenerateValueArray(AvailableBuildings);

        if (AvailableBuildings.Num() > 0)
        {
            CurrentBuildIndex = 0;
            CurrentBuildingData = AvailableBuildings[CurrentBuildIndex];
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No buildings available in GameInstance!"));
            return;
        }
    }

    bIsBuildingMode = true;
    bIsDismantlingMode = false;

    UWorld* World = GetWorld();
    if (!World) return;

    // 기존 Preview 제거
    if (PreviewBuilding)
    {
        PreviewBuilding->Destroy();
        PreviewBuilding = nullptr;
    }

    // PreviewMaterial 1회 로드
    if (!PreviewMaterial)
    {
        PreviewMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Build/Material/M_BuildingPreview.M_BuildingPreview"));
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
                    PreviewMIDs.Add(MID);
                }
            }
            MeshComp->SetCastShadow(false);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("BuildMode started. Current building: %s"), *CurrentBuildingData->GetName());
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

    // 1. 플레이어 앞 위치 계산 (기존 코드)
    ABuilding* DefaultBuilding = CurrentBuildingData->BuildingClass->GetDefaultObject<ABuilding>();
    UStaticMeshComponent* MeshComp = DefaultBuilding->FindComponentByClass<UStaticMeshComponent>();
    float ForwardSize = 100.f;
    if (MeshComp && MeshComp->GetStaticMesh())
    {
        ForwardSize = MeshComp->GetStaticMesh()->GetBounds().BoxExtent.X * 2.f + 50.f;
    }
    FVector Location = PlayerLocation + Forward * ForwardSize;

    // 2. 지면 스냅 (라인트레이스)
    FVector TraceStart = Location;
    FVector TraceEnd = Location - FVector(0, 0, 1000.f);

    FHitResult HitResult;
    FCollisionQueryParams TraceParams;
    TraceParams.bTraceComplex = true;
    TraceParams.AddIgnoredActor(PlayerPawn);
    TraceParams.AddIgnoredActor(PreviewBuilding);

    // 건물 무시
    for (TActorIterator<ABuilding> It(GetWorld()); It; ++It)
    {
        ABuilding* Building = *It;
        if (Building && Building != PreviewBuilding)
        {
            TraceParams.AddIgnoredActor(Building);
        }
    }

    if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, TraceParams))
    {
        Location = HitResult.Location;
    }

    // 3. Preview 위치 적용
    PreviewBuilding->SetActorLocation(Location);

    // 4. Overlap 검사 준비
    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_Building); // 설치된 건물

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(PreviewBuilding); // Preview 자신 무시

    // 5. 메시 단위 Overlap 체크
    bool bOverlap = false;
    TArray<UStaticMeshComponent*> MeshComponents;
    PreviewBuilding->GetComponents<UStaticMeshComponent>(MeshComponents);

    for (UStaticMeshComponent* Comp : MeshComponents)
    {
        if (!Comp || !Comp->GetStaticMesh()) continue;

        FTransform CompTransform = Comp->GetComponentTransform();
        FVector BoxExtent = Comp->GetStaticMesh()->GetBounds().BoxExtent;
        FVector BoxCenter = CompTransform.TransformPosition(Comp->GetStaticMesh()->GetBounds().Origin);

        if (World->OverlapAnyTestByObjectType(
            BoxCenter,
            CompTransform.GetRotation(),
            ObjectParams,
            FCollisionShape::MakeBox(BoxExtent),
            Params))
        {
            bOverlap = true;
            UE_LOG(LogTemp, Warning, TEXT("Overlap"));
            break; // 하나라도 겹치면 설치 불가
        }
    }

    bCanBuild = !bOverlap;

    // 6. Dynamic Material Instance 색상/투명도 업데이트
    FLinearColor Color = bCanBuild ? FLinearColor::White : FLinearColor::Red;
    float Opacity = 0.5f;

    for (UMaterialInstanceDynamic* MID : PreviewMIDs)
    {
        if (MID)
        {
            MID->SetVectorParameterValue(FName("PreviewColor"), Color);
            MID->SetScalarParameterValue(FName("PreviewOpacity"), Opacity);
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
    if(bCanBuild)
    {
        ABuilding* NewBuilding = World->SpawnActor<ABuilding>(
            CurrentBuildingData->BuildingClass,
            SpawnTransform
        );
        NewBuilding->BuildingData = CurrentBuildingData;

        if (NewBuilding)
        {
            UE_LOG(LogTemp, Log, TEXT("Building placed: %s"), *NewBuilding->GetName());
            NewBuilding->OnBuildComplete();
        }

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
    
}

void UBuildingSubsystem::BeginDismantling()
{
    if (PreviewBuilding)
    {
        PreviewBuilding->Destroy();
        PreviewBuilding = nullptr;
    }

    PreviewMIDs.Empty();
    bIsDismantlingMode = true;
    bIsBuildingMode = false;
    UE_LOG(LogTemp, Warning, TEXT("Destruction mode on"));
}

// BuildingSubsystem.cpp
void UBuildingSubsystem::UpdateDismantleHighlight()
{
    if (!bIsDismantlingMode) return;

    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn) return;

    FVector Start = PlayerPawn->GetActorLocation();
    FVector Forward = PlayerPawn->GetActorForwardVector();
    FVector End = Start + Forward * 500.f;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(PlayerPawn);

    ABuilding* HitBuilding = nullptr;
    if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
    {
        HitBuilding = Cast<ABuilding>(HitResult.GetActor());
    }

    // 이전 하이라이트 초기화
    if (HighlightedBuilding && HighlightedBuilding != HitBuilding)
    {
        if (OriginalMaterials.Contains(HighlightedBuilding))
        {
            TArray<UStaticMeshComponent*> Comps;
            HighlightedBuilding->GetComponents<UStaticMeshComponent>(Comps);

            for (int32 i = 0; i < Comps.Num(); i++)
            {
                UStaticMeshComponent* Comp = Comps[i];
                TArray<UMaterialInterface*>& Mats = OriginalMaterials[HighlightedBuilding];
                for (int32 j = 0; j < Mats.Num(); j++)
                {
                    Comp->SetMaterial(j, Mats[j]);
                }
            }
            OriginalMaterials.Remove(HighlightedBuilding);
        }
        HighlightedBuilding = nullptr;
    }

    // 새로운 하이라이트 적용
    if (HitBuilding && HighlightedBuilding != HitBuilding)
    {
        TArray<UStaticMeshComponent*> Comps;
        HitBuilding->GetComponents<UStaticMeshComponent>(Comps);

        // 원본 Material 저장
        TArray<UMaterialInterface*> Mats;
        for (UStaticMeshComponent* Comp : Comps)
        {
            for (int32 j = 0; j < Comp->GetNumMaterials(); j++)
            {
                Mats.Add(Comp->GetMaterial(j));
            }
        }
        OriginalMaterials.Add(HitBuilding, Mats);

        // MID 적용
        for (UStaticMeshComponent* Comp : Comps)
        {
            for (int32 i = 0; i < Comp->GetNumMaterials(); i++)
            {
                UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(PreviewMaterial, this);
                if (MID)
                {
                    MID->SetVectorParameterValue(FName("PreviewColor"), FLinearColor::Blue);
                    MID->SetScalarParameterValue(FName("PreviewOpacity"), 0.5f);
                    Comp->SetMaterial(i, MID);
                }
            }
        }

        HighlightedBuilding = HitBuilding;
    }
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

    PreviewMIDs.Empty();
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

    if (bIsDismantlingMode)
    {
        UpdateDismantleHighlight();
    }
}

void UBuildingSubsystem::AddPreviewRotation(float AxisValue)
{
    if (!PreviewBuilding || FMath::IsNearlyZero(AxisValue))
        return;

    // AxisValue는 Mouse Wheel 입력 ±1 등, PreviewRotationSpeed와 곱해서 회전
    PreviewYaw += AxisValue * PreviewRotationSpeed;

    // 0~360 범위로 제한
    if (PreviewYaw > 360.f) PreviewYaw -= 360.f;
    if (PreviewYaw < 0.f) PreviewYaw += 360.f;

    FRotator NewRot = PreviewBuilding->GetActorRotation();
    NewRot.Yaw = PreviewYaw;
    PreviewBuilding->SetActorRotation(NewRot);
}

void UBuildingSubsystem::NextBuilding()
{
    if (AvailableBuildings.Num() == 0) return;

    CurrentBuildIndex++;
    if (CurrentBuildIndex >= AvailableBuildings.Num())
        CurrentBuildIndex = 0; // 순환

    SetCurrentBuilding(AvailableBuildings[CurrentBuildIndex]);
}

void UBuildingSubsystem::PrevBuilding()
{
    if (AvailableBuildings.Num() == 0) return;

    CurrentBuildIndex--;
    if (CurrentBuildIndex < 0)
        CurrentBuildIndex = AvailableBuildings.Num() - 1; // 순환

    SetCurrentBuilding(AvailableBuildings[CurrentBuildIndex]);
}

void UBuildingSubsystem::SetCurrentBuilding(UBuildingDataAsset* NewBuilding)
{
    if (!NewBuilding || !bIsBuildingMode) return;

    CurrentBuildingData = NewBuilding;

    // 기존 Preview 제거
    if (PreviewBuilding)
    {
        PreviewBuilding->Destroy();
        PreviewBuilding = nullptr;
    }

    // Preview 스폰만 수행 (CurrentBuildingData는 이미 설정됨)
    UWorld* World = GetWorld();
    if (!World) return;

    if (!PreviewMaterial)
    {
        PreviewMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Build/Material/M_BuildingPreview.M_BuildingPreview"));
    }

    PreviewBuilding = World->SpawnActorDeferred<ABuilding>(
        CurrentBuildingData->BuildingClass,
        FTransform::Identity,
        nullptr,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

    if (PreviewBuilding)
    {
        PreviewBuilding->SetActorEnableCollision(false);
        PreviewBuilding->FinishSpawning(FTransform::Identity);

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
                    PreviewMIDs.Add(MID);
                }
            }
            MeshComp->SetCastShadow(false);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Current Building: %s"), *CurrentBuildingData->GetName());
}