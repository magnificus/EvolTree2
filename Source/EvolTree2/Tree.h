// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Branch.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Tree.generated.h"


UENUM(BlueprintType)
enum class FitnessFunction : uint8
{
	Height,
	NumLeafs,
	Sun_Straight_Above,
	Above_And_Weights
};

UCLASS()
class EVOLTREE2_API ATree : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATree();
	~ATree();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	


	TMap<FString, float> TreeParameters;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
	FitnessFunction FitnessFun;

    // Called every frame

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
		FString Initial = "X";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
	FString CurrTotal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
		int Generations = 4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
	TMap<FString, FString> EvolvingRules;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual")
		UStaticMesh *BranchMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual")
		UStaticMesh *LeafMesh;


	UInstancedStaticMeshComponent *LeafMeshC;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
		float WidthMP = 0.9f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
		float MinWidth = 0.05f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
		float MinLeafAttachWidth = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
		float MaxLeafAttachWidth = 0.7f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
		int MaxBranchSegments = 50;

	float CurrentWidth = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool ShowDebug = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool AvoidClipping = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Leafs")
		bool Leafs = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Leafs")
		float LeafDensity = true;

	TArray<Branch*> Branches;

	UPROPERTY(EditAnywhere)
	USplineComponent* Spline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
		float ForwardLen = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
		float RotationInterval = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
		float ForwardMaxRot = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
		FRandomStream Stream;

	UFUNCTION(BlueprintCallable)
		void Evolve();

	ATree* GetSingleParentChild(FTransform Trans);
	static ATree* GetTwoParentChild(ATree *T1, ATree *T2, FTransform Trans);

	UFUNCTION(BlueprintCallable)
		void CopyFrom(ATree* From);

	UFUNCTION(BlueprintCallable)
		void Mutate();

	void NewBranch(float Roll);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation")
		int RuleMutationAttempts = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation")
		float RuleMutationChance = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation")
		float RuleRemoveChance = 0.05f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool BuildInConstructionScript = false;
	FTransform Turtle;
	Branch* CurrentBranch;
	void InterpretChar(TCHAR in);

	UFUNCTION(BlueprintCallable)
	void Build(FString &in);

	UFUNCTION(BlueprintCallable)
		void UpdateFitness();

	float GetSunStraightAbove(float Radius, int SamplesSide);


	UPROPERTY(BlueprintReadWrite)
	float Fitness = 0.0;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	TArray<FString> AvailableSymbols = {"F", "X", "A", "B", "C", "D", "+", "-", "[", "]", "*", "'", ".", ":"};

};
