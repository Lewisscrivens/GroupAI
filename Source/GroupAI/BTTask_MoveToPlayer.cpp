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
	// Get the current AIController from the owner.
	AAI* enemy = Cast<AAI>(OwnerComp.GetAIOwner());
	// Get the current player pawn.
	AGroupAICharacter* player = Cast<AGroupAICharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	// If the player was successfully obtained move to its location.
	if (player)
	{
		enemy->MoveToActor(player, 5.0f, true, true, true, 0, true);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
