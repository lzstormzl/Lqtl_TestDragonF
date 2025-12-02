#include "Dialogue/TWDialogueNode.h"
#include "SocialDialogueSettings.h"

TArray<class UDialogueNode_NPC*> UDialogueNode_NPC::GetReplyChain(class USocialComponent* SocialComponent)
{
	TArray<UDialogueNode_NPC*> FollowUpReplies;
	UDialogueNode_NPC* CurrentNode = this;

    FollowUpReplies.Add(CurrentNode);

    while (CurrentNode)
	{
		if (CurrentNode != this)
		{
			FollowUpReplies.Add(CurrentNode);
		}

		TArray<UDialogueNode_NPC*> NPCRepliesToRet = CurrentNode->NPCReplies;
		if (const USocialDialogueSettings* DialogueSettings = GetDefault<USocialDialogueSettings>())
		{
			//Need to process the conditions using higher/leftmost nodes first 
			NPCRepliesToRet.Sort([DialogueSettings](const UDialogueNode_NPC& NodeA, const UDialogueNode_NPC& NodeB) {
				//return DialogueSettings->bEnableVerticalWiring ? NodeA.NodePos.X < NodeB.NodePos.X : NodeA.NodePos.Y < NodeB.NodePos.Y;
                return NodeA.NodePos.Y < NodeB.NodePos.Y;
				});
		}
		//If we don't find another node after this the loop will exit
		CurrentNode = nullptr;

		// if(SocialComponent->IsSharedComponent())
		// {
		// 	int32 Idx = 0;
		// 	int32 BestReplyIdx = INT_MAX;
		// 	for (auto& Reply : NPCRepliesToRet)
		// 	{
		// 		//If we're shared, find the earliest passing condition across all players
		// 		for (auto& GroupMemberComp : SocialComponent->GroupMembers)
		// 		{
		// 			if (Reply != this && Reply->AreConditionsMet(GroupMemberComp->GetOwningPawn(), GroupMemberComp->GetOwningController(), GroupMemberComp))
		// 			{
		// 				if (Idx < BestReplyIdx)
		// 				{
		// 					CurrentNode = Reply;
		// 					BestReplyIdx = Idx;
		// 				}
		// 			}
		// 		}

		// 		++Idx;
		// 	}
		// }
		// else
		// {
			//Find the next valid reply. We'll then repeat this cycle until we run out
			for (auto& Reply : NPCRepliesToRet)
			{
				if (Reply != this && Reply->AreConditionsMet(nullptr, nullptr, SocialComponent))
				{
					CurrentNode = Reply;
					break; // just use the first reply with valid conditions
				}
			}
		//}

	}
    return FollowUpReplies;
}

TArray<class UDialogueNode_NPC*> UTWDialogueNode::GetNPCReplies(class USocialComponent* USocialComponent)
{
	TArray<class UDialogueNode_NPC*> ValidReplies;

	for (auto& NPCReply : NPCReplies)
	{
		if (NPCReply->AreConditionsMet(nullptr, nullptr, USocialComponent))
		{
			ValidReplies.Add(NPCReply);
		}
	}

	return ValidReplies;
}

TArray<class UDialogueNode_Player*> UTWDialogueNode::GetPlayerReplies(class USocialComponent* USocialComponent)
{
	TArray<class UDialogueNode_Player*> ValidReplies;

	//If any players in the party can select a reply, everyone can select it
	// if (SocialComponent->IsSharedComponent())
	// {
	// 	for (auto& GroupMemberComp : NarrativeComponent->GroupMembers)
	// 	{
	// 		for (auto& PlayerReply : PlayerReplies)
	// 		{
	// 			if (PlayerReply && PlayerReply->AreConditionsMet(GroupMemberComp->GetOwningPawn(), GroupMemberComp->GetOwningController(), GroupMemberComp))
	// 			{
	// 				ValidReplies.AddUnique(PlayerReply);
	// 			}
	// 		}
	// 	}
	// }
	//else
	//{
		for (auto& PlayerReply : PlayerReplies)
		{
			if (PlayerReply && PlayerReply->AreConditionsMet(nullptr, nullptr, USocialComponent))
			{
				ValidReplies.Add(PlayerReply);
			}
		}
	//}


	if (const USocialDialogueSettings* DialogueSettings = GetDefault<USocialDialogueSettings>())
	{
		//Sort the replies by their Y position in the graph
		ValidReplies.Sort([DialogueSettings](const UDialogueNode_Player& NodeA, const UDialogueNode_Player& NodeB) {
			//return DialogueSettings->bEnableVerticalWiring ? NodeA.NodePos.X < NodeB.NodePos.X : NodeA.NodePos.Y < NodeB.NodePos.Y;
			return NodeA.NodePos.Y < NodeB.NodePos.Y;
			});
	}

	return ValidReplies;
}