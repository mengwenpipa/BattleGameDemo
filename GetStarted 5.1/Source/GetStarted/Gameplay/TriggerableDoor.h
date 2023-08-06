// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerableDoor.generated.h"

UCLASS()
class GETSTARTED_API ATriggerableDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggerableDoor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* TriggerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* TriggerBox;

	UPROPERTY(BlueprintReadWrite, Category = "Trigger Door|Trigger Switch")
	FVector InitTriggerLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Trigger Door|Door Switch")
	FVector InitDoorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Door|Door Switch")
	float DelayTime;

	FTimerHandle CloseTimeHandle;

	bool bIsPlayerOnTrigger;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32  OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Trigger Door|Trigger Switch")
	void RaiseTrigger();

	UFUNCTION(BlueprintImplementableEvent, Category = "Trigger Door|Trigger Switch")
	void LowerTriggger();

	UFUNCTION(BlueprintImplementableEvent, Category = "Trigger Door|Door Switch")
	void OpenDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Trigger Door|Door Switch")
	void CloseDoor();

	UFUNCTION(BlueprintCallable, Category = "Trigger Door|Trigger Switch")
	void UpdateTriggerLocation(FVector Offset);
	
	UFUNCTION(BlueprintCallable, Category = "Trigger Door|Door Switch")
	void UpdateDoorLocation(FVector Offset);
};
