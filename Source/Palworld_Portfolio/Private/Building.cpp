// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

// Sets default values
ABuilding::ABuilding()
{
    PrimaryActorTick.bCanEverTick = true;

    // SceneRoot ���� �� ��Ʈ�� ����
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    // Static Mesh ������Ʈ ���� �� SceneRoot�� Attach
    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
    BuildingMesh->SetupAttachment(SceneRoot);
    BuildingMesh->SetCollisionObjectType(ECC_Building);

    // Inventory ������Ʈ
    Inven = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
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

