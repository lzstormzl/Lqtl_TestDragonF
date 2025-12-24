// Copyright Tale Weavers

#include "LQTeamFunctionLibrary.h"
#include "LQTeamSettings.h"
#include "LQTeamInterface.h"
#include "GameFramework/Actor.h"

TEnumAsByte<ETeamAttitude::Type> ULQTeamFunctionLibrary::GetTeamAttitudeBetweenActors(const AActor* ActorA, const AActor* ActorB)
{
	if (!ActorA || !ActorB)
	{
		return ETeamAttitude::Neutral;
	}

	const ULQTeamSettings* Settings = ULQTeamSettings::Get();
	if (!Settings)
	{
		return ETeamAttitude::Neutral;
	}

	// Get team tags for both actors
	const FGameplayTag TeamTagA = GetTeamTagForActor(ActorA);
	const FGameplayTag TeamTagB = GetTeamTagForActor(ActorB);

	if (!TeamTagA.IsValid() || !TeamTagB.IsValid())
	{
		return ETeamAttitude::Neutral;
	}

	const FLQTeamAttitudeMapping* Mapping = Settings->TeamAttitudeMappings.FindByPredicate([&](const FLQTeamAttitudeMapping& Map) { return Map.TeamTag == TeamTagA; });
	if (!Mapping)
	{
		return ETeamAttitude::Neutral;
	}

	const TEnumAsByte<ETeamAttitude::Type>* Attitude = Mapping->AttitudeMap.Find(TeamTagB);
	if (Attitude)
	{
		return *Attitude;
	}
	return ETeamAttitude::Neutral;
}

FGameplayTag ULQTeamFunctionLibrary::GetTeamTagForActor(const AActor* Actor)
{
	if (!Actor)
	{
		return FGameplayTag();
	}

	// Check for override via interface
	const ILQTeamInterface* TeamInterface = Cast<ILQTeamInterface>(Actor);
	if (TeamInterface)
	{
		FGameplayTag OverriddenTag = TeamInterface->Execute_OverrideClassTeamTag(Actor);
		if (OverriddenTag.IsValid())
		{
			return OverriddenTag;
		}
	}

	// Check for teamtag defined in project settings
	const ULQTeamSettings* Settings = ULQTeamSettings::Get();
	if (!Settings)
	{
		return FGameplayTag();
	}

	const UClass* ActorClass = Actor->GetClass();
	for (const TPair<TSoftClassPtr<AActor>, FGameplayTag>& Pair : Settings->ActorSoftClassToTeamTagMap)
	{
		if (ActorClass->IsChildOf(Pair.Key.Get()))
		{
			return Pair.Value;
		}
	}
	return FGameplayTag();
}
