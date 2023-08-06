// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/WeaponItem.h"
#include "Animation/AnimInstance.h"
#include "BaseEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Kismet/GamePlayStatics.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCapsuleComponent()->SetCapsuleSize(35.0f, 100.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.15f;

	BaseTurnRate = 65.0f;

	BaseLookUpRate = 65.0f;

	MaxHealth = 100.0f;
	Health = MaxHealth;

	MaxStamina = 100.0f;
	Stamina = MaxStamina;
	StaminaConsumeRate = 20.0f;
	ExhaustedStaminaRatio = 0.167f;
	StaminaStatus = EPlayerStaminaStatus::EPSS_Normal;

	RunningSpeed = 600.0f;
	SprintingSpeed = 900.0f;
	MovementStatus = EPlayerMovementStatus::EPMS_Normal;
	bLeftShiftKeyDown = false;

	Coins = 0;

	bAttackKeyDown = false;

	bIsAttacking = false;

	AttackTarget = nullptr;

	InterpSpeed = 15.0f;
	bInterpToEnemy = false;



}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive())
	{
		return;
	}

	switch (StaminaStatus)
	{
	case EPlayerStaminaStatus::EPSS_Normal:
		if (bLeftShiftKeyDown)
		{
			if (Stamina - StaminaConsumeRate * DeltaTime <= MaxStamina * ExhaustedStaminaRatio)
			{
				StaminaStatus = EPlayerStaminaStatus::EPSS_Exhausted;
			}
			Stamina -= StaminaConsumeRate * DeltaTime;
			SetMovementStatus(EPlayerMovementStatus::EPMS_Sprinting);
		}
		else
		{
			Stamina = FMath::Clamp(Stamina + StaminaConsumeRate * DeltaTime, 0.0f, MaxStamina);
			SetMovementStatus(EPlayerMovementStatus::EPMS_Normal);
		}
		break;
	case EPlayerStaminaStatus::EPSS_Exhausted:
		if (bLeftShiftKeyDown)
		{
			if (Stamina - StaminaConsumeRate * DeltaTime <= 0.0f)
			{
				StaminaStatus = EPlayerStaminaStatus::EPSS_ExhaustedRecovering;
				LeftShiftKeyUp();
				SetMovementStatus(EPlayerMovementStatus::EPMS_Normal);
			}
			else
			{
				Stamina -= StaminaConsumeRate * DeltaTime;
			}
		}
		else
		{
			StaminaStatus = EPlayerStaminaStatus::EPSS_ExhaustedRecovering;
			Stamina = FMath::Clamp(Stamina + StaminaConsumeRate * DeltaTime, 0.0f, MaxStamina);
			SetMovementStatus(EPlayerMovementStatus::EPMS_Normal);
		}
		break;
	case EPlayerStaminaStatus::EPSS_ExhaustedRecovering:
		if (Stamina + StaminaConsumeRate * DeltaTime >= MaxStamina * ExhaustedStaminaRatio)
		{
			StaminaStatus = EPlayerStaminaStatus::EPSS_Normal;
		}
		Stamina += StaminaConsumeRate * DeltaTime;
		LeftShiftKeyUp();
		SetMovementStatus(EPlayerMovementStatus::EPMS_Normal);

		break;
	default:
		break;
	}

	if (bInterpToEnemy && AttackTarget)
	{
		const FRotator LookAtYaw(0.0f,UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttackTarget->GetActorLocation()).Yaw,0.0f);
		const FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

}

void AMainPlayer::Jump()
{

	if (!bIsAttacking && IsAlive())
	{
		Super::Jump();
	}

}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump",IE_Pressed, this, &AMainPlayer::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainPlayer::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainPlayer::LeftShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainPlayer::LeftShiftKeyUp);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainPlayer::InteractKeyDown);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMainPlayer::AttackKeyDown);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AMainPlayer::AttackKeyUp);


	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainPlayer::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainPlayer::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMainPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMainPlayer::LookUp);

	PlayerInputComponent->BindAxis(TEXT("TurnAtRate"), this, &AMainPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpAtRate"), this, &AMainPlayer::LookUpAtRate);


}

void AMainPlayer::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0 && !bIsAttacking && IsAlive())
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);//获取控制器前面正方向的单位向量
		AddMovementInput(Direction, Value);
	}
}

void AMainPlayer::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0 && !bIsAttacking && IsAlive())
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);//获取控制器右边正方向的单位向量
		AddMovementInput(Direction, Value);
	}
}

