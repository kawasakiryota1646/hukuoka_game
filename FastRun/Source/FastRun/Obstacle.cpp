// Fill out your copyright notice in the Description page of Project Settings.

#include "Obstacle.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AObstacle::AObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // メッシュ作成
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = MeshComp;

    // 衝突設定（ぶつかったらHitイベントが発生）
    MeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    MeshComp->SetNotifyRigidBodyCollision(true);   // ← これ重要！
    MeshComp->SetGenerateOverlapEvents(false);

    // タグ追加（死の判定用）
    Tags.Add(FName("Obstacle"));
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

