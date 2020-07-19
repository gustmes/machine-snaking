// Fill out your copyright notice in the Description page of Project Settings.

#include "SnakePlayerController.h"
#include "Snake.h"
#include "MachineSnakingGameInstance.h"
#include "GameFramework/PlayerState.h"

ASnakePlayerController::ASnakePlayerController()
{
	bAutoManageActiveCameraTarget = false;
}

void ASnakePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Snake = Cast<ASnake>(InPawn);
}

void ASnakePlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	if (UMachineSnakingGameInstance* GameInstance = GetGameInstance<UMachineSnakingGameInstance>())
	{
		if (PlayerState)
		{
			PlayerState->bOnlySpectator = GameInstance->IsAIGame();
		}
	}
}

void ASnakePlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	Snake = nullptr;
}

void ASnakePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent) return;
	InputComponent->BindAxis("MoveUp", this, &ASnakePlayerController::MoveUp);
	InputComponent->BindAxis("MoveRight", this, &ASnakePlayerController::MoveRight);
}

void ASnakePlayerController::MoveUp(float AxisValue)
{
	if (!Snake) return;
	if (FMath::Abs(AxisValue) < 0.1f) return;

	int32 UpDirection = (AxisValue > 0.0f) ? -1 : 1;
	Snake->SetMoveDirection(FIntPoint(0, UpDirection));
}

void ASnakePlayerController::MoveRight(float AxisValue)
{
	if (!Snake) return;
	if (FMath::Abs(AxisValue) < 0.1f) return;

	int32 RightDirection = (AxisValue > 0.0f) ? 1 : -1;
	Snake->SetMoveDirection(FIntPoint(RightDirection, 0));
}