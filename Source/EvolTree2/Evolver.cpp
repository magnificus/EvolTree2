// Fill out your copyright notice in the Description page of Project Settings.

#include "Evolver.h"
#include "Engine.h"
#include "CoreMinimal.h"

// Sets default values
AEvolver::AEvolver()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEvolver::BeginPlay()
{
	Super::BeginPlay();
	InitPopulation();
	SetupComplete();

}

// Called every frame
void AEvolver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEvolver::InitPopulation() {
	for (int i = 0; i < FMath::Sqrt(NumTrees); i++) {
		for (int j = 0; j < NumTrees / FMath::Sqrt(NumTrees); j++) {
			ATree *NewTree = Cast<ATree>(GetWorld()->SpawnActor<AActor>(TreeClass, FTransform(FVector(TreeSpacing * i, TreeSpacing * j, 0))));
			Trees.Add(NewTree);
		}
	}
}

void AEvolver::NextGeneration() {
	Trees.Sort([](const ATree &A, const ATree &B) {return A.Fitness < B.Fitness; });
	TArray<ATree*> NewTrees;
	TArray<FTransform> ChildPositions;
	TArray<ATree*> ToBeModifiedTrees;
	NewTrees.Add(Trees[0]);
	for (int i = 1; i < Trees.Num(); i++) {
		float ratio = 1 - ((float)i) / (float)Trees.Num();
		ratio *= 2 * KillRate;
		if (FMath::FRand() > ratio)
			NewTrees.Add(Trees[i]);
		else {
			ChildPositions.Add(Trees[i]->GetActorTransform());
			ToBeModifiedTrees.Add(Trees[i]);
			//Trees[i]->Destroy();
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Killed %i trees this generation"), ChildPositions.Num());
	bool GlobalFitness = EvaluationType == FitnessEvaluationType::Global;

	for (int i = 0; i < ChildPositions.Num(); i++) {
		FTransform T = ChildPositions[i];
		ATree* Parent1 = NewTrees[FMath::RandRange(0, NewTrees.Num() - 1)];
		ATree* Parent2 = NewTrees[FMath::RandRange(0, NewTrees.Num() - 1)];
		ATree::GetTwoParentChild(Parent1, Parent2, T, !GlobalFitness, ToBeModifiedTrees[i]);
		NewTrees.Add(ToBeModifiedTrees[i]);
		//NewTrees.Add(ATree::GetTwoParentChild(Parent1, Parent2, T, !GlobalFitness));
		//NewTrees.Add(Parent->GetSingleParentChild(T));
	}
	if (GlobalFitness) {
		ATree::UpdateFitnessGlobal(Trees, FVector(-TreeSpacing / 2, -TreeSpacing / 2, 0), FVector(FMath::Sqrt(NumTrees)*TreeSpacing + TreeSpacing / 2, FMath::Sqrt(NumTrees)*TreeSpacing + TreeSpacing / 2, 0), 200, Log);
	}
	Trees = NewTrees;
}