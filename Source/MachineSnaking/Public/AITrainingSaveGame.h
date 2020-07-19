// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Training.h"
#include "AITrainingSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class MACHINESNAKING_API UAITrainingSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FTrainingData TrainingData;

	UPROPERTY()
	TArray<FGeneration> Generations;
	
};
