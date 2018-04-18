// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Waypoint.h"
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

public:

	AAI();

	virtual void Possess(APawn* inPawn) override;

	UFUNCTION(BlueprintCallable)
	void InspectLocation(FVector door);

	class AEnemy* GetEnemy();
	AWaypoint* targetWaypoint;
	uint8 ID;
	uint8 moving;
};
