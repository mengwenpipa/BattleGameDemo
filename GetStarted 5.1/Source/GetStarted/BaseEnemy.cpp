// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "MainPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GamePlayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChaseVolume = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseVolume"));
	ChaseVolume->SetupAttachment(GetRootComponent());
	ChaseVolume->InitSphereRadius(800.0f);
	ChaseVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ChaseVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ChaseVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	AttackVolume = CreateDefaultSubobject<USphereComponent>(TEXT("AttackVolume"));
	AttackVolume->SetupAttachment(GetRootComponent());
	AttackVolume->InitSphereRadius(100.0f);
	AttackVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetDrawSize(FVector2D(125.0f, 10.0f));

	LeftAttackCollosion = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftAttackCollosion"));
	LeftAttackCollosion->SetupAttachment(GetMesh(), "LeftAttackSocket");
	DeactiveLeftAttackCollosion();

	RightAttackCollosion = CreateDefaultSubobject<UBoxComponent>(TEXT("RightAttackCollosion"));
	RightAttackCollosion->SetupAttachment(GetMesh(), "RightAttackSocket");
	DeactiveRightAttackCollosion();

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;
	bAttackVolumnOverlapping = false;

	InterpSpeed = 15.0f;
	bInterpToPlayer = false;
	
	MaxHealth = 100.0f;
	Health = MaxHealth;

	Damage = 10.0f;



}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	ChaseVolume->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::onChaseVolumeOverlapBegin);
	ChaseVolume->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::onChaseVolumeOverlapEnd);

	AttackVolume->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::onAttackVolumeOverlapBegin);
	AttackVolume->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::onAttackVolumeOverlapEnd);

	LeftAttackCollosion->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::onLeftAttackCollosionOverlapBegin);
	LeftAttackCollosion->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::onLeftAttackCollosionOverlapEnd);

	RightAttackCollosion->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::onRightAttackCollosionOverlapBegin);
	RightAttackCollosion->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::onRightAttackCollosionOverlapEnd);

	HealthBar = Cast<UProgressBar>(HealthBarWidgetComponent->GetUserWidgetObject()->GetWidgetFromName("HealthBar"));
	HealthBar->SetPercent(Health / MaxHealth);
	HealthBar->SetVisibility(ESlateVisibility::Hidden);

	AIController = Cast<AAIController>(GetController());


}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToPlayer && HasValidTarget() && IsAlive())
	{
		const FRotator LookAtYaw(0.0f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerPawn(this,0)->GetActorLocation()).Yaw, 0.0f);
		const FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemy::onChaseVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsAlive())
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{ 
			HealthBar->SetVisibility(ESlateVisibility::Visible);
			MoveToTarget(MainPlayer);
		}
	}
}

void ABaseEnemy::onChaseVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && IsAlive())
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			HealthBar->SetVisibility(ESlateVisibility::Hidden);
			EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;

			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void ABaseEnemy::onAttackVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsAlive())
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			MainPlayer->UpdateAttackTarget();

			bAttackVolumnOverlapping = true;
			Attack();
		}
	}
}

void ABaseEnemy::onAttackVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && IsAlive())
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			bAttackVolumnOverlapping = false;
			if (EnemyMovementStatus != EEnemyMovementStatus::EEMS_Attacking)
			{
				MoveToTarget(MainPlayer);
			}
		}
	}
}

void ABaseEnemy::MoveToTarget(AMainPlayer* TargetPlayer)
{

	if (IsAlive())
	{
		EnemyMovementStatus = EEnemyMovementStatus::EEMS_MoveToTarget;

		if (AIController)
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(TargetPlayer);
			MoveRequest.SetAcceptanceRadius(10.0f);
			FNavPathSharedPtr NavPath;

			AIController->MoveTo(MoveRequest, &NavPath);

			/*auto PathPoints = NavPath->GetPathPoints();
			for (auto Point : PathPoints)
			{
				FVector Location = Point.Location;
				UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.0f, 8, FLinearColor::Red, 10.0f, 1.5f);
			}*/
		}
	}
}

