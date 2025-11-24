#include "DialogueEditorTabFactories.h"
#include "DialogueGraphEditor.h"

FDialogueGraphEditorSummoner::FDialogueGraphEditorSummoner(TSharedPtr<class FDialogueGraphEditor> InDialogueEditorPtr) 
    : FDocumentTabFactoryForObjects<UEdGraph>(("Document"), InDialogueEditorPtr)
{
    _dialogueEditor = InDialogueEditorPtr;
}

void FDialogueGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	check(_dialogueEditor.IsValid());
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	_dialogueEditor.Pin()->OnGraphEditorFocused(GraphEditor);
}

TAttribute<FText> FDialogueGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return TAttribute<FText>(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> FDialogueGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return OnCreateGraphEditorWidget.Execute(DocumentID);
}

const FSlateBrush* FDialogueGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return FAppStyle::GetBrush("ClassIcon.DialogueAsset");
}