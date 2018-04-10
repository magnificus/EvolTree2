// Fill out your copyright notice in the Description page of Project Settings.

#include "Tree.h"
#include "Branch.h"
#include "DrawDebugHelpers.h"
#include <Components/SplineMeshComponent.h>
#include <Components/SplineComponent.h>
#include "Engine/World.h"


// Sets default values
ATree::ATree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Spline = CreateDefaultSubobject<USplineComponent>(FName("Spline"));
	Spline->SetupAttachment(RootComponent);

}

ATree::~ATree() {
	for (Branch *B : Branches)
		delete B;
}

void ATree::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	CurrTotal = Initial;
	CurrentWidth = 1.0f;
	for (Branch *B : Branches)
		delete B;
	Branches.Empty();
	for (int i = 0; i < Generations; i++)
		Evolve();
	Build(CurrTotal);
}
// Called when the game starts or when spawned
void ATree::BeginPlay()
{

	Super::BeginPlay();
}

// Called every frame
void ATree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#define randR Stream.FRandRange(-ForwardMaxRot, ForwardMaxRot)
#define updateR(in) Turtle.SetRotation(Turtle.GetRotation() * FQuat(in));

void ATree::InterpretChar(TCHAR In) {
	switch (In) {
	case 'F': {
		FRotator ToRotate = FRotator(randR, randR, randR);
		updateR(ToRotate);

		FVector Dir = Turtle.GetRotation().GetForwardVector();
		FVector ToAdd = Dir * ForwardLen;
		Turtle.AddToTranslation(ToAdd);
		CurrentWidth *= WidthMP;
		//CurrentBranch->WidthEnd = CurrentWidth;
		CurrentBranch->Points.Add(Turtle);
		break;
	}
	case 'G': {
		break;
	}
	case '+': {
		updateR(FRotator(0, RotateAngle, 0));
		break;
	}
	case '-': {
		updateR(FRotator(0, -RotateAngle, 0));
		break;
	}
	case '[': {
		Branch *CBranch = new Branch();
		Turtle.SetRotation(Turtle.GetRotation() * FQuat(FRotator(Stream.FRand() < 0.5 ? RotateRoll : -RotateRoll,0 , 0)));
		CBranch->Points.Add(Turtle);
		CBranch->Parent = CurrentBranch;
		CBranch->WidthStart = CurrentWidth;
		CurrentWidth *= WidthMP;
		//CBranch->WidthEnd = CurrentWidth;
		CurrentBranch->Children.Add(CBranch);
		CurrentBranch = CBranch;
		Branches.Add(CBranch);
		break;
	}
	case ']': {
		Turtle = CurrentBranch->Points[0];
		CurrentWidth = CurrentBranch->WidthStart;
		CurrentBranch = CurrentBranch->Parent;
		break;
	}
	}

	if (ShowDebug) {
			DrawDebugSphere(
				GetWorld(),
				Turtle.GetLocation(),
				24,
				32,
				FColor(255, 0, 0),
				true,
				5.0f
			);
	}
}


void ATree::Build(FString &In) {
	Turtle = GetActorTransform();
	CurrentBranch = new Branch();
	CurrentBranch->Points.Add(Turtle);
	Branches.Add(CurrentBranch);
	for (int i = 0; i < CurrTotal.Len(); i++) {
		const TCHAR c = CurrTotal[i];
		InterpretChar(c);
	}
	TSet<Branch*> InvalidBranches;

	for (Branch *B : Branches) {
		if (InvalidBranches.Contains(B->Parent)) {
			InvalidBranches.Add(B);
			continue;
		}

		if (AvoidClipping) {
			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true);
			//RV_TraceParams.bTraceComplex = true;
			//RV_TraceParams.bTraceAsyncScene = true;
			//RV_TraceParams.bReturnPhysicalMaterial = false;

			//Re-initialize hit info
			FHitResult RV_Hit(ForceInit);

			GetWorld()->LineTraceSingleByChannel(
				RV_Hit,        //result
				B->Points[0].GetLocation(),    //start
				B->Points[B->Points.Num() - 1].GetLocation(), //end
				ECC_Visibility, //collision channel
				RV_TraceParams
			);

			//UE_LOG(LogTemp, Display, TEXT("Line trace from %s to %s "), *(B->Points[0].ToString()), *(B->Points[B->Points.Num() - 1].ToString()));
			if (RV_Hit.Component != nullptr) {
				UE_LOG(LogTemp, Display, TEXT("Collision"));
				InvalidBranches.Add(B);
				continue;
			}
		}


		TArray<FVector> VectorPoints;
		for (FTransform t : B->Points) {
			VectorPoints.Add(t.GetLocation());
		}
		Spline->SetSplinePoints(VectorPoints, ESplineCoordinateSpace::Local);

		float Start = B->WidthStart;
		for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 1; i++) {
			FVector Loc1, Loc2;
			FVector Tan1, Tan2;
			Spline->GetLocationAndTangentAtSplinePoint(i, Loc1, Tan1, ESplineCoordinateSpace::Local);
			Spline->GetLocationAndTangentAtSplinePoint(i+1, Loc2, Tan2, ESplineCoordinateSpace::Local);
			USplineMeshComponent *S = NewObject<USplineMeshComponent>(this);
			S->SetMobility(EComponentMobility::Type::Static);
			S->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			S->SetStaticMesh(Mesh);
			S->SetForwardAxis(ESplineMeshAxis::Z);
			S->SetStartAndEnd(Loc1, Tan1, Loc2, Tan2);
			S->SetupAttachment(RootComponent);
			S->SetCollisionResponseToChannel(ECollisionChannel::ECC_MAX, ECollisionResponse::ECR_Overlap);

			float S1 = FMath::Pow(WidthMP, i) * Start;
			float S2 = FMath::Pow(WidthMP, i+1) * Start;

			S->SetStartScale(FVector2D(S1,S1));
			S->SetEndScale(FVector2D(S2, S2));
			S->RegisterComponent();

		}
	}
}

void ATree::Evolve() {
	FString newS;
	for (int i = 0; i < CurrTotal.Len(); i++) {
		const TCHAR c = CurrTotal[i];
		FString currC = FString::Chr(c);
		if (EvolvingRules.Contains(currC)){
			newS += EvolvingRules[currC]; 
			}
		else
			newS += currC;
	}
	CurrTotal = newS;
}