// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyCard.generated.h"

/**
 * 
 */
UCLASS()
class GROUPAI_API AKeyCard : public AActor
{
	GENERATED_BODY()
	
public:

	AKeyCard();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = key)
	class UStaticMeshComponent* keyMesh;
};