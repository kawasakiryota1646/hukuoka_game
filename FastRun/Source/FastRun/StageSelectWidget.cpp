// Fill out your copyright notice in the Description page of Project Settings.


#include "StageSelectWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void UStageSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Stage1)
		Stage1->OnClicked.AddDynamic(this, &UStageSelectWidget::OnStage1Clicked);

	if (Stage2)
		Stage2->OnClicked.AddDynamic(this, &UStageSelectWidget::OnStage2Clicked);

	if (ReturnTitle)
		ReturnTitle->OnClicked.AddDynamic(this, &UStageSelectWidget::OnReturnClicked);
}

void UStageSelectWidget::OnStage1Clicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("stage1"));
}

void UStageSelectWidget::OnStage2Clicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("stage2"));
}

void UStageSelectWidget::OnReturnClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}