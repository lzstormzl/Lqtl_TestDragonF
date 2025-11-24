#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueBlueprint.h"
#include "BlueprintEditor.h"
#include "WorkFlowOrientedApp/WorkflowTabManager.h"

class FDialogueGraphEditor : public FBlueprintEditor
{
public:

    //Constructor
    FDialogueGraphEditor() {}
    
    //Destructor
    virtual ~FDialogueGraphEditor() {}

    virtual void OnSelectedNodesChangedImpl(const TSet<class UObject*>& NewSelection) override;

    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

    void InitDialogueEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UDialogueBlueprint* InDialogue);

    //void RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager) override;
    static FText GetLocalizedMode(FName InMode);

    virtual void CreateDefaultCommands() override;

    // Phục hồi lại Dialogue Graph hoặc tạo ra 1 cái mới nếu nó chưa tồn tại
    virtual void RestoreDialogueGraph();

    void OnDialogueNodeDoubleClicked(UEdGraphNode* Node);

    /** Setup all the events that the graph editor can handle - need to override this as default BP editor
double click jumps to parent node, we need custom double clicked functionality TODO: See if cleaner way to do this*/
	virtual void SetupGraphEditorEvents(UEdGraph* InGraph, SGraphEditor::FGraphEditorEvents& InEvents);

    bool Dialogue_GetBoundsForSelectedNodes(class FSlateRect& Rect, float Padding);


protected:
    UDialogueBlueprint* DialogueBlueprint;

    TSharedPtr<class IDetailsView> DetailsView;

public:
    static const FName DialogueEditorMode;
};