// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ClearHUD.generated.h"

/**
 * 
 */

class UClearWidget;


UCLASS()
class FASTRUN_API AClearHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ClearWidgetClass;

private:
	UClearWidget* ClearWidget;

	
};
