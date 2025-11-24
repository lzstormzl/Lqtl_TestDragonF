#include "DialogueBlueprint.h"
#include "Dialogue/DialogueBlueprintGeneratedClass.h"

UDialogueBlueprint::UDialogueBlueprint()
{
    //Constructor
    DialogueTemplate = CreateDefaultSubobject<UDialogue>(TEXT("DialogueTemplate"));
	DialogueTemplate->SetFlags(RF_Transactional | RF_ArchetypeObject);
}

UClass* UDialogueBlueprint::GetBlueprintClass() const
{
	return UDialogueBlueprintGeneratedClass::StaticClass();
}


void UDialogueBlueprint::PostLoad()
{
    Super::PostLoad();

    DialogueTemplate->SetFlags(RF_Transactional | RF_ArchetypeObject);
    // Thực hiện load 
}

bool UDialogueBlueprint::ValidateGeneratedClass(const UClass* InClass)
{
    return true;
}