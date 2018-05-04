// Not currently in use code moved to myenemy blueprint.

#include "BTTask_Radio.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Enemy.h"
#include "AI.h"
#include <Kismet/GameplayStatics.h>


EBTNodeResult::Type UBTTask_Radio::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	bool goToPlayer = false;
	TArray<AActor*> FoundAIControllers;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundAIControllers);

	for (AActor* controller : FoundAIControllers)
	{
		AEnemy* enemyPawn = Cast<AEnemy>(controller);
		AAI* controllerCast = Cast<AAI>(enemyPawn);
		if (controllerCast && controllerCast->canSeePlayer)
		{
			goToPlayer = true;
		}
	}

	for (AActor* controller : FoundAIControllers)
	{
		AEnemy* enemyPawn = Cast<AEnemy>(controller);
		AAI* controllerCast = Cast<AAI>(enemyPawn);

		if (goToPlayer)
		{
			controllerCast->GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(controllerCast->GetBlackboardComponent()->GetKeyID("CanSeePlayer"), true);
		}
	}

	return EBTNodeResult::Succeeded;
}
