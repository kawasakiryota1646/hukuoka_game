// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveActor.h"

// Sets default values
AMoveActor::AMoveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Mesh コンポーネント作成
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMesh.Succeeded())
    {
        MeshComp->SetStaticMesh(CubeMesh.Object);
        MeshComp->SetWorldScale3D(FVector(1.f, 1.f, 1.f)); // 横長にしてプラットフォームっぽく
    }
}

// Called when the game starts or when spawned
void AMoveActor::BeginPlay()
{
	Super::BeginPlay();
	
    if (PlatformMesh)
    {
        MeshComp->SetStaticMesh(PlatformMesh);
    }
}

// Called every frame
void AMoveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    FVector Location = GetActorLocation();

    // 移動
    Location.X += MoveSpeed * Direction * DeltaTime;

    // 範囲チェック
    if (Location.X > MaxX)
    {
        Location.X = MaxX;
        Direction = -1; // 左に戻る
    }
    else if (Location.X < MinX)
    {
        Location.X = MinX;
        Direction = 1; // 右に戻る
    }

    SetActorLocation(Location);
}

