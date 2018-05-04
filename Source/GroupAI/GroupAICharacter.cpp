#include "GroupAICharacter.h"
#include "GroupAI.h"
#include "GroupAIProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Door.h"
#include <GameFramework/Character.h>
#include <Components/SceneComponent.h>
#include "KeyCard.h"
#include <TransformVectorized.h>
#include <WorldCollision.h>

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGroupAICharacter

AGroupAICharacter::AGroupAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// If true then show the debugging tools in editor.
	DebugEnabled = false;
	HoldingObject = false;

	// Crouching variables Initialization.
	Crouching = false;
	Crouched = false;
	StandingHeight = 80.0f;
	CrouchedHeight = 32.0f;
	CrouchSpeed = 5.0f;
	CurrentPosition = StandingHeight;

	// Objective variables.
	hasKeyCard = false;
	hasHackComputer = false;
	exitOpen = false;

	// Movement variables.
	WalkSpeed = 300.0f;
	RunSpeed = 600.0f;
	CrouchedWalkSpeed = 150.0f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	Running = false;

	// Setting the default throw force.
	ThrowForce = 1000.0f;

	// Setting the max grab distance.
	MaxGrabDistance = 200.0f;
	MaxPickupWeight = 50.0f;

	// By default the player is not hidden.
	hidden = false;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-29.56f, 1.75f, 64.f); // Change position in editor.
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a grab handle to pick up physics objects.
	GrabHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicHandle"));
}

// Constructor from BP.
void AGroupAICharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AGroupAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If holding object is not null set the current grab location to 100 units forward from where the player is looking.
	if (HoldingObject)
	{
		GrabHandle->SetTargetLocation(FirstPersonCameraComponent->GetComponentLocation() + (FirstPersonCameraComponent->GetForwardVector() * 100.0f));
	}

	// Handle crouch or un-crouch animation.
	if (Crouching && Crouched)
	{
		Uncrouch();
	}
	else if (Crouching)
	{
		StartCrouch();
	}
}

// Input.
void AGroupAICharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up game play key bindings
	check(PlayerInputComponent);

	// Bind jump events.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind run event.
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AGroupAICharacter::StartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AGroupAICharacter::StopRunning);

	// Bind player interaction events.
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AGroupAICharacter::Throw);
	//	NOW DONE IN BP	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AGroupAICharacter::PickUp);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGroupAICharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGroupAICharacter::MoveRight);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AGroupAICharacter::CrouchTrigger);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turn rate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGroupAICharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGroupAICharacter::LookUpAtRate);
}

void AGroupAICharacter::Throw()
{
	// Of holding object.
	if (HoldingObject)
	{
		// Print debug message if debug is enabled.
		if (DebugEnabled)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Trown Object."));
			}
		}

		// Get the component attached to the grab handle and add a forward force to it to simulate throwing objects.
		UPrimitiveComponent* item = GrabHandle->GetGrabbedComponent();
		GrabHandle->ReleaseComponent();
		item->AddImpulse(FirstPersonCameraComponent->GetForwardVector() * ThrowForce);
		// No longer holding the object.
		HoldingObject = false;
	}
}

void AGroupAICharacter::PickUp()
{
	// Only allow objects to be picked up if not already holding an object.
	if (!HoldingObject)
	{
		// If Debug is enabled print debug message.
		if (DebugEnabled)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Tried pickup."));
		}
		// Run a line trace to check if the item can be picked up.
		LineTrace(0); 
	}
}

