// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class FASTRUN_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
    virtual bool Initialize() override;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* StartButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ExitButton;

    UFUNCTION()
    void OnStartClicked();
    UFUNCTION()

    void OnExitClicked();
    UFUNCTION()
    void NativeConstruct();



};