void AMainPlayer::Turn(float Value)
{
	if (Value != 0.0f && IsAlive())
	{	
		AddControllerYawInput(Value);
	}
}

void AMainPlayer::LookUp(float Value)
{
	if (IsAlive())
	{
		if (GetControlRotation().Pitch > 180.0f && GetControlRotation().Pitch < 270.0f && Value > 0.0f)
		{
			return;
		}
		else if (GetControlRotation().Pitch > 45.0f && GetControlRotation().Pitch < 180.0f && Value < 0.0f)
		{
			return;
		}
		AddControllerPitchInput(Value);
	}
}

void AMainPlayer::TurnAtRate(float Rate)
{
	float Value = Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds();

	if (Value != 0 && IsAlive())
	{
		AddControllerYawInput(Value);
	}
}

void AMainPlayer::LookUpAtRate(float Rate)
{
	if (IsAlive())
	{
		float Value = Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds();

		if (Value != 0)
		{
			AddControllerPitchInput(Value);
		}
	}
	
}

void AMainPlayer::IncreaseHealth(float Value)
{
	Health = FMath::Clamp(Health + Value, 0.0f, MaxHealth);
}

void AMainPlayer::IncreaseStamina(float Value)
{
	Stamina = FMath::Clamp(Stamina + Value, 0.0f, MaxStamina);
}

void AMainPlayer::IncreaseCoin(int Value)
{
	Coins += Value;
}

float AMainPlayer::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
	return Health;
}

void AMainPlayer::SetMovementStatus(EPlayerMovementStatus Status)
{
	if (IsAlive())
	{
		MovementStatus = Status;
		switch (MovementStatus)
		{
		case EPlayerMovementStatus::EPMS_Sprinting:
			GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
			break;
		default:
			GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
			break;
		}
	}
}

void AMainPlayer::InteractKeyDown()
{
	if (OverlappingWeapon && IsAlive())
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->UnEquip(this);
			OverlappingWeapon->Equip(this);
		}
		else
		{
			OverlappingWeapon->Equip(this);
		}
	}
	else
	{
		if (EquippedWeapon)
		{
			
		}
		else
		{

		}
	}
}

void AMainPlayer::AttackKeyDown()
{
	if (IsAlive())
	{
		bAttackKeyDown = true;

		if (bHasWeapon)
		{
			Attack();

		}
	}
	
}

void AMainPlayer::Attack()
{
	if (!bIsAttacking && 
		(!GetMovementComponent()->IsFalling())
		&& !(bIsAttacking)
		&& IsAlive())
	{
		bIsAttacking = true;
		bInterpToEnemy = true;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AttackMontage)
		{
			float PlayerRate = FMath::RandRange(1.25f, 1.75f);
			FString SectionName = FString::FromInt(FMath::RandRange(1, 2));
			AnimInstance->Montage_Play(AttackMontage, PlayerRate);
			AnimInstance->Montage_JumpToSection(FName(*SectionName), AttackMontage);
		}
	}
}

void AMainPlayer::AttackEnd()
{
	bIsAttacking = false;
	bInterpToEnemy = false;

	if (bAttackKeyDown && IsAlive())
	{
		AttackKeyDown();
	}
}

void AMainPlayer::UpdateAttackTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		AttackTarget = nullptr;
		return;
	}

	ABaseEnemy* ClosetEnemy = nullptr;
	float MinDistance = 1000.0f;

	const FVector Location = GetActorLocation();

	for (auto Actor : OverlappingActors)
	{
		ABaseEnemy* Enemy = Cast<ABaseEnemy>(Actor);
		if (Enemy && Enemy->EnemyMovementStatus != EEnemyMovementStatus::EEMS_Dead)
		{
			float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
			if (DistanceToActor < MinDistance)
			{
				MinDistance = DistanceToActor;
				ClosetEnemy = Enemy;
			}
		}
	}

	AttackTarget = ClosetEnemy;

}

void AMainPlayer::Die()
{
	SetMovementStatus(EPlayerMovementStatus::EPMS_Dead);
	if (EquippedWeapon)
	{
		EquippedWeapon->DeactiveAttackCollision();
		EquippedWeapon->DeactiveDisplayMeshCollision();
	}
}

void AMainPlayer::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	FTimerHandle DeathTimeHandle;
	auto Lambda = [this]()
	{
		RestartLevel();
	};

	GetWorldTimerManager().SetTimer(DeathTimeHandle, FTimerDelegate::CreateLambda(Lambda), 1.0f, false);

	
}

void AMainPlayer::RestartLevel()
{
	FString LevelName = UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, FName(*LevelName));
}

