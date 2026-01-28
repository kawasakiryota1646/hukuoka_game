// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseWidget.generated.h"


class UCharacter;
class UButton;
/**
 * 
 */
UCLASS()
class FASTRUN_API UPauseWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
    virtual bool Initialize() override;

    // Resumeボタン
    UPROPERTY(meta = (BindWidget))
    UButton* ResumeButton;

    // Quitボタン
    UPROPERTY(meta = (BindWidget))
    UButton* QuitButton;

    UFUNCTION()
    void OnResumeClicked();

    UFUNCTION()
    void OnQuitClicked();


};
