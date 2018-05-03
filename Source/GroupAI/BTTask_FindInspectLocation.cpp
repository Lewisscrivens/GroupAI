// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindInspectLocation.h"
#include "GroupAICharacter.h"
#include <Engine/World.h>
#include <NameTypes.h>
#include <BrainComponent.h>
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI.h"
#include <AI/Navigation/NavigationSystem.h>


EBTNodeResult::Type UBTTask_FindInspectLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Add random chance that AI will walk next to you as the search location.

	// Ran when lost sight of player so first get the players current location.
	AGroupAICharacter* player = Cast<AGroupAICharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	FVector playerCurrentLocation = player->GetActorLocation();
	
	FVector playersLastLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(FName("PlayersLastLocation"));

	FVector searchLocationBase = FVector(((playersLastLocation.X + playerCurrentLocation.X) / 2), ((playersLastLocation.Y + playerCurrentLocation.Y) / 2), playersLastLocation.Z);
	FNavLocation searchLocation;

	bool result = GetWorld()->GetNavigationSystem()->GetRandomPointInNavigableRadius(searchLocationBase, 5.0f, searchLocation);

	if (result)
	{
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(OwnerComp.GetBlackboardComponent()->GetKeyID("InspectLocation"), searchLocation.Location);
		OwnerComp.GetBlackboardComponent()->ClearValue(OwnerComp.GetBlackboardComponent()->GetKeyID("PlayersLastLocation"));
	}

	return EBTNodeResult::Succeeded;
}
