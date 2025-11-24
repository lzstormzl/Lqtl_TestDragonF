#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType, meta = (BitFlags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class E_AttackFlags : uint8
{
	None			= 0			UMETA(Hidden),
	Unblockable		= 1 << 0	UMETA(DisplayName = "Unblockable"),
	Uninteruptable	= 1 << 1	UMETA(DisplayName = "Uninteruptable"),
	TrueDamage		= 1 << 2	UMETA(DisplayName = "True Damage"),

	IgnoreDodge		= 1 << 3	UMETA(DisplayName = "Ignore Dodge"),
	IgnoreWaterForm = 1 << 4	UMETA(DisplayName = "Ignore Water Form"),
	IgnoreParry		= 1 << 5	UMETA(DisplayName = "Ignore Parry")
};
ENUM_CLASS_FLAGS(E_AttackFlags)