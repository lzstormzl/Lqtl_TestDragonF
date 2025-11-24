#pragma once

#include "CoreMinimal.h"

#include "UObject/NoExportTypes.h"

//Thư viện plugins
#include "TWDialogueNode.h"

#include "Dialogue.generated.h"



/**
 *
 */

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName="Dialogue"))
class LQTL_PLUGINS_RUNTIME_API UDialogue : public UObject
{
	GENERATED_BODY()

	// ---------------------Functions-----------------------
public: 
	UDialogue();

	// Get World Context
	//virtual UWorld* GetWorld() const override;

	// Khởi tạo Dialogue 
	virtual bool Initialize(class USocialComponent* InitializingComp, FName StartFromID);

	// Deinitialize Dialogue
	//virtual void Deinitialize();

	// Sao chép và khởi tạo Dialogue từ Dialogue Template
	virtual void DuplicateAndInitializeFromDialogue(UDialogue* DialogueTemplate);

	TArray<UTWDialogueNode*> GetNodes() const;

	void Play();

	bool GenerateDialogueChunk(UDialogueNode_NPC* Node);

	UDialogueNode_NPC* GetReplyByID(const FName& ID) const;
	TArray<UDialogueNode_NPC*> GetNPCRepliesByIDs(const TArray<FName>& IDs) const;
	TArray<UDialogueNode_Player*> GetPlayerRepliesByIDs(const TArray<FName>& IDs) const;

	//Used to check at any time on client or server if we have a valid chunk, meaning we can call play() and begin the dialogue
	bool HasValidChunk() const;

	//Called once we've played through all the NPC dialogues and the players reponses have been sent to the UI 
	void NPCFinishedTalking();

	void SelectDialogueOption(UDialogueNode_Player* PlayerNode);

	//Skips the current dialogue line, providing bSkippable is true  
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void SkipCurrentLine();

	//Ends the current dialogue line 
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void EndCurrentLine();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void FinishNPCDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void FinishPlayerDialogue();

	virtual void PlayNPCDialogueNode(class UDialogueNode_NPC* NPCReply);
	virtual void PlayPlayerDialogueNode(class UDialogueNode_Player* PlayerReply);


	TArray<FName> MakeIDsFromNPCNodes(const TArray<UDialogueNode_NPC*> Nodes) const;
	TArray<FName> MakeIDsFromPlayerNodes(const TArray<UDialogueNode_Player*> Nodes) const;

	//Called by the client when they have received the next dialogue chunk from the server
	void ClientReceiveDialogueChunk(const TArray<FName>& NPCReplies, const TArray<FName>& PlayerReplies);

	UFUNCTION()
	virtual void PlayNextNPCReply();

	//Process all the events on a given node
	virtual void ProcessNodeEvents(class UTWDialogueNode* Node, bool bStartEvents);

	virtual void ExitDialogue();

private:
	
	UEdGraph *GetDialogueGraph() const { return _dialogueGraph; }
		
private:
	UPROPERTY()
	UEdGraph *_dialogueGraph;
	

	// ---------------------Variables-----------------------
public:
	
	// Các node reply
	UPROPERTY()
	TArray<class UDialogueNode_NPC*> NPCRepliesChain;

	//The player responses once NPC has finished talking the current chunk 
	UPROPERTY()
	TArray<class UDialogueNode_Player*> AvailableResponses;

	// Social Component hiện tại
	UPROPERTY(BlueprintReadOnly, Category = "Component")
	class USocialComponent* OwningComp;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	class APawn* OwningPawn;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	class APlayerController* OwningController;

	//The first thing the NPC says in the dialog
	UPROPERTY()
	class UDialogueNode_NPC* RootDialogue;

	// Dialogue Node trong Dialogue Asset
	UPROPERTY()
	TArray<class UDialogueNode_NPC*> NPCReplies;

	UPROPERTY()
	TArray<class UDialogueNode_Player*> PlayerReplies;

	

	

protected:

	// Node hiện tại 
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	class UTWDialogueNode* CurrentNode;

	// Line hiện tại đang được chơi
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FDialogueLine CurrentLine;

};