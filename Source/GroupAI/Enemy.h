// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Components/CapsuleComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Engine/EngineTypes.h>
#include <GameFramework/Actor.h>
#include <Components/SphereComponent.h>
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

	UPROPERTY(EditAnywhere, Category = Collision)
	class USphereComponent* doorCollider;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void ObsticalHit(class UPrimitiveComponent* hitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector impulse, const FHitResult & hit);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	TArray<AActor*> waypointsInScene;
	
};
