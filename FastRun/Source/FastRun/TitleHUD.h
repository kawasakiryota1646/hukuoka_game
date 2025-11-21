// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TitleWidget.h"
#include "TitleHUD.generated.h"

class UUserWidget; 

/**
 * 
 */
UCLASS()
class FASTRUN_API ATitleHUD : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTitleWidget> TitleWidget;

};
