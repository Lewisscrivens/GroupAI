// Fill out your copyright notice in the Description page of Project Settings.

#include "AI.h"
#include "GroupAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include <BehaviorTree/BlackboardData.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Bool.h>
#include <GameFramework/Controller.h>
#include "Enemy.h"



AAI::AAI()
{
	blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	behaviourTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviourComponent"));

	targetWaypoint = 0;
}

void AAI::Possess(APawn* inPawn)
{
	Super::Possess(inPawn);

	AEnemy* enemy = Cast<AEnemy>(inPawn);

	if (enemy && enemy->enemyBehaviour)
	{
		blackboardComponent->InitializeBlackboard(*enemy->enemyBehaviour->BlackboardAsset);
	
		ID = blackboardComponent->GetKeyID("Target");

		behaviourTreeComponent->StartTree(*enemy->enemyBehaviour); 
	}
}

void AAI::InspectLocation(FVector door)
{
	StopMovement();
	MoveToLocation(door);
}

AEnemy* AAI::GetEnemy()
{
	return Cast<AEnemy>(GetPawn());
}

// Reset the moving value in the blackboard.
void AAI::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (!GetEnemy()->waitingForDoor)
	{
		targetWaypoint->beingVisited = false;

		moving = blackboardComponent->GetKeyID("Moving");
		bool result = blackboardComponent->SetValue<UBlackboardKeyType_Bool>(moving, false);
	}
}