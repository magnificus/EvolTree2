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
	Trees.Sort([](const ATree &A,const ATree &B) {return A.Fitness < B.Fitness; });
	TArray<ATree*> NewTrees;
	TArray<FTransform> ChildPositions;
	for (int i = 1; i < Trees.Num(); i++) {
		float ratio = ((float)i) / (float)Trees.Num();
		ratio *= 2 * KillRate;
		if (FMath::FRand() > ratio)
			NewTrees.Add(Trees[i]);
		else {
			ChildPositions.Add(Trees[i]->GetActorTransform());
			Trees[i]->Destroy();
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Killed %i trees this generation"), ChildPositions.Num());
	for (FTransform T : ChildPositions) {
		ATree* Parent = NewTrees[FMath::RandRange(0, NewTrees.Num() - 1)];
		NewTrees.Add(Parent->GetSingleParentChild(T));
	}
	Trees = NewTrees;
}