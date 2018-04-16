// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include <BehaviorTree/BehaviorTreeTypes.h>
#include "Waypoint.h"
#include "BTTask_MoveToWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class GROUPAI_API UBTTask_MoveToWaypoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	int lastWaypoint;

	uint8 moving;
};
