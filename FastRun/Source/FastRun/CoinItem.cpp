// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RunnerCharacter.h"

// Sets default values
ACoinItem::ACoinItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    RootComponent = CollisionComp;
    CollisionComp->InitSphereRadius(100.f);
    CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);

    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ACoinItem::OnOverlap);
}

// Called when the game starts or when spawned
void ACoinItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoinItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    AddActorLocalRotation(FRotator(0.f, RotateSpeed * DeltaTime, 0.f));

}

void ACoinItem::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (ARunnerCharacter* Player = Cast<ARunnerCharacter>(OtherActor))
    {
        // スコア加算処理（Player側に関数を作るのが良い）
        Player->AddScore(1);

        if (PickupSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                PickupSound,
                GetActorLocation()
            );
        }

        Destroy(); // アイテムを消す
    }
}