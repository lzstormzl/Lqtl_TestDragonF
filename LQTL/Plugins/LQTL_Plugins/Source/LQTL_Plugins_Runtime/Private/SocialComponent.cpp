#include "SocialComponent.h"


DEFINE_LOG_CATEGORY(LogSocial);

// Sets default values for this component's properties
USocialComponent::USocialComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USocialComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USocialComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool USocialComponent::StartDialogue(TSubclassOf<UDialogue> DialogueClass, FName StartFromID)
{
	// Implement dialogue starting logic here
	if(IsValid(DialogueClass))
	{
		/**Nếu đang có 1 dialogue đang chạy thì make sure finish dialogue đó trước khi chạy dialogue mới */
		if (CurrentDialogue)
		{
			OnDialogueFinished.Broadcast(CurrentDialogue); // Call tới để dialogue Finish

			//CurrentDialogue->Deinitialize();
			CurrentDialogue = nullptr;
		}

		CurrentDialogue = MakeDialogue(DialogueClass, StartFromID);

		if (CurrentDialogue)
		{
			OnDialogueStarted.Broadcast(CurrentDialogue);

			CurrentDialogue->OwningComp = this;
			CurrentDialogue->Play();
		}
		return true;
	}

	return false;
}

void USocialComponent::SelectDialogueOption(class UDialogueNode_Player* Option)
{
	if (Option)
	{
		if (CurrentDialogue)
		{
			// if (!HasAuthority())
			// {
			// 	ServerSelectDialogueOption(Option->GetID());
			// }

			CurrentDialogue->SelectDialogueOption(Option);

		}//No local dialogue running, must be a shared one? 
		// else if (SharedNarrativeComp && SharedNarrativeComp->CurrentDialogue)
		// {
		// 	if (!HasAuthority())
		// 	{
		// 		ServerSelectSharedDialogueOption(Option->GetID());
		// 	}
		// 	else
		// 	{
		// 		SharedNarrativeComp->CurrentDialogue->SelectDialogueOption(Option);

		// 		for (auto& GroupMember : SharedNarrativeComp->GroupMembers)
		// 		{
		// 			if (GroupMember != this)
		// 			{
		// 				GroupMember->ClientSelectDialogueOption(Option->GetID(), GetOwningPawn());
		// 			}
		// 		}
		// 	}

		// 	//Conditions are all verified, we don't need to wait for the server to replicate this back to us
		// 	SharedNarrativeComp->CurrentDialogue->SelectDialogueOption(Option);
		// }
	}
}

void USocialComponent::ExitDialogue()
{
	if (CurrentDialogue)
	{
		if (HasAuthority())
		{
			ClientExitDialogue();
		}

		OnDialogueFinished.Broadcast(CurrentDialogue);

		//CurrentDialogue->Deinitialize();
		CurrentDialogue = nullptr;
	}
	// else if (SharedNarrativeComp && SharedNarrativeComp->CurrentDialogue)
	// {
	// 	if (HasAuthority())
	// 	{
	// 		for (auto& GroupMember : SharedNarrativeComp->GroupMembers)
	// 		{
	// 			if (GroupMember)
	// 			{
	// 				GroupMember->ClientExitDialogue();
	// 			}
	// 		}
	// 	}

	// 	OnDialogueFinished.Broadcast(SharedNarrativeComp->CurrentDialogue);

	// 	SharedNarrativeComp->CurrentDialogue->Deinitialize();
	// 	SharedNarrativeComp->CurrentDialogue = nullptr;
	// }
}

void USocialComponent::ClientExitDialogue_Implementation()
{
	if (!HasAuthority())
	{
		ExitDialogue();
	}
}

bool USocialComponent::HasAuthority() const
{
	return GetOwnerRole() >= ROLE_Authority;
}

APawn* USocialComponent::GetOwningPawn() const
{

	if (OwnerPC)
	{
		return OwnerPC->GetPawn();
	}

	APlayerController* OwningController = Cast<APlayerController>(GetOwner());
	APawn* OwningPawn = Cast<APawn>(GetOwner());

	if (OwningPawn)
	{
		return OwningPawn;
	}

	if (!OwningPawn && OwningController)
	{
		return OwningController->GetPawn();
	}

	return nullptr;
}

