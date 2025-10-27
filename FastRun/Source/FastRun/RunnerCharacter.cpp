// Fill out your copyright notice in the Description page of Project Settings.


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
    GetCharacterMovement()->JumpZVelocity = 1200.f;  // ジャンプ力
    GetCharacterMovement()->AirControl = 0.5f;       // 空中操作
    GetCharacterMovement()->GravityScale = 2.0f;     // 重力
    GetCharacterMovement()->MaxWalkSpeed = 1000.0f;    // 最大移動速度

    // 衝突イベント登録
    GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
    GetCapsuleComponent()->SetGenerateOverlapEvents(false);
    //GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ARunnerCharacter::OnHit);
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARunnerCharacter::OnBeginOverlap);
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARunnerCharacter::OnOverlap);
}

void ARunnerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 常に前方に進む
    if (!bIsDead)
    {
        AddMovementInput(GetActorForwardVector(), ForwardSpeed * DeltaTime);
        UpdateLaneMovement(DeltaTime);
    }

    //空中にいるかどうかを判定
    IsInAir = GetCharacterMovement()->IsFalling();

    // 落下チェック
    if (!bIsDead && GetActorLocation().Z < FallThreshold)
    {
        OnDeath();
    }
}

void ARunnerCharacter::BeginPlay()
{
    Super::BeginPlay();

    OnActorBeginOverlap.AddDynamic(this, &ARunnerCharacter::OnOverlapBegin);
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

    // 移動・入力停止
    GetCharacterMovement()->DisableMovement();
    DisableInput(nullptr);



    // フェードアウトや演出を入れたい場合はここで実装可能
    // 2秒後に次のレベル（または同じレベル再読み込み）
    FTimerHandle ClearTimer;
    GetWorldTimerManager().SetTimer(ClearTimer, this, &ARunnerCharacter::RestartLevel, 2.0f, false);
}

void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ARunnerCharacter::MoveLeft);
    PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ARunnerCharacter::MoveRight);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARunnerCharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARunnerCharacter::StopJumping);
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

void ARunnerCharacter::Jump()
{
    if (!bIsDead)
    {
        Super::Jump();
    }
}


void ARunnerCharacter::UpdateLaneMovement(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    FVector DesiredLocation = FVector(CurrentLocation.X, CurrentLane * LaneOffset, CurrentLocation.Z);
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, DesiredLocation, DeltaTime, 10.f);
    SetActorLocation(NewLocation);
}

void ARunnerCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

    if (bIsDead) return;

    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Obstacles"))
    {
        OnDeath();
    }
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

void ARunnerCharacter::OnDeath()
{
    if (bIsDead) return;
    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("You Died!"));

    // 移動・入力停止
    GetCharacterMovement()->DisableMovement();
    DisableInput(nullptr);


    // 0.5秒後にレベル再読み込み
    FTimerHandle RestartTimer;
    GetWorldTimerManager().SetTimer(RestartTimer, this, &ARunnerCharacter::RestartLevel, 0.5f, false);
}

void ARunnerCharacter::RestartLevel()
{
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}