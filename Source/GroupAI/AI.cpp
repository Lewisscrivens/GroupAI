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
	// Setup the blackboard component.
	blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	// Setup the behavior tree component.
	behaviourTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviourComponent"));

	// Setup the perception component.
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	PerceptionComponent->ConfigureSense(*sightConfig);
	PerceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAI::OnTargetPerceptionUpdated);

	// Initialize variables.
	targetWaypoint = 0;
	canSeePlayer = false;
	chasingPlayer = false;
}

void AAI::Possess(APawn* inPawn)
{
	Super::Possess(inPawn);

	// Get the enemy pawn.
	AEnemy* enemy = GetEnemy();

	// If both the enemy pawn and its behavior tree is not null.
	if (enemy && enemy->enemyBehaviour)
	{
		// Get the blackboard from the current pawns behaviour tree.
		blackboardComponent->InitializeBlackboard(*enemy->enemyBehaviour->BlackboardAsset);
		// Set the current ID (target key).
		ID = blackboardComponent->GetKeyID("Target");
		// Start the behaviour tree.
		behaviourTreeComponent->StartTree(*enemy->enemyBehaviour); 
	}

	// If the perception component is not null initialize its variables.
	if (PerceptionComponent)
	{
		sightConfig->SightRadius = 1300.0f;
		sightConfig->LoseSightRadius = 1500.0f;
		sightConfig->PeripheralVisionAngleDegrees = 90.0f;
		sightConfig->DetectionByAffiliation.bDetectEnemies = true;
		sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		PerceptionComponent->ConfigureSense(*sightConfig);
	}

	// Set the current walk speed to the starting max walk speed in this pawns movement component.
	walkSpeed = GetEnemy()->GetCharacterMovement()->MaxWalkSpeed;
	runSpeed = walkSpeed * 3;
}

// Ran when the AI perceives an actor (Sight or loss of sight).
void AAI::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// If the actor is the player then create a pointer to the player.
	AGroupAICharacter* player = Cast<AGroupAICharacter>(Actor);
	// Get the canSeePlayer variable key from this controllers blackboard.
	uint8 canSeePlayerKey = blackboardComponent->GetKeyID("CanSeePlayer");

	// If the perceived actor was the player (player is not null).
	if (player)
	{
		// Also if the player is not hidden then he can be perceived.
		if (!player->hidden)
		{
			// If gained sight of player start chasing the player and increase movement speed.
			if (Stimulus.IsActive())
			{
				GetEnemy()->GetCharacterMovement()->MaxWalkSpeed = runSpeed;
				canSeePlayer = true;
				GetEnemy()->chasingPlayer = true;
			}
			else // Otherwise if lost sight of player.
			{
				canSeePlayer = false;
				// Find last seen location and set it in blackboard.
				LastSeenLocation();
				// Stop the current movement in the character movement component.
				StopMovement();	
			}
		}
		else // If the player is hidden then the controller cannot see the player.
		{
			canSeePlayer = false;
		}
		// Depending on what the can see player value was set it in this controllers blackboard.
		blackboardComponent->SetValue<UBlackboardKeyType_Bool>(canSeePlayerKey, canSeePlayer);
	}
}

// Return the current pawn for this controller as type Enemy.cpp.
AEnemy* AAI::GetEnemy()
{
	return Cast<AEnemy>(GetPawn());
}

void AAI::LastSeenLocation()
{
	// Ran when lost sight of player so first get the players current location.
	AGroupAICharacter* player = Cast<AGroupAICharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	// If the player cast succeeded.
	if (player)
	{
		// Get current location of the player.
		FVector playersLastLocation = player->GetActorLocation();
		// Set the obtained location in the blackboard of this controller.
		blackboardComponent->SetValue<UBlackboardKeyType_Vector>(blackboardComponent->GetKeyID("PlayersLastLocation"), playersLastLocation);
	}
}

// Reset the moving value in the blackboard.
void AAI::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	// Get moving key from the blackboard.
	moving = blackboardComponent->GetKeyID("Moving");
	// If the enemy is not chasing the player set moving to false. (This was a quick bug fix due to how the behavior tree works)
	if (!chasingPlayer) blackboardComponent->SetValue<UBlackboardKeyType_Bool>(moving, false);
	// Set the inspect variable to true after a move is completed. (Inspect the current area after each movement)
	blackboardComponent->SetValue<UBlackboardKeyType_Bool>(blackboardComponent->GetKeyID("Inspect"), true);
}