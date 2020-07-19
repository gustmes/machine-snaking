// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake.h"
#include "SnakePart.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "SnakeFood.h"
#include "MachineSnakingGameModeBase.h"
#include "Kismet/GameplayStatics.h"

ASnake::ASnake()
{
	MoveDistance = 10.0f;
	UpdateRate = 0.1f;
}

void ASnake::BeginPlay()
{
	Super::BeginPlay();

	SetInitialLocation();

	Grow(GetActorLocation());

	FoodEaten = 0;
	Fitness = 0.0f;
}

void ASnake::SetInitialLocation()
{
	AMachineSnakingGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AMachineSnakingGameModeBase>();
	if (!GameMode) return;
	SetActorLocation(GameMode->GetRandomInitialLocation());

	OnFoodEaten.AddDynamic(GameMode, &AMachineSnakingGameModeBase::SpawnSnakeFood);
}

void ASnake::SetMoveDirection(const FIntPoint& Direction)
{
	if (!CanSetDirection(Direction)) return;

	MoveDirection = Direction;

	if (!GetDistanceToFood(OUT DistanceToFood))
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO GET DISTANCE TO FOOD!"));
	}
	
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_SnakeMovement)) return;
	GetWorldTimerManager().SetTimer(TimerHandle_SnakeMovement, this, &ASnake::Move, UpdateRate, true);
}

void ASnake::Move()
{
	if (!SnakeParts.Num()) return;
	ASnakePart* Head = SnakeParts.Last();

	UWorld* World = GetWorld();
	if (!World) return;

	FVector HeadLocation = Head->GetActorLocation();
	HeadLocation += FVector(MoveDirection.X * MoveDistance, MoveDirection.Y * MoveDistance, 0.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Head);

	FHitResult Hit;
	bool bHasHit = World->LineTraceSingleByChannel(OUT Hit, Head->GetActorLocation(), HeadLocation, ECollisionChannel::ECC_WorldDynamic, QueryParams);

	if (!bHasHit)
	{
		MoveBody(HeadLocation);
		OnMoveFinished.Broadcast();
		return;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor) return;

	if (ASnakeFood* SnakeFood = Cast<ASnakeFood>(HitActor))
	{
		Eat(SnakeFood);
		OnMoveFinished.Broadcast();
	}
	else
	{
		LoseGame();
	}
}

bool ASnake::CanSetDirection(const FIntPoint& Direction) const
{
	if (!GetWorldTimerManager().IsTimerActive(TimerHandle_SnakeMovement)) return true;
	return (Direction.X != MoveDirection.X && Direction.Y != MoveDirection.Y);
}

void ASnake::MoveBody(const FVector& NewLocation)
{
	FVector NextLocation = NewLocation;
	for (int32 i = SnakeParts.Num() - 1; i >= 0; --i)
	{
		if (!SnakeParts[i]) continue;
		FVector Location = SnakeParts[i]->GetActorLocation();
		SnakeParts[i]->SetActorLocation(NextLocation);
		NextLocation = Location;
	}

	float NewDistanceToFood = 0.0f;
	if (GetDistanceToFood(OUT NewDistanceToFood))
	{
		if (NewDistanceToFood > DistanceToFood) ++DistancingSteps;
		if (NewDistanceToFood < DistanceToFood) ++ClosingSteps;
		DistanceToFood = NewDistanceToFood;
	}
}

void ASnake::Eat(ASnakeFood* Food)
{
	if (!Food) return;

	FVector FoodLocation = Food->GetActorLocation();
	Food->Destroy();
	Grow(FoodLocation);
	++FoodEaten;

	OnFoodEaten.Broadcast();
}

void ASnake::Grow(const FVector& NewLocation)
{
	ASnakePart* OldPart = (SnakeParts.Num()) ? SnakeParts.Last() : nullptr;

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;

	ASnakePart* NewHead = World->SpawnActor<ASnakePart>(SnakePartClass, GetTransform(), SpawnParams);
	if (!NewHead) return;
	NewHead->SetActorLocation(NewLocation);

	SnakeParts.Add(NewHead);

	if (OldPart)
	{
		OldPart->SetPartMaterial(BodyMaterial);
	}

	NewHead->SetPartMaterial(HeadMaterial);
}

void ASnake::LoseGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Lose Game!"));
	GetWorldTimerManager().ClearTimer(TimerHandle_SnakeMovement);

	if (!GetWorld()) return;
	if (AMachineSnakingGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AMachineSnakingGameModeBase>())
	{
		GameMode->OnGameLost(this);
	}
}

bool ASnake::EnsureFood()
{
	if (Food) return true;
	Food = UGameplayStatics::GetActorOfClass(this, ASnakeFood::StaticClass());
	return (Food != nullptr);
}

bool ASnake::GetDistanceToFood(float& OutDistance)
{
	if (!EnsureFood() || !SnakeParts.Num()) return false;
	OutDistance = FVector::Dist2D(Food->GetActorLocation(), SnakeParts.Last()->GetActorLocation());
	return true;
}

float ASnake::GetFitnessLevel() const
{
	// 10 points for each Food eaten, +1 point for each step that brings the snake closer to the food, 
	// -1.5 points for each step that takes the snake further away from the food.
	return 10.0f * FoodEaten + 1.0f * ClosingSteps - 1.5f * DistancingSteps;
}

TArray<float> ASnake::GetNNInputs() const
{
	TArray<float> Inputs;

	if (!SnakeParts.Num()) return Inputs;
	ASnakePart* Head = SnakeParts.Last();
	if (!Head) return Inputs;

	const UWorld* World = GetWorld();
	if (!World) return Inputs;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Head);

	const FVector& HeadLocation = Head->GetActorLocation();

	// Check 90, 0, and -90 degrees rotation from head
	for (int32 i = -1; i <= 1; ++i)
	{
		bool bIsBlockedHit = false;

		const FVector& LocationToCheck = HeadLocation + 
			FVector(MoveDirection.X * MoveDistance, MoveDirection.Y * MoveDistance, 0.0f).RotateAngleAxis(-90.0f * i, FVector::UpVector);

		FHitResult Hit;
		bool bHasHit = World->LineTraceSingleByChannel(OUT Hit, Head->GetActorLocation(), LocationToCheck, ECollisionChannel::ECC_WorldDynamic, QueryParams);

		if (bHasHit)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				// If hit anything other than snake food, blocked hit.
				ASnakeFood* SnakeFood = Cast<ASnakeFood>(HitActor);
				if (!SnakeFood) bIsBlockedHit = true;
			}
		}

		Inputs.Add((bIsBlockedHit) ? 0.0f : 1.0f);
	}

	if (!Food) return Inputs;

	const FVector& MoveDirection = Head->GetActorForwardVector();
	const FVector& DirectionToFood = (HeadLocation - Food->GetActorLocation()).GetSafeNormal();
	Inputs.Add(FVector::DotProduct(MoveDirection, DirectionToFood));

	return Inputs;
}