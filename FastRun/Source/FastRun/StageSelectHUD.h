// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "StageSelectHUD.generated.h"

/**
 * 
 */

class UStageSelectWidget;

UCLASS()
class FASTRUN_API AStageSelectHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> StageSelectWidgetClass;

private:
	UStageSelectWidget* StageSelectWidget;

};
