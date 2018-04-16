#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SceneComponent.h>
#include "Enemy.h"
#include <GameFramework/Character.h>
#include "Door.generated.h"

UCLASS()
class ADoor : public AActor
{
	GENERATED_BODY()

public:
	
	ADoor();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	void RebuildNavMesh();

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Open(float dt);

	UFUNCTION()
	void Close(float dt);

	class USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = Door)
	class UStaticMeshComponent* door;

	UFUNCTION()
	void Interact(FVector from);

	UPROPERTY(BlueprintReadOnly)
	bool opening;
	bool closing;

	UPROPERTY(BlueprintReadOnly)
	bool closed;

	float dotProduct;
	float maxRotation;
	float addRotation;
	float direction;
	float currentRotation;
	float closeTime;
};
