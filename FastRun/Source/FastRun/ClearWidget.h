// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClearWidget.generated.h"

class UButton;


/**
 * 
 */
UCLASS()
class FASTRUN_API UClearWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual bool Initialize() override;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* ClearButton;


    UFUNCTION()
    void OnClearClicked();
    UFUNCTION()
    void NativeConstruct();



};
