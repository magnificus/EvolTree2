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
	if (LeafMeshC)
		LeafMeshC->ClearInstances();
	else {
		LeafMeshC = NewObject<UInstancedStaticMeshComponent>(this);
		LeafMeshC->SetWorldTransform(GetActorTransform());
		LeafMeshC->RegisterComponent();
		LeafMeshC->SetStaticMesh(LeafMesh);
		LeafMeshC->SetFlags(RF_Transactional);
		AddInstanceComponent(LeafMeshC);
	}

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
#define randRotator FRotator(Stream.FRand()*360,Stream.FRand()*360,Stream.FRand()*360)

void ATree::NewBranch(float Roll) {
	Branch *CBranch = new Branch();
	Turtle.SetRotation(Turtle.GetRotation() * FQuat(FRotator(Roll, 0, 0)));
	CBranch->Points.Add(Turtle);
	CBranch->Parent = CurrentBranch;
	CBranch->WidthStart = CurrentWidth;
	CurrentWidth *= WidthMP;
	CurrentBranch->Children.Add(CBranch);
	CurrentBranch = CBranch;
	Branches.Add(CBranch);
}

void ATree::InterpretChar(TCHAR In) {
	switch (In) {
	case 'F': {
		FRotator ToRotate = FRotator(randR, randR, randR);
		updateR(ToRotate);

		FVector Dir = Turtle.GetRotation().GetForwardVector();
		FVector ToAdd = Dir * ForwardLen;
		Turtle.AddToTranslation(ToAdd);
		CurrentWidth *= WidthMP;
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
		NewBranch(Stream.FRand() < 0.5 ? RotateRoll : -RotateRoll);
		break;
	}
	case ']': {
		Turtle = CurrentBranch->Points[0];
		CurrentWidth = CurrentBranch->WidthStart;
		CurrentBranch = CurrentBranch->Parent;
		break;
	}
	//case '{': {
	//	NewBranch(-RotateRoll);
	//	break;
	//}
	//case '}': {
	//	Turtle = CurrentBranch->Points[0];
	//	CurrentWidth = CurrentBranch->WidthStart;
	//	CurrentBranch = CurrentBranch->Parent;
	//	break;
	//}
	}


}


void ATree::Build(FString &In) {
	Turtle = FTransform(FRotator(0,90,0), FVector(0,0,0));
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

		TArray<FVector> VectorPoints;
		for (FTransform t : B->Points) {
			VectorPoints.Add(t.GetLocation());
		}
		Spline->SetSplinePoints(VectorPoints, ESplineCoordinateSpace::World);



		float Start = B->WidthStart;
		for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 1; i++) {
			if (AvoidClipping) {
				FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true);
				RV_TraceParams.bTraceComplex = true;
				FHitResult RV_Hit(ForceInit);

				FVector StartL = GetActorLocation() + GetActorRotation().RotateVector(VectorPoints[i]);    //start
				FVector EndL = GetActorLocation() + GetActorRotation().RotateVector(VectorPoints[i+1]);
				FVector Diff = (EndL - StartL);
				GetWorld()->LineTraceSingleByChannel(
					RV_Hit,        //result
					StartL,
					EndL, //end
					ECC_Visibility, //collision channel
					RV_TraceParams
				);
				if (ShowDebug) {
					DrawDebugLine(
						GetWorld(),
						StartL,
						EndL,
						FColor::Red,
						true, 5, 0, 5
					);
				}
				if (RV_Hit.GetComponent() && FVector::Dist(RV_Hit.ImpactPoint, StartL) > 5.0f) {
					UE_LOG(LogTemp, Display, TEXT("Collision"));
					InvalidBranches.Add(B);
					break;
				}
			}
			FVector Loc1, Loc2;
			FVector Tan1, Tan2;
			Spline->GetLocationAndTangentAtSplinePoint(i, Loc1, Tan1, ESplineCoordinateSpace::World);
			Spline->GetLocationAndTangentAtSplinePoint(i+1, Loc2, Tan2, ESplineCoordinateSpace::World);
			USplineMeshComponent *S = NewObject<USplineMeshComponent>(this);
			S->SetMobility(EComponentMobility::Type::Static);
			S->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			S->SetStaticMesh(BranchMesh);
			S->SetForwardAxis(ESplineMeshAxis::Z);
			S->SetStartAndEnd(Loc1, Tan1, Loc2, Tan2);
			S->SetupAttachment(RootComponent);

			float S1 = FMath::Pow(WidthMP, i) * Start;
			float S2 = FMath::Pow(WidthMP, i+1) * Start;

			S->SetStartScale(FVector2D(S1,S1));
			S->SetEndScale(FVector2D(S2, S2));
			S->SetCollisionResponseToAllChannels(ECR_Block);
			S->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			S->bGenerateOverlapEvents = true;

			S->RegisterComponent();
			S->MarkRenderStateDirty();

			// add twigs with leafs
			if (Leafs && B->WidthStart < MaxLeafAttachWidth && B->WidthStart > MinLeafAttachWidth) {
				for (int i = 0; i < LeafDensity * B->WidthStart; i++) {
					float PosOffset = Stream.FRand();
					FRotator RR = randRotator;
					LeafMeshC->AddInstance(FTransform(RR, Loc1 + (Loc2 - Loc1)*PosOffset));
				}
			}
		}
	}
	Spline->UpdateSpline();
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