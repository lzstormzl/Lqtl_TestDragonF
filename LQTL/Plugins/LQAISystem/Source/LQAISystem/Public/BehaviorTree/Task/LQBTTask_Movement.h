// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "StructUtils/InstancedStruct.h"
#include "LQBTTask_Movement.generated.h"

UENUM(BlueprintType)
enum class ELQMovementType : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	KeepSafeDistance UMETA(DisplayName = "Keep Safe Distance"),
};

UENUM(BlueprintType)
enum class ELQEndMovementPolicy : uint8
{
	ForDuration UMETA(DisplayName = "For Duration"),
	ReachTarget UMETA(DisplayName = "Reach Target"),
};

USTRUCT(BlueprintType)
struct FLQMovementTargetLocationContext
{
	GENERATED_BODY()

	FLQMovementTargetLocationContext() {}
	FLQMovementTargetLocationContext(AActor* InOwnerActor, AActor* InTargetActor) : OwnerActor(InOwnerActor), TargetActor(InTargetActor) {}

	bool IsValid() const
	{
		return (OwnerActor != nullptr && TargetActor != nullptr);
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> OwnerActor = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> TargetActor = nullptr;
};

/**
 * Base struct for getting target strafe location
 */
USTRUCT(BlueprintType)
struct LQAISYSTEM_API FLQMovementTargetLocationBase
{
	GENERATED_BODY()

	virtual ~FLQMovementTargetLocationBase() = default;

	/**
	 * Gets the target location to strafe to
	 * @param	OwnerPawn		The pawn executing the movement
	 * @param	TargetActor		The target actor to maintain distance from
	 * @param	IdealSafeDistance	The ideal distance to maintain from target
	 * @return	The target location to move to
	 */
	virtual bool GetTargetLocation(const FLQMovementTargetLocationContext& Context, FVector& OutLocation) const
	{
		return false;
	}
};

/**
 * Adaptively maintains ideal safe distance from target
 * Moves closer if too far, farther if too close, or circle-strafes at ideal distance
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Adaptive Strafe At Ideal Distance"))
struct LQAISYSTEM_API FLQMovementTargetLocation_AdaptiveStrafe : public FLQMovementTargetLocationBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IdealSafeDistance = 800.0f;

	virtual bool GetTargetLocation(const FLQMovementTargetLocationContext& Context, FVector& OutLocation) const override;

	mutable bool bLeftDirection = false;
};

/**
 * Data container for keep safe distance movement settings
 */
USTRUCT(BlueprintType)
struct LQAISYSTEM_API FLQKeepSafeDistanceMovementData
{
	GENERATED_BODY()

	FLQKeepSafeDistanceMovementData();

	/** Ideal safe distance to maintain from target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Distance")
	float IdealSafeDistance = 700.0f;

	/** Policy for ending movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Distance")
	ELQEndMovementPolicy EndMovementPolicy = ELQEndMovementPolicy::ForDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Distance", meta = (InlineEditConditionToggle))
	bool bEndIfTooCloseTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Distance", meta = (EditCondition = "bEndIfTooCloseTarget == true"))
	float CloseDistanceToEnd = 300.0f;

	/** Base duration before choosing new strafe location (only for Duration policy) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Distance : For Duration", meta = (EditCondition = "EndMovementPolicy == ELQEndMovementPolicy::ForDuration"))
	float MovementDuration = 5.0f;

	/** Random variance (0 to value) added to movement and interval durations (only for Duration policy) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Distance : For Duration", meta = (EditCondition = "EndMovementPolicy == ELQEndMovementPolicy::ForDuration"))
	float DurationVariance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Distance : For Duration", meta = (EditCondition = "EndMovementPolicy == ELQEndMovementPolicy::ForDuration"))
	float AdditionalDistanceCheck = 50.0f;

	/** Strategy for getting target strafe location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Distance", meta = (ExcludeBaseStruct))
	TInstancedStruct<FLQMovementTargetLocationBase> TargetLocationStrategy {};
};

/**
 * Custom movement task
 */
UCLASS(meta = (DisplayName = "Movement"), Category = "LQ AI")
class LQAISYSTEM_API ULQBTTask_Movement : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	ULQBTTask_Movement();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;

protected:
	/** Type of movement behavior */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	ELQMovementType MovementType = ELQMovementType::Normal;

	/** Blackboard key for target actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition = "MovementType == ELQMovementType::KeepSafeDistance", EditConditionHides))
	FBlackboardKeySelector TargetActorBBKey;

	/** Blackboard key for target loation to move to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition = "MovementType == ELQMovementType::KeepSafeDistance", EditConditionHides))
	FBlackboardKeySelector DestinationLocationBBKey;

	/** Keep safe distance movement settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition = "MovementType == ELQMovementType::KeepSafeDistance", EditConditionHides))
	FLQKeepSafeDistanceMovementData KeepSafeDistanceData;

private:
	bool IsInSafeRange(APawn* OwnerPawn, AActor* TargetActor) const;
	bool IsLocationInSafeRange(const FVector& Location, AActor* TargetActor) const;
	bool IsTooCloseToTarget(APawn* OwnerPawn, AActor* TargetActor) const;
	bool GenNextStrafeLocation(APawn* OwnerPawn, AActor* TargetActor, FVector& OutLocation);

	float TimeElapsed = 0.0f;
	float CurrentMovementDuration = 0.0f;
	FVector LastStrafeLocation = FVector::ZeroVector;

	TWeakObjectPtr<AActor> CachedFocusActor;
	bool bCachedAllowStrafing = false;
};
