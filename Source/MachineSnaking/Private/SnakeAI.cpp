// Fill out your copyright notice in the Description page of Project Settings.

#include "SnakeAI.h"
#include "Snake.h"
#include "Kismet/GameplayStatics.h"
#include "MachineSnakingGameInstance.h"

void ASnakeAI::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]() 
	{
		Snake = Cast<ASnake>(UGameplayStatics::GetActorOfClass(this, ASnake::StaticClass()));
		if (!Snake) return;

		Possess(Snake);

		Snake->OnMoveFinished.AddDynamic(this, &ASnakeAI::GetNextAIMove);
		GetNextAIMove();
	});

	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
}

FIntPoint ASnakeAI::GetRandomValidDirection() const
{
	if (!Snake) return FIntPoint(0, 0);
	TArray<FIntPoint> PossibleDirections;
	PossibleDirections.Add(FIntPoint(1, 0));
	PossibleDirections.Add(FIntPoint(-1, 0));
	PossibleDirections.Add(FIntPoint(0, 1));
	PossibleDirections.Add(FIntPoint(0, -1));

	PossibleDirections.FilterByPredicate([&](const FIntPoint& Direction) 
	{
		return (Snake) ? Snake->CanSetDirection(Direction) : false;
	});

	if (!PossibleDirections.Num()) return FIntPoint(0, 0);
	int32 RandomIndex = FMath::RandRange(0, PossibleDirections.Num() - 1);
	if (!PossibleDirections.IsValidIndex(RandomIndex)) return FIntPoint(0, 0);
	return PossibleDirections[RandomIndex];
}

void ASnakeAI::GetNextAIMove()
{
	if (!Snake) return;

	if (!Genes.Num())
	{
		Snake->SetMoveDirection(GetRandomValidDirection());
		return;
	}

	// Use genes and Inputs from snake to calculate NN outputs and apply to player
	const TArray<float>& Inputs = Snake->GetNNInputs();
}