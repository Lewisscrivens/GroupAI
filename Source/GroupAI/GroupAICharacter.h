// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Components/CapsuleComponent.h>
#include <ObjectMacros.h>
#include "GroupAICharacter.generated.h"


class UInputComponent;

UCLASS(config = Game)
class AGroupAICharacter : public ACharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = PhysicHandle)
	class UPhysicsHandleComponent* GrabHandle;

public:
	AGroupAICharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Used to show debug lines for ray traces etc. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		bool DebugEnabled;

	/** Does the physics handle grab handle have an attatched physics object. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		bool HoldingObject;

	/** Is crouching or uncrouching currently taking place. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		bool Crouching;

	/** Is the characyer crouched */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		bool Crouched;

	/** Max distance physics objects can be picked up. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float MaxGrabDistance;

	/** Max pickup weight for a physics object. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float MaxPickupWeight;

	/** Force to add to the physics object attatched to the physics handle grab handle. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float ThrowForce;

	/** Standing capsule height. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float StandingHeight;

	/** Crouching capsule height. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float CrouchedHeight;

	/** Speed of the crouch/uncrouch animation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float CrouchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float CurrentPosition;

	/** Walk speed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float WalkSpeed;

	/** Crouched walk speed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float CrouchedWalkSpeed;

	/** Run speed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		float RunSpeed;

	/** Running?. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
		bool Running;

	/** Has Key card?. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	bool hasKeyCard;

	/** Has Key card?. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	bool hasHackComputer;

	/** Is the player hidden. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	bool hidden;

	/** Is the player hidden. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	bool exitOpen;

	/** Pickup a projectile from the gamescene. */
	UFUNCTION(BlueprintCallable)
	void PickUp();

protected:

	/** Throw a projectile picked up from the gamescene. */
	void Throw();

	/** Line trace from camera + start point to the end point. */
	bool LineTrace(int process);

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/** Pickup a projectile from the gamescene. */
	void CrouchTrigger();
	void StartCrouch();
	void Uncrouch();

	void StartRunning();
	void StopRunning();

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