void ABaseEnemy::Attack()
{

	if (HasValidTarget() && IsAlive())
	{
		if (AIController)
		{
			AIController->StopMovement();
		}

		if (EnemyMovementStatus != EEnemyMovementStatus::EEMS_Attacking)
		{
			EnemyMovementStatus = EEnemyMovementStatus::EEMS_Attacking;
			bInterpToPlayer = true;

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance && AttackMontage)
			{
				const float PlayerRate = FMath::RandRange(0.9f, 1.1f);
				const FString SectionName = FString::FromInt(FMath::RandRange(1, 3));
				AnimInstance->Montage_Play(AttackMontage, PlayerRate);
				AnimInstance->Montage_JumpToSection(FName(*SectionName), AttackMontage);
			}
		}
	}
}

void ABaseEnemy::AttackEnd()
{
	bInterpToPlayer = false;
	if (HasValidTarget() && IsAlive())
	{
		EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;
		
		if (bAttackVolumnOverlapping)
		{
			Attack();
		}
	}
	
}

void ABaseEnemy::onLeftAttackCollosionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsAlive())
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			if (MainPlayer->HitParticles)
			{
				const USkeletalMeshSocket* AttackSocket = GetMesh()->GetSocketByName("LeftAttackSocket");
				if (AttackSocket)
				{
					const FVector SocketLocation = AttackSocket->GetSocketLocation((USkeletalMeshComponent*)GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(this, MainPlayer->HitParticles, SocketLocation, FRotator(0.0f), true);
				}
			}

			if (MainPlayer->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, MainPlayer->HitSound);
			}

			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(MainPlayer, Damage / 2, AIController, this, DamageTypeClass);
			}
		}
	}
}

void ABaseEnemy::onLeftAttackCollosionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABaseEnemy::onRightAttackCollosionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsAlive())
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			if (MainPlayer->HitParticles)
			{
				const USkeletalMeshSocket* AttackSocket = GetMesh()->GetSocketByName("RightAttackSocket");
				if (AttackSocket)
				{
					const FVector SocketLocation = AttackSocket->GetSocketLocation((USkeletalMeshComponent*)GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(this, MainPlayer->HitParticles, SocketLocation, FRotator(0.0f), true);
				}
			}

			if (MainPlayer->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, MainPlayer->HitSound);
			}

			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(MainPlayer, Damage / 2, AIController, this, DamageTypeClass);
			}
		}
	}
}

void ABaseEnemy::onRightAttackCollosionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ABaseEnemy::ActiveLeftAttackCollosion()
{
	LeftAttackCollosion->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftAttackCollosion->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftAttackCollosion->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftAttackCollosion->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

void ABaseEnemy::DeactiveLeftAttackCollosion()
{
	LeftAttackCollosion->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseEnemy::ActiveRightAttackCollosion()
{
	RightAttackCollosion->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightAttackCollosion->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightAttackCollosion->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightAttackCollosion->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ABaseEnemy::DeactiveRightAttackCollosion()
{
	RightAttackCollosion->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float ABaseEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - Damage <= 0.0f)
	{
		Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
		Die();
	}
	else
	{
		Health -= Damage;
	}

	HealthBar->SetPercent(Health / MaxHealth);

	return Health;
}

void ABaseEnemy::Die()
{
	EnemyMovementStatus = EEnemyMovementStatus::EEMS_Dead;

	HealthBar->SetVisibility(ESlateVisibility::Hidden);

	DeactiveLeftAttackCollosion();
	DeactiveRightAttackCollosion();
	ChaseVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0))->UpdateAttackTarget();
	
}

bool ABaseEnemy::HasValidTarget()
{
	return Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(this, 0))->MovementStatus
			!= EPlayerMovementStatus::EPMS_Dead;
}

void ABaseEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	FTimerHandle DeathTimeHandle;
	auto Lambda = [this]()
	{
		Destroy();
	};

	GetWorldTimerManager().SetTimer(DeathTimeHandle, FTimerDelegate::CreateLambda(Lambda), 1.0f, false);

}

