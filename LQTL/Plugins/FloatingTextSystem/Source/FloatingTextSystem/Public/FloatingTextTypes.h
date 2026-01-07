// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "FloatingTextTypes.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFloatingTextSystem, Log, All);

/**
 * Enum for categorizing floating text types
 * Used as key in widget template mapping
 */
UENUM(BlueprintType)
enum class EFloatingTextType : uint8
{
	Damage_Default UMETA(DisplayName = "Damage (Default)"),
	Damage_Critical UMETA(DisplayName = "Damage (Critical)"),
	Healing UMETA(DisplayName = "Healing"),
};

/**
 * Base struct for floating text data - polymorphic base for all text types
 */
USTRUCT(BlueprintType)
struct FLOATINGTEXTSYSTEM_API FFloatingTextDataBase
{
	GENERATED_BODY()

	virtual ~FFloatingTextDataBase() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
	EFloatingTextType TextType = EFloatingTextType::Damage_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Text")
	FText DisplayText;
};

/**
 * Internal wrapper for pooled widget instances
 */
USTRUCT()
struct FFloatingTextWidgetInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<class UFloatingTextWidget> Widget;

	bool bIsActive = false;
	FTimerHandle DeactivationTimer;

	FFloatingTextWidgetInstance()
		: Widget(nullptr)
		, bIsActive(false)
	{
	}
};
