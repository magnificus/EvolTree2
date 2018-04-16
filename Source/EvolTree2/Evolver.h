// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tree.h"
#include "Evolver.generated.h"

UCLASS()
class EVOLTREE2_API AEvolver : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEvolver();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		TSubclassOf<ATree> TreeClass;
	UPROPERTY(EditAnywhere)
		int NumTrees = 100;

	UPROPERTY(EditAnywhere, Category = "Visual")
		float TreeSpacing = 400;
	UFUNCTION(BlueprintCallable)
	void InitPopulation();
	TArray<ATree*> Trees;
	

	
	
};
