// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TitleWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void ATitleHUD::BeginPlay()
{

    Super::BeginPlay();

    if (TitleWidget)
    {
        UTitleWidget* Widget = CreateWidget<UTitleWidget>(GetWorld(), TitleWidget);
        if (Widget)
        {
            Widget->AddToViewport();
        }
    }






}