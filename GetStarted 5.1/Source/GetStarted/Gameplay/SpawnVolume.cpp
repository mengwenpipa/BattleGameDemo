// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	RootComponent = SpawnBox;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	const FVector Origin = SpawnBox->GetComponentLocation();
	const FVector Extent = SpawnBox->GetScaledBoxExtent();

	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActorClass()
{
	if (SpawnActorClassesArray.Num() > 0)
	{
		int index = FMath::RandRange(0, SpawnActorClassesArray.Num() - 1);
		return SpawnActorClassesArray[index];
	}
	
	return nullptr;

	
}

void ASpawnVolume::SpawnActor_Implementation(UClass* SpawnClass, FVector SpawnLocation)
{
	if (SpawnClass)
	{
		GetWorld()->SpawnActor<AActor>(SpawnClass, SpawnLocation, FRotator(0.0f));
	}

}

