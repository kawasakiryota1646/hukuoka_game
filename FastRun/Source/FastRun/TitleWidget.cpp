// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UTitleWidget::Initialize()
{
    if (!Super::Initialize())
    {
        return false;
    }

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UTitleWidget::OnStartClicked);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("StartButton is NULL! Check BindWidget name."));
    }

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &UTitleWidget::OnExitClicked);
    }

    return true;
}

void UTitleWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UTitleWidget::OnStartClicked()
{
    // ƒQ[ƒ€ƒŒƒxƒ‹‚É‘JˆÚ
    UGameplayStatics::OpenLevel(this, FName("StageSelect"));

}

void UTitleWidget::OnExitClicked()
{
    //ƒQ[ƒ€I—¹
    UKismetSystemLibrary::QuitGame(this,nullptr,EQuitPreference::Quit,false);
}