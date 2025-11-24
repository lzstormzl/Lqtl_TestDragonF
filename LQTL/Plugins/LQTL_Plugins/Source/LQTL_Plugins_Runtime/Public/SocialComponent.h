#pragma once

#include "CoreMinimal.h"
#include "UObject/TextProperty.h" //Fixes a build error complaining about incomplete type UTextProperty
#include "Components/ActorComponent.h"

// Thư viện Plugin
#include "Dialogue/Dialogue.h"
#include "Quest/Quest.h"
#include "SocialComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSocial, Log, All);

//General
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuTaskCompleted, const UNarrativeDataTask*, NarrativeTask, const FString&, Name);

// Dialogue 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueBegan, class UDialogue*, Dialogue); // Dialogue started
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueFinished, class UDialogue*, Dialogue); // Dialogue finished
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDialogueOptionSelected, class UDialogue*, Dialogue, class UDialogueNode_Player*, PlayerReply);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDialogueRepliesAvailable, class UDialogue*, Dialogue, const TArray<UDialogueNode_Player*>&, PlayerReplies);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNPCDialogueLineStarted, class UDialogue*, Dialogue, class UDialogueNode_NPC*, Node, const FDialogueLine&, DialogueLine); // NPC Dialogue line start

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPlayerDialogueLineStarted, class UDialogue*, Dialogue, class UDialogueNode_Player*, Node, const FDialogueLine&, DialogueLine); // Player Dialogue line start
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPlayerDialogueLineFinished, class UDialogue*, Dialogue, class UDialogueNode_Player*, Node, const FDialogueLine&, DialogueLine); // Player Dialogue line finish

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LQTL_PLUGINS_RUNTIME_API USocialComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USocialComponent();

	bool HasAuthority() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

   // Event
public:

	// /**Called when a narrative data task is completed by the player */
	// UPROPERTY(BlueprintAssignable, Category = "Quests")
	// FOnNarrativeTaskCompleted OnNarrativeDataTaskCompleted;

	/**Called when a quest objective has been completed.*/
	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnQuestBranchCompleted OnQuestBranchCompleted;

	/**Called when a quest objective is updated and we've received a new objective*/
	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnQuestNewState OnQuestNewState;

	/**Called when a quest task in a quest step has made progress. ie 6 out of 10 wolves killed*/
	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnQuestTaskProgressChanged OnQuestTaskProgressChanged;

	/**Called when a quest task in a step is completed*/
	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnQuestTaskCompleted OnQuestTaskCompleted;

	/**Called when a quest is completed.*/
	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnQuestSucceeded OnQuestSucceeded;

	/**Called when a quest is failed.*/
	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnQuestFailed OnQuestFailed;

	/**Called when a quest is started.*/
	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnQuestStarted OnQuestStarted;

	/**Called when a quest is forgotten.*/
	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnQuestForgotten OnQuestForgotten;

	/**Called when a quest is restarted.*/
	UPROPERTY(BlueprintAssignable, Category = "Quests")
	FOnQuestRestarted OnQuestRestarted;

	// /**Called when a save has begun*/
	// UPROPERTY(BlueprintAssignable, Category = "Saving/Loading")
	// FOnBeginSave OnBeginSave;

	// /**Called when a save has completed*/
	// UPROPERTY(BlueprintAssignable, Category = "Saving/Loading")
	// FOnSaveComplete OnSaveComplete;

	// /**Called when a load has begun*/
	// UPROPERTY(BlueprintAssignable, Category = "Saving/Loading")
	// FOnBeginLoad OnBeginLoad;

	// /**Called when a load has completed*/
	// UPROPERTY(BlueprintAssignable, Category = "Saving/Loading")
	// FOnLoadComplete OnLoadComplete;

	/**Called when a dialogue has been Start for any reason*/
	UPROPERTY(BlueprintAssignable, Category = "Dialogues")
	FOnDialogueBegan OnDialogueStarted;

	/**Called when a dialogue has been finished for any reason*/
	UPROPERTY(BlueprintAssignable, Category = "Dialogues")
	FOnDialogueFinished OnDialogueFinished;

	/**Called when a dialogue option is selected*/
	UPROPERTY(BlueprintAssignable, Category = "Dialogues")
	FDialogueOptionSelected OnDialogueOptionSelected;

	UPROPERTY(BlueprintAssignable, Category = "Dialogues")
	FNPCDialogueLineStarted OnNPCDialogueLineStarted;

	UPROPERTY(BlueprintAssignable, Category = "Dialogues")
	FPlayerDialogueLineStarted OnPlayerDialogueLineStarted;

	UPROPERTY(BlueprintAssignable, Category = "Dialogues")
	FPlayerDialogueLineFinished OnPlayerDialogueLineFinished;

	/**Called when the NPC(s) have finished talking and the players replies are available*/
	UPROPERTY(BlueprintAssignable, Category = "Dialogues")
	FDialogueRepliesAvailable OnDialogueRepliesAvailable;

	//Function
public:	
	UFUNCTION(BlueprintCallable, Category = "Dialogues")
	virtual bool StartDialogue(TSubclassOf<UDialogue> DialogueClass, FName StartFromID);

	/**Selects a dialogue option. Will update the dialogue and automatically start playing the next bit of dialogue*/
	UFUNCTION(BlueprintCallable, Category = "Dialogues")
	virtual void SelectDialogueOption(class UDialogueNode_Player* Option);

	UFUNCTION(BlueprintPure, Category = "Narrative")
	virtual APawn* GetOwningPawn() const;

	UFUNCTION(BlueprintPure, Category = "Narrative")
	virtual APlayerController* GetOwningController() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogues")
	UDialogue* GetCurrentDialogue() const;

	UFUNCTION(Client, Reliable, Category = "Dialogues")
	virtual void ClientExitDialogue();

	/**Exit the dialogue, will never fail*/
	virtual void ExitDialogue();

private:
	virtual UDialogue* MakeDialogue(TSubclassOf<UDialogue> DialogueClass, FName StartFromID);
	//Variables

public:
	UDialogue* CurrentDialogue;

public:
	//We set this flag to true during loading so we don't broadcast any quest update delegates as we load quests back in
	bool bIsLoading;

	protected:
	UPROPERTY()
	class APlayerController* OwnerPC;

public:
	/**Đưa quest. Return Quest if Success*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Quests", meta = (AdvancedDisplay = "1"))
	virtual class UQuest* BeginQuest(TSubclassOf<class UQuest> QuestClass, FName StartFromID = NAME_None);

	/**Given a Quest class return its active quest object if we've started this quest */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quests")
	class UQuest* GetQuest(TSubclassOf<class UQuest> QuestClass) const;

	/**Given a Quest class return its active quest object if we've started this quest */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quests")
	TArray<class UQuest*> GetQuestList() const {return QuestList;};

	/**Returns a list of all quests that are in progress, in chronological order.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quests")
	TArray<UQuest*> GetInProgressQuests() const;

	/**Returns a list of all failed quests, in chronological order.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quests")
	TArray<UQuest*> GetFailedQuests() const;

	/**Returns a list of all succeeded quests, in chronological order.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quests")
	TArray<UQuest*> GetSucceededQuests() const;

protected:
	/**
	Creates a new quest and initializes it from a given quest class. 
	*/
	virtual class UQuest* CreateQuest(TSubclassOf<class UQuest> QuestClass);

public:
	//A list of all the quests the player is involved in
	UPROPERTY(VisibleAnywhere, Category = "Quests")
	TArray<class UQuest*> QuestList;
	
};
