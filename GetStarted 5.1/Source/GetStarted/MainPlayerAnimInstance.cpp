// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerAnimInstance.h"
#include "GetStarted/MainPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainPlayerAnimInstance::NativeInitializeAnimation()
{
	PlayerRef = Cast<AMainPlayer>(TryGetPawnOwner());
}

void UMainPlayerAnimInstance::UpdataAnimationProperties()
{
	if (PlayerRef == nullptr)
	{
		PlayerRef = Cast<AMainPlayer>(TryGetPawnOwner());
	}

	if (PlayerRef != nullptr)
	{
		FVector SpeedVector = PlayerRef->GetVelocity();
		FVector PlanarSpeed = FVector(SpeedVector.X, SpeedVector.Y, 0.0f);
		Speed = PlanarSpeed.Size();

		bIsInAir = PlayerRef->GetMovementComponent()->IsFalling();
	}
}
