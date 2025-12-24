// Copyright Tale Weavers


#include "LQHitboxManager.h"

bool bShowDebug = false;
FAutoConsoleCommandWithWorldAndArgs LQShowHitboxDebugCommand(TEXT("LQ.Debug.ShowHitbox"),TEXT("Show hitbox debug info"),
                                                             FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
                                                             {
	                                                             bShowDebug = !bShowDebug;
                                                             })
);


// Sets default values for this component's properties
ULQHitboxManager::ULQHitboxManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void ULQHitboxManager::GetCurrentDebugHitboxSettings(FHitboxDebugSettings& OutSettings)
{
	auto DebugType = bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	DebugSetting.DrawDebugType = DebugType;
	OutSettings = DebugSetting;
}


ULQHitboxManager* ULQHitboxManager::GetFromActor(AActor* Actor)
{
	if (Actor)
	{
		return Actor->FindComponentByClass<ULQHitboxManager>();
	}
	return nullptr;
}


// Called when the game starts
void ULQHitboxManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void ULQHitboxManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULQHitboxManager::ReceiveHitboxNotify(const FHitResult& HitResult)
{
	UE_LOGFMT(LogLQHitboxSystem, Log, "Hitbox hit actor: {0}, component: {1}", UKismetSystemLibrary::GetDisplayName(HitResult.GetActor()), GetNameSafe(HitResult.GetComponent()));
}
