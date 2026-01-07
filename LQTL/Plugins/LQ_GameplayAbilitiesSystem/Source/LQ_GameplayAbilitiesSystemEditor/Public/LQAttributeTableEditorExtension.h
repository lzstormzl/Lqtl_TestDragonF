// Copyright Tale Weavers

#pragma once

#include "CoreMinimal.h"

class UDataTable;
class UAttributeSet;
class FExtender;
class FToolBarBuilder;

/**
 * Extension for DataTable editor to add "Populate" functionality
 * for FLQGameplayAttributeDataTableRow tables
 */
class FLQAttributeTableEditorExtension : public TSharedFromThis<FLQAttributeTableEditorExtension>
{
public:
	void Initialize();
	void Shutdown();

private:
	/** Toolbar extension callback */
	void ExtendDataTableToolbar(FToolBarBuilder& ToolbarBuilder);

	/** Check if current data table is using FLQGameplayAttributeDataTableRow */
	bool IsLQAttributeDataTable() const;

	/** Get the currently edited DataTable from the asset editor */
	UDataTable* GetCurrentDataTable() const;

	/** Generate the dropdown menu content */
	TSharedRef<SWidget> GeneratePopulateMenu();

	/** Get all AttributeSet subclasses dynamically */
	TArray<UClass*> GetAllAttributeSetClasses() const;

	/** Populate the data table with attributes from the selected AttributeSet class */
	void PopulateFromAttributeSet(UClass* AttributeSetClass);

	/** Get all FGameplayAttributeData properties from an AttributeSet class using reflection */
	TArray<FProperty*> GetAttributeProperties(UClass* AttributeSetClass) const;

	/** Check if a row with the given attribute already exists */
	bool DoesAttributeRowExist(UDataTable* DataTable, FProperty* AttributeProperty) const;

private:
	TSharedPtr<FExtender> ToolbarExtender;
};
