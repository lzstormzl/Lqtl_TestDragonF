// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "LQTeamInterface.generated.h"

UINTERFACE(MinimalAPI)
class ULQTeamInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LQTEAM_API ILQTeamInterface
{
	GENERATED_BODY()

public:
	// Provide a last chance for an actor to override it's team tag, instead of using the config in project settings
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LQ Team")
	FGameplayTag OverrideClassTeamTag() const;
};
