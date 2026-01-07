// Copyright Tale Weavers

#include "BehaviorTree/Task/LQBTTask_Movement.h"
#include "LQAITypes.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LQAIFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

bool FLQMovementTargetLocation_AdaptiveStrafe::GetTargetLocation(const FLQMovementTargetLocationContext& Context, FVector& OutLocation) const
{
	if (!Context.IsValid())
	{
		return false;
	}

	const FVector TargetLocation = Context.TargetActor->GetActorLocation();
	const FVector OwnerLocation = Context.OwnerActor->GetActorLocation();

	// Calculate 2D distance (horizontal plane only)
	FVector DeltaToTarget = TargetLocation - OwnerLocation;
	DeltaToTarget.Z = 0;
	const float CurrentDistance = DeltaToTarget.Size();

	// Direction from target to owner (normalized)
	FVector DirectionFromTarget = -DeltaToTarget.GetSafeNormal();

	// Define tolerance for "at ideal distance" (10% of ideal distance)
	const float DistanceTolerance = IdealSafeDistance * 0.1f;
	const float DistanceDifference = FMath::Abs(CurrentDistance - IdealSafeDistance);

	FVector DesiredLocation;

	if (DistanceDifference <= DistanceTolerance)
	{
		// At ideal distance - circle strafe perpendicular to target direction
		const float StrafeAngle = bLeftDirection ? 90.0f : -90.0f;
		const FVector StrafeDirection = DirectionFromTarget.RotateAngleAxis(StrafeAngle, FVector::UpVector);

		// Choose strafe distance that ensures meaningful movement
		// Should be larger than AcceptableRadius + AdditionalDistanceCheck to avoid constant re-targeting
		const float MinStrafeDistance = 200.0f;
		const float MaxStrafeDistance = 400.0f;
		const float StrafeDistance = FMath::FRandRange(MinStrafeDistance, MaxStrafeDistance);

		DesiredLocation = OwnerLocation + (StrafeDirection * StrafeDistance);

		// Ensure the strafe location maintains ideal distance from target
		FVector DeltaToNewLocation = DesiredLocation - TargetLocation;
		DeltaToNewLocation.Z = 0;
		const FVector DirectionToNewLocation = DeltaToNewLocation.GetSafeNormal();
		DesiredLocation = TargetLocation + (DirectionToNewLocation * IdealSafeDistance);
	}
	else
	{
		// Not at ideal distance - move toward ideal distance with angular variation
		const float AngleVariation = FMath::FRandRange(0.0f, 45.0f);
		const float AdjustedAngle = bLeftDirection ? AngleVariation : -AngleVariation;
		const FVector AdjustedDirection = DirectionFromTarget.RotateAngleAxis(AdjustedAngle, FVector::UpVector);
		DesiredLocation = TargetLocation + (AdjustedDirection * IdealSafeDistance);
	}

	// Maintain the target's Z coordinate as base
	DesiredLocation.Z = TargetLocation.Z;

	// Try to project to navmesh
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Context.OwnerActor->GetWorld());
	if (NavSys)
	{
		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(DesiredLocation, NavLocation, FVector(200.0f, 200.0f, 200.0f)))
		{
			DesiredLocation = NavLocation.Location;
		}
	}

	OutLocation = DesiredLocation;
	return true;
}

ULQBTTask_Movement::ULQBTTask_Movement()
{
	NodeName = "Movement";
	bNotifyTick = true;

	TargetActorBBKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ULQBTTask_Movement, TargetActorBBKey), AActor::StaticClass());
	TargetActorBBKey.AllowNoneAsValue(false);

	DestinationLocationBBKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ULQBTTask_Movement, DestinationLocationBBKey));
	DestinationLocationBBKey.AllowNoneAsValue(false);
}

