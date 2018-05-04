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
	// Ran when lost sight of player so first get the players current location.
	AGroupAICharacter* player = Cast<AGroupAICharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	FVector playerCurrentLocation = player->GetActorLocation();
	
	// Get the players last location from the blackboard to figure out a search location.
	FVector playersLastLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(FName("PlayersLastLocation"));

	// Find a base search location in between the current players location and the last seen location.
	FVector searchLocationBase = FVector(((playersLastLocation.X + playerCurrentLocation.X) / 2), ((playersLastLocation.Y + playerCurrentLocation.Y) / 2), playersLastLocation.Z);
	FNavLocation searchLocation;

	// Using the search location base get a random point within 5 units.
	bool result = GetWorld()->GetNavigationSystem()->GetRandomPointInNavigableRadius(searchLocationBase, 5.0f, searchLocation);

	// If a search point was successfully generated then set the corresponding inspect location in the owning controllers blackboard.
	if (result)
	{
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(OwnerComp.GetBlackboardComponent()->GetKeyID("InspectLocation"), searchLocation.Location);
		// Clear the players last location value from the current owning controllers blackboard as it is no longer needed.
		OwnerComp.GetBlackboardComponent()->ClearValue(OwnerComp.GetBlackboardComponent()->GetKeyID("PlayersLastLocation"));
	}

	return EBTNodeResult::Succeeded;
}
