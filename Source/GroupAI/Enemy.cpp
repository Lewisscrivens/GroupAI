// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Waypoint.h"
#include "Engine/World.h"
#include "AI.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Actor.h>
#include <Engine/Engine.h>
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BlackboardData.h>
#include <GameFramework/Character.h>
#include <Components/SceneComponent.h>
#include <AIController.h>
#include <NoExportTypes.h>
#include <Engine/EngineTypes.h>

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Fill way points array with all way points in the scene.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypoint::StaticClass(), waypointsInScene);

	//doorCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//doorCollider->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	//Mirror the method on component hit to detect when a door is in the way.
	//doorCollider->OnComponentHit.AddDynamic(this, &AEnemy::ObsticalHit);
}

/*void AEnemy::ObsticalHit(class UPrimitiveComponent* hitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector impulse, const FHitResult & hit)
//(class UPrimitiveComponent* primComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ADoor* door = Cast<ADoor>(OtherActor);

	if (door)
	{
		door->Interact(GetCapsuleComponent()->GetForwardVector());
	}
}*/

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::OpenDoor(ADoor* door)
{
	door->Interact(GetCapsuleComponent()->GetForwardVector());

	
}
