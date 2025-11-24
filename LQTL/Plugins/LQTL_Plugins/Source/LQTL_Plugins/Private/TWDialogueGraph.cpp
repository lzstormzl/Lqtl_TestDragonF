#include "TWDialogueGraph.h"

#include "DialogueGraphNode.h"
#include "DialogueGraphNode_NPC.h"
#include "DialogueGraphNode_Player.h"
#include "DialogueGraphNode_Root.h"
#include "Dialogue/TWDialogueNode.h"
#include "DialogueBlueprint.h"
#include "TWDialogueEditorSettings.h"

void UTWDialogueGraph::NotifyGraphChanged(const FEdGraphEditAction& Action)
{
    Super::NotifyGraphChanged(Action);

	if (Action.Action == EEdGraphActionType::GRAPHACTION_RemoveNode)
	{
		if (UDialogue* Dialogue = Cast<UDialogueBlueprint>(GetOuter())->DialogueTemplate)
		{
			for (auto& DeletedNode : Action.Nodes)
			{
				if (UDialogueGraphNode* DialogueGraphNode = const_cast<UDialogueGraphNode*>(Cast<UDialogueGraphNode>(DeletedNode)))
				{
					//NPC node was deleted, go through all nodes and remove 
					if (UDialogueGraphNode_NPC* NPCNode = Cast<UDialogueGraphNode_NPC>(DialogueGraphNode))
					{
						for (auto& NPCR : Dialogue->NPCReplies)
						{
							if (NPCR)
							{
								NPCR->NPCReplies.Remove(CastChecked<UDialogueNode_NPC>(NPCNode->TWDialogueNode));
							}
						}

						for (auto& PR : Dialogue->PlayerReplies)
						{
							if (PR)
							{
								PR->NPCReplies.Remove(CastChecked<UDialogueNode_NPC>(NPCNode->TWDialogueNode));
							}
						}

						Dialogue->NPCReplies.Remove(CastChecked<UDialogueNode_NPC>(NPCNode->TWDialogueNode));
					}
					else if (UDialogueGraphNode_Player* PlayerNode = Cast<UDialogueGraphNode_Player>(DialogueGraphNode))
					{
						for (auto& NPCR : Dialogue->NPCReplies)
						{
							NPCR->PlayerReplies.Remove(CastChecked<UDialogueNode_Player>(PlayerNode->TWDialogueNode));
						}

						for (auto& PR : Dialogue->PlayerReplies)
						{
							PR->PlayerReplies.Remove(CastChecked<UDialogueNode_Player>(PlayerNode->TWDialogueNode));
						}

						Dialogue->PlayerReplies.Remove(CastChecked<UDialogueNode_Player>(PlayerNode->TWDialogueNode));
					}
				}
			}
		}
	}
}       

void UTWDialogueGraph::PinRewired(UDialogueGraphNode* Node, UEdGraphPin* Pin)
{
	if (UDialogue* Dialogue = Cast<UDialogueBlueprint>(GetOuter())->DialogueTemplate)
	{
		//Output pin got wired into something so set it 
		if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
		{
			if (UDialogueGraphNode* WiredFrom = Cast<UDialogueGraphNode>(Node))
			{
				if (WiredFrom->TWDialogueNode)
				{
					//Rebuild replies
					WiredFrom->TWDialogueNode->NPCReplies.Empty();
					WiredFrom->TWDialogueNode->PlayerReplies.Empty();

					for (auto& LinkedToPin : Pin->LinkedTo)
					{
						//Figure out what we wired into and update the related array 
						if (UDialogueGraphNode_NPC* WiredToNPC = Cast<UDialogueGraphNode_NPC>(LinkedToPin->GetOwningNode()))
						{
							WiredFrom->TWDialogueNode->NPCReplies.AddUnique(CastChecked<UDialogueNode_NPC>(WiredToNPC->TWDialogueNode));
						}
						else if (UDialogueGraphNode_Player* WiredToPlayer = Cast<UDialogueGraphNode_Player>(LinkedToPin->GetOwningNode()))
						{
							WiredFrom->TWDialogueNode->PlayerReplies.AddUnique(CastChecked<UDialogueNode_Player>(WiredToPlayer->TWDialogueNode));
						}
					}
				}
			}
		}
	}
}

void UTWDialogueGraph::NodeAdded(UEdGraphNode* AddedNode)
{
	if (UDialogueBlueprint* DialogueAsset = Cast<UDialogueBlueprint>(GetOuter()))
	{
		check(DialogueAsset->DialogueTemplate);

		if (UDialogueGraphNode_Root* RootGraphNode = Cast<UDialogueGraphNode_Root>(AddedNode))
		{	
			//Add text to the root node
			UDialogue* DialogueCDO = Cast<UDialogue>(DialogueAsset->GeneratedClass->GetDefaultObject());

			if (!DialogueAsset->DialogueTemplate->RootDialogue)
			{
				UDialogueNode_NPC* RootNode = MakeNPCReply(RootGraphNode, DialogueAsset->DialogueTemplate);
			
				/*if (DialogueCDO->Speakers.IsValidIndex(0))
				{
					RootNode->Line.Text = FText::Format(LOCTEXT("DefaultRootNodeText", "Hi there, i'm {0}."), FText::FromString(DialogueCDO->Speakers[0].SpeakerID.ToString()));
					RootNode->SpeakerID = DialogueCDO->Speakers[0].SpeakerID;
				}*/

				DialogueAsset->DialogueTemplate->RootDialogue = RootNode;
			}
			return;
		}
	}

	UDialogueBlueprint* DialogueAsset = CastChecked<UDialogueBlueprint>(GetOuter());
	UDialogue* Dialogue = DialogueAsset->DialogueTemplate;

	ensure(Dialogue);

	if (UDialogueGraphNode_NPC* NPCNode = Cast<UDialogueGraphNode_NPC>(AddedNode))
	{
		if (!NPCNode->TWDialogueNode)
		{
			NPCNode->TWDialogueNode = MakeNPCReply(NPCNode, Dialogue);
		}
		else
		{
			NPCNode->TWDialogueNode->Rename(nullptr, Dialogue);
		}
	}
	else if (UDialogueGraphNode_Player* PlayerNode = Cast<UDialogueGraphNode_Player>(AddedNode))
	{
		if (!PlayerNode->TWDialogueNode)
		{
			PlayerNode->TWDialogueNode = MakePlayerReply(PlayerNode, Dialogue);
		}
		else
		{
			PlayerNode->TWDialogueNode->Rename(nullptr, Dialogue);
		}
	}
}

UDialogueNode_NPC* UTWDialogueGraph::MakeNPCReply(class UDialogueGraphNode_NPC* Node, class UDialogue* Dialogue)
{
	if (UDialogueBlueprint* DialogueAsset = Cast<UDialogueBlueprint>(GetOuter()))
	{
		FSoftClassPath NPCReplyClassPath = GetDefault<UTWDialogueEditorSettings>()->DefaultNPCDialogueClass;
		UClass* NPCReplyClass = (NPCReplyClassPath.IsValid() ? LoadObject<UClass>(NULL, *NPCReplyClassPath.ToString()) : UDialogueNode_NPC::StaticClass());

		if (NPCReplyClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Unable to load Dialogue Class '%s'. Falling back to generic UDialogueNode_NPC."), *NPCReplyClassPath.ToString());
			NPCReplyClass = UDialogueNode_NPC::StaticClass();
		}

		UDialogueNode_NPC* NPCReply = NewObject<UDialogueNode_NPC>(Dialogue, NPCReplyClass);
		NPCReply->OwningDialogue = Dialogue;
		Node->TWDialogueNode = NPCReply;
		Dialogue->NPCReplies.AddUnique(NPCReply);

		NPCReply->SetID(*(DialogueAsset->GetName() + "_" + NPCReply->GetName()));

		return NPCReply;
	}

	return nullptr;
}

UDialogueNode_Player* UTWDialogueGraph::MakePlayerReply(class UDialogueGraphNode_Player* Node, class UDialogue* Dialogue)
{
	if (UDialogueBlueprint* DialogueAsset = Cast<UDialogueBlueprint>(GetOuter()))
	{
		FSoftClassPath PlayerReplyClassPath = GetDefault<UTWDialogueEditorSettings>()->DefaultPlayerDialogueClass;
		UClass* PlayerReplyClass = (PlayerReplyClassPath.IsValid() ? LoadObject<UClass>(NULL, *PlayerReplyClassPath.ToString()) : UDialogueNode_Player::StaticClass());

		if (PlayerReplyClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Unable to load Dialogue Class '%s'. Falling back to generic UDialogueNode_Player."), *PlayerReplyClassPath.ToString());
			PlayerReplyClass = UDialogueNode_Player::StaticClass();
		}

		UDialogueNode_Player* PlayerReply = NewObject<UDialogueNode_Player>(Dialogue, PlayerReplyClass);
		Node->TWDialogueNode = PlayerReply;
		PlayerReply->OwningDialogue = Dialogue;
		Dialogue->PlayerReplies.AddUnique(PlayerReply);

		PlayerReply->SetID(*(DialogueAsset->GetName() + "_" + PlayerReply->GetName()));

		return PlayerReply;
	}

	return nullptr;
}