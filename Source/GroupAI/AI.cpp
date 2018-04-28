// Fill out your copyright notice in the Description page of Project Settings.

#include "AI.h"
#include "GroupAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include <BehaviorTree/BlackboardData.h>
#include <GameFramework/Controller.h>
#include <GameFramework/Actor.h>
#include "Perception/AISense.h"
#include "Perception/AIPerceptionSystem.h"
#include <Perception/AIPerceptionComponent.h>
#include <Perception/AIPerceptionTypes.h>
#include <Perception/AISense_Sight.h>
#include <Perception/AISenseConfig_Sight.h>
#include <Engine/Engine.h>
#include "Enemy.h"
#include "GroupAICharacter.h"


AAI::AAI()
{
	blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	behaviourTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviourComponent"));

	// Setup the perception component
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	PerceptionComponent->ConfigureSense(*sightConfig);
	PerceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAI::OnTargetPerceptionUpdated);

	targetWaypoint = 0;
	canSeePlayer = false;
	chasingPlayer = false;
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

	if (PerceptionComponent)
	{
		sightConfig->SightRadius = 1300.0f;
		sightConfig->LoseSightRadius = 1500.0f;
		sightConfig->PeripheralVisionAngleDegrees = 100.0f;
		sightConfig->DetectionByAffiliation.bDetectEnemies = true;
		sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		PerceptionComponent->ConfigureSense(*sightConfig);
	}

	walkSpeed = GetEnemy()->GetCharacterMovement()->MaxWalkSpeed;
	runSpeed = walkSpeed * 3;
}

void AAI::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	AGroupAICharacter* player = Cast<AGroupAICharacter>(Actor);
	uint8 canSeePlayerKey = blackboardComponent->GetKeyID("CanSeePlayer");

	if (player)
	{
		if (Stimulus.IsActive() && !player->hidden)
		{
			GetEnemy()->GetCharacterMovement()->MaxWalkSpeed = runSpeed;
			canSeePlayer = true;
			GetEnemy()->chasingPlayer = true;

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Can See you."));
		}
		else
		{
			GetEnemy()->GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
			canSeePlayer = false;
			// Set movement to false so that another waypoint is found. (Return to patrol behaviour.)
			blackboardComponent->SetValue<UBlackboardKeyType_Bool>(blackboardComponent->GetKeyID("Moving"), false);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Lost you."));
		}

		blackboardComponent->SetValue<UBlackboardKeyType_Bool>(canSeePlayerKey, canSeePlayer);
	}
}

AEnemy* AAI::GetEnemy()
{
	return Cast<AEnemy>(GetPawn());
}

// Reset the moving value in the blackboard.
void AAI::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (!GetEnemy()->chasingPlayer)
	{
		moving = blackboardComponent->GetKeyID("Moving");
		targetWaypoint->beingVisited = false;
		GetEnemy()->waypointsInScene.Remove(targetWaypoint);
		blackboardComponent->SetValue<UBlackboardKeyType_Bool>(moving, false);
		blackboardComponent->SetValue<UBlackboardKeyType_Bool>(blackboardComponent->GetKeyID("Inspect"), true);
	}
}