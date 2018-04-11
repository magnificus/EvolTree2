// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Branch.h"
#include "Tree.generated.h"

USTRUCT(BlueprintType)
struct FBuildingRule {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FTransform Trans;
    UPROPERTY(BlueprintReadWrite)
	bool NewBranch;
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
		FString Initial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
	FString CurrTotal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
		int Generations = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Algorithm")
	TMap<FString, FString> EvolvingRules;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual")
		UStaticMesh *Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual")
		float WidthMP = 0.9f;

	float CurrentWidth = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool ShowDebug = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool AvoidClipping = true;

	TArray<Branch*> Branches;

	UPROPERTY(EditAnywhere)
	USplineComponent* Spline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
		float ForwardLen = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
		float RotateAngle = 25.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
		float RotateRoll = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
		float ForwardMaxRot = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
		FRandomStream Stream;

	UFUNCTION(BlueprintCallable)
		void Evolve();

	FTransform Turtle;
	Branch* CurrentBranch;
	void InterpretChar(TCHAR in);

	UFUNCTION(BlueprintCallable)
	void Build(FString &in);

	
	virtual void OnConstruction(const FTransform& Transform) override;
};
