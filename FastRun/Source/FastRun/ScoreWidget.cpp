// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RunnerCharacter.h"

FText UScoreWidget::GetScoreText() const
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        ARunnerCharacter* Player = Cast<ARunnerCharacter>(PC->GetPawn());

        if (Player->bIsCleared)
        {
            FSlateColor CurrentColor = ScoreText->GetColorAndOpacity();
            FLinearColor LinearColor = CurrentColor.GetSpecifiedColor();

            LinearColor.A = 1.0f; // © ‚±‚±‚¾‚¯•ÏX

            ScoreText->SetColorAndOpacity(FSlateColor(LinearColor));
        }

        if (Player)
        {
            if(Player->Score>=10)
            return FText::FromString(FString::Printf(TEXT("%d"), Player->GetScore()));
        }
    }
    return FText::FromString(TEXT(""));
}