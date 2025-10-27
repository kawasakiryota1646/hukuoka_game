#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleActor.generated.h"

UCLASS()
class FASTRUN_API AObstacleActor : public AActor
{
    GENERATED_BODY()

public:
    AObstacleActor();

protected:
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionBox;

    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* MeshComp;
};
