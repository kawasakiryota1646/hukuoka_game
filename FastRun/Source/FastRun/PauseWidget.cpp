// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include"RunnerCharacter.h"

bool UPauseWidget::Initialize()
{
    if (!Super::Initialize())
    {
        return false;
    }

    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UPauseWidget::OnResumeClicked);
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UPauseWidget::OnQuitClicked);
    }

    return true;
}

void UPauseWidget::OnResumeClicked()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)return;
    UGameplayStatics::SetGamePaused(this, false);
    if (PC)
    {
        //FInputModeGameOnly InputMode;
        //PC->SetInputMode(InputMode);
        //PC->bShowMouseCursor = false;
        //PC->EnableInput(PC);

        if (ACharacter* Character = PC->GetCharacter())
        {
            if (ARunnerCharacter* Runner = Cast<ARunnerCharacter>(Character))
            {
                Runner->ResumeGame();
            }

        }

    }

    RemoveFromParent();
}

void UPauseWidget::OnQuitClicked()
{
    UGameplayStatics::OpenLevel(this, FName("Title"));
}

