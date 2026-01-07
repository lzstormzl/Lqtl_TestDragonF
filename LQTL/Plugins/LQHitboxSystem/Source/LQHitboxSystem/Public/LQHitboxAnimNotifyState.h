// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "LQHitboxManager.h"
#include "LQHitboxTypes.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "LQAbilitySystemTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "LQHitboxAnimNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class LQHITBOXSYSTEM_API ULQHitboxAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UFUNCTION()
	static TArray<FName> GetSocketNames();

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FString GetNotifyName_Implementation() const override;
#endif

	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TInstancedStruct<FHitboxShapeBase> HitboxShape;

	UPROPERTY(EditAnywhere)
	TInstancedStruct<FHitboxAttackData> AttackData;

private:
	UPROPERTY()
	TObjectPtr<ULQHitboxManager> HitboxManager;
	TMultiMap<TObjectKey<AActor>, TObjectKey<UPrimitiveComponent>> AlreadyHitActors;
	static inline TArray<FName> CurrentSockets;
};
