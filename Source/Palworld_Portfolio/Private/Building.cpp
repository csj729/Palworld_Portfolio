// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Inven = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	// Static Mesh 컴포넌트를 생성하고 루트 컴포넌트로 설정합니다.
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
	RootComponent = BuildingMesh;
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuilding::OnBuildComplete()
{
    // BuildingData가 유효한지 확인하고, 데이터 에셋에서 MaxHP를 가져와 CurrentHealth를 설정합니다.
    if (BuildingData)
    {
        CurrentHealth = BuildingData->MaxHP;
        UE_LOG(LogTemp, Warning, TEXT("%s Build Complete"), *BuildingData->DisplayName.ToString());

        // 메시 컴포넌트가 있다면, 데이터 에셋의 메시로 설정합니다.
        if (BuildingMesh && BuildingData->PreviewMesh)
        {
            BuildingMesh->SetStaticMesh(BuildingData->PreviewMesh);
        }
    }
}

void ABuilding::OnDismantle()
{
	Destroy();
}

