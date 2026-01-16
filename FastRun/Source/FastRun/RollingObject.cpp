// Fill out your copyright notice in the Description page of Project Settings.


#include "RollingObject.h"
#include"Components/StaticMeshComponent.h"
#include"Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARollingObject::ARollingObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Root);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));


	Tags.Add("Obstacle");

}

// Called when the game starts or when spawned
void ARollingObject::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

}

// Called every frame
void ARollingObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerPawn) return;

	// まだ動いてないなら距離チェック
	if (!bIsActive)
	{
		float Dist = FVector::Dist(
			GetActorLocation(),
			PlayerPawn->GetActorLocation()
		);

		if (Dist < ActivateDistance)
		{
			bIsActive = true;
		}
		else
		{
			return; 
		}
	}
	//前進
	FVector Move = FVector(-MoveSpeed * DeltaTime, 0.f, 0.f);
	AddActorWorldOffset(Move, true);

	//回転
	FRotator Roll = FRotator(0.f, RollSpeed * DeltaTime, 0.f);
	Mesh->AddLocalRotation(Roll);

}

