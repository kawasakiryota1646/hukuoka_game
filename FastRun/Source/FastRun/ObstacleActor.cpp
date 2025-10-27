#include "ObstacleActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
AObstacleActor::AObstacleActor()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;

    // Overlap検出をONにする（これが最重要）
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionObjectType(ECC_WorldStatic);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
    CollisionBox->SetGenerateOverlapEvents(true);

    //メッシュ（見た目用）
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);

    // 見た目のメッシュには衝突判定を持たせない
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // タグ追加（RunnerCharacterがこれを検出）
    Tags.Add(FName("Obstacle"));
}
