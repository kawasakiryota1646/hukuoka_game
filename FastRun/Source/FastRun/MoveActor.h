// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MoveActor.generated.h"

UCLASS()
class FASTRUN_API AMoveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoveActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // 動くスピード
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MoveSpeed = 200.f;

    // 左右方向の移動範囲（X座標）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MinX = -300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxX = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    UStaticMesh* PlatformMesh;

private:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;

    // 現在の移動方向（1 = 右、-1 = 左）
    int32 Direction = 1;
};
