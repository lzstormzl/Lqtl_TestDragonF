#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestEditorSettings.generated.h"

/**
 * 
 */
UCLASS(config=Engine, defaultconfig)
class TWQUESTEDITOR_API UQuestEditorSettings : public UObject
{
	GENERATED_BODY()

public:

	UQuestEditorSettings();

	UPROPERTY(EditAnywhere, config, Category = "Graph Style")
	FLinearColor FailedNodeColor;

	UPROPERTY(EditAnywhere, config, Category = "Graph Style")
	FLinearColor SuccessNodeColor;

	UPROPERTY(EditAnywhere, config, Category = "Graph Style")
	FLinearColor StateNodeColor;

	UPROPERTY(EditAnywhere, config, Category = "Graph Style")
	FLinearColor RootNodeColor;

	UPROPERTY(EditAnywhere, config, Category = "Graph Style")
	FLinearColor TaskNodeColor;

	UPROPERTY(EditAnywhere, config, Category = "Graph Style")
	FLinearColor PersistentTasksNodeColor;

	UPROPERTY(EditAnywhere, config, Category = "Quests", noclear, meta = (MetaClass = "/Script/LQTL_Plugins_Runtime.Quest"))
	FSoftClassPath DefaultQuestClass;

	UPROPERTY(EditAnywhere, config, Category = "Quests", noclear, meta = (MetaClass = "/Script/LQTL_Plugins_Runtime.QuestBranch"))
	FSoftClassPath DefaultQuestBranch;

	UPROPERTY(EditAnywhere, config, Category = "Quests", noclear, meta = (MetaClass = "/Script/LQTL_Plugins_Runtime.QuestState"))
	FSoftClassPath DefaultQuestState;

	UPROPERTY(EditAnywhere, config, Category = "Graph Defaults", noclear, meta = (MetaClass = "/Script/TWQuestEditor.QuestNodeUserWidget"))
	TSoftClassPtr<class UQuestNodeUserWidget> DefaultQuestWidgetClass;
};
