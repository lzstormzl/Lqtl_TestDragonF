// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LQHitboxSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig)
class LQHITBOXSYSTEM_API ULQHitboxSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Config)
	TEnumAsByte<ETraceTypeQuery> DefaultTraceChannel = ETraceTypeQuery::TraceTypeQuery1;
};