// Line trace, process: 0 = Pickup, 1 = Check for ceiling when crouching.
bool AGroupAICharacter::LineTrace(int process)
{
	// Camera offset.
	float startDistance = 5.0f;

	// Line trace variable initialization.
	FHitResult hit;
	FVector Start;
	FVector End;
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;

	//Ignore Actors
	TraceParams.AddIgnoredComponent(GetCapsuleComponent());

	// Used to trace color in the line trace for debug.
	FColor lineTraceColour = FColor::Red;
	int lineTraceTimeLimit = 30; // Variable for controlling how long the line trace lasts.
	
	// If the process is 0 meaning something is being picked up.
	if (process == 0)
	{
		// Set start to the current camera location and end to a distance forward.
		Start = FirstPersonCameraComponent->GetComponentLocation() + (FirstPersonCameraComponent->GetForwardVector() * startDistance);
		End = Start + (FirstPersonCameraComponent->GetForwardVector() * MaxGrabDistance);
		// Perform a single line trace.
		GetWorld()->LineTraceSingleByChannel(hit, Start, End, ECC_Camera, TraceParams);

		// If the line trace has been blocked by an actor.
		if (hit.bBlockingHit)
		{
			AKeyCard* keyRef = Cast<AKeyCard>(hit.Actor);
			ADoor* doorRef = Cast<ADoor>(hit.Actor);

			// If the hit actor is a key destroy the key and set has keycard to true.
			if (keyRef)
			{
				keyRef->Destroy();
				hasKeyCard = true;
			}
			else if (doorRef) // Otherwise if the hit object is a door.
			{
				if (!doorRef->exitDoor)// The door is not an exit door.
				{
					if (doorRef->needsKeyCard && hasKeyCard) // The door needs a key card which the player has open the door.
					{
						doorRef->Interact(GetFirstPersonCameraComponent()->GetForwardVector());
					}
					else if (!doorRef->needsKeyCard) // The door doesn't need a key card open the door.
					{
						doorRef->Interact(GetFirstPersonCameraComponent()->GetForwardVector());
					}
					// Exit door is handled in the player blueprint.
				}
			}
			// On item that can be picked up. Check weight.
			else if (hit.Component->IsSimulatingPhysics() && hit.Component->GetMass() < MaxPickupWeight && !keyRef)
			{
				// Grab the hit physics object.
				HoldingObject = true;
				GrabHandle->GrabComponentAtLocationWithRotation(hit.GetComponent(), hit.BoneName, hit.Location, FRotator());
			}
			if (DebugEnabled) // If debug is enabled then print the hit component/actors name.
			{
				if (hit.Component != NULL)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, hit.Component->GetName());
				}
				else if (hit.Actor != NULL)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, hit.Actor->GetName());
				}
			}
		}
	}// If process 1 which means if were checking if the player can stand up using a sphere trace.
	else if (process == 1)
	{
		// Start location is the players location and end location is standing height above the players location.
		Start = FVector(GetCapsuleComponent()->GetComponentLocation().X, GetCapsuleComponent()->GetComponentLocation().Y, GetCapsuleComponent()->GetComponentLocation().Z + CrouchedHeight);
		End = FVector(Start.X, Start.Y, Start.Z + StandingHeight);
		GetWorld()->SweepSingleByChannel(hit, Start, End, FQuat(),ECC_Camera, FCollisionShape::MakeSphere(32.0f),TraceParams);
		// If blocked then return false so the player cannot stand.
		if (hit.bBlockingHit)
		{
			return false;
		}
	}
	// Show debug lines for line trace.
	if (DebugEnabled)
	{
		DrawDebugLine(GetWorld(), hit.TraceStart, hit.TraceEnd, lineTraceColour, false, lineTraceTimeLimit, 0.0f, 1.0f);
	}
	// Return true so if it was process one the player can stand.
	return true;
}

void AGroupAICharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGroupAICharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGroupAICharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGroupAICharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGroupAICharacter::CrouchTrigger()
{
	// Only trigger crouch if its not in progress and if the character is not jumping.
	if (!Crouching && !GetCharacterMovement()->IsFalling())
	{
		//If trying to stand check if underneath something using the line trace process 1.
		if (Crouched && LineTrace(1))
		{
			Crouching = true;
		}
		else if (!Crouched)// If standing then crouch.
		{
			Crouching = true;
		}
	}
}

void AGroupAICharacter::StartCrouch()
{
	if (Running) StopRunning();// Stop running when crouched.

	// Reduce walk speed slowly to the crouched walk speed.
	if (GetCharacterMovement()->MaxWalkSpeed > CrouchedWalkSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed -= 15.0f;
	}
	// Slow the crouch speed down when and after reaching half way.
	if (CrouchSpeed > 1 && GetCapsuleComponent()->GetScaledCapsuleHalfHeight() <= ((StandingHeight + CrouchedHeight) / 2))
	{
		CrouchSpeed -= 0.6f;
	}
	// If not yet at crouched height decrease capsule height at a rate of "crouch speed".
	if (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - CrouchSpeed > CrouchedHeight)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - CrouchSpeed, true);
	}
	else// Otherwise The player is now crouched.
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchedHeight, true);
		Crouched = true;
		// Disable jump while crouched.
		GetCharacterMovement()->SetJumpAllowed(false);
		Crouching = false;
		CrouchSpeed = 5.0f;
	}

	GetFirstPersonCameraComponent()->SetRelativeLocation(FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 10.0f));
}

void AGroupAICharacter::Uncrouch()
{
	// Slowly increase speed back to normal walk speed.
	if (GetCharacterMovement()->MaxWalkSpeed < WalkSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed += 15.0f;
	}
	// Slow the crouch speed down when and after reaching half way.
	if (CrouchSpeed > 1 && GetCapsuleComponent()->GetScaledCapsuleHalfHeight() >= ((StandingHeight + CrouchedHeight) / 2))
	{
		CrouchSpeed -= 0.6f;
	}
	// If adding the current crouch speed will not go over the standing height then do so.
	if (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + CrouchSpeed < StandingHeight)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + CrouchSpeed, true);
	}
	else // Otherwise set current height to standing height and finish standing up.
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(StandingHeight, true);
		GetFirstPersonCameraComponent()->SetRelativeLocation(FVector(0.0f, 0.0f, StandingHeight));
		Crouched = false;
		GetCharacterMovement()->SetJumpAllowed(true);
		Crouching = false;
		CrouchSpeed = 5.0f;
	}

	GetFirstPersonCameraComponent()->SetRelativeLocation(FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 10.0f));
}


void AGroupAICharacter::StartRunning()
{
	// Only begin running if the player is not crouching or crouched.
	if (!Crouching && !Crouched)
	{
		Running = true;
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void AGroupAICharacter::StopRunning()
{
	// Choose correct movement speed to move back to in-case somehow the player is running while crouched.
	if (Crouched)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchedWalkSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	Running = false;
}