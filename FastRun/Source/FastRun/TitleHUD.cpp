// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void ATitleHUD::BeginPlay()
{
	// WidgetBlueprint‚ÌClass‚ðŽæ“¾‚·‚é
	FString Path = TEXT("/Game/UI/WBP_Title.WBP_Title_C");
	TSubclassOf<UUserWidget> WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();

	// PlayerController‚ðŽæ“¾‚·‚é
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// WidgetClass‚ÆPlayerController‚ªŽæ“¾‚Å‚«‚½‚©”»’è‚·‚é
	if (WidgetClass && PlayerController)
	{
		// Widget‚ðì¬‚·‚é
		UUserWidget* UserWidget = UWidgetBlueprintLibrary::Create(GetWorld(), WidgetClass, PlayerController);

		// Viewport‚É’Ç‰Á‚·‚é
		UserWidget->AddToViewport(0);

		// MouseCursor‚ð•\Ž¦‚·‚é
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, UserWidget, EMouseLockMode::DoNotLock, true, false);
		PlayerController->SetShowMouseCursor(true);
	}






}