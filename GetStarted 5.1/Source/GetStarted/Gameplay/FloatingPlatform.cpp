// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	RootComponent = PlatformMesh;

	StartPoint = FVector(0.0f);
	EndPoint = FVector(0.0f);

	InterpSpeed = 200.0f;
	DelayTime = 2.0f;
	bInterping = true;


}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	Distance = (EndPoint - StartPoint).Size();
	
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping)
	{
		const FVector CurrentLocation = GetActorLocation();

		const FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);

		SetActorLocation(NewLocation);

		float NewDistance = (GetActorLocation() - StartPoint).Size();

		if (Distance - NewDistance <= 0.5f)
		{
			auto ToggleInterpState = [this]()
			{
				bInterping = !bInterping;
			};
			ToggleInterpState();
			GetWorldTimerManager().SetTimer(InterpTimeHandle, FTimerDelegate::CreateLambda(ToggleInterpState), DelayTime, false);
			FVector TmpVector = StartPoint;
			StartPoint = EndPoint;
			EndPoint = TmpVector;
		}
	}

	

}

