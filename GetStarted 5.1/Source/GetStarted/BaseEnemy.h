// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EEMS_Idle UMETA(Displayname = "Idle"),
	EEMS_MoveToTarget UMETA(Displayname = "MoveToTarget"),
	EEMS_Attacking UMETA(Displayname = "Attacking"),
	EEMS_Dead UMETA(Displayname = "Dead"),
};


UCLASS()
class GETSTARTED_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* ChaseVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* AttackVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	EEnemyMovementStatus EnemyMovementStatus;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	class UWidgetComponent* HealthBarWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	class UProgressBar* HealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bAttackVolumnOverlapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float InterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	bool bInterpToPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	class UBoxComponent* LeftAttackCollosion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	UBoxComponent* RightAttackCollosion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class UDamageType> DamageTypeClass;
	
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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void onChaseVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void onChaseVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

	UFUNCTION()
	virtual void onAttackVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void onAttackVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMainPlayer* TargetPlayer);

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION()
	void onLeftAttackCollosionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void onLeftAttackCollosionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

	UFUNCTION()
	void onRightAttackCollosionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void onRightAttackCollosionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void ActiveLeftAttackCollosion();

	UFUNCTION(BlueprintCallable)
	void DeactiveLeftAttackCollosion();

	UFUNCTION(BlueprintCallable)
	void ActiveRightAttackCollosion();

	UFUNCTION(BlueprintCallable)
	void DeactiveRightAttackCollosion();

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Die();

	FORCEINLINE bool IsAlive() { return EnemyMovementStatus != EEnemyMovementStatus::EEMS_Dead; };

	bool HasValidTarget();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();
};
