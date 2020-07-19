// Fill out your copyright notice in the Description page of Project Settings.

#include "MachineSnakingGameInstance.h"
#include "AITrainingSaveGame.h"
#include "Kismet/GameplayStatics.h"

const FString SAVE_SLOT = FString("Slot");

UMachineSnakingGameInstance::UMachineSnakingGameInstance()
{
	TArray<int32> HiddenLayers;
	HiddenLayers.Add(5);

	TrainingData = FTrainingData(50, 5, 4, HiddenLayers, 3);
}

void UMachineSnakingGameInstance::Init()
{
	Super::Init();
}

void UMachineSnakingGameInstance::StartTraining()
{
	SaveGame = Cast<UAITrainingSaveGame>(UGameplayStatics::LoadGameFromSlot(SAVE_SLOT, 0));
	if (!SaveGame || SaveGame->TrainingData != TrainingData)
	{
		CreateNewTrainingSave();
	}

	if (!SaveGame) return;
	CurrentGeneration = SaveGame->Generations.Last();
}

void UMachineSnakingGameInstance::CreateNewTrainingSave()
{
	// If there are no data to be loaded, create a new save game object.
	SaveGame = Cast<UAITrainingSaveGame>(UGameplayStatics::CreateSaveGameObject(UAITrainingSaveGame::StaticClass()));
	if (!SaveGame) return;
	SaveGame->TrainingData = TrainingData;

	// Start the first generation with random chromossomes.
	FGeneration GenerationZero;
	for (int32 i = 0; i < TrainingData.PopulationSize; ++i)
	{
		GenerationZero.Individuals.Add(FIndividual(TrainingData.NumGenes));
	}
	SaveGame->Generations.Add(GenerationZero);

	UGameplayStatics::SaveGameToSlot(SaveGame, SAVE_SLOT, 0);
}