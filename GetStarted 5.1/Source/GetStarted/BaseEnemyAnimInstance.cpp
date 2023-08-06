// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAnimInstance.h"
#include "BaseEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBaseEnemyAnimInstance::NativeInitializeAnimation()
{
	EnemyRef = Cast<ABaseEnemy>(TryGetPawnOwner());
}

void UBaseEnemyAnimInstance::UpdataAnimationProperties()
{
	if (EnemyRef == nullptr)
	{
		EnemyRef = Cast<ABaseEnemy>(TryGetPawnOwner());
	}

	if (EnemyRef != nullptr)
	{
		FVector SpeedVector = EnemyRef->GetVelocity();
		FVector PlanarSpeed = FVector(SpeedVector.X, SpeedVector.Y, 0.0f);
		Speed = PlanarSpeed.Size();

		//float Direction = CalculateDirection(EnemyRef->GetVelocity(), EnemyRef->GetActorRotation());
	}
}
