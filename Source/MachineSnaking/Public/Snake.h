// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Snake.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFoodEatenSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveFinishedSignature);

class ASnakePart;
class UMaterial;

UCLASS()
class MACHINESNAKING_API ASnake : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<ASnakePart> SnakePartClass;

	UPROPERTY(EditDefaultsOnly, Category = "Setup", meta = (ClampMin = 0.0f))
	float MoveDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Setup", meta = (ClampMin = 0.05f))
	float UpdateRate;

	TArray<ASnakePart*> SnakeParts;

	FIntPoint MoveDirection;

	FTimerHandle TimerHandle_SnakeMovement;

	int32 FoodEaten;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UMaterial* HeadMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UMaterial* BodyMaterial;

	AActor* Food;

	float Fitness;
	int32 ClosingSteps;
	int32 DistancingSteps;
	float DistanceToFood;

public:
	UPROPERTY()
	FOnFoodEatenSignature OnFoodEaten;

	UPROPERTY()
	FOnMoveFinishedSignature OnMoveFinished;

public:
	ASnake();

	void SetMoveDirection(const FIntPoint& Direction);

	bool CanSetDirection(const FIntPoint& Direction) const;

	float GetFitnessLevel() const;

	TArray<float> GetNNInputs() const;

protected:
	virtual void BeginPlay() override;

private:
	void SetInitialLocation();

	void SpawnHead();

	UFUNCTION()
	void Move();

	void Eat(class ASnakeFood* Food);

	void Grow(const FVector& NewLocation);

	void MoveBody(const FVector& NewLocation);

	void LoseGame();

	bool EnsureFood();

	bool GetDistanceToFood(float& OutDistance);
};
