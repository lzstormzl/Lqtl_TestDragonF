#include "Dialogue/Dialogue.h"
#include "SocialComponent.h"
#include "Dialogue/DialogueBlueprintGeneratedClass.h"
#include "SocialDialogueSettings.h"
#include "Dialogue/TWDialogueNode.h"

UDialogue::UDialogue()
{
	SetFlags(RF_Public);
}

bool UDialogue::Initialize(class USocialComponent* InitializingComp, FName StartFromID)
{
    if (!HasAnyFlags(RF_ClassDefaultObject))
	{
        if(InitializingComp)
        {
            if (UDialogueBlueprintGeneratedClass* BGClass = Cast<UDialogueBlueprintGeneratedClass>(GetClass()))
            {
                BGClass->InitializeDialogue(this);

                if (NPCReplies.Num() == 0 || !RootDialogue)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Dialogue này không có NPC Reply nào!"));
                    return false;
                }

				UDialogueNode_NPC* StartDialogue = StartFromID.IsNone() ? RootDialogue : GetReplyByID(StartFromID);

				if (!StartDialogue && !StartFromID.IsNone())
				{
					StartDialogue = RootDialogue;
				}

                if(StartDialogue)
                {
                    OwningComp = InitializingComp;
					OwningController = OwningComp->GetOwningController();
					OwningPawn = OwningComp->GetOwningPawn();

					if (RootDialogue)
					{
						RootDialogue->OwningDialogue = this;
					}

                    // for (auto& Reply : NPCReplies)
			        // {
					//     if (Reply)
					//     {
					//         Reply->OwningDialogue = this;
					//     Reply->OwningComponent = OwningComp;
					//     }
				    // }

				    // for (auto& Reply : PlayerReplies)
				    // {
					//     if (Reply)
					//     {
					//         Reply->OwningDialogue = this;
					//         Reply->OwningComponent = OwningComp;
					//     }
				    // }

                    const bool bHasValidDialogue = GenerateDialogueChunk(StartDialogue);
                    if (!bHasValidDialogue)
					{
						 return false;
					}
                }
                return true;
            }       
        }
    }
    return false;
}

void UDialogue::DuplicateAndInitializeFromDialogue(UDialogue* DialogueTemplate)
{
    if(DialogueTemplate)
    {
        UDialogue* NewDialogue = Cast<UDialogue>(StaticDuplicateObject(DialogueTemplate, this, NAME_None, RF_Transactional));
        NewDialogue->SetFlags(RF_Transient | RF_DuplicateTransient);

        RootDialogue = NewDialogue->RootDialogue;
		NPCReplies = NewDialogue->NPCReplies;
		PlayerReplies = NewDialogue->PlayerReplies;
    }
}

TArray<UTWDialogueNode*> UDialogue::GetNodes() const
{
    TArray<UTWDialogueNode*> ReturnNodes;
    for (auto& Reply : NPCReplies)
	{
		ReturnNodes.Add(Reply);
	}
    return ReturnNodes;
}

void UDialogue::Play()
{

    if(NPCRepliesChain.Num())
    {
        PlayNextNPCReply();
    }
}

UDialogueNode_NPC* UDialogue::GetReplyByID(const FName& ID) const
{
	for (UDialogueNode_NPC* Reply : NPCReplies)
	{
		if (Reply->GetID() == ID)
		{
			return Reply;
		}
	}
	return nullptr;
}

void UDialogue::SkipCurrentLine()
{
	if (OwningComp && OwningComp->GetNetMode() == NM_Standalone)
	{
		//Skip whatever dialogue line is currently playing
		if (CurrentNode && CurrentNode->bIsSkippable)
		{
			EndCurrentLine();
		}
	}
}

void UDialogue::EndCurrentLine()
{
	if (CurrentNode)
	{
		//Unbind all listeners for line ending, they need to be reset up when the next line plays 
		// if (DialogueAudio)
		// {
		// 	DialogueAudio->OnAudioFinished.RemoveAll(this);
		// }

		// if (DialogueSequencePlayer && DialogueSequencePlayer->SequencePlayer)
		// {
		// 	DialogueSequencePlayer->SequencePlayer->OnFinished.RemoveAll(this);
		// }

		if (CurrentNode->IsA<UDialogueNode_NPC>())
		{
			// if (GetWorld())
			// {
			// 	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NPCReplyFinished);
			// }

			FinishNPCDialogue();
		}
		else
		{
			// if (GetWorld())
			// {
			// 	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PlayerReplyFinished);
			// }
			FinishPlayerDialogue();
		}
	}
}

