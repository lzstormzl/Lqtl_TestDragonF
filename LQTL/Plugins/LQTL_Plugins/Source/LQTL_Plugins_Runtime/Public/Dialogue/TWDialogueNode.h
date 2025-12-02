#pragma once

#include "CoreMinimal.h"
#include "TaleWeaversNodeBase.h"

#include "Styling/SlateBrush.h"

#include "TWDialogueNode.generated.h"

USTRUCT(BlueprintType)
struct FSpeakerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Speakers")
	FName SpeakerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speakers")
	FText SpeakerName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speakers")
	FSlateBrush PortraitImage;
};

USTRUCT(BlueprintType)
struct FDialogueLine
{
    GENERATED_BODY()

public:

    FDialogueLine()
    {
        DisplayText = FText::GetEmpty();
    }

    // The text to display for this dialogue line
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Line", meta = (MultiLine = true))
    FText DisplayText;
};

UCLASS(BlueprintType, Blueprintable)
class LQTL_PLUGINS_RUNTIME_API UTWDialogueNode : public UTaleWeaversNodeBase
{
    GENERATED_BODY()

public:

    UTWDialogueNode() { bIsSkippable = true; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speaker", meta = (ShowOnlyInnerProperties))
    FSpeakerInfo SpeakerInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details - Dialogue Node", meta = (ShowOnlyInnerProperties))
    FDialogueLine Line;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AdvancedDisplay))
	TArray<class UDialogueNode_NPC*> NPCReplies;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AdvancedDisplay))
	TArray<class UDialogueNode_Player*> PlayerReplies;

    /**Should pressing the enter key allow this line to be skipped?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Details - Dialogue Node")
	bool bIsSkippable;

    UPROPERTY()
	class UDialogue* OwningDialogue;

	//Name of custom event to call when this is reached 
	UPROPERTY(BlueprintReadWrite, Category = "Details - Dialogue Node", meta = (AdvancedDisplay))
	FName OnPlayNodeFuncName;
    

    // Tạo các property của dialogue node ở đây

    //  AlternativeLines để tạo cho dòng dialogue đó randomly hơn bình thường -> sẽ random chọn dòng dialogue mặc định hoặc dòng alternative này 

    virtual FDialogueLine GetDialogueLine() { return Line; }

    TArray<class UDialogueNode_NPC*> GetNPCReplies(class USocialComponent* SocialComponent);
	TArray<class UDialogueNode_Player*> GetPlayerReplies(class USocialComponent* USocialComponent);
};

UCLASS(BlueprintType)
class LQTL_PLUGINS_RUNTIME_API UDialogueNode_NPC : public UTWDialogueNode
{
	GENERATED_BODY()

public:

	/**The ID of the speaker for this node */
	UPROPERTY(BlueprintReadWrite, Category = "Details - NPC Dialogue Node")
	FName SpeakerID;

	//Sequence to play when player is selecting their reply after this shot has played 
	// UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Details - NPC Dialogue Node")
	// class UNarrativeDialogueSequence* SelectingReplyShot;

	/**Grab this NPC node, appending all follow up responses to that node. Since multiple NPC replies can be linked together, 
	we need to grab the chain of replies the NPC has to say. */
	
	TArray<class UDialogueNode_NPC*> GetReplyChain(class USocialComponent* SocialComponent);
	

};

UCLASS(BlueprintType)
class LQTL_PLUGINS_RUNTIME_API UDialogueNode_Player : public UTWDialogueNode
{
	GENERATED_BODY()

public:

	//Runs a wildcard replace on a player reply 
	// UFUNCTION(BlueprintPure, Category = "Details")
	// virtual FText GetOptionText(class UDialogue* InDialogue) const;

	FORCEINLINE bool IsAutoSelect() const {return bAutoSelect /*|| IsRoutingNode()*/; };

protected:

	/**The shortened text to display for dialogue option when it shows up in the list of available responses. If left empty narrative will just use the main text. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Details - Player Dialogue Node")
	FText OptionText;

	/**If true, this dialogue option will be automatically selected instead of the player having to select it from the UI*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Details - Player Dialogue Node")
	bool bAutoSelect = false;

};