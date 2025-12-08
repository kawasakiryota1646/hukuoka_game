// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "RunnerCharacter.generated.h"

class UImage;

UCLASS()
class FASTRUN_API ARunnerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ARunnerCharacter();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void Jump() override;
    virtual void Landed(const FHitResult& Hit) override;
protected:
    virtual void BeginPlay() override;

    // カメラ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArmComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* CameraComp;

    //クリアUIクラス（Blueprintで設定）
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> ClearWidgetClass;

    //ゲームオーバーUIクラス（Blueprintで設定）
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

    //表示中のUI
    UPROPERTY()
    UUserWidget* ClearWidgetInstance;

    UPROPERTY()
    UUserWidget* GameOverWidgetInstance;

    // カウントダウンUI
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> CountdownWidgetClass;
    FTimerHandle CountdownTimerHandle;
    int32 CountdownCount = 3;

    UUserWidget* CountdownWidgetInstance;
    UFUNCTION()
    void StartCountdown();
    UFUNCTION()
    void CountdownTick();

    // カウントダウン中フラグ
    bool bIsCountdown = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> SpeedEffectClass;

    UUserWidget* SpeedEffectWidget;
    UImage* MotionImage;

    //サウンド
    UPROPERTY(EditAnywhere, Category = "Sound")
    USoundBase* ClearSound;

    // 速度関係
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float ForwardSpeed = 400.f; // 初期スピード

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxForwardSpeed = 2500.f; // 最大スピード

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AccelerationRate = 100.f; // 毎秒どれくらい上げるか
    float LaneOffset = 300.f;

    int CurrentLane = 0;
    FVector TargetLocation;

    // スライディング中かどうか
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsSliding = false;

    // スライディング時間
    UPROPERTY(EditAnywhere, Category = "Movement")
    float SlideDuration = 0.9f;

    // スライディング中の速度倍率
    UPROPERTY(EditAnywhere, Category = "Movement")
    float SlideSpeedMultiplier = 1.5f;

    // 元のカプセル高さ
    float OriginalCapsuleHalfHeight;

    // スライディング開始・終了
    void StartSlide();
    void StopSlide();

    void UpdateLaneMovement(float DeltaTime);
    void MoveLeft();
    void MoveRight();
protected:

    // 死亡判定
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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

    UFUNCTION()
    void MoveLevel();



    // === アニメーション用 ===
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsInAir_BP = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DeathMontage;


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* GoalMontage;


    UPROPERTY(BlueprintReadOnly, Category = "Score")
    int32 Score = 0;

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Value);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> ScoreWidgetClass;

    // Blueprintから読めるGetter
    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 GetScore() const { return Score; }

    UUserWidget* ScoreWidgetInstance;
};
