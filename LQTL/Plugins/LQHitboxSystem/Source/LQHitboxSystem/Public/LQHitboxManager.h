// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "LQHitboxTypes.h"
#include "Components/ActorComponent.h"
#include "LQHitboxManager.generated.h"


UCLASS(ClassGroup=(LQ), meta=(BlueprintSpawnableComponent))
class LQHITBOXSYSTEM_API ULQHitboxManager : public UActorComponent
{
	GENERATED_BODY()
	friend class ULQHitboxAnimNotifyState;

public:
	ULQHitboxManager();
	static void GetCurrentDebugHitboxSettings(FHitboxDebugSettings& OutSettings);
	static ULQHitboxManager* GetFromActor(AActor* Actor);
	// static inline bool bShowDebug = false;

protected:
	static inline FHitboxDebugSettings DebugSetting = FHitboxDebugSettings();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ReceiveHitboxNotify(const FHitResult& HitResult);
};
