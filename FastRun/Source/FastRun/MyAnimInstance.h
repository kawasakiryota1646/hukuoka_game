// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FASTRUN_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void UpdateAnimationProperties(float DeltaTime);

    virtual void NativeInitializeAnimation() override;

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    class ARunnerCharacter* MyChar;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Speed;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsInAir;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsAccelerating;
	
};
