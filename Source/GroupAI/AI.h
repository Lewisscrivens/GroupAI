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

	UPROPERTY(visibleAnywhere, Category = AIBehaviour)
	class UBlackboardComponent* blackboardComponent;

	UPROPERTY(visibleAnywhere, Category = AIBehaviour)
	class UBehaviorTreeComponent* behaviourTreeComponent;

	UPROPERTY()
	class UAISenseConfig_Sight* sightConfig;

public:

	AAI();

	virtual void Possess(APawn* inPawn) override;

	UFUNCTION(Category = AIBehaviour)
	void OnTargetPerceptionUpdated(class AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(Category = AIBehaviour)
	class AEnemy* GetEnemy();

	UFUNCTION(Category = AIBehaviour)
	void LastSeenLocation();

	UPROPERTY(BlueprintReadWrite)
	bool canSeePlayer;
	AWaypoint* targetWaypoint;
	uint8 ID;
	uint8 moving;
	bool chasingPlayer;
	float runSpeed;
	float walkSpeed;
};
