// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MachineSnakingGameModeBase.h"
#include "SnakeFood.h"
#include "Engine/World.h"
#include "MachineSnakingGameInstance.h"
#include "SnakeAI.h"
#include "Snake.h"

const int32 WALL_SIZE = 10;

AMachineSnakingGameModeBase::AMachineSnakingGameModeBase()
{
	Width = 200;
	Height = 200;
}

void AMachineSnakingGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (UMachineSnakingGameInstance* GameInstance = GetGameInstance<UMachineSnakingGameInstance>())
	{
		if (GameInstance->IsAIGame())
		{
			SpawnAI();
		}
	}

	SpawnWalls();
	SpawnSnakeFood();
}

void AMachineSnakingGameModeBase::SpawnWalls()
{
	UWorld* World = GetWorld();
	if (!World) return;

	int32 MaxWidth = FMath::RoundToInt(Width / WALL_SIZE);
	int32 MaxHeight = FMath::RoundToInt(Height / WALL_SIZE);

	for (int32 i = 0; i < MaxWidth; ++i)
	{
		World->SpawnActor<AActor>(WallClass, GridIndexToWorldLocation(FIntPoint(i, 0)), FRotator::ZeroRotator);
		World->SpawnActor<AActor>(WallClass, GridIndexToWorldLocation(FIntPoint(i, MaxHeight-1)), FRotator::ZeroRotator);
	}

	for (int32 i = 0; i < MaxHeight; ++i)
	{
		World->SpawnActor<AActor>(WallClass, GridIndexToWorldLocation(FIntPoint(0, i)), FRotator::ZeroRotator);
		World->SpawnActor<AActor>(WallClass, GridIndexToWorldLocation(FIntPoint(MaxWidth-1, i)), FRotator::ZeroRotator);
	}
}

FVector AMachineSnakingGameModeBase::GridIndexToWorldLocation(const FIntPoint& Index) const
{
	return FVector(Index.X * WALL_SIZE * 1.0f, Index.Y * WALL_SIZE * 1.0f, 0.0f);
}

FVector AMachineSnakingGameModeBase::GetRandomInitialLocation() const
{
	int32 MaxWidth = FMath::RoundToInt(Width / WALL_SIZE);
	int32 MaxHeight = FMath::RoundToInt(Height / WALL_SIZE);

	return GridIndexToWorldLocation(FIntPoint(FMath::RandRange(2, MaxWidth - 2), FMath::RandRange(2, MaxHeight - 2)));
}

FVector AMachineSnakingGameModeBase::GetRandomEmptyLocation() const
{
	TArray<FVector> PotentialLocations;

	int32 MaxWidth = FMath::RoundToInt(Width / WALL_SIZE);
	int32 MaxHeight = FMath::RoundToInt(Height / WALL_SIZE);

	UWorld* World = GetWorld();
	if (!World) return FVector();

	for (int32 Index = 0; Index < MaxWidth * MaxHeight; ++Index)
	{
		FIntPoint Position(FMath::FloorToInt(Index * 1.0f / MaxHeight), Index % MaxHeight);
		FVector Location = GridIndexToWorldLocation(Position);

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(OUT Hit, Location - FVector(0.0f, 0.0f, 10.0f), Location + FVector(0.0f, 0.0f, 10.0f),
			ECollisionChannel::ECC_WorldDynamic)) continue;

		PotentialLocations.Add(Location);
	}

	if (!PotentialLocations.Num()) return FVector();
	int32 RandomIndex = FMath::RandRange(0, PotentialLocations.Num() - 1);
	if (!PotentialLocations.IsValidIndex(RandomIndex)) return FVector();

	return PotentialLocations[RandomIndex];
}

void AMachineSnakingGameModeBase::SpawnSnakeFood()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->SpawnActor<ASnakeFood>(SnakeFoodClass, GetRandomEmptyLocation(), FRotator::ZeroRotator);
}

void AMachineSnakingGameModeBase::SpawnAI()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASnakeAI* AI = World->SpawnActor<ASnakeAI>(ASnakeAI::StaticClass(), FTransform::Identity, SpawnParams);
}

void AMachineSnakingGameModeBase::OnGameLost(ASnake* SnakePawn)
{
	if (!SnakePawn) return;
	if (AController* Controller = SnakePawn->GetController())
	{
		Controller->UnPossess();
	}

	if (UMachineSnakingGameInstance* GameInstance = GetGameInstance<UMachineSnakingGameInstance>())
	{
		const EPlayType& PlayType = GameInstance->PlayType;
		if (PlayType == EPlayType::Training)
		{
			const float& Fitness = SnakePawn->GetFitnessLevel();
		}
		else
		{
			BP_OnGameLost();
		}
	}
}