// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "BFL_Bitmask.generated.h"

UCLASS()
class UBFL_Bitmask : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, 
		meta = (Bitmask = "_Container"))
	static inline bool ContainFlags(int32 _Container, int32 _CheckFlags, bool _ContainAll) {
		return false;
	}
};