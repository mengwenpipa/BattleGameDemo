// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Level1GameMode.generated.h"

/**
 * 
 */
UCLASS()
class GETSTARTED_API ALevel1GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Widgets")
	TSubclassOf<class UUserWidget> MainUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Widgets")
	UUserWidget* MainUI;

protected:
	virtual void BeginPlay() override;
	
};
