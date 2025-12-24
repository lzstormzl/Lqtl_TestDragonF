// Copyright Tale Weavers


#include "LQAbilitySystemTypes.h"

#include "DataTableEditorUtils.h"
#include "DataTableEditorUtils.h"
#include "Kismet/KismetStringLibrary.h"

void FLQGameplayAttributeDataTableRow::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
#if WITH_EDITOR
	UDataTable* DataTable = const_cast<UDataTable*>(InDataTable);
	if (DataTable)
	{
		if (auto Found = DataTable->FindRow<FLQGameplayAttributeDataTableRow>(InRowName, TEXT(""), false))
		{
			
			FDataTableEditorUtils::RenameRow(DataTable, InRowName, (FName)Attribute.GetUProperty()->GetName());
		}
	}
#endif
}
