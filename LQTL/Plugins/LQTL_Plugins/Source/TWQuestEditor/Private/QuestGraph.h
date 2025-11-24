#pragma once

#include "CoreMinimal.h"

// Thư viện Unreal
#include "EdGraph/EdGraph.h"
#include "GraphEditAction.h"

// Thư viện Plugin
#include "QuestGraphNode_State.h"
#include "QuestGraphNode_Branch.h"
#include "QuestGraphNode_PersistentTasks.h"

#include "QuestGraph.generated.h"

class UQuestGraphNode;
class UQuestGraphNode_State; 
class UQuestGraphNode_Branch;
class UQuest;
class UQuestNode;
class UQuestBranch;
class UQuestState;

/**
 * 
 */
UCLASS()
class UQuestGraph : public UEdGraph
{
	GENERATED_BODY()

public:

	virtual void OnCreated(); // Khi Graph đã được tạo
	virtual void OnLoaded(); // Khi Graph đã được load 
	virtual void Initialize();	// Khi Graph được khởi tạo
	virtual void NotifyGraphChanged(const FEdGraphEditAction& Action); // Được gọi khi graph có sự thay đổi

	// Nối lại pin
	void PinRewired(UQuestGraphNode* Node, UEdGraphPin* Pin);

	void NodeAdded(UEdGraphNode* AddedNode);
	
	// Pointer to the graphs persistent tasks node for convenience
	class UQuestGraphNode_PersistentTasks* PTNode;

// protected:

// 	void PIEStarted(bool bIsSimulating);

// 	//Called when the players objective is updated in a PIE Session. Used to show debug updates to designers
// 	UFUNCTION()
// 	void OnPIEObjectiveUpdated(FText QuestName, TArray<FText> NewObjectives, FText NewStateText);

// 	//Helper functions when we are processing the nodes into a Quest 
	//Create Quest State for Graph
	UQuestState* MakeState(UQuestGraphNode_State* Node, UQuest* Quest);
 	UQuestBranch* MakeBranch(UQuestGraphNode_Branch* Node, UQuest* Quest);

};
