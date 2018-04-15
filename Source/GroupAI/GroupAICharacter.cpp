// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

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
	DebugEnabled = true;
	HoldingObject = false;

	// Crouching variables Initialization.
	Crouching = false;
	Crouched = false;
	StandingHeight = 94.0f;
	CrouchedHeight = 42.0f;
	CrouchSpeed = 5.0f;
	CurrentPosition = StandingHeight;

	// Running variables.
	WalkSpeed = 300.0f;
	RunSpeed = 600.0f;
	CrouchedWalkSpeed = 150.0f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	Running = false;

	// Setting the default throw force.
	ThrowForce = 1000.0f;

	// Setting the max grab distance.
	MaxGrabDistance = 250.0f;
	MaxPickupWeight = 100.0f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-29.56f, 1.75f, 64.f); // Change position in editor.
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GrabHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicHandle"));
	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
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

	if (HoldingObject)
	{
		GrabHandle->SetTargetLocation(FirstPersonCameraComponent->GetComponentLocation() + (FirstPersonCameraComponent->GetForwardVector() * 100.0f));
	}

	// Handle crouch or uncrouch animation.
	if (Crouching && Crouched)
	{
		Uncrouch();
	}
	else if (Crouching)
	{
		Crouch();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

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
	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AGroupAICharacter::PickUp);

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
	if (HoldingObject)
	{
		if (DebugEnabled)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Trown Object."));
			}
		}

		UPrimitiveComponent* item = GrabHandle->GetGrabbedComponent();
		GrabHandle->ReleaseComponent();
		item->AddImpulse(FirstPersonCameraComponent->GetForwardVector() * ThrowForce);

		HoldingObject = false;
	}
}

void AGroupAICharacter::PickUp()
{
	if (!HoldingObject)
	{
		if (GEngine && DebugEnabled)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Tried pickup."));
		}
		LineTrace(); // Run a line trace to check if the item can be picked up.
	}
}

// Line trace code, could be useful for something else.
void AGroupAICharacter::LineTrace()
{
	// Camera offset.
	float startDistance = 100.0f;

	FHitResult hit;
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation() + (FirstPersonCameraComponent->GetForwardVector() * startDistance); // Start position from where the player is, + Translation to put the start of the vector infront of the player.
	const FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * MaxGrabDistance);// (fireDistance) units in facing direction in front of the camera.

	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	//Ignore Actors
	TraceParams.AddIgnoredComponent(GetCapsuleComponent());// Ignore the player capsule.

	GetWorld()->LineTraceSingleByChannel(hit, Start, End, ECC_Camera, TraceParams);

	// Show debug lines for line trace.
	if (DebugEnabled)
	{
		FColor lineTraceColour;
		int lineTraceTimeLimit = 30; // Variable for controlling how long the line trace lasts.

									 // On item that can be picked up. Check weight.
		if (hit.bBlockingHit && hit.Component->IsSimulatingPhysics() && hit.Component->GetMass() < MaxPickupWeight)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit"))
				lineTraceColour = FColor::Green;

			// Grab the hit physics object.
			HoldingObject = true;
			GrabHandle->GrabComponent(hit.GetComponent(), hit.BoneName, hit.Location, true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Not hit"))
				lineTraceColour = FColor::Red;
		}

		DrawDebugLine(GetWorld(), hit.TraceStart, hit.TraceEnd, lineTraceColour, false, lineTraceTimeLimit, 0.0f, 1.0f);
	}
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
		Crouching = true;
	}
}

void AGroupAICharacter::Crouch()
{
	if (Running) StopRunning();

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
	if (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() >= CrouchedHeight)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - CrouchSpeed, false);
	}
	else// Otherwise The player is now crouched.
	{
		Crouched = true;
		// Disable jump while crouched.
		GetCharacterMovement()->SetJumpAllowed(false);
		Crouching = false;
		CrouchSpeed = 5.0f;
	}
}

void AGroupAICharacter::Uncrouch()
{
	if (GetCharacterMovement()->MaxWalkSpeed < WalkSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed += 15.0f;
	}
	// Slow the crouch speed down when and after reaching half way.
	if (CrouchSpeed > 1 && GetCapsuleComponent()->GetScaledCapsuleHalfHeight() >= ((StandingHeight + CrouchedHeight) / 2))
	{
		CrouchSpeed -= 0.6f;
	}

	if (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() < StandingHeight)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + CrouchSpeed, false);
	}
	else
	{
		Crouched = false;

		GetCharacterMovement()->SetJumpAllowed(true);

		Crouching = false;
		CrouchSpeed = 5.0f;
	}
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