void UDialogue::FinishNPCDialogue()
{
	//FString RoleString = OwningComp && OwningComp->HasAuthority() ? "Server" : "Client";
	//UE_LOG(LogNarrative, Warning, TEXT("FinishNPCDialogue called on %s with node %s"), *RoleString, *GetNameSafe(CurrentNode));

	if (UDialogueNode_NPC* NPCNode = Cast<UDialogueNode_NPC>(CurrentNode))
	{
		//FinishDialogueNode(NPCNode, CurrentLine, CurrentSpeaker);

		if (OwningComp)
		{
			//OwningComp->CompleteNarrativeDataTask(NAME_PlayDialogueNodeTask, NPCNode->GetID().ToString());

			ProcessNodeEvents(NPCNode, false);

			//We need to re-check OwningComp validity, as ProcessEvents may have ended this dialogue
			if (OwningComp)
			{
				//Call delegates and BPNativeEvents
				//OwningComp->OnNPCDialogueLineFinished.Broadcast(this, NPCNode, CurrentLine, CurrentSpeaker);
				//OnNPCDialogueLineFinished(NPCNode, CurrentLine, CurrentSpeaker);

				PlayNextNPCReply();
			}
		}
	}
}

void UDialogue::FinishPlayerDialogue()
{
	//FString RoleString = OwningComp && OwningComp->HasAuthority() ? "Server" : "Client";
	//UE_LOG(LogNarrative, Warning, TEXT("FinishPlayerDialogue called on %s with node %s"), *RoleString, *GetNameSafe(CurrentNode));
	//Players dialogue node has finished, generate the next chunk of dialogue! 
	if (UDialogueNode_Player* PlayerNode = Cast<UDialogueNode_Player>(CurrentNode))
	{
		if (!OwningComp || !PlayerNode)
		{
			UE_LOG(LogSocial, Verbose, TEXT("UDialogue::PlayerDialogueNodeFinished was called but had a null OwningComp or PlayerNode. "));
			return;
		}

		//FinishDialogueNode(PlayerNode, CurrentLine, CurrentSpeaker, CurrentSpeakerAvatar, CurrentListenerAvatar);

		//Call delegates and BPNativeEvents
		OwningComp->OnPlayerDialogueLineFinished.Broadcast(this, PlayerNode, CurrentLine);
		//OnPlayerDialogueLineFinished(PlayerNode, CurrentLine);

		//No need, generate dialogue chunk already did this: if (PlayerNode->AreConditionsMet(OwningPawn, OwningController, OwningComp))
		{
			
			ProcessNodeEvents(PlayerNode, false);

			if (OwningComp && OwningComp->HasAuthority())
			{
				//OwningComp->CompleteNarrativeDataTask(NAME_PlayDialogueNodeTask, PlayerNode->GetID().ToString());

				//Player selected a reply with nothing leading off it, dialogue has ended 
				if (PlayerNode->NPCReplies.Num() <= 0)
				{
					ExitDialogue();
					return;
				}

				//Find the first valid NPC reply after the option we selected
				UDialogueNode_NPC* NextReply = nullptr;

				for (auto& NextNPCReply : PlayerNode->NPCReplies)
				{
					if (/*NextNPCReply->AreConditionsMet(OwningPawn, OwningController, OwningComp)*/ 
						NextNPCReply->AreConditionsMet(nullptr,nullptr, OwningComp))
					{
						NextReply = NextNPCReply;
						break;
					}
				}

				//If we can generate more dialogue from the reply that was selected, do so, otherwise exit dialogue 
				if (GenerateDialogueChunk(NextReply))
				{
					//If we're shared, inform all party members a new chunk has arrived to play
					// if (OwningComp->IsSharedComponent())
					// {
					// 	for (UNarrativeComponent* GroupMemberComponent : OwningComp->GroupMembers)
					// 	{
					// 		if (GroupMemberComponent)
					// 		{
					// 			GroupMemberComponent->ClientRecieveDialogueChunk(MakeIDsFromNPCNodes(NPCReplyChain), MakeIDsFromPlayerNodes(AvailableResponses));
					// 		}
					// 	}
					// }

					//RPC the dialogue chunk to the client so it can play it
					ClientReceiveDialogueChunk(MakeIDsFromNPCNodes(NPCRepliesChain), MakeIDsFromPlayerNodes(AvailableResponses));

					Play();
				}
				else
				{
					UE_LOG(LogSocial, Warning, TEXT("No more chunks generated from response. Ending dialogue! "));
					ExitDialogue();
				}
			}
		}
	}
}

TArray<FName> UDialogue::MakeIDsFromNPCNodes(const TArray<UDialogueNode_NPC*> Nodes) const
{
	TArray<FName> IDs;

	for (auto& Node : Nodes)
	{
		IDs.Add(Node->GetID());
	}

	return IDs;
}

TArray<FName> UDialogue::MakeIDsFromPlayerNodes(const TArray<UDialogueNode_Player*> Nodes) const
{
	TArray<FName> IDs;

	for (auto& Node : Nodes)
	{
		IDs.Add(Node->GetID());
	}

	return IDs;
}

TArray<UDialogueNode_NPC*> UDialogue::GetNPCRepliesByIDs(const TArray<FName>& IDs) const
{
	TArray<UDialogueNode_NPC*> Replies;

	for (auto& ID : IDs)
	{
		for (auto& Reply : NPCReplies)
		{
			if (Reply && Reply->GetID() == ID)
			{
				Replies.Add(Reply);
				break;
			}
		}
	}

	return Replies;
}

TArray<UDialogueNode_Player*> UDialogue::GetPlayerRepliesByIDs(const TArray<FName>& IDs) const
{
	TArray<UDialogueNode_Player*> Replies;

	for (auto& ID : IDs)
	{
		for (auto& Reply : PlayerReplies)
		{
			if (Reply && Reply->GetID() == ID)
			{
				Replies.Add(Reply);
				break;
			}
		}
	}

	return Replies;
}

void UDialogue::ClientReceiveDialogueChunk(const TArray<FName>& NPCReplyIDs, const TArray<FName>& PlayerReplyIDs)
{	
	if (OwningComp && !OwningComp->HasAuthority())
	{
		//Resolve the nodes the server sent us 
		NPCRepliesChain = GetNPCRepliesByIDs(NPCReplyIDs);
		AvailableResponses = GetPlayerRepliesByIDs(PlayerReplyIDs);

		//Server ensures chunks are valid before sending them to us. If they aren't something has gone very wrong
		check(HasValidChunk());

		Play();
	}
}

void UDialogue::PlayNextNPCReply()
{
    if (NPCRepliesChain.IsValidIndex(0))
	{
		UDialogueNode_NPC* NPCNode = NPCRepliesChain[0];
		NPCRepliesChain.Remove(NPCNode);
		PlayNPCDialogueNode(NPCNode);
	}
	else //NPC has nothing left to say 
	{
		NPCFinishedTalking();
	}
}

void UDialogue::NPCFinishedTalking()
{
	//Ensure that a narrative event etc hasn't started a new dialogue
	if (AvailableResponses.Num() && OwningComp && OwningComp->CurrentDialogue == this)
	{
		// if (SharedDialoguePlayerAvatar)
		// {
		// 	SetSharedDialoguePlayerAvatar(nullptr);
		// }

		//if (const USocialDialogueSettings* DialogueSettings = GetDefault<USocialDialogueSettings>())
		//{
		//	if (/*DialogueSettings->bAutoSelectSingleResponse &&*/ AvailableResponses.Num() == 1)
		//	{
		//		OwningComp->SelectDialogueOption(AvailableResponses.Last());
		//		return;
		//	}
		//}

		//If a response is autoselect, select it and early out 
		for (auto& AvailableResponse : AvailableResponses)
		{
			if(AvailableResponse && AvailableResponse->IsAutoSelect())
			{
				OwningComp->SelectDialogueOption(AvailableResponse);
				return;
			}
		}

		//AActor* const PlayerAvatar = GetPlayerAvatar();
		//AActor* ListeningActor = NPCActor;

		//We want all the players to look at us since we need to talk. Set CurrentSpeakerAvatar temporarily to fool UpdateSpeakerRotations() into pointing everyone at us. 
		//AActor* OldCurrentSpeaker = CurrentSpeakerAvatar;
		//CurrentSpeakerAvatar = PlayerAvatar;

		// if (bAutoRotateSpeakers)
		// {
		// 	UpdateSpeakerRotations();
		// }

		// CurrentSpeakerAvatar = OldCurrentSpeaker;

		/**
		* Things like Over The Shoulder shots require a listener and a speaker to line up the shot, but since we're selecting a reply, the 
		* selecting reply shot will use the last NPC that spoke as the listener 
		*/
		// if (SpeakerAvatars.Contains(CurrentSpeaker.SpeakerID))
		// {
		// 	ListeningActor = SpeakerAvatars[CurrentSpeaker.SpeakerID];
		// }

		/*
		* Order of precendence for what camera shot to choose :
		*
		* 1. Last NPC node that played has a select reply sequence defined 
		* 2. Player speaker info has a sequence defined
		* 3. Player speaker info has a shot defined
		* 4. Dialogue has any shots added to its DialogueShots
		*/
		UDialogueNode_NPC* LastNode = Cast<UDialogueNode_NPC>(CurrentNode);

		// if (LastNode && LastNode->SelectingReplyShot)
		// {
		// 	PlayDialogueSequence(LastNode->SelectingReplyShot, PlayerAvatar, ListeningActor);
		// }
		// else if (PlayerSpeakerInfo.SelectingReplyShot)
		// {
		// 	PlayDialogueSequence(PlayerSpeakerInfo.SelectingReplyShot, PlayerAvatar, ListeningActor);
		// }
		// else if (DefaultDialogueShot)
		// {
		// 	PlayDialogueSequence(DefaultDialogueShot, PlayerAvatar, ListeningActor);
		// }

		//NPC has finished talking. Let UI know it can show the player replies. Shared comps don't need to broadcast this, clients put their own ones up
		OwningComp->OnDialogueRepliesAvailable.Broadcast(this, AvailableResponses);

		//Also make sure we stop playing any dialogue audio that was previously playing
		// if (DialogueAudio)
		// {
		// 	DialogueAudio->Stop();
		// 	DialogueAudio->DestroyComponent();
		// }
	}
	else
	{
		//There were no replies for the player, end the dialogue 
		ExitDialogue();
	}
}

void UDialogue::ExitDialogue()
{
	if (OwningComp)
	{
		// if (OwningComp->IsSharedComponent())
		// {
		// 	for (auto& GroupMemberComp : OwningComp->GroupMembers)
		// 	{
		// 		if (GroupMemberComp)
		// 		{
		// 			GroupMemberComp->ClientExitDialogue();
		// 		}
		// 	}
		// }

		OwningComp->ExitDialogue();
	}
}

void UDialogue::SelectDialogueOption(UDialogueNode_Player* Option)
{
	//Validate that the option that was selected is actually one of the available options
	if (AvailableResponses.Contains(Option))// GenerateDialogueChunk() already did this && Option->AreConditionsMet(OwningPawn, OwningController, OwningComp))
	{
		PlayPlayerDialogueNode(Option);

		if (OwningComp)
		{
			OwningComp->OnDialogueOptionSelected.Broadcast(this, Option);
		}
	}
}

void UDialogue::PlayPlayerDialogueNode(class UDialogueNode_Player* PlayerReply)
{
	//NPC replies should be fully gone before we play a player response
	check(!NPCRepliesChain.Num());
	check(OwningComp && PlayerReply);

	if (PlayerReply)
	{
		CurrentNode = PlayerReply;
		
		ProcessNodeEvents(PlayerReply, true);

		//If a node has no text, just process events then go to the next line 
		// if (PlayerReply->IsRoutingNode())
		// {
		// 	FinishPlayerDialogue();
		// 	return;
		// }

		//CurrentLine = PlayerReply->GetRandomLine(OwningComp->GetNetMode() == NM_Standalone);
        CurrentLine = PlayerReply->GetDialogueLine();
		//ReplaceStringVariables(PlayerReply, CurrentLine, CurrentLine.Text);

		if (OwningComp)
		{
			//Call delegates and BPNativeEvents
			OwningComp->OnPlayerDialogueLineStarted.Broadcast(this, PlayerReply, CurrentLine);
		}

		//OnPlayerDialogueLineStarted(PlayerReply, CurrentLine);

		//Actual playing of the node is inside a BlueprintNativeEvent so designers can override how NPC dialogues are played 
		//PlayPlayerDialogue(PlayerReply, CurrentLine);

		//CurrentSpeaker = PlayerSpeakerInfo;

		// const float Duration = GetLineDuration(CurrentNode, CurrentLine);

		// if (!FMath::IsNearlyEqual(Duration, -1.f))
		// {
		// 	if (Duration > 0.01f && GetWorld())
		// 	{
		// 		//Give the reply time to play, then play the next one! 
		// 		GetWorld()->GetTimerManager().SetTimer(TimerHandle_PlayerReplyFinished, this, &UDialogue::FinishPlayerDialogue, Duration, false);
		// 	}
		// 	else
		// 	{
		// 		FinishPlayerDialogue();
		// 	}
		// }

	}
}

void UDialogue::PlayNPCDialogueNode(class UDialogueNode_NPC* NPCReply)
{
	check(OwningComp && NPCReply);

	//FString RoleString = OwningComp && OwningComp->HasAuthority() ? "Server" : "Client";
	//UE_LOG(LogNarrative, Warning, TEXT("PlayNPCDialogueNode called on %s with node %s"), *RoleString, *GetNameSafe(NPCReply));

	if (NPCReply)
	{
		CurrentNode = NPCReply; // Set current node hiện tại là node này
		//CurrentLine = NPCReply->GetRandomLine(OwningComp->GetNetMode() == NM_Standalone); // lấy line dialogue hiện tại (bỏ qua phần net workmode) (Lấy line ngẫu nhiên)
        CurrentLine = NPCReply->GetDialogueLine();
		//ReplaceStringVariables(NPCReply, CurrentLine, CurrentLine.Text); // Cần tìm hiểu

		//CurrentSpeaker = GetSpeaker(NPCReply->SpeakerID); // Set current speaker

		ProcessNodeEvents(NPCReply, true); // Thực hiện Event của node

		//If a node has no text, just finish it, firing its events 
		// if (NPCReply->IsRoutingNode()) // Node không có gì thì skip
		// {
		// 	FinishNPCDialogue();
		// 	return;
		// }

		//Actual playing of the node is inside a BlueprintNativeEvent so designers can override how NPC dialogues are played 
		//PlayNPCDialogue(NPCReply, CurrentLine, CurrentSpeaker);

		if (OwningComp)
		{
			//Call delegates and BPNativeEvents
			OwningComp->OnNPCDialogueLineStarted.Broadcast(this, NPCReply, CurrentLine);
		}

		//OnNPCDialogueLineStarted(NPCReply, CurrentLine, CurrentSpeaker);

		//const float Duration = GetLineDuration(CurrentNode, CurrentLine);

		// if (!FMath::IsNearlyEqual(Duration, -1.f))
		// {
		// 	//if (Duration > 0.01f && GetWorld())
		// 	{
		// 		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NPCReplyFinished);
		// 		//Give the reply time to play, then play the next one! 
		// 		GetWorld()->GetTimerManager().SetTimer(TimerHandle_NPCReplyFinished, this, &UDialogue::FinishNPCDialogue, Duration, false);
		// 	}
		// 	else
		// 	{
		// 		FinishNPCDialogue();
		// 	}
		// }
	}
	else 
	{
		//Somehow we were given a null NPC reply to play, just try play the next one
		PlayNextNPCReply();
	}
}

bool UDialogue::GenerateDialogueChunk(UDialogueNode_NPC* Node)
{
	if (Node /* && OwningComp && OwningComp->HasAuthority()*/ )
	{	
		//Generate the NPC reply chain
		NPCRepliesChain = Node->GetReplyChain(OwningComp);

		//Grab all the players responses to the last thing the NPC had to say
		if (NPCRepliesChain.Num() && NPCRepliesChain.IsValidIndex(NPCRepliesChain.Num() - 1))
		{
			if (UDialogueNode_NPC* LastNPCNode = NPCRepliesChain.Last())
			{
				AvailableResponses = LastNPCNode->GetPlayerReplies(OwningComp);
			}
		}

		//Did we generate a valid chunk?
		if (HasValidChunk())
		{
			return true;
		}
	}

	return false;
}

bool UDialogue::HasValidChunk() const
{
	bool bReplyChainHasContent = false;

	// Kiểm tra coi có NPC Reply nào không
	for (auto& Reply : NPCRepliesChain)
	{
		if (Reply)
		{
			bReplyChainHasContent = true;
			break;
		}
	}

	// Nếu có Player Response hoặc NPC Reply thì trả về true
    if (AvailableResponses.Num() || bReplyChainHasContent)
	{
		return true;
	}
    return false;
}

void UDialogue::ProcessNodeEvents(class UTWDialogueNode* Node, bool bStartEvents)
{
	if (Node)
	{
		struct SOnPlayedStruct
		{
			UTWDialogueNode* Node;
			bool bStarted;
		};

		SOnPlayedStruct Parms;
		Parms.Node = Node;
		Parms.bStarted = bStartEvents;

		if (UFunction* Func = FindFunction(Node->OnPlayNodeFuncName))
		{
			ProcessEvent(Func, &Parms);
		}

		Node->ProcessEvents(OwningPawn, OwningController, OwningComp, bStartEvents ? EEventRuntime::Start : EEventRuntime::End);
	}
}