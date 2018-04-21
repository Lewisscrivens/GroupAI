// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_FindPlayer.h"
#include "GroupAI.h"
#include "GroupAICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Enemy.h"
#include <Engine/World.h>
#include <GameFramework/Controller.h>
#include "AI.h"


UBTService_FindPlayer::UBTService_FindPlayer()
{
	bCreateNodeInstance = true;
}

void UBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAI* enemyPlayerController = Cast<AAI>(OwnerComp.GetAIOwner());

	if(enemyPlayerController)
	{
		AGroupAICharacter* player = Cast<AGroupAICharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
		if (player)
		{
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(enemyPlayerController->ID, player);
		}
	}

}
