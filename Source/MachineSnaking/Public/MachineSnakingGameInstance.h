// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Training.h"
#include "MachineSnakingGameInstance.generated.h"

class UAITrainingSaveGame;

UENUM(BlueprintType)
enum class EPlayType : uint8
{
	Player,
	Training,
	AI
};

/**
 * 
 */
UCLASS()
class MACHINESNAKING_API UMachineSnakingGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	EPlayType PlayType;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FTrainingData TrainingData;

	FGeneration CurrentGeneration;

	int32 IndividualIndex;

private:
	class UAITrainingSaveGame* SaveGame;

public:
	UMachineSnakingGameInstance();

	void Init() override;

	UFUNCTION(BlueprintCallable)
	void StartTraining();

	FORCEINLINE bool IsAIGame() const { return PlayType == EPlayType::Training || PlayType == EPlayType::AI; }

private:
	void CreateNewTrainingSave();
};
