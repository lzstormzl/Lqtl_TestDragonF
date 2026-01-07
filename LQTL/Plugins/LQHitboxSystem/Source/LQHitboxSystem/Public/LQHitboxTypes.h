// Copyright Tale Weavers

#pragma once
#include "CoreMinimal.h"
#include "LQAbilitySystemTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "LQHitboxTypes.generated.h"

struct FPostApplyDamageData;
struct FPreApplyDamageData;
struct FGameplayTag;
DECLARE_LOG_CATEGORY_EXTERN(LogLQHitboxSystem, Log, All);

USTRUCT(BlueprintType)
struct FHitboxDebugSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::None;

	UPROPERTY(EditAnywhere)
	float DebugDuration = 0.1f;

	UPROPERTY(EditAnywhere)
	FLinearColor DebugTraceColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere)
	FLinearColor DebugTraceHitColor = FLinearColor::Green;
};

USTRUCT()
struct FHitboxHitResultWrapper
{
	GENERATED_BODY()
	TObjectKey<AActor> HitActorKey;
	TObjectKey<UPrimitiveComponent> HitComponentKey;
	FHitboxHitResultWrapper() = default;
	explicit FHitboxHitResultWrapper(const FHitResult& HitResult);
};

USTRUCT(BlueprintType)
struct FHitboxShapeBase
{
	GENERATED_BODY()

public:
	virtual void BeginTrace(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration);
	virtual bool TickTrace(UObject* TraceWorldOwner, FHitResult& OutHitResult) const { return false; }
	virtual ~FHitboxShapeBase() = default;

	UPROPERTY(EditAnywhere, meta=(GetOptions="LQHitboxSystem.LQHitboxAnimNotifyState.GetSocketNames"))
	FName SocketName = NAME_None;

	UPROPERTY()
	TObjectPtr<UMeshComponent> MyMeshComp;

protected:
};

USTRUCT()
struct FHitboxShape_Sphere final : public FHitboxShapeBase
{
	GENERATED_BODY()

public:
	virtual bool TickTrace(UObject* TraceWorldOwner, FHitResult& OutHitResult) const override;

	UPROPERTY(EditAnywhere)
	float Radius = 50.f;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery3;
};

USTRUCT()
struct FHitboxShape_Box final : public FHitboxShapeBase
{
	GENERATED_BODY()

public:
	virtual bool TickTrace(UObject* TraceWorldOwner, FHitResult& OutHitResult) const override;

	UPROPERTY(EditAnywhere)
	FVector HalfExtent = FVector(50.f, 50.f, 50.f);

	UPROPERTY(EditAnywhere)
	FRotator Orientation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery3;
};

USTRUCT()
struct FHitboxShape_Capsule final : public FHitboxShapeBase
{
	GENERATED_BODY()

public:
	virtual bool TickTrace(UObject* TraceWorldOwner, FHitResult& OutHitResult) const override;

	UPROPERTY(EditAnywhere)
	float Radius = 50.f;

	UPROPERTY(EditAnywhere)
	float HalfHeight = 100.f;

	UPROPERTY(EditAnywhere)
	FRotator Orientation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery3;
};

USTRUCT()
struct FHitboxArray final : public FHitboxShapeBase
{
	GENERATED_BODY()

public:
	virtual bool TickTrace(UObject* TraceWorldOwner, FHitResult& OutHitResult) const override;

	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FHitboxShapeBase>> HitboxShapes;
};

USTRUCT(BlueprintType)
struct FHitboxAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TInstancedStruct<FPreApplyDamageData> PreApplyDamageData;

	UPROPERTY(EditAnywhere)
	TInstancedStruct<FPostApplyDamageData> PostApplyDamageData;
	FHitboxAttackData() = default;
	explicit FHitboxAttackData(EExternalAttackDataSource Source);
	void SetSource(EExternalAttackDataSource Source);
};
