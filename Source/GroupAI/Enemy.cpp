// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Engine/World.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Actor.h>
#include <Engine/Engine.h>
#include "Door.h"
#include <BehaviorTree/BehaviorTree.h>
#include <GameFramework/Character.h>
#include <Components/SceneComponent.h>
#include <AIController.h>
#include <NoExportTypes.h>
#include <Engine/EngineTypes.h>
#include <GameFramework/Pawn.h>
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include <BehaviorTree/BlackboardData.h>
#include "AI.h"
#include <Platform.h>

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	waitingForDoor = false;
	chasingPlayer = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Fill way points array with all way points in the scene.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypoint::StaticClass(), waypointsInScene);

	// Get the current pawns controller.
	enemyController = Cast<AAI>(GetController());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If currentDoor is set and the enemy is waiting for a door.
	if (currentDoor && waitingForDoor)
	{
		// If the current door is not closing or opening resume there move.
		if (!currentDoor->closing && !currentDoor->opening)
		{
			enemyController->ResumeMove(FAIRequestID::CurrentRequest);
			waitingForDoor = false;
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Ran from the MyEnemy blueprint as I needed to handle the collision in bp.
void AEnemy::OpenDoor(ADoor* door)
{
	// If the door is not null and its not an exit door.
	if (door)
	{
		if (!door->exitDoor)
		{
			// Current door equals the current door being opened.
			currentDoor = door;
			// Currently waiting for the door to open/close.
			waitingForDoor = true;

			// Pause the current move while the door opens/closes.
			enemyController->PauseMove(FAIRequestID::CurrentRequest);

			// Run the open door code for the current door.
			door->Interact(enemyController->GetEnemy()->GetCapsuleComponent()->GetForwardVector());
		}
	}
}