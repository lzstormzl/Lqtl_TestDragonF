#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "BlueprintEditorModes.h"
#include "Framework/Docking/TabManager.h"
#include "DialogueGraphEditor.h"

class FDialogueEditorMode : public FBlueprintEditorApplicationMode
{
public:
    // Constructor
    FDialogueEditorMode(TSharedPtr<class FDialogueGraphEditor> inDialogueEditor);
    
    virtual void RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager) override;

    //.......
    virtual void PostActivateMode() override;

protected:
	TWeakPtr<class FDialogueGraphEditor> _dialogueEditor;



	// Set of spawnable tabs in behavior tree editing mode
	FWorkflowAllowedTabSet _dialogueTabs;
};