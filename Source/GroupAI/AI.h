// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Waypoint.h"
#include <Perception/AIPerceptionTypes.h>
#include "AI.generated.h"

/**
 * 
 */
UCLASS()
class GROUPAI_API AAI : public AAIController
{
	GENERATED_BODY()
	
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY()
	class UBlackboardComponent* blackboardComponent;

	UPROPERTY()
	class UBehaviorTreeComponent* behaviourTreeComponent;

	UPROPERTY()
	class UAISenseConfig_Sight* sightConfig;

public:

	AAI();

	virtual void Possess(APawn* inPawn) override;

	UFUNCTION()
	void OnTargetPerceptionUpdated(class AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	class AEnemy* GetEnemy();

	AWaypoint* targetWaypoint;
	uint8 ID;
	uint8 moving;

	FAIRequestID patrol;
	FAIRequestID chase;

private:

	float runSpeed;
	float walkSpeed;
	bool canSeePlayer;
	bool chasingPlayer;
};
