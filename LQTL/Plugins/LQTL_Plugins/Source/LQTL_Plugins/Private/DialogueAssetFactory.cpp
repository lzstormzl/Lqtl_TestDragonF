#include "DialogueAssetFactory.h"
#include "DialogueBlueprint.h"
#include "Dialogue/DialogueBlueprintGeneratedClass.h"
#include "TWDialogueEditorSettings.h"
#include "Dialogue/Dialogue.h"
#include "Kismet2/KismetEditorUtilities.h"

UDialogueAssetFactory::UDialogueAssetFactory()
{
	SupportedClass = UDialogueBlueprint::StaticClass();
	ParentClass = UDialogue::StaticClass();

	bCreateNew = true;
	bEditAfterNew = true;
}

UObject *UDialogueAssetFactory::FactoryCreateNew(UClass *Class, UObject *InParent, FName Name, EObjectFlags Flags, UObject *Context, FFeedbackContext *Warn)
{
    return NewObject<UDialogueBlueprint>(InParent, Class, Name, Flags);
}

UObject* UDialogueAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	// Kiểm trả lớp con của Class có phải UDialogueBlueprint không
	check(Class->IsChildOf(UDialogueBlueprint::StaticClass()));

	UDialogueBlueprint* DialogueBP = nullptr;

	//Lấy đường dẫn của Dialogue Class trên Settings
	FSoftClassPath DialogueClassPath = GetDefault<UTWDialogueEditorSettings>()->DefaultDialogueClass;
	
	UClass* DialogueClass = (DialogueClassPath.IsValid() ? LoadObject<UClass>(NULL, *DialogueClassPath.ToString()) : UDialogue::StaticClass());

	if(DialogueClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Không thể load Dialogue Class '%s'. Đang cố thử load lại lần nữa."), *DialogueClassPath.ToString());
		DialogueClass = UDialogue::StaticClass();
	}

	DialogueBP = CastChecked<UDialogueBlueprint>(FKismetEditorUtilities::CreateBlueprint(DialogueClass,
		InParent,
		Name,
		BPTYPE_Normal,
		UDialogueBlueprint::StaticClass(),
		UDialogueBlueprintGeneratedClass::StaticClass(),
		CallingContext
	));

	return DialogueBP;
}