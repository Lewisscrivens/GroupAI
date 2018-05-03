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

	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(OwnerComp.GetBlackboardComponent()->GetKeyID("Inspect"), false);

	// After inspection clear the players last location as this code wouldnt be ran if the AI was looking for the player.
	OwnerComp.GetBlackboardComponent()->ClearValue(FName("PlayersLastLocation"));
	
	AEnemy* pawn = Cast<AEnemy>(enemy->GetPawn());
	pawn->chasingPlayer = false;

	return EBTNodeResult::Succeeded;
}
