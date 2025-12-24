// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LQTeamTypes.h"
#include "GameplayTagContainer.h"
#include "LQTeamSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "LQ Team"))
class LQTEAM_API ULQTeamSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public: 
	static const ULQTeamSettings* Get();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Config, Category = "LQ Team", meta = (Categories = "LQTeam.TeamTag"))
	TMap<TSoftClassPtr<AActor>, FGameplayTag> ActorSoftClassToTeamTagMap;

	// Array of team attitude mappings
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Config, Category = "LQ Team")
	TArray<FLQTeamAttitudeMapping> TeamAttitudeMappings;
};
