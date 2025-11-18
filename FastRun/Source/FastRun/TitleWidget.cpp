// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonPlay->OnClicked.AddUniqueDynamic(this, &UTitleWidget::OnButtonPlayClicked);

}

void UTitleWidget::OnButtonPlayClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("stage1")));

}