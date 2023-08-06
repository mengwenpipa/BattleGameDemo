// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupItem.h"
#include "MainPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"

APickupItem::APickupItem()
{
	TriggerVolume->SetSphereRadius(50.0f);
}

void APickupItem::onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::onOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		const AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);

		if (MainPlayer)
		{
			if (OverlapParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, OverlapParticle, GetActorLocation(), FRotator(0.0f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
			OnPickedup(MainPlayer);
			Destroy();
		}
	}
}

void APickupItem::onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::onOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
