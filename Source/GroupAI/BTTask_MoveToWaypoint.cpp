// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToWaypoint.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Enemy.h"
#include "AI.h"
#include "GroupAICharacter.h"
#include <BrainComponent.h>
#include <Engine/Engine.h>
#include <Kismet/GameplayStatics.h>
#include <BehaviorTree/BTNode.h>
#include <Object.h>



EBTNodeResult::Type UBTTask_MoveToWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Return possessed controller.
	AAI* enemy = Cast<AAI>(OwnerComp.GetAIOwner());
	// Fill way points array with all way points in the scene.
	TArray<AActor*> patrolWaypoints = enemy->GetEnemy()->waypointsInScene;
	AWaypoint* targetWaypointPointer = 0;
	int currentWaypoint = 0;
	
	// If the current enemy controller is not null.
	if (enemy)
	{
	// Redo the check if a waypoint was not found.
	RedoLoop:
		// Get a random waypoint within the patrol waypoints index range.
		int random = FMath::RandRange(0, patrolWaypoints.Num() - 1);

		// Used to avoid going to the same waypoint twice.
		if (lastWaypoint == random)
		{
			random--;
		}

		AWaypoint* chosenWaypoint = NULL;
		AWaypoint* waypointCast = NULL;

		// For each waypoint in the patrol way point's array.
		for (AActor* waypointActor : patrolWaypoints)
		{
			// If the current waypoint counter is equal to the randomly chosen one.
			if (random == currentWaypoint)
			{
				// Get the waypoint class.
				waypointCast = Cast<AWaypoint>(waypointActor);

				// Check if the waypoint is being visited if not.
				if (!waypointCast->beingVisited)
				{
					// Set current waypoint as chosen waypoint.
					chosenWaypoint = waypointCast;
					// The waypoint is now being visited.
					waypointCast->beingVisited = true;
					// Set the current waypoint in the controller.
					enemy->targetWaypoint = waypointCast;
					// Set the last waypoint as the currently chosen one for the next time this is ran.
					lastWaypoint = currentWaypoint;

					// Break the for loop as we have found the waypoint.
					break;
				}
			}
			// Keep track of current waypoint index.
			currentWaypoint++;
		}

		// If the chosen waypoint has been set.
		if (chosenWaypoint)
		{
			// Move to the chosen waypoint.
			enemy->MoveToActor(chosenWaypoint, 5.0f, true, true, true, 0, true);
		}
		else // Otherwise redo the search for a random waypoint.
		{
			goto RedoLoop;
		}

		// Return to normal state as we are now returning to the patrol behavior.
		if (enemy)
		{
			enemy->GetEnemy()->chasingPlayer = false;
			enemy->GetEnemy()->GetCharacterMovement()->MaxWalkSpeed = enemy->walkSpeed;
		}

		// Set the player is moving.
		moving = OwnerComp.GetBlackboardComponent()->GetKeyID("Moving");
		bool result = OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(moving, true);

		return EBTNodeResult::Succeeded;
	}

	// If the search did not succeed then return failed.
	return EBTNodeResult::Failed;
}
