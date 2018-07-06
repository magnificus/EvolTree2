// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Branch.generated.h"
/**
 * 
 */

UCLASS()
class UBranch : public UObject
{
public:

	GENERATED_BODY()
	UBranch *Parent;
	TArray<UBranch*> Children;
	TArray<FTransform> Points;
	float WidthStart = 1;
};