APlayerController* USocialComponent::GetOwningController() const
{
	//We cache this on beginplay as to not re-find it every time 
	if (OwnerPC)
	{
		return OwnerPC;
	}

	APlayerController* OwningController = Cast<APlayerController>(GetOwner());
	APawn* OwningPawn = Cast<APawn>(GetOwner());

	if (OwningController)
	{
		return OwningController;
	}

	if (!OwningController && OwningPawn)
	{
		return Cast<APlayerController>(OwningPawn->GetController());
	}

	return nullptr;
}

UDialogue* USocialComponent::GetCurrentDialogue() const
{
	return CurrentDialogue;
}

UDialogue* USocialComponent::MakeDialogue(TSubclassOf<UDialogue> DialogueClass, FName StartFromID)
{
	if(!IsValid(DialogueClass))
	{
		return nullptr;
	}

	if(UDialogue* NewDialogue = NewObject<UDialogue>(GetOwner(), DialogueClass))
	{
		// TODO: Khởi tạo các biến của Dialogue nếu cần thiết
		if(NewDialogue->Initialize(this, StartFromID))
		{
			return NewDialogue;
		}
	}
	return nullptr;
}

class UQuest* USocialComponent::BeginQuest(TSubclassOf<class UQuest> QuestClass, FName StartFromID /*= NAME_None*/)
{

	if (QuestClass == UQuest::StaticClass())
	{
		UE_LOG(LogSocial, Warning, TEXT("BeginQuest was passed UQuest. Supplied quest must be a child of UQuest. "));
		return nullptr;
	}

	if (UQuest* NewQuest = CreateQuest(QuestClass))
	{
		// //If loading from save file don't send update since server will batch all quests and send them to client to begin 
		//  if (!bIsLoading && HasAuthority() && GetNetMode() != NM_Standalone)
		// {
		// 	SendNarrativeUpdate(FNarrativeUpdate::BeginQuest(QuestClass, StartFromID));
		// }

		/*Call this after SendNarrativeUpdate as BeginQuest itself may trigger another SendNarrativeUpdate and we want BeginQuest to get called
		before any other narrative updates come through*/
		NewQuest->BeginQuest(StartFromID);

		return NewQuest;
	}

	return nullptr;
}

class UQuest* USocialComponent::CreateQuest(TSubclassOf<class UQuest> QuestClass)
{
	if (IsValid(QuestClass))
	{
		//If the quest is already in the players quest list issue a warning
		if (IsValid(GetQuest(QuestClass)))
		{
			UE_LOG(LogSocial, Warning, TEXT("Component was asked to begin a quest the player is already doing. Use RestartQuest() to replay a started quest. "));
			return nullptr;
		}

		if (UQuest* NewQuest = NewObject<UQuest>(GetOwner(), QuestClass))
		{
			const bool bInitializedSuccessfully = NewQuest->Initialize(this);

			if (bInitializedSuccessfully)
			{
				QuestList.Add(NewQuest);
				return NewQuest;
			}
		}
	}
	return nullptr;
}

class UQuest* USocialComponent::GetQuest(TSubclassOf<class UQuest> QuestClass) const
{
	for (auto& QIP : QuestList)
	{
		if (QIP && QIP->GetClass()->IsChildOf(QuestClass))
		{
			return QIP;
		}
	}
	return nullptr;
}

TArray<UQuest*> USocialComponent::GetFailedQuests() const
{
	TArray<UQuest*> FailedQuests;
	for (auto QIP : QuestList)
	{
		if (QIP->QuestCompletion == EQuestCompletion::QC_Failed)
		{
			FailedQuests.Add(QIP);
		}
	}
	return FailedQuests;
}


TArray<UQuest*> USocialComponent::GetSucceededQuests() const
{
	TArray<UQuest*> SucceededQuests;
	for (auto QIP : QuestList)
	{
		if (QIP->QuestCompletion == EQuestCompletion::QC_Succeded)
		{
			SucceededQuests.Add(QIP);
		}
	}
	return SucceededQuests;
}

TArray<UQuest*> USocialComponent::GetInProgressQuests() const
{
	TArray<UQuest*> InProgressQuests;
	for (auto QIP : QuestList)
	{
		if (QIP->QuestCompletion == EQuestCompletion::QC_Started)
		{
			InProgressQuests.Add(QIP);
		}
	}
	return InProgressQuests;
}