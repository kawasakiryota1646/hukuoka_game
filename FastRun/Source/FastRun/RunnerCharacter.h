// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RunnerCharacter.generated.h"

UCLASS()
class FASTRUN_API ARunnerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ARunnerCharacter();

protected:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float ForwardSpeed = 600.f; // 1秒あたりの前進速度

    UPROPERTY(EditAnywhere, Category = "Movement")
    float LaneOffset = 300.f; // レーンの間隔（左右移動用）

    int CurrentLane = 0; // -1, 0, 1 の3レーン制にする例

    void MoveLeft();
    void MoveRight();
    void Jump();

    FVector TargetLocation;
    void UpdateLaneMovement(float DeltaTime);

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    /** カメラ用のスプリングアーム（距離調整用） */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArmComp;

    /** 実際のカメラ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* CameraComp;
};
