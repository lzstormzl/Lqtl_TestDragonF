#include "AssetTypeActions_DialogueBlueprint.h"
#include "DialogueBlueprint.h"
#include "DialogueGraphEditor.h"

//Constructor
FAssetTypeActions_DialogueBlueprint::FAssetTypeActions_DialogueBlueprint(uint32 InAssetCategory) : Category(InAssetCategory)
{

}

UClass* FAssetTypeActions_DialogueBlueprint::GetSupportedClass() const
{
    return UDialogueBlueprint::StaticClass();
}

void FAssetTypeActions_DialogueBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
    for (auto Object : InObjects)
	{
		if (UDialogueBlueprint* DialogueBlueprint = Cast<UDialogueBlueprint>(Object)) // Nếu Cast được Object thành DialogueBlueprint
		{
			TSharedRef<FDialogueGraphEditor> dialogueGraphEditor(new FDialogueGraphEditor());
			dialogueGraphEditor->InitDialogueEditor(EToolkitMode::Standalone,
			EditWithinLevelEditor, DialogueBlueprint);
		}
	}
}


uint32 FAssetTypeActions_DialogueBlueprint::GetCategories()
{
	return Category;
}