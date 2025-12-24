// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "LQTeamFunctionLibrary.generated.h"

UCLASS()
class LQTEAM_API ULQTeamFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Returns the attitude between two actors based on their team tags and LQTeamSettings
	UFUNCTION(BlueprintCallable, Category = "LQ Team")
	static TEnumAsByte<ETeamAttitude::Type> GetTeamAttitudeBetweenActors(const AActor* ActorA, const AActor* ActorB);

	// Returns the team tag for an actor, or an invalid tag if not found
	UFUNCTION(BlueprintCallable, Category = "LQ Team")
	static FGameplayTag GetTeamTagForActor(const AActor* Actor);
};
