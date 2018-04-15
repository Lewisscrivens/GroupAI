// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_Inspect.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Enemy.h"
#include "AI.h"
#include "GroupAICharacter.h"
#include <BrainComponent.h>
#include "Waypoint.h"
#include <Kismet/GameplayStatics.h>
#include <AITypes.h>


EBTNodeResult::Type UBTTask_Inspect::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAI* enemy = Cast<AAI>(OwnerComp.GetAIOwner());
	uint8 moveKey = OwnerComp.GetBlackboardComponent()->GetKeyID("LocationOnHold");
	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(moveKey, enemy->targetWaypoint);

	enemy->StopMovement();

	return EBTNodeResult::Succeeded;
}
