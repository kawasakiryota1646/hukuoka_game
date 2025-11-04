#include "RunnerCharacter.h"
#include "Components/CapsuleComponent.h"
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
    GetCharacterMovement()->MaxWalkSpeed = 1800.0f;

    // 衝突イベント登録
    GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARunnerCharacter::OnBeginOverlap);
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARunnerCharacter::OnOverlap);
}

void ARunnerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsDead)
    {
        //徐々に加速させる
        if (ForwardSpeed < MaxForwardSpeed)
        {
            ForwardSpeed += AccelerationRate * DeltaTime;
            ForwardSpeed = FMath::Clamp(ForwardSpeed, 0.f, MaxForwardSpeed);
        }
        AddMovementInput(GetActorForwardVector(), ForwardSpeed * DeltaTime);
        UpdateLaneMovement(DeltaTime);
    }

    // アニメーション用の情報更新
    FVector Velocity = GetVelocity();
    Speed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
    bIsInAir_BP = GetCharacterMovement()->IsFalling();

    // 落下判定
    if (!bIsDead && GetActorLocation().Z < FallThreshold)
    {
        OnDeath();
    }
}

void ARunnerCharacter::BeginPlay()
{
    Super::BeginPlay();
    OnActorBeginOverlap.AddDynamic(this, &ARunnerCharacter::OnOverlapBegin);
    OriginalCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
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
    GetMesh()->AddLocalOffset(FVector(0.f, 0.f, 35.f));


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

void ARunnerCharacter::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bIsDead) return;
    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Obstacle"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap with Obstacle!"));
        OnDeath();
    }
}

void ARunnerCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bIsDead) return;
    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Obstacle"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap with Obstacle!"));
        OnDeath();
    }
}

void ARunnerCharacter::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    if (bIsDead || bIsCleared) return;
    if (OtherActor && OtherActor->ActorHasTag("Goal"))
    {
        OnClear();
    }
}

void ARunnerCharacter::OnClear()
{
    bIsCleared = true;
    UE_LOG(LogTemp, Warning, TEXT("Level Cleared!"));
    GetCharacterMovement()->DisableMovement();
    DisableInput(nullptr);

    FTimerHandle ClearTimer;
    GetWorldTimerManager().SetTimer(ClearTimer, this, &ARunnerCharacter::RestartLevel, 2.0f, false);
}

void ARunnerCharacter::OnDeath()
{
    if (bIsDead) return;
    bIsDead = true;
    UE_LOG(LogTemp, Warning, TEXT("You Died!"));
    GetCharacterMovement()->DisableMovement();
    DisableInput(nullptr);

    FTimerHandle RestartTimer;
    GetWorldTimerManager().SetTimer(RestartTimer, this, &ARunnerCharacter::RestartLevel, 0.5f, false);
}

void ARunnerCharacter::RestartLevel()
{
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}
