#pragma once

#include "CoreMinimal.h"

#include "LQTL_TT_Development/Object/Hitbox Effect/HitboxEffects.h"

#include "S_HitboxData.generated.h"

USTRUCT(BlueprintType)
struct LQTL_TT_DEVELOPMENT_API FS_HitboxData
{
	GENERATED_BODY()

	// UHitboxEffectBase to apply when the hitbox overlapped with a valid target.
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite)
	TArray<TObjectPtr<UHitboxEffectBase>> Modifiers;

	// A bone of the owning skeletal mesh to attach the hitbox to.
	// If = None, the hitbox will be attached to the root of the skeletal mesh.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox Shape")
	FName AttachedBoneName;

	// Radius of the capsule.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox Shape")
	float Radius = 100.0f;

	// Local offset of the start of the capsule.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox Shape")
	FVector StartOffset;

	// Local offset of the end of the capsule.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox Shape")
	FVector EndOffset;

	// Determine the general direction the hitbox is coming from.
	// NOTE: This DOES take world rotation into account.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HitDirection;
};