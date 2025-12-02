#include "TWDialogueEditorSettings.h"

// Thư viện Unreal
#include "UObject/ConstructorHelpers.h"

// Thư viện Tale Weavers Plugins 
#include "TWDialogueNodeUserWidget.h"
#include "Dialogue/TWDialogueNode.h"

UTWDialogueEditorSettings::UTWDialogueEditorSettings()
{
    PlayerNodeColor = FLinearColor(0.65f, 0.28f, 0.f);

    DefaultDialogueNodeClass = UTWDialogueNode::StaticClass();
    
    DefaultNPCDialogueClass = UDialogueNode_NPC::StaticClass();
    DefaultPlayerDialogueClass = UDialogueNode_Player::StaticClass();
    DefaultDialogueClass = UDialogue::StaticClass();

    auto DialogueNodeUserWidgetFinder = ConstructorHelpers::FClassFinder<UTWDialogueNodeUserWidget>(TEXT("WidgetBlueprint'/LQTL_Plugins/Widgets/Editor/WBP_DefaultDialogueNode.WBP_DefaultDialogueNode_C'"));
    check(DialogueNodeUserWidgetFinder.Succeeded())
    if (DialogueNodeUserWidgetFinder.Succeeded())
    {
        DefaultDialogueWidgetClass = DialogueNodeUserWidgetFinder.Class;
    }
}