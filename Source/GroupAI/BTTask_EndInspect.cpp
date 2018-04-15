// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_EndInspect.h"
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


EBTNodeResult::Type UBTTask_EndInspect::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAI* enemy = Cast<AAI>(OwnerComp.GetAIOwner());
	uint8 key = OwnerComp.GetBlackboardComponent()->GetKeyID("LocationOnHold");
	AWaypoint* locationOnHold = Cast<AWaypoint>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(key));

	enemy->MoveToActor(locationOnHold, 5.0f, true, true, true, 0, true);

	return EBTNodeResult::Succeeded;
}
