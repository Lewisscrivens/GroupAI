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

	// Move to way points 
	if (enemy)
	{
		int random = FMath::RandRange(0, patrolWaypoints.Num() - 1);

		// Used to avoid going to the same waypoint twice.
		if (lastWaypoint == random)
		{
			random--;
		}

		AWaypoint* chosenWaypoint = NULL;
		AWaypoint* waypointCast = NULL;

	RedoLoop:
		for (AActor* waypointActor : patrolWaypoints)
		{
			if (random == currentWaypoint)
			{
				waypointCast = Cast<AWaypoint>(waypointActor);

				if (!waypointCast->beingVisited)
				{
					chosenWaypoint = waypointCast;
					waypointCast->beingVisited = true;
					enemy->targetWaypoint = waypointCast;
					lastWaypoint = currentWaypoint;

					break;
				}
			}
			currentWaypoint++;
		}

		if (chosenWaypoint)
		{
			enemy->MoveToActor(chosenWaypoint, 5.0f, true, true, true, 0, true);
		}
		else
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

	return EBTNodeResult::Failed;
}
