// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Training.h"
#include "MachineSnakingGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MACHINESNAKING_API AMachineSnakingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (ClampMin = 1, AllowPrivateAccess = "true"))
	int32 Width;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (ClampMin = 1, AllowPrivateAccess = "true"))
	int32 Height;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<AActor> WallClass;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<class ASnakeFood> SnakeFoodClass;

public:
	AMachineSnakingGameModeBase();

	FVector GetRandomInitialLocation() const;
	FVector GetRandomEmptyLocation() const;

	UFUNCTION()
	void SpawnSnakeFood();

	UFUNCTION()
	void OnGameLost(class ASnake* SnakePawn);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Game Lost")
	void BP_OnGameLost();

private:
	void SpawnWalls();

	FVector GridIndexToWorldLocation(const FIntPoint& Index) const;

	void SpawnAI();
};
