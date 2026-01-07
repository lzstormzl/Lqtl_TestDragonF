// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LQHitboxSettings.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS(Config=Game, defaultconfig)
class LQHITBOXSYSTEM_API ULQHitboxSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config)
	TEnumAsByte<ETraceTypeQuery> DefaultTraceChannel = ETraceTypeQuery::TraceTypeQuery1;

};