EBTNodeResult::Type ULQBTTask_Movement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (MovementType == ELQMovementType::Normal)
	{
		// Use default UBTTask_MoveTo behavior with original BlackboardKey
		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}

	if (MovementType == ELQMovementType::KeepSafeDistance)
	{
		AAIController* AIController = OwnerComp.GetAIOwner();
		if (!AIController)
		{
			return EBTNodeResult::Failed;
		}

		APawn* OwnerPawn = AIController->GetPawn();
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (!OwnerPawn || !BlackboardComp)
		{
			return EBTNodeResult::Failed;
		}

		if (TargetActorBBKey.IsNone())
		{
			UE_LOG(LogAI, Warning, TEXT("ULQBTTask_Movement::ExecuteTask: 'TargetActorBBKey' is none for (%s)"), *OwnerPawn->GetName());
			return EBTNodeResult::Failed;
		}

		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorBBKey.SelectedKeyName));
		if (!TargetActor)
		{
			UE_LOG(LogAI, Warning, TEXT("ULQBTTask_Movement::ExecuteTask: null target in blackboard key 'TargetActorBBKey' (%s) for (%s)"), *TargetActorBBKey.SelectedKeyName.ToString(), *OwnerPawn->GetName());
			return EBTNodeResult::Failed;
		}

		if (DestinationLocationBBKey.IsNone())
		{
			UE_LOG(LogAI, Warning, TEXT("ULQBTTask_Movement::ExecuteTask: 'DestinationLocationBBKey' movement destination is not set for (%s)"), *OwnerPawn->GetName());
			return EBTNodeResult::Failed;
		}

		// Cache current focus state
		CachedFocusActor = AIController->GetFocusActor();

		// Cache current strafe state and enable strafing
		if (ACharacter* Character = Cast<ACharacter>(OwnerPawn))
		{
			if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
			{
				bCachedAllowStrafing = MovementComp->bUseControllerDesiredRotation;
				MovementComp->bUseControllerDesiredRotation = true;
				UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::ExecuteTask: Enabled strafing (cached: %s)"), bCachedAllowStrafing ? TEXT("true") : TEXT("false"));
			}
		}

		// Set focus to target actor
		AIController->SetFocus(TargetActor);
		UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::ExecuteTask: Set focus to %s (cached: %s)"), *TargetActor->GetName(), CachedFocusActor.IsValid() ? *CachedFocusActor->GetName() : TEXT("None"));

		// If already in safe range and using InSafeRange policy, succeed immediately
		if (KeepSafeDistanceData.EndMovementPolicy == ELQEndMovementPolicy::ReachTarget && IsInSafeRange(OwnerPawn, TargetActor))
		{
			return EBTNodeResult::Succeeded;
		}

		// Initialize timing with random variance
		TimeElapsed = 0.0f;
		CurrentMovementDuration = KeepSafeDistanceData.MovementDuration + FMath::FRandRange(0.0f, KeepSafeDistanceData.DurationVariance);
		LastStrafeLocation = FVector::ZeroVector;

		// Initialize strafe direction if using AdaptiveStrafe strategy
		if (KeepSafeDistanceData.TargetLocationStrategy.IsValid())
		{
			if (FLQMovementTargetLocation_AdaptiveStrafe* AdaptiveStrafe = KeepSafeDistanceData.TargetLocationStrategy.GetMutablePtr<FLQMovementTargetLocation_AdaptiveStrafe>())
			{
				AdaptiveStrafe->bLeftDirection = FMath::RandBool();
				UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::ExecuteTask: Set strafe direction to %s"), AdaptiveStrafe->bLeftDirection ? TEXT("Left") : TEXT("Right"));
			}
		}

		// Copy safe distance key to parent blackboard key
		BlackboardKey = DestinationLocationBBKey;
		BlackboardKey.ResolveSelectedKey(*BlackboardComp->GetBlackboardAsset());

		// Get strafe location and write to destination blackboard key
		FVector StrafeLocation;
		bool bFoundStrafeLocation = GenNextStrafeLocation(OwnerPawn, TargetActor, StrafeLocation);
		if (bFoundStrafeLocation)
		{
			BlackboardComp->SetValueAsVector(BlackboardKey.SelectedKeyName, StrafeLocation);
			return Super::ExecuteTask(OwnerComp, NodeMemory);
		}
		return EBTNodeResult::Failed;
	}

	UE_LOG(LogAI, Warning, TEXT("ULQBTTask_Movement::ExecuteTask: Unknown 'MovementType' value"));
	return EBTNodeResult::Failed;
}

