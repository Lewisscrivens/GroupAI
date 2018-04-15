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
#include "Waypoint.h"
#include <Kismet/GameplayStatics.h>
#include <BehaviorTree/BTNode.h>
#include <Object.h>


EBTNodeResult::Type UBTTask_MoveToWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Return possessed controller.
	AAI* enemy = Cast<AAI>(OwnerComp.GetAIOwner());
	// Fill way points array with all way points in the scene.
	TArray<AActor*> waypointInScene = enemy->GetEnemy()->waypointsInScene;

	AWaypoint* targetWaypointPointer = 0;

	int currentWaypoint = 0;

	// Move to way points 
	if (enemy != NULL)
	{
		int random = FMath::RandRange(0, waypointInScene.Num() - 1);

		for (AActor* waypoint : waypointInScene)
		{
			if (random == currentWaypoint)
			{
				AWaypoint* temp = Cast<AWaypoint>(waypoint);
				
				if (!temp->beingVisited)
				{
					targetWaypointPointer = temp;

					break;
				}
			}

			currentWaypoint++;
		}

		enemy->MoveToActor(targetWaypointPointer, 5.0f, true, true, true, 0, true);
		targetWaypointPointer->beingVisited = true;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Going to " + targetWaypointPointer->GetName()));

		// Set the player is moving.
		moving = OwnerComp.GetBlackboardComponent()->GetKeyID("Moving");
		bool result = OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(moving, true);

		// Initialise the movement variable to false.
		uint8 moveKey = OwnerComp.GetBlackboardComponent()->GetKeyID("Waiting");
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(moveKey, false);

		enemy->targetWaypoint = targetWaypointPointer;

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
