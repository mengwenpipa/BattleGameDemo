// Fill out your copyright notice in the Description page of Project Settings.


#include "Level1GameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ALevel1GameMode::BeginPlay()
{
	Super::BeginPlay();

	if (MainUIClass)
	{
		MainUI = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(this,0), MainUIClass);
		if (MainUI)
		{
			MainUI->AddToViewport();
		}
	}
}
