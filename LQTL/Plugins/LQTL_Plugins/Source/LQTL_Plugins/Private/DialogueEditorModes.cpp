#include "DialogueEditorModes.h"
#include "DialogueGraphEditor.h"
#include "DialogueEditorTabFactories.h"
#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"


FDialogueEditorMode::FDialogueEditorMode(TSharedPtr<class FDialogueGraphEditor> inDialogueEditor) 
    : FBlueprintEditorApplicationMode(inDialogueEditor, 
        FDialogueGraphEditor::DialogueEditorMode, 
        FDialogueGraphEditor::GetLocalizedMode,
        false,
        false
    )
{
    _dialogueEditor = inDialogueEditor;

    //Đăng ký Details View Tab
    //_dialogueTabs.RegisterFactory(MakeShareable(new FDialogueDetailsSummoner(InDialogueGraphEditor)));

    if (UToolMenu* Toolbar = inDialogueEditor->RegisterModeToolbarIfUnregistered(GetModeName()))
	{                     //Lấy Toolbar
        inDialogueEditor->GetToolbarBuilder()->AddCompileToolbar(Toolbar);
        inDialogueEditor->GetToolbarBuilder()->AddScriptingToolbar(Toolbar);
        inDialogueEditor->GetToolbarBuilder()->AddBlueprintGlobalOptionsToolbar(Toolbar);
        inDialogueEditor->GetToolbarBuilder()->AddDebuggingToolbar(Toolbar);
    }

    // ................................................
    LayoutExtender = MakeShared<FLayoutExtender>();
    TabLayout->ProcessExtensions(*LayoutExtender.Get());
}

void FDialogueEditorMode::RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager)
{   
    // override lại hàm RegisterTabFactories từ FBlueprintEditorApplicationMode
    FBlueprintEditorApplicationMode::RegisterTabFactories(InTabManager);

    // kiểm tra xem _dialogueEditor có valid không -> Nếu không thì engine crash ở đây
    check(_dialogueEditor.IsValid());
    // Lấy con trỏ thô 
	TSharedPtr<FDialogueGraphEditor> DialogueEditorPtr = _dialogueEditor.Pin();

    //
    DialogueEditorPtr->DocumentManager->RegisterDocumentFactory(MakeShareable(new FDialogueGraphEditorSummoner(DialogueEditorPtr)));

    // Đăng ký _dialogueTabs ?
    DialogueEditorPtr->PushTabFactories(_dialogueTabs);

    FApplicationMode::RegisterTabFactories(InTabManager);

}

void FDialogueEditorMode::PostActivateMode()
{
    check(_dialogueEditor.IsValid());
    TSharedPtr<FDialogueGraphEditor> DialogueEditorPtr = _dialogueEditor.Pin();
    DialogueEditorPtr->RestoreDialogueGraph();

    FBlueprintEditorApplicationMode::PostActivateMode();
}