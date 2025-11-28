// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinItem.generated.h"

UCLASS()
class FASTRUN_API ACoinItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoinItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 見た目
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* MeshComp;

	//サウンド
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* PickupSound;

	// 当たり判定（オーバーラップ用）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionComp;

	// 回転アニメーション
	UPROPERTY(EditAnywhere, Category = "Movement")
	float RotateSpeed = 100.f;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// プレイヤーが触れたとき
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

};
