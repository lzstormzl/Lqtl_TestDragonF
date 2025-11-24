#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class E_DamageElement : uint8
{
	None	UMETA(DisplayName = "Normal"),
	Light	UMETA(DisplayName = "Light"),
	Dark	UMETA(DisplayName = "Dark"),
};
