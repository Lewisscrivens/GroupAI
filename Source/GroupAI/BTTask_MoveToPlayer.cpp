// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToPlayer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Enemy.h"
#include "AI.h"
#include "GroupAICharacter.h"
#include <BrainComponent.h>


EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAI* enemy = Cast<AAI>(OwnerComp.GetAIOwner());
	AGroupAICharacter* player = Cast<AGroupAICharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (player)
	{
		enemy->MoveToActor(player, 5.0f, true, true, true, 0, true);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
