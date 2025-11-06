// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreWidget.h"

#include "Kismet/GameplayStatics.h"
#include "RunnerCharacter.h"

FText UScoreWidget::GetScoreText() const
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        ARunnerCharacter* Player = Cast<ARunnerCharacter>(PC->GetPawn());
        if (Player)
        {
            return FText::FromString(FString::Printf(TEXT("Score: %d"), Player->GetScore()));
        }
    }
    return FText::FromString(TEXT("Score: 0"));
}