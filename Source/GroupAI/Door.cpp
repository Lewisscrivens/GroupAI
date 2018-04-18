
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

// Sets default values
ADoor::ADoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	door->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorMesh(TEXT("/Game/Assets/World/Models/DoorMesh.DoorMesh"));

	if (DoorMesh.Succeeded())
	{
		door->SetStaticMesh(DoorMesh.Object);
		door->SetRelativeLocation(FVector(0.0f, 50.0f, -100.0f));
	}

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

	if (opening)
	{
		Open(DeltaTime);
	}

	if (closing)
	{
		Close(DeltaTime);
	}

	if (closeTime < GetGameTimeSinceCreation() && !closed && !closing)
	{
		opening = false;
		closing = true;
	}

}

void ADoor::Open(float deltaTime)
{
	closed = false;

	currentRotation = door->RelativeRotation.Yaw;
	addRotation = direction * deltaTime * 100;

	if (FMath::IsNearlyEqual(currentRotation, maxRotation, 1.5f))
	{
		door->SetCanEverAffectNavigation(true);

		closing = false;
		opening = false;
	}
	else if (opening)
	{
		FRotator NewRotation = FRotator(0.0f, addRotation, 0.0f);
		door->AddRelativeRotation(FQuat(NewRotation), false, 0, ETeleportType::None);
	}
}

void ADoor::Close(float deltaTime)
{
	currentRotation = door->RelativeRotation.Yaw;
	closed = true;

	if (currentRotation > 0)
	{
		addRotation = -deltaTime * 80;
	}
	else
	{
		addRotation = deltaTime * 80;
	}

	if (FMath::IsNearlyEqual(currentRotation, 0.0f, 1.5f))
	{
		door->SetCanEverAffectNavigation(false);

		closing = false;
		opening = false;
	}
	else if (closing)
	{
		FRotator temp = FRotator(0.0f, addRotation, 0.0f);
		door->AddRelativeRotation(FQuat(temp), false, 0, ETeleportType::None);
	}
}

void ADoor::Interact(FVector from)
{
	dotProduct = FVector::DotProduct(root->GetForwardVector(), from);

	// get 1 or -1 from the the dot product for direction
	direction = FMath::Sign(dotProduct);

	// Final rotation after closing/opening.
	maxRotation = direction * 90.0f;

	if (closed) 
	{
		closing = false;
		opening = true;

		closeTime = GetGameTimeSinceCreation() + doorStayOpenFor;
	}
	else 
	{
		opening = false;	
		closing = true;
	}
}