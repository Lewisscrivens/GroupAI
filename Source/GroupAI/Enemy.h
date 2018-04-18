// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Components/CapsuleComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Engine/EngineTypes.h>
#include <GameFramework/Actor.h>
#include "Door.h"
#include "AI.h"
#include "Waypoint.h"
#include "Enemy.generated.h"

UCLASS()
class GROUPAI_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(EditAnywhere, Category = Behaviour)
	class UBehaviorTree* enemyBehaviour;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void OpenDoor(ADoor* door);

public:

	TArray<AActor*> waypointsInScene;

	UPROPERTY(BlueprintReadOnly)
	ADoor* currentDoor;

	UPROPERTY(BlueprintReadOnly)
	AAI* enemyController;

	UPROPERTY(BlueprintReadOnly)
	bool waitingForDoor;

	UPROPERTY(BlueprintReadOnly)
	AWaypoint* lastWaypoint;
};
