// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RollingObject.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class FASTRUN_API ARollingObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARollingObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	//親
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	//見た目
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	//当たり判定
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UBoxComponent* Collision;

	//移動速度
	UPROPERTY(EditAnywhere,Category="Movement")
	float MoveSpeed = 1200.f;

	//回転速度
	UPROPERTY(EditAnywhere, Category = "Movement")
	float RollSpeed = 720.f;

	// 動作開始距離
	UPROPERTY(EditAnywhere, Category = "Activation")
	float ActivateDistance = 6000.f;

	// 動作中か
	bool bIsActive = false;

	// プレイヤー参照
	APawn* PlayerPawn;
};
