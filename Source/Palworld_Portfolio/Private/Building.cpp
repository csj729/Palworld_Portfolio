// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Inven = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	// Static Mesh ������Ʈ�� �����ϰ� ��Ʈ ������Ʈ�� �����մϴ�.
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
    // BuildingData�� ��ȿ���� Ȯ���ϰ�, ������ ���¿��� MaxHP�� ������ CurrentHealth�� �����մϴ�.
    if (BuildingData)
    {
        CurrentHealth = BuildingData->MaxHP;
        UE_LOG(LogTemp, Warning, TEXT("%s Build Complete"), *BuildingData->DisplayName.ToString());

        // �޽� ������Ʈ�� �ִٸ�, ������ ������ �޽÷� �����մϴ�.
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

