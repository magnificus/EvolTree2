// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
struct Branch
{
public:
	//Branch();
	//~Branch();

	Branch *Parent;
	TArray<Branch*> Children;
	TArray<FTransform> Points;
	float WidthStart = 1;
	//float WidthEnd = 1;
};
