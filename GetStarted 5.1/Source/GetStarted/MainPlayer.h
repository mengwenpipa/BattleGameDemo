// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.generated.h"

UENUM(BlueprintType)
enum class EPlayerMovementStatus : uint8
{
	EPMS_Normal UMETA(Displayname = "Normal"),
	EPMS_Sprinting UMETA(Displayname = "Sprinting"),
	EPMS_Dead UMETA(Displayname = "Dead")
};

UENUM(BlueprintType)
enum class EPlayerStaminaStatus : uint8
{
	EPSS_Normal UMETA(Displayname = "Normal"),
	EPSS_Exhausted UMETA(Displayname = "Exhausted"),
	EPSS_ExhaustedRecovering UMETA(Displayname = "ExhaustedRecovering")
};

UCLASS()
class GETSTARTED_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainPlayer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* FollowCamera;

	float BaseTurnRate;

	float BaseLookUpRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float StaminaConsumeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats", meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1))
	float ExhaustedStaminaRatio;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	EPlayerStaminaStatus StaminaStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float SprintingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	EPlayerMovementStatus MovementStatus;


	bool bLeftShiftKeyDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bHasWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class AWeaponItem* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class AWeaponItem* OverlappingWeapon;

	bool bAttackKeyDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* AttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	class ABaseEnemy* AttackTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<ABaseEnemy> EnemyFilter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float InterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	bool bInterpToEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	class USoundCue* HitSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Jump() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	//鼠标输入处理函数
	void Turn(float Value);

	void LookUp(float Value);

	//游戏手柄输入处理函数
	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	UFUNCTION(BlueprintCallable)
	void IncreaseHealth(float Value);

	UFUNCTION(BlueprintCallable)
	void IncreaseStamina(float Value);

	UFUNCTION(BlueprintCallable)
	void IncreaseCoin(int Value);

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE void LeftShiftKeyDown() { bLeftShiftKeyDown = true; };

	FORCEINLINE void LeftShiftKeyUp() { bLeftShiftKeyDown = false; };

	void SetMovementStatus(EPlayerMovementStatus Status);

	void InteractKeyDown();

	void AttackKeyDown();

	FORCEINLINE void AttackKeyUp() { bAttackKeyDown = false; };

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	void UpdateAttackTarget();

	void Die();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	FORCEINLINE bool IsAlive() { return MovementStatus != EPlayerMovementStatus::EPMS_Dead; };
	
	void RestartLevel();

};
