
#include "Door.h"
#include "ConstructorHelpers.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/ActorComponent.h>
#include <AI/Navigation/NavRelevantInterface.h>
#include <AI/Navigation/NavigationSystem.h>
#include <GameFramework/Actor.h>
#include <AI/Navigation/NavigationTypes.h>
#include <Array.h>
#include <Engine/World.h>
#include <GameFramework/Character.h>
#include <Engine/Engine.h>

// Sets default values
ADoor::ADoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the root component for this actor.
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	// Create the door mesh component.
	door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	door->SetupAttachment(RootComponent);

	// Find the default model for the door.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorMesh(TEXT("/Game/Assets/World/Models/DoorMesh.DoorMesh"));

	if (DoorMesh.Succeeded())
	{
		door->SetStaticMesh(DoorMesh.Object);
		door->SetRelativeLocation(FVector(0.0f, 50.0f, -100.0f));
	}

	// Initialize variables.
	closed = true;

	opening = false;
	closing = false;

	dotProduct = 0.0f;
	maxRotation = 0.0f;
	addRotation = 0.0f;
	direction = 0.0f;
	currentRotation = 0.0f;
	closeTime = 0.0f;
	doorStayOpenFor = 15;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the door is opening open.
	if (opening)
	{
		Open(DeltaTime);
	}

	// If the door is closing close.
	if (closing)
	{
		Close(DeltaTime);
	}

	// If the door is open, not closing and has reached the open limit close the door.
	if (closeTime < GetGameTimeSinceCreation() && !closed && !closing)
	{
		opening = false;
		closing = true;
		door->SetCanEverAffectNavigation(false);
	}

}

void ADoor::Open(float DeltaTime)
{
	// The door is now open.
	closed = false;
	// Current rotation is equal to the current door mesh rotation.
	currentRotation = door->RelativeRotation.Yaw;
	addRotation = direction * 2;

	// When the current rotation reaches the current max rotation (90 or -90) end this method loop.
	if (FMath::IsNearlyEqual(currentRotation, maxRotation, 1.5f))
	{
		// Door can now effect navigation when open so the AI does not become stuck.
		door->SetCanEverAffectNavigation(true);
		// The door is now neither opening or closing.
		closing = false;
		opening = false;
	}
	else if (opening)// If the door is still opening then add rotation to the door.
	{
		FRotator NewRotation = FRotator(0.0f, addRotation, 0.0f);
		door->AddRelativeRotation(FQuat(NewRotation), false, 0, ETeleportType::None);
	}
}

void ADoor::Close(float DeltaTime)
{
	currentRotation = door->RelativeRotation.Yaw;
	closed = true;

	//	Set add roation relative to door closing direction.
	if (currentRotation > 0)
	{
		addRotation = -2;
	}
	else
	{
		addRotation = 2;
	}

	// When the current rotation reaches 0 stop closing.
	if (FMath::IsNearlyEqual(currentRotation, 0.0f, 1.5f))
	{
		closing = false;
		opening = false;
	}
	else if (closing)// If the door is still closing add roation to the door mesh.
	{
		FRotator temp = FRotator(0.0f, addRotation, 0.0f);
		door->AddRelativeRotation(FQuat(temp), false, 0, ETeleportType::None);
	}
}

void ADoor::Interact(FVector from)
{
	// Find the vector direction from the player/enemy to the door.
	dotProduct = FVector::DotProduct(root->GetForwardVector(), from);
	// get 1 or -1 from the the dot product for direction
	direction = FMath::Sign(dotProduct);
	// Final rotation after closing/opening.
	maxRotation = direction * 90.0f;

	// If the door is closed open the door.
	if (closed) 
	{
		closing = false;
		opening = true;

		// If the door doesn't need key card make close normally
		if(!needsKeyCard) closeTime = GetGameTimeSinceCreation() + doorStayOpenFor;
		else closeTime = GetGameTimeSinceCreation() + 3.0f;// Otherwise close within 3 seconds.
	}
	else // Otherwise close the door.
	{
		opening = false;	
		closing = true;
	}
}