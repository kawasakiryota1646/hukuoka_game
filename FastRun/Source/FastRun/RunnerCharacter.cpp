#include "RunnerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/Image.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h" 
ARunnerCharacter::ARunnerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // === カメラ設定 ===
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 500.f;
    SpringArmComp->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
    SpringArmComp->bUsePawnControlRotation = false;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    // === キャラ移動設定 ===
    GetCharacterMovement()->JumpZVelocity = 1200.f;
    GetCharacterMovement()->AirControl = 0.5f;
    GetCharacterMovement()->GravityScale = 2.0f;
    GetCharacterMovement()->MaxWalkSpeed = 2500.0f;

    OriginalCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

    // 衝突イベント登録
    GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARunnerCharacter::OnBeginOverlap);
}

void ARunnerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (bIsCountdown) return;
    //死亡 or クリアしたら動作を完全停止
    if (bIsDead || bIsCleared) return;

    // --- 前進処理 ---
    if (ForwardSpeed < MaxForwardSpeed)
    {
        ForwardSpeed += AccelerationRate * DeltaTime;
        ForwardSpeed = FMath::Clamp(ForwardSpeed, 0.f, MaxForwardSpeed);
    }

    AddMovementInput(GetActorForwardVector(), ForwardSpeed * DeltaTime);
    UpdateLaneMovement(DeltaTime);

    // アニメ情報更新
    FVector Velocity = GetVelocity();
    Speed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
    bIsInAir_BP = GetCharacterMovement()->IsFalling();

    // 落下判定
    if (GetActorLocation().Z < FallThreshold)
    {
        OnDeath();
    }

    if (MotionImage)
    {
        if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(MotionImage->Slot))
        {
            ShakeTime += DeltaTime;

            float Strength = FMath::Clamp(ForwardSpeed / MaxForwardSpeed, 0.f, 1.f);
            float OffsetX = FMath::Sin(ShakeTime * 40.f) * 5.f * Strength;
            float OffsetY = FMath::Cos(ShakeTime * 30.f) * 3.f * Strength;

            Slot->SetPosition(
                OriginalPos + FVector2D(OffsetX, OffsetY)
            );
        }
    }

}

void ARunnerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // プレイヤーコントローラーを強制取得
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PC)
    {
      
        PC->bShowMouseCursor = false;
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);

        UE_LOG(LogTemp, Warning, TEXT("Force Input ENABLED!"));
    }

    if (ScoreWidgetClass)
    {
        ScoreWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ScoreWidgetClass);
        if (ScoreWidgetInstance)
        {
            ScoreWidgetInstance->AddToViewport();
        }
    }
    if (SpeedEffectClass)
    {
        SpeedEffectWidget = CreateWidget<UUserWidget>(GetWorld(), SpeedEffectClass);
        if (SpeedEffectWidget)
        {
            SpeedEffectWidget->AddToViewport();

            // MotionImage 取得
            MotionImage = Cast<UImage>(SpeedEffectWidget->GetWidgetFromName(TEXT("MotionImage")));
        }
    }

    if (CountdownWidgetClass)
    {
        CountdownWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), CountdownWidgetClass);
        if (CountdownWidgetInstance)
        {
            CountdownWidgetInstance->AddToViewport();
        }
    }

    if (MotionImage)
    {
        if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(MotionImage->Slot))
        {
            OriginalPos = Slot->GetPosition();
        }
    }
    StartCountdown();

}

void ARunnerCharacter::StartCountdown()
{
    bIsCountdown = true;

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
    }

    CountdownCount = 3;

    GetWorldTimerManager().SetTimer(
        CountdownTimerHandle,
        this,
        &ARunnerCharacter::CountdownTick,
        1.0f,
        true
    );
}
void ARunnerCharacter::CountdownTick()
{
    UTextBlock* Text = Cast<UTextBlock>(CountdownWidgetInstance->GetWidgetFromName(TEXT("CountdownText")));

    if (!Text) return;

    if (CountdownCount > 0)
    {
        if (CountSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, CountSound, GetActorLocation());
        }

        Text->SetText(FText::AsNumber(CountdownCount));
    }
    else if (CountdownCount == 0)
    {
        if (StartSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this,StartSound, GetActorLocation());
        }

        Text->SetText(FText::FromString(TEXT("START!")));
        if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Text->Slot))
        {
            Slot->SetPosition(FVector2D(-600.f, -250.f));   // ← 好きな位置に変更
        }
    }
    else
    {
        CountdownWidgetInstance->RemoveFromParent();
        bIsCountdown = false;

        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            EnableInput(PC);
        }

        GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
        return;
    }

    CountdownCount--;
}


void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ARunnerCharacter::MoveLeft);
    PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ARunnerCharacter::MoveRight);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARunnerCharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARunnerCharacter::StopJumping);

    PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ARunnerCharacter::StartSlide);

    PlayerInputComponent->BindAction("PauseGame", IE_Pressed, this, &ARunnerCharacter::TogglePause);

}

void ARunnerCharacter::Jump()
{
    if (!bIsDead && !bIsInAir_BP)
    {
        Super::Jump();
        bIsInAir_BP = true;
    }
}

void ARunnerCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    bIsInAir_BP = false;
}

void ARunnerCharacter::MoveLeft()
{
    if (bIsDead) return;
    if (CurrentLane > -1)
    {
        CurrentLane--;
        TargetLocation = GetActorLocation() + FVector(0, -LaneOffset, 0);
    }
}

void ARunnerCharacter::MoveRight()
{
    if (bIsDead) return;
    if (CurrentLane < 1)
    {
        CurrentLane++;
        TargetLocation = GetActorLocation() + FVector(0, LaneOffset, 0);
    }
}

void ARunnerCharacter::StartSlide()
{
    if (bIsSliding) return;
    if (!GetCharacterMovement()->IsMovingOnGround()) return;

    bIsSliding = true;

    // カプセル縮小
    GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalCapsuleHalfHeight * 0.5f);

    // 見た目調整
    GetMesh()->AddLocalOffset(FVector(0.f, 0.f, 50.f));

    // スピードアップ
    GetCharacterMovement()->MaxWalkSpeed *= SlideSpeedMultiplier;

    //既存タイマークリア
    GetWorldTimerManager().ClearTimer(RollTimerHandle);

    //2秒後に自動で終了
    GetWorldTimerManager().SetTimer(
        RollTimerHandle,
        this,
        &ARunnerCharacter::StopSlide,
        SlideDuration,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("Roll Start"));
}

void ARunnerCharacter::StopSlide()
{
    if (!bIsSliding) return;
    bIsSliding = false;

    // カプセルを戻す
    GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalCapsuleHalfHeight);

    // スピードを戻す
    GetCharacterMovement()->MaxWalkSpeed /= SlideSpeedMultiplier;

    //メッシュを戻る
    GetMesh()->AddLocalOffset(FVector(0.f, 0.f, -50.f));

    UE_LOG(LogTemp, Warning, TEXT("Slide End!"));
}

void ARunnerCharacter::UpdateLaneMovement(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    FVector DesiredLocation = FVector(CurrentLocation.X, CurrentLane * LaneOffset, CurrentLocation.Z);
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, DesiredLocation, DeltaTime, 10.f);
    SetActorLocation(NewLocation);
}

void ARunnerCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bIsDead || bIsCleared) return;
    if (!OtherActor || OtherActor == this) return;

    // --- Obstacle Death ---
    if (OtherActor->ActorHasTag("Obstacle"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit Obstacle!"));
        OnDeath();
    }
    // --- Goal Clear ---
    else if (OtherActor->ActorHasTag("Goal"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit Goal!"));
        OnClear();
    }
}
void ARunnerCharacter::OnClear()
{
    bIsCleared = true;

    if (SpringArmComp)
    {
        // カメラを真横向き（右側から見る場合）
        SpringArmComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
        SpringArmComp->SocketOffset = FVector(0.f, 0.f, -100.f);
        SpringArmComp->TargetArmLength = 400.f; 
    }

    FRotator MeshRot = GetMesh()->GetRelativeRotation();
    MeshRot.Yaw += 180.f;
    GetMesh()->SetRelativeRotation(MeshRot);

    //モンタージュ再生
    if (GoalMontage)
    {
        UAnimInstance* Anim = GetMesh()->GetAnimInstance();
        if (Anim)
        {
            UE_LOG(LogTemp, Warning, TEXT("Play Goal Montage"));
            Anim->Montage_Play(GoalMontage);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AnimInstance is NULL!"));
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Level Cleared!"));

    //アニメーションを遅らせる

    FTimerHandle DelayHandle;
    GetWorldTimerManager().SetTimer(
        DelayHandle,
        [this]()
        {
            GetCharacterMovement()->DisableMovement();
            if (APlayerController* PC = Cast<APlayerController>(GetController()))
            {
                DisableInput(PC);
            }
        },
        0.1f,    // ← これでアニメ再生が確実に始まる
        false
    );

    // UI
    if (ClearWidgetClass)
    {
        ClearWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ClearWidgetClass);
        if (ClearWidgetInstance)
        {
            ClearWidgetInstance->AddToViewport();
        }
    }

    if (ClearSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ClearSound, GetActorLocation());
    }

    // レベル移動
    FTimerHandle ClearTimer;
    GetWorldTimerManager().SetTimer(ClearTimer, this, &ARunnerCharacter::MoveLevel, 10.0f, false);
}
void ARunnerCharacter::OnDeath()
{
    if (bIsDead) return;
    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("You Died!"));

    // ① アニメを再生
    if (DeathMontage)
    {
        UAnimInstance* Anim = GetMesh()->GetAnimInstance();
        if (Anim)
        {
            Anim->Montage_Play(DeathMontage);
            UE_LOG(LogTemp, Warning, TEXT("Death Montage Playing!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AnimInstance is NULL!"));
        }
    }

    // ② 移動停止は少し遅らせる
    FTimerHandle DelayHandle;
    GetWorldTimerManager().SetTimer(
        DelayHandle,
        [this]()
        {
            GetCharacterMovement()->DisableMovement();

            if (APlayerController* PC = Cast<APlayerController>(GetController()))
            {
                DisableInput(PC);
            }
        },
        0.1f,
        false
    );

    // ③ UI
    if (GameOverWidgetClass)
    {
        GameOverWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
        if (GameOverWidgetInstance)
        {
            GameOverWidgetInstance->AddToViewport();
        }
    }
    //Sound
    if (DeadSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DeadSound, GetActorLocation());
    }



    // ④ レベルリスタート
    FTimerHandle RestartTimer;
    GetWorldTimerManager().SetTimer(RestartTimer, this, &ARunnerCharacter::RestartLevel, 1.0f, false);
}
//リスタート
void ARunnerCharacter::RestartLevel()
{
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ARunnerCharacter::MoveLevel()
{
    FString CurrentLevelName = GetWorld()->GetName();


    if (CurrentLevelName == TEXT("stage2"))
    {
        // Stage2クリア → ClearLevel
        UGameplayStatics::OpenLevel(this, FName("Clear"));
    }
    else
    {
        UGameplayStatics::OpenLevel(this, FName("StageSelect"));

    }
}

void ARunnerCharacter::TogglePause()
{
    if (bIsDead || bIsCleared) return;

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    if (!bIsPaused)
    {
        // ---- Pause ----
        bIsPaused = true;

        UGameplayStatics::SetGamePaused(GetWorld(), true);

        if (PauseWidgetClass)
        {
            PauseWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);
            if (PauseWidgetInstance)
            {
                PauseWidgetInstance->AddToViewport();
            }
        }

        PC->bShowMouseCursor = true;
        PC->SetInputMode(FInputModeUIOnly());
        UGameplayStatics::SetGamePaused(GetWorld(), true);
    }
}

void ARunnerCharacter::ResumeGame()
{

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    bIsPaused = false;


    //Pause解除
    UGameplayStatics::SetGamePaused(GetWorld(), false);

    //Pause UI 削除
    if (PauseWidgetInstance)
    {
        PauseWidgetInstance->RemoveFromParent();
        PauseWidgetInstance = nullptr;
    }

    //既存カウントダウン削除
    if (CountdownWidgetInstance)
    {
        CountdownWidgetInstance->RemoveFromParent();
        CountdownWidgetInstance = nullptr;
    }

    //Widget作り直す
    CountdownWidgetInstance =
        CreateWidget<UUserWidget>(PC, CountdownWidgetClass);

    if (CountdownWidgetInstance)
    {
        CountdownWidgetInstance->AddToViewport(100);
    }

    if (PC)
    {
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
        //DisableInput(PC);
    }

    //カウントダウン開始
    StartCountdown();
}

//スコア
void ARunnerCharacter::AddScore(int32 Amount)
{
    Score += Amount;
    UE_LOG(LogTemp, Warning, TEXT("Score: %d"), Score);

    // スピードアップ処理
    if (Score % 2 == 0) // 1枚ごとにスピードアップ
    {
        ForwardSpeed += 30.f;  // 前進スピードを100上げる
        GetCharacterMovement()->MaxWalkSpeed += 70.f;

        UE_LOG(LogTemp, Warning, TEXT("Speed Up! New Speed: %f"), ForwardSpeed);
    }
    if (Score % 10== 0)
    {
        if (SpeedSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, SpeedSound, GetActorLocation());
        }



        if (MotionImage)
        {
            float NewAlpha = FMath::Clamp(Score * 5.f, 0.f, 5.f); 

            FLinearColor Col = MotionImage->ColorAndOpacity; 

            Col.A = NewAlpha; 

            MotionImage->SetColorAndOpacity(Col);
        }
    }

}