// Copyright Tale Weavers

#pragma once

#include "NativeGameplayTags.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayTagContainer.h"
#include "LQTeamTypes.generated.h"

namespace LQTeam::Tag
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NPC);
}

// Defines a team tag's attitude towards other team tags
USTRUCT(BlueprintType)
struct FLQTeamAttitudeMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team", meta = (Categories = "LQTeam.TeamTag"))
	FGameplayTag TeamTag;

	// Map of other team tags to their attitude
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team", meta = (Categories = "LQTeam.TeamTag"))
	TMap<FGameplayTag, TEnumAsByte<ETeamAttitude::Type>> AttitudeMap;
};