// Copyright Tale Weavers


#include "OnHitBehaviorBase.h"

class UWorld* UOnHitBehaviorBase::GetWorld() const
{
	return GetOuter()->GetWorld();
}

void UOnHitBehaviorBase::ExecuteOnHit_Implementation(AActor* Instigator, const FGameplayAbilitySpecHandle&, const FHitResult& TargetHitResult, const TInstancedStruct<FHitboxAttackData>& AnimHitboxData)
{
}
