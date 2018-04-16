// Fill out your copyright notice in the Description page of Project Settings.

#include "Evolver.h"


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