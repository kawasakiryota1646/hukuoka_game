// Fill out your copyright notice in the Description page of Project Settings.


#include "ClearHUD.h"
#include"ClearWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void AClearHUD::BeginPlay()
{
	Super::BeginPlay();

	if (ClearWidgetClass)
	{
		ClearWidget = CreateWidget<UClearWidget>(GetWorld(), ClearWidgetClass);
		if (ClearWidget)
		{
			ClearWidget->AddToViewport();

			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				PC->SetInputMode(FInputModeUIOnly());
				PC->bShowMouseCursor = true;
			}
		}
	}
}

