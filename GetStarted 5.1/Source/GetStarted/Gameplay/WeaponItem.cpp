// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/WeaponItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "MainPlayer.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "BaseEnemy.h"

AWeaponItem::AWeaponItem()
{
	TriggerVolume->SetSphereRadius(64.0f);

	DisplayMesh->DestroyComponent();
	//新组件的标识符不要跟刚Destory掉的组件的标识符相同，因为组件的DestroyComponent方法
	//只是给了一个标记而已，具体什么时候Destroy要看UE引擎的处理。防止同时出现两个标识符相同的组件。
	DisplayMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DisplaySkeletalMesh"));
	DisplayMesh->SetupAttachment(GetRootComponent());
	ActiveDisplayMeshCollision();

	AttackCollosion = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollosion"));
	AttackCollosion->SetupAttachment(DisplayMesh, "WeaponSocket");
	DeactiveAttackCollision();

	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCueAsset(TEXT("/Script/Engine.SoundCue'/Game/Assets/Audios/Blade_Cue.Blade_Cue'"));

	if (SoundCueAsset.Succeeded())
	{
		OnEquippedSound = SoundCueAsset.Object;
	}

	bShouldReservedIdleParticle = false;
	WeaponState = EWeaponState::EWS_CanPickup;

	Damage = 25.0f;


	
}

void AWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	AttackCollosion->OnComponentBeginOverlap.AddDynamic(this, &AWeaponItem::onAttackCollosionOverlapBegin);
	AttackCollosion->OnComponentEndOverlap.AddDynamic(this, &AWeaponItem::onAttackCollosionOverlapEnd);

}

void AWeaponItem::onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::onOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor && WeaponState == EWeaponState::EWS_CanPickup)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			MainPlayer->OverlappingWeapon = this;
		}
	}
}

void AWeaponItem::onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::onOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer && MainPlayer->OverlappingWeapon == this)
		{
			MainPlayer->OverlappingWeapon = nullptr;
		}
	}

}

void AWeaponItem::Equip(AMainPlayer* MainPlayer)
{
	if (MainPlayer)
	{
		WeaponState = EWeaponState::EWS_Equipped;
		DeactiveDisplayMeshCollision();

		const USkeletalMeshSocket* RightHandSocket = MainPlayer->GetMesh()->GetSocketByName("RightHandSocket");
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, MainPlayer->GetMesh());
			MainPlayer->bHasWeapon = true;
			MainPlayer->EquippedWeapon = this;

			bNeedRotate = false;
			if (OnEquippedSound)
			{
				UGameplayStatics::PlaySound2D(this, OnEquippedSound);
			}
			if (!bShouldReservedIdleParticle)
			{
				IdleParticleComponent->Deactivate();
			}
		}
	}
}

void AWeaponItem::UnEquip(AMainPlayer* MainPlayer)
{
	if (MainPlayer  
		&& (!MainPlayer->GetMovementComponent()->IsFalling())
		&& !(MainPlayer->bIsAttacking))
	{
		WeaponState = EWeaponState::EWS_CanPickup;
		ActiveDisplayMeshCollision();

		MainPlayer->bHasWeapon = false;
		MainPlayer->EquippedWeapon = nullptr;
		if (MainPlayer->OverlappingWeapon == nullptr)
		{
			MainPlayer->OverlappingWeapon = this;
		}

		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		SetActorRotation(FRotator(0.0f));
		SetActorScale3D(FVector(1.0f));

		bNeedRotate = true;

		IdleParticleComponent->Activate();
	}
}

void AWeaponItem::ActiveDisplayMeshCollision()
{
	DisplayMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DisplayMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	DisplayMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DisplayMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}

void AWeaponItem::DeactiveDisplayMeshCollision()
{
	DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AWeaponItem::onAttackCollosionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ABaseEnemy* BaseEnemy = Cast<ABaseEnemy>(OtherActor);
		if (BaseEnemy)
		{
			if (BaseEnemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = ((USkeletalMeshComponent*)DisplayMesh)->GetSocketByName("WeaponSocket");
				if (WeaponSocket)
				{
					const FVector SocketLocation = WeaponSocket->GetSocketLocation((USkeletalMeshComponent*)DisplayMesh);
					UGameplayStatics::SpawnEmitterAtLocation(this, BaseEnemy->HitParticles, SocketLocation, FRotator(0.0f), true);
				}
			}

			if (BaseEnemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, BaseEnemy->HitSound);
			}

			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(BaseEnemy, Damage, WeaponOwner, this, DamageTypeClass);
			}
		}
	}
}

void AWeaponItem::onAttackCollosionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AWeaponItem::ActiveAttackCollision()
{
	AttackCollosion->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackCollosion->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackCollosion->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackCollosion->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

void AWeaponItem::DeactiveAttackCollision()
{
	AttackCollosion->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