void ULQBTTask_Movement::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (MovementType != ELQMovementType::KeepSafeDistance)
	{
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* OwnerPawn = AIController ? AIController->GetPawn() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!OwnerPawn || !BlackboardComp)
	{
		UE_VLOG(AIController, LogAI, Verbose, TEXT("ULQBTTask_Movement::TickTask: Missing OwnerPawn or BlackboardComp, finishing with Failed"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorBBKey.SelectedKeyName));
	if (!TargetActor)
	{
		UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::TickTask: TargetActor is null, finishing with Failed"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Check if too close to target and should end
	if (KeepSafeDistanceData.bEndIfTooCloseTarget && IsTooCloseToTarget(OwnerPawn, TargetActor))
	{
		const float CurrentDistance = FVector::Dist(OwnerPawn->GetActorLocation(), TargetActor->GetActorLocation());
		UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::TickTask: Too close to target (%.2f <= %.2f), finishing with Succeeded"), CurrentDistance, KeepSafeDistanceData.CloseDistanceToEnd);
		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	switch (KeepSafeDistanceData.EndMovementPolicy)
	{
		case ELQEndMovementPolicy::ReachTarget: 
		{
			// Check if in safe range
			if (IsInSafeRange(OwnerPawn, TargetActor))
			{
				UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::TickTask: Reached safe range with ReachTarget policy, finishing with Succeeded"));
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return;
			}
			break;
		}
		case ELQEndMovementPolicy::ForDuration:
		{
			TimeElapsed += DeltaSeconds;
			if (TimeElapsed >= CurrentMovementDuration)
			{
				UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::TickTask: Duration elapsed (%.2f >= %.2f), finishing with Succeeded"), TimeElapsed, CurrentMovementDuration);
				AIController->StopMovement();
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}

			if (UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent())
			{
				const EPathFollowingReachMode ReachMode = bReachTestIncludesAgentRadius.GetValue(BlackboardComp) ?
					(bReachTestIncludesGoalRadius.GetValue(BlackboardComp) ? EPathFollowingReachMode::OverlapAgentAndGoal : EPathFollowingReachMode::OverlapAgent) :
					(bReachTestIncludesGoalRadius.GetValue(BlackboardComp) ? EPathFollowingReachMode::OverlapGoal : EPathFollowingReachMode::ExactLocation);

				if (PathFollowingComp->HasReached(LastStrafeLocation, ReachMode, AcceptableRadius.GetValue(BlackboardComp) + KeepSafeDistanceData.AdditionalDistanceCheck))
				{
					// Near destination, generate new strafe location and request move
					FVector StrafeLocation;
					const bool bFoundLocationResult = GenNextStrafeLocation(OwnerPawn, TargetActor, StrafeLocation);
					if (bFoundLocationResult)
					{
						BlackboardComp->SetValueAsVector(BlackboardKey.SelectedKeyName, StrafeLocation);
						Super::ExecuteTask(OwnerComp, NodeMemory);
					}
					else
					{
						UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::TickTask: Failed to generate next strafe location, finishing with Succeeded"));
						FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					}
				}
			}
		}
	}
}

void ULQBTTask_Movement::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (MovementType == ELQMovementType::KeepSafeDistance)
	{
		AAIController* AIController = OwnerComp.GetAIOwner();
		if (AIController)
		{
			APawn* OwnerPawn = AIController->GetPawn();

			// Restore focus state
			if (CachedFocusActor.IsValid())
			{
				AIController->SetFocus(CachedFocusActor.Get());
				UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::OnTaskFinished: Restored focus to %s"), *CachedFocusActor->GetName());
			}
			else
			{
				AIController->ClearFocus(EAIFocusPriority::Gameplay);
				UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::OnTaskFinished: Cleared focus"));
			}

			// Restore strafe state
			if (ACharacter* Character = Cast<ACharacter>(OwnerPawn))
			{
				if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
				{
					MovementComp->bUseControllerDesiredRotation = bCachedAllowStrafing;
					UE_VLOG(OwnerPawn, LogAI, Verbose, TEXT("ULQBTTask_Movement::OnTaskFinished: Restored strafing to %s"), bCachedAllowStrafing ? TEXT("true") : TEXT("false"));
				}
			}
		}
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void ULQBTTask_Movement::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	UE_LOG(LogAI, Verbose, TEXT("ULQBTTask_Movement::TickTask: finished with gameplay task deactivated"));
	Super::OnGameplayTaskDeactivated(Task);
}

bool ULQBTTask_Movement::IsInSafeRange(APawn* OwnerPawn, AActor* TargetActor) const
{
	if (!OwnerPawn || !TargetActor)
	{
		return false;
	}

	const float Distance = FVector::Dist(OwnerPawn->GetActorLocation(), TargetActor->GetActorLocation());
	const float DistanceTolerance = KeepSafeDistanceData.IdealSafeDistance * 0.1f;
	const float DistanceDifference = FMath::Abs(Distance - KeepSafeDistanceData.IdealSafeDistance);
	return DistanceDifference <= DistanceTolerance;
}

bool ULQBTTask_Movement::IsLocationInSafeRange(const FVector& Location, AActor* TargetActor) const
{
	if (!TargetActor)
	{
		return false;
	}

	const float Distance = FVector::Dist(Location, TargetActor->GetActorLocation());
	const float DistanceTolerance = KeepSafeDistanceData.IdealSafeDistance * 0.1f;
	const float DistanceDifference = FMath::Abs(Distance - KeepSafeDistanceData.IdealSafeDistance);
	return DistanceDifference <= DistanceTolerance;
}

bool ULQBTTask_Movement::IsTooCloseToTarget(APawn* OwnerPawn, AActor* TargetActor) const
{
	if (!OwnerPawn || !TargetActor)
	{
		return false;
	}

	const float Distance = FVector::Dist(OwnerPawn->GetActorLocation(), TargetActor->GetActorLocation());
	return Distance <= KeepSafeDistanceData.CloseDistanceToEnd;
}

bool ULQBTTask_Movement::GenNextStrafeLocation(APawn* OwnerPawn, AActor* TargetActor, FVector& OutLocation)
{
	if (!KeepSafeDistanceData.TargetLocationStrategy.IsValid())
	{
		return false;
	}

	const FLQMovementTargetLocationBase& Strategy = KeepSafeDistanceData.TargetLocationStrategy.Get<FLQMovementTargetLocationBase>();
	FVector NewStrafeLocation;
	bool bResult = Strategy.GetTargetLocation(FLQMovementTargetLocationContext(OwnerPawn, TargetActor), NewStrafeLocation);
	if (bResult)
	{
		LastStrafeLocation = NewStrafeLocation;
		OutLocation = NewStrafeLocation;
	}
	return bResult;
}

FLQKeepSafeDistanceMovementData::FLQKeepSafeDistanceMovementData()
{
	TargetLocationStrategy = TInstancedStruct<FLQMovementTargetLocationBase>::Make<FLQMovementTargetLocation_AdaptiveStrafe>();
}

