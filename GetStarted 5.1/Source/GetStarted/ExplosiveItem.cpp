// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveItem.h"
#include "MainPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "BaseEnemy.h"

AExplosiveItem::AExplosiveItem()
{
	Damage = 20.0f;
}

void AExplosiveItem::onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::onOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor)
	{
		const AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		const ABaseEnemy* BaseEnemy = Cast<ABaseEnemy>(OtherActor);

		if (MainPlayer || BaseEnemy)
		{
			if (OverlapParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, OverlapParticle, GetActorLocation(), FRotator(0.0f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
			UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);
			Destroy();
		}
	}
}

void AExplosiveItem::onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::onOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
