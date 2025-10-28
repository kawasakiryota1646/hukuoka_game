// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "RunnerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    MyChar = Cast<ARunnerCharacter>(TryGetPawnOwner());
}

void UMyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if (MyChar == nullptr)
    {
        MyChar = Cast<ARunnerCharacter>(TryGetPawnOwner());
    }

    if (MyChar)
    {
        FVector Velocity = MyChar->GetVelocity();
        Velocity.Z = 0;
        Speed = Velocity.Size(); // Å© BlendSpaceÇ…ìnÇ∑Speed
        bIsInAir = MyChar->GetMovementComponent()->IsFalling(); // Å© JumpëJà⁄óp
    }
}