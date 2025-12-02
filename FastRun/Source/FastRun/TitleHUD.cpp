// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TitleWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void ATitleHUD::BeginPlay()
{

    Super::BeginPlay();

    if (!TitleWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("TitleWidgetClass is null!"));
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("No PlayerController found"));
        return;
    }

    UUserWidget* Widget = CreateWidget<UUserWidget>(PC, TitleWidget);
    if (!Widget)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create Title widget"));
        return;
    }

    Widget->AddToViewport();

    // マウス＆入力モードをUI優先に
    PC->bShowMouseCursor = true;
    PC->SetInputMode(FInputModeUIOnly().SetWidgetToFocus(Widget->TakeWidget()));
    UE_LOG(LogTemp, Warning, TEXT("Title HUD: widget added, mouse shown, input set to UIOnly"));





}