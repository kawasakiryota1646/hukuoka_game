// Fill out your copyright notice in the Description page of Project Settings.


#include "ClearWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UClearWidget::Initialize()
{
    if (!Super::Initialize())
    {
        return false;
    }

    if (ClearButton)
    {
        ClearButton->OnClicked.AddDynamic(this, &UClearWidget::OnClearClicked);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("StartButton is NULL! Check BindWidget name."));
    }

    return true;
}

void UClearWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UClearWidget::OnClearClicked()
{


    // ƒQ[ƒ€ƒŒƒxƒ‹‚É‘JˆÚ
    UGameplayStatics::OpenLevel(this, FName("Title"));

}

