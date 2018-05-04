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
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>


EBTNodeResult::Type UBTTask_EndInspect::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Cast to the AI controller type using this tasks AI owner.
	AAI* enemy = Cast<AAI>(OwnerComp.GetAIOwner());

	// Set the inspect value to false in the owning controllers blackboard to end the inspection behavior.
	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(OwnerComp.GetBlackboardComponent()->GetKeyID("Inspect"), false);

	// After inspection clear the players last location as this code wouldn't be ran if the AI was looking for the player.
	OwnerComp.GetBlackboardComponent()->ClearValue(FName("PlayersLastLocation"));
	
	// Cast to he enemy pawn class to stop chasing the player as it will return to patrol behavior.
	AEnemy* pawn = Cast<AEnemy>(enemy->GetPawn());
	pawn->chasingPlayer = false;

	return EBTNodeResult::Succeeded;
}
