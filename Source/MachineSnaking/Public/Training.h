#pragma once

#include "CoreMinimal.h"
#include "Training.generated.h"

USTRUCT(BlueprintType)
struct FTrainingData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1))
	int32 PopulationSize;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1))
	int32 BestIndividualSelection;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1))
	int32 InputLayerSize;

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> HiddenLayersSizes;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1))
	int32 OutputLayerSize;

	int32 NumGenes;

	FTrainingData()
	{
		NumGenes = GetNumGenes();
	}

	FTrainingData(const int32& IN_PopulationSize, const int32& IN_BestIndividualSelection,
		const int32& IN_InputLayerSize, const TArray<int32>& IN_HiddenLayersSizes, const int32& IN_OutputLayerSize)
		: PopulationSize(IN_PopulationSize), BestIndividualSelection(IN_BestIndividualSelection),
		InputLayerSize(IN_InputLayerSize), HiddenLayersSizes(IN_HiddenLayersSizes), OutputLayerSize(IN_OutputLayerSize)
	{
		NumGenes = GetNumGenes();
	}

	FORCEINLINE int32 GetNumGenes() const
	{
		TArray<int32> LayerSizes = HiddenLayersSizes;
		LayerSizes.EmplaceAt(0, InputLayerSize);
		LayerSizes.Add(OutputLayerSize);
		
		int32 Num = 0;
		for (int32 i = 0; i < LayerSizes.Num() - 1; ++i)
		{
			if (!LayerSizes.IsValidIndex(i) || !LayerSizes.IsValidIndex(i + 1)) continue;
			Num += (LayerSizes[i + 1] * (LayerSizes[i]+1));
		}

		return Num;
	}

	FORCEINLINE bool operator==(const FTrainingData& Other) const
	{
		return PopulationSize == Other.PopulationSize && BestIndividualSelection == Other.BestIndividualSelection && InputLayerSize == Other.InputLayerSize
			&& OutputLayerSize == Other.OutputLayerSize && HiddenLayersSizes == Other.HiddenLayersSizes;
	}

	FORCEINLINE bool operator!=(const FTrainingData& Other) const { return !((*this) == Other); }
};

USTRUCT()
struct FIndividual
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> Chromossome;

	UPROPERTY()
	float Fitness;

	FORCEINLINE bool operator>(const FIndividual& Other) const { return Fitness > Other.Fitness; }
	FORCEINLINE bool operator>=(const FIndividual& Other) const { return Fitness >= Other.Fitness; }
	FORCEINLINE bool operator<(const FIndividual& Other) const { return Fitness < Other.Fitness; }
	FORCEINLINE bool operator<=(const FIndividual& Other) const { return Fitness <= Other.Fitness; }

	FIndividual() {}

	// Randomly initializes a number of genes
	FIndividual(const int32& Num)
	{
		for (int32 i = 0; i < Num; ++i)
		{
			Chromossome.Add(FMath::RandRange(-1.0f, 1.0f));
		}
	}

	FIndividual(const TArray<float>& Genes) : Chromossome(Genes)
	{}
};

USTRUCT()
struct FGeneration
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FIndividual> Individuals;

	FORCEINLINE void SortByFitness()
	{
		Individuals.Sort();
	}

	FGeneration()
	{}

	FIndividual& operator[] (const int32& Index)
	{
		return Individuals[Index];
	}
};