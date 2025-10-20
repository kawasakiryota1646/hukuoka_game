// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"



// Sets default values
ARunnerCharacter::ARunnerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // --- カメラ設定を追加 ---
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 500.f; // キャラとカメラの距離
    SpringArmComp->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f)); // 少し上から見下ろす
    SpringArmComp->bUsePawnControlRotation = false;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    GetCharacterMovement()->JumpZVelocity = 1200.f;   // ジャンプの高さ
    GetCharacterMovement()->AirControl = 0.5f;       // 空中で左右に動ける度合い
    GetCharacterMovement()->GravityScale = 2.0f;     // 重力の強さ（1.0が標準）
    GetCharacterMovement()->MaxWalkSpeed = 600.f;    // 地上での最大移動速度
}

// Called when the game starts or when spawned

// Called every frame
void ARunnerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 常に前方向に移動
    AddMovementInput(GetActorForwardVector(), ForwardSpeed * DeltaTime);

    // 左右レーン移動処理
    UpdateLaneMovement(DeltaTime);
}

// Called to bind functionality to input
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
    if (CurrentLane > -1)
    {
        CurrentLane--;
        TargetLocation = GetActorLocation() + FVector(0, -LaneOffset, 0);
    }
}

void ARunnerCharacter::MoveRight()
{
    if (CurrentLane < 1)
    {
        CurrentLane++;
        TargetLocation = GetActorLocation() + FVector(0, LaneOffset, 0);
    }
}

void ARunnerCharacter::Jump()
{
    Super::Jump(); 

}

void ARunnerCharacter::UpdateLaneMovement(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    FVector DesiredLocation = FVector(CurrentLocation.X, CurrentLane * LaneOffset, CurrentLocation.Z);
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, DesiredLocation, DeltaTime, 10.f);
    SetActorLocation(NewLocation);
}