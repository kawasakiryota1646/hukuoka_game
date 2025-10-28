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

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void Jump() override;
    virtual void Landed(const FHitResult& Hit) override; // ← 追加！

protected:
    virtual void BeginPlay() override;

    // カメラ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArmComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* CameraComp;

    // 移動
    UPROPERTY(EditAnywhere, Category = "Movement")
    float ForwardSpeed = 600.f;
    UPROPERTY(EditAnywhere, Category = "Movement")
    float LaneOffset = 300.f;

    int CurrentLane = 0;
    FVector TargetLocation;

    void UpdateLaneMovement(float DeltaTime);
    void MoveLeft();
    void MoveRight();

    // 死亡判定
    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
        class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

    UFUNCTION()
    void OnClear();

    UPROPERTY(BlueprintReadOnly)
    bool bIsCleared = false;

    UFUNCTION()
    void OnDeath();

    UFUNCTION()
    void RestartLevel();

    bool bIsDead = false;
    UPROPERTY(EditAnywhere, Category = "Death")
    float FallThreshold = -200.f;

    // === アニメーション用 ===
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsInAir_BP = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed = 0.f;
};
