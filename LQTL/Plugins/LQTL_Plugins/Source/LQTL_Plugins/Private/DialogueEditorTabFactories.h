#pragma once

#include "CoreMinimal.h"
#include "DialogueGraphEditor.h"
#include "EdGraph/EdGraph.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

struct FDialogueGraphEditorSummoner : public FDocumentTabFactoryForObjects<UEdGraph>
{
    public:

    DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditorWidget, UEdGraph*);

    // constructor
    FDialogueGraphEditorSummoner(TSharedPtr<class FDialogueGraphEditor> InDialogueEditorPtr);

    virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;

    // Pure Override -> Bắt buộc phải override nhưng function dưới thì mới new được
    protected:
    virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
    virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
    virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;

    protected:
    TWeakPtr<class FDialogueGraphEditor> _dialogueEditor;
    FOnCreateGraphEditorWidget OnCreateGraphEditorWidget;
};