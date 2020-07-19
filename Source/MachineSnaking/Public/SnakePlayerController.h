// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SnakePlayerController.generated.h"

class ASnake;

/**
 * 
 */
UCLASS()
class MACHINESNAKING_API ASnakePlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	ASnake* Snake;

public:
	ASnakePlayerController();

	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

	virtual void OnUnPossess() override;

	virtual void SetupInputComponent() override;

private:
	UFUNCTION()
	void MoveUp(float AxisValue);

	UFUNCTION()
	void MoveRight(float AxisValue);
};
