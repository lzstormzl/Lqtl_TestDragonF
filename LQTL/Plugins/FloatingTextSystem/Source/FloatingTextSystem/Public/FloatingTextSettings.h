// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FloatingTextWidget.h"
#include "FloatingTextTypes.h"
#include "FloatingTextSettings.generated.h"

/**
 * Project settings for FloatingTextSystem
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Floating Text System"))
class FLOATINGTEXTSYSTEM_API UFloatingTextSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const UFloatingTextSettings* Get();

	/** Default widget class if no mapping found */
	UPROPERTY(Config, EditAnywhere, Category = "Widget Templates")
	TSoftClassPtr<UFloatingTextWidget> DefaultWidgetClass;

	/**
	 * Map from floating text type to widget class
	 * Key: EFloatingTextType (e.g., Damage_Default, Damage_Critical, Healing, Status)
	 * Value: Widget class to use for this text type
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Widget Templates")
	TMap<EFloatingTextType, TSubclassOf<UFloatingTextWidget>> WidgetTemplateMap;

	/** Default pool size per widget type */
	UPROPERTY(Config, EditAnywhere, Category = "Performance", meta = (ClampMin = "1", ClampMax = "100"))
	int32 DefaultPoolSize = 10;

	/** Maximum pool size per widget type (hard limit) */
	UPROPERTY(Config, EditAnywhere, Category = "Performance", meta = (ClampMin = "1", ClampMax = "500"))
	int32 MaxPoolSize = 50;

	/** Duration floating text stays visible (seconds) */
	UPROPERTY(Config, EditAnywhere, Category = "Display", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float DisplayDuration = 0.2f;

	/** Default widget draw size (2D size in world space) */
	UPROPERTY(Config, EditAnywhere, Category = "Display")
	FVector2D DefaultWidgetDrawSize = FVector2D(200.0f, 50.0f);

	/** Default pivot point for widgets */
	UPROPERTY(Config, EditAnywhere, Category = "Display")
	FVector2D DefaultPivot = FVector2D(0.5f, 0.5f);

	/** Z-offset from spawn location/actor */
	UPROPERTY(Config, EditAnywhere, Category = "Display")
	float ZOffset = 100.0f;
};
