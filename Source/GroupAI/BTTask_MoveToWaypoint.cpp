// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToWaypoint.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
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

		for (AActor* waypointActor : patrolWaypoints)
		{
			if (random == currentWaypoint)
			{
				AWaypoint* waypoint = Cast<AWaypoint>(waypointActor);

				if (!waypoint->beingVisited)
				{
					targetWaypointPointer = waypoint;
					lastWaypoint = currentWaypoint;

					break;
				}
			}
			currentWaypoint++;
		}

		enemy->patrol = enemy->MoveToActor(targetWaypointPointer, 5.0f, true, true, true, 0, true);
		targetWaypointPointer->beingVisited = true;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Going to " + targetWaypointPointer->GetName()));

		// Set the player is moving.
		moving = OwnerComp.GetBlackboardComponent()->GetKeyID("Moving");
		bool result = OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(moving, true);

		enemy->targetWaypoint = targetWaypointPointer;

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
