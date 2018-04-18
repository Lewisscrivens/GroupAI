// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Engine/World.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Actor.h>
#include <Engine/Engine.h>
#include "Door.h"
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BlackboardData.h>
#include <GameFramework/Character.h>
#include <Components/SceneComponent.h>
#include <AIController.h>
#include <NoExportTypes.h>
#include <Engine/EngineTypes.h>
#include <GameFramework/Pawn.h>

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	waitingForDoor = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Fill way points array with all way points in the scene.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypoint::StaticClass(), waypointsInScene);

	enemyController = Cast<AAI>(GetController());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (currentDoor && waitingForDoor)
	{
		if (!currentDoor->closing && !currentDoor->opening && lastWaypoint)
		{
			enemyController->MoveToActor(lastWaypoint, 5.0f, true, true, true, 0, true);
			lastWaypoint = NULL;
			waitingForDoor = false;
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::OpenDoor(ADoor* door)
{
	if (door)
	{
		if (!door->exitDoor)
		{
			currentDoor = door;
			waitingForDoor = true;
			lastWaypoint = enemyController->targetWaypoint;

			enemyController->StopMovement();

			door->Interact(enemyController->GetEnemy()->GetCapsuleComponent()->GetForwardVector());
		}
	}
}