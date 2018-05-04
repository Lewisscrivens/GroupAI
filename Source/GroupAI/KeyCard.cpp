// Fill out your copyright notice in the Description page of Project Settings.

#include "KeyCard.h"
#include <ConstructorHelpers.h>
#include <Engine//StaticMesh.h>


AKeyCard::AKeyCard()
{
	// Create key card mesh object.
	keyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("keyMesh"));

	// Get the cube model to be used as the key mesh.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> KeyMeshFile(TEXT("/Game/Assets/World/Props/Cube.Cube"));

	// If the files was obtained successfully the initialize it into a key card shape.
	if (KeyMeshFile.Succeeded())
	{
		keyMesh->SetStaticMesh(KeyMeshFile.Object);
		keyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		keyMesh->SetRelativeScale3D(FVector(0.1f, 0.2f, 0.01f));
		keyMesh->SetSimulatePhysics(true);
	}

}

void AKeyCard::BeginPlay()
{
	Super::BeginPlay();

}

void AKeyCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
