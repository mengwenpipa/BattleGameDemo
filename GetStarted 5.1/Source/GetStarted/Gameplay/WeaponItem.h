// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/InteractableItem.h"
#include "WeaponItem.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_CanPickup UMETA(DisplayName = "CanPickUp"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
};

/**
 * 
 */
UCLASS()
class GETSTARTED_API AWeaponItem : public AInteractableItem
{
	GENERATED_BODY()

public:
	AWeaponItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Sound")
	class USoundCue* OnEquippedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Parrical")
	bool bShouldReservedIdleParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Attack")
	class UBoxComponent* AttackCollosion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	TSubclassOf<class UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Attack")
	class AController* WeaponOwner;



protected:
	virtual void BeginPlay() override;


public:
	void onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex) override;

	void Equip(class AMainPlayer* MainPlayer);

	void UnEquip(AMainPlayer* MainPlayer);

	void ActiveDisplayMeshCollision();

	void DeactiveDisplayMeshCollision();

	UFUNCTION()
	void onAttackCollosionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void onAttackCollosionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void ActiveAttackCollision();

	UFUNCTION(BlueprintCallable)
	void DeactiveAttackCollision();
};
