#include "Quest/QuestTask.h"
#include "Quest/Quest.h"
#include "SocialComponent.h"
#include "Quest/QuestSM.h"

#include <TimerManager.h>


#define LOCTEXT_NAMESPACE "QuestTask"

UQuestTask::UQuestTask()
{
	CurrentProgress = 0;
	RequiredQuantity = 1;
	bOptional = false;
	bHidden = false;
	TickInterval = 0.f;
	bIsActive = false;
}

void UQuestTask::BeginTask()
{

	bIsActive = true;

	//If we come back to a state we've been at before in a quest, we need to do the task again so reset any previous progress 
	if (OwningComp && !OwningComp->bIsLoading)
	{
		CurrentProgress = 0;
	}

	//Cache all the useful values tasks will want
	if (UQuestBranch* OwningBranch = GetOwningBranch())
	{
		OwningQuest = OwningBranch->GetOwningQuest();

		if (OwningQuest)
		{
			OwningComp = OwningQuest->GetOwningSocialComponent();
		
			if (OwningComp)
			{
				OwningPawn = OwningComp->GetOwningPawn();
				OwningController = OwningComp->GetOwningController();
			}
		}
	}

	K2_BeginTask();

	if (OwningComp && OwningComp->HasAuthority())
	{
		if (TickInterval > 0.f)
		{
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimer(TimerHandle_TickTask, this, &UQuestTask::TickTask, TickInterval, true);
			}
		}

		//Fire the first tick off after BeginTask since begin task will usually init things that TickTask may need
		TickTask();
	}
}

void UQuestTask::TickTask_Implementation()
{

}

void UQuestTask::EndTask()
{
	if (bIsActive)
	{
		bIsActive = false;

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(TimerHandle_TickTask);
		}

		K2_EndTask();
	}
}

void UQuestTask::SetProgress(const int32 NewProgress)
{

	/*If we're loading OwningComp may be invalid as BeginTask hasnt cached it yet. 
	//If we're just loading a save, set the progress but don't bother updating any quest stuff except
	//for on the current state (this is why we also check bIsActive)*/
	if (!OwningComp || (OwningComp->bIsLoading && !bIsActive))
	{
		CurrentProgress = FMath::Clamp(NewProgress, 0, RequiredQuantity);
		return;
	}

	if (bIsActive && NewProgress >= 0)
	{
		if (NewProgress != CurrentProgress)
		{
			const int32 OldProgress = CurrentProgress;

			CurrentProgress = FMath::Clamp(NewProgress, 0, RequiredQuantity);

			//Dont use IsComplete() because it would check if the task is optional which we don't want 
			if (CurrentProgress >= RequiredQuantity)
			{
				K2_OnTaskCompleted();

				if (UQuestBranch* Branch = GetOwningBranch())
				{
					Branch->OnQuestTaskComplete(this);
				}
			}

			if (OwningQuest)
			{
				OwningQuest->OnQuestTaskProgressChanged(this, GetOwningBranch(), OldProgress, CurrentProgress);
			}
		}
	}
}

void UQuestTask::AddProgress(const int32 ProgressToAdd /*= 1*/)
{
	SetProgress(CurrentProgress + ProgressToAdd);
}

bool UQuestTask::IsComplete() const
{
	return CurrentProgress >= RequiredQuantity || bOptional;
}

void UQuestTask::CompleteTask()
{
	SetProgress(RequiredQuantity);
}

UQuestBranch* UQuestTask::GetOwningBranch() const
{
	return Cast<UQuestBranch>(GetOuter());
}

FText UQuestTask::GetTaskDescription_Implementation() const
{
	return LOCTEXT("DefaultNarrativeTaskDescription", "Task Description");
}

FText UQuestTask::GetTaskProgressText_Implementation() const
{
	return FText::Format(LOCTEXT("ProgressText", "({0}/{1})"), CurrentProgress, RequiredQuantity);
}

FText UQuestTask::GetTaskNodeDescription_Implementation() const
{
	return GetTaskDescription();
}

#undef LOCTEXT_NAMESPACE