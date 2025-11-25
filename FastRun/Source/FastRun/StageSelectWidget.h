// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageSelectWidget.generated.h"


class UButton;


/**
 * 
 */
UCLASS()
class FASTRUN_API UStageSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Stage1;

	UPROPERTY(meta = (BindWidget))
	UButton* Stage2;

	UPROPERTY(meta = (BindWidget))
	UButton* ReturnTitle;

	UFUNCTION()
	void OnStage1Clicked();

	UFUNCTION()
	void OnStage2Clicked();

	UFUNCTION()
	void OnReturnClicked();

};
