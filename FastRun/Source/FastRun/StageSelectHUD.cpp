// Fill out your copyright notice in the Description page of Project Settings.


#include "StageSelectHUD.h"
#include "StageSelectWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void AStageSelectHUD::BeginPlay()
{
	Super::BeginPlay();

	if (StageSelectWidgetClass)
	{
		StageSelectWidget = CreateWidget<UStageSelectWidget>(GetWorld(), StageSelectWidgetClass);
		if (StageSelectWidget)
		{
			StageSelectWidget->AddToViewport();

			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				PC->SetInputMode(FInputModeUIOnly());
				PC->bShowMouseCursor = true;
			}
		}
	}
}

