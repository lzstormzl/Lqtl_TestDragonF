// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "S_CombatResource.h"
#include "LQTL_TT_Development/Enum/E_DamageElement.h"

#include "S_AttackData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FS_AttackData : public FTableRowBase
{
	GENERATED_BODY()
	
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FS_CombatResource attackBaseStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FS_CombatResource selfEffectBaseStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	E_DamageElement attackElement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "E_AttackFlags"))
	int32 attackFlags;
};
