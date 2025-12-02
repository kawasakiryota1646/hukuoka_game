#include "RunnerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/Image.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

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

    // ★ 死亡 or クリアしたら動作を完全停止！
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
}

void ARunnerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // プレイヤーコントローラーを強制取得
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PC)
    {
        // 念のため先に Disable してから Enable
        DisableInput(PC);
        EnableInput(PC);
        PC->bShowMouseCursor = true;
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

            // MotionImage 取得（UMGの名前と一致する必要あり）
            MotionImage = Cast<UImage>(SpeedEffectWidget->GetWidgetFromName(TEXT("MotionImage")));
        }
    }

}


void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ARunnerCharacter::MoveLeft);
    PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ARunnerCharacter::MoveRight);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARunnerCharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARunnerCharacter::StopJumping);

    PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ARunnerCharacter::StartSlide);
    PlayerInputComponent->BindAction("Slide", IE_Released, this, &ARunnerCharacter::StopSlide);
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
    if (bIsSliding || !GetCharacterMovement()->IsMovingOnGround()) return;

    bIsSliding = true;

    // カプセルを小さくしてしゃがむような姿勢に
    GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalCapsuleHalfHeight * 0.5f);

    // 👇 メッシュを少し上にずらす（見た目用）
    GetMesh()->AddLocalOffset(FVector(0.f, 0.f, 40.f));


    // スライディング時は速度を少し上げる
    GetCharacterMovement()->MaxWalkSpeed *= SlideSpeedMultiplier;

    UE_LOG(LogTemp, Warning, TEXT("Slide Start!"));
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
    GetMesh()->AddLocalOffset(FVector(0.f, 0.f, -35.f));

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

    // ① モンタージュは AnimInstance 経由で再生（絶対安全）
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

    // ② DisableMovement はアニメ再生の後に少し遅延させる（重要）
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
    GetWorldTimerManager().SetTimer(ClearTimer, this, &ARunnerCharacter::MoveLevel, 2.0f, false);
}
void ARunnerCharacter::OnDeath()
{
    if (bIsDead) return;
    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("You Died!"));

    // ① まずアニメを再生！！
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

    // ② 移動停止は少し遅らせる（←重要）
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
        0.1f,    // ← これが大事！アニメを止めずに済む
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

    // ④ レベルリスタート
    FTimerHandle RestartTimer;
    GetWorldTimerManager().SetTimer(RestartTimer, this, &ARunnerCharacter::RestartLevel, 5.5f, false);
}
//リスタート
void ARunnerCharacter::RestartLevel()
{
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ARunnerCharacter::MoveLevel()
{
    UGameplayStatics::OpenLevel(this, FName("stage2"));
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
    if (Score > 30)
    {
        if (MotionImage)
        {
            float NewAlpha = FMath::Clamp(Score * 5.f, 0.f, 5.f);

            FLinearColor Col = MotionImage->ColorAndOpacity;
            Col.A = NewAlpha;
            MotionImage->SetColorAndOpacity(Col);
        }
    }

}