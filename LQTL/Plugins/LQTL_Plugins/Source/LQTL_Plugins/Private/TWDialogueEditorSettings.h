#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TWDialogueEditorSettings.generated.h"

UCLASS(config=Engine, defaultconfig)
class LQTL_PLUGINS_API UTWDialogueEditorSettings : public UObject
{
    GENERATED_BODY()

public:

    UTWDialogueEditorSettings();

    UPROPERTY(EditAnywhere, config, Category = "Graph Style")
	FLinearColor PlayerNodeColor;

    UPROPERTY(EditAnywhere, config, Category = "Graph Defaults", noclear, meta = (MetaClass = "/Script/LQTL_Plugins_Runtime.TWDialogueNode"))
	FSoftClassPath DefaultDialogueNodeClass;

    UPROPERTY(EditAnywhere, config, Category = "Graph Defaults", noclear, meta = (MetaClass="/Script/LQTL_Plugins_Runtime.DialogueNode_NPC"))
	FSoftClassPath DefaultNPCDialogueClass;

    UPROPERTY(EditAnywhere, config, Category = "Graph Defaults", noclear, meta = (MetaClass = "/Script/LQTL_Plugins_Runtime.DialogueNode_Player"))
	FSoftClassPath DefaultPlayerDialogueClass;

    UPROPERTY(EditAnywhere, config, Category = "Graph Defaults", noclear, meta = (MetaClass = "/Script/LQTL_Plugins_Runtime.Dialogue"))
	FSoftClassPath DefaultDialogueClass;

    UPROPERTY(EditAnywhere, config, Category = "Graph Defaults", noclear, meta = (MetaClass = "/Script/LQTL_Plugins.TWDialogueNodeUserWidget"))
	TSoftClassPtr<class UTWDialogueNodeUserWidget> DefaultDialogueWidgetClass;
};