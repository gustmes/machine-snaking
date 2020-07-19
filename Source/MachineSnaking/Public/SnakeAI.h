// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SnakeAI.generated.h"

/**
 * 
 */
UCLASS()
class MACHINESNAKING_API ASnakeAI : public AAIController
{
	GENERATED_BODY()

private:
	class ASnake* Snake;

	TArray<float> Genes;

public:
	UFUNCTION()
	void GetNextAIMove();

protected:
	virtual void BeginPlay() override;
	
private:
	FIntPoint GetRandomValidDirection() const;
};
