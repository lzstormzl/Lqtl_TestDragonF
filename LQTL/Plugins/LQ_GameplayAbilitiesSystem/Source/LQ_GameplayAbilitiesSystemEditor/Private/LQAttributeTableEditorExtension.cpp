// Copyright Tale Weavers

#include "LQAttributeTableEditorExtension.h"

#include "LQAbilitySystemTypes.h"

#include "DataTableEditorModule.h"
#include "DataTableEditorUtils.h"
#include "AttributeSet.h"
#include "GameplayAbilitiesModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Editor.h"
#include "Engine/DataTable.h"
#include "UObject/UObjectIterator.h"

#define LOCTEXT_NAMESPACE "LQAttributeTableEditorExtension"

void FLQAttributeTableEditorExtension::Initialize()
{
	// Get the DataTable editor module
	FDataTableEditorModule& DataTableEditorModule = FModuleManager::LoadModuleChecked<FDataTableEditorModule>("DataTableEditor");

	// Create and register the toolbar extender
	ToolbarExtender = MakeShared<FExtender>();
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		nullptr,
		FToolBarExtensionDelegate::CreateSP(this, &FLQAttributeTableEditorExtension::ExtendDataTableToolbar)
	);

	DataTableEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

void FLQAttributeTableEditorExtension::Shutdown()
{
	if (ToolbarExtender.IsValid())
	{
		if (FModuleManager::Get().IsModuleLoaded("DataTableEditor"))
		{
			FDataTableEditorModule& DataTableEditorModule = FModuleManager::GetModuleChecked<FDataTableEditorModule>("DataTableEditor");
			DataTableEditorModule.GetToolBarExtensibilityManager()->RemoveExtender(ToolbarExtender);
		}
		ToolbarExtender.Reset();
	}
}

void FLQAttributeTableEditorExtension::ExtendDataTableToolbar(FToolBarBuilder& ToolbarBuilder)
{
	// Only add the button if we're editing an LQ attribute data table
	if (!IsLQAttributeDataTable())
	{
		return;
	}

	ToolbarBuilder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateSP(this, &FLQAttributeTableEditorExtension::GeneratePopulateMenu),
		LOCTEXT("PopulateButton_Label", "Populate"),
		LOCTEXT("PopulateButton_Tooltip", "Populate this DataTable with attributes from an AttributeSet class"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Plus")
	);
}

bool FLQAttributeTableEditorExtension::IsLQAttributeDataTable() const
{
	UDataTable* DataTable = GetCurrentDataTable();
	if (!DataTable)
	{
		return false;
	}

	return DataTable->RowStruct == FLQGameplayAttributeDataTableRow::StaticStruct();
}

UDataTable* FLQAttributeTableEditorExtension::GetCurrentDataTable() const
{
	if (GEditor)
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		if (AssetEditorSubsystem)
		{
			TArray<UObject*> EditedAssets = AssetEditorSubsystem->GetAllEditedAssets();

			// First pass: Try to find the focused DataTable editor
			for (UObject* Asset : EditedAssets)
			{
				if (UDataTable* DataTable = Cast<UDataTable>(Asset))
				{
					// Check if this is the right type of data table
					if (DataTable->RowStruct == FLQGameplayAttributeDataTableRow::StaticStruct())
					{
						// Check if this asset has an open editor
						IAssetEditorInstance* Editor = AssetEditorSubsystem->FindEditorForAsset(DataTable, false);
						if (Editor)
						{
							// Get the editor's tab manager and find its active tab
							TSharedPtr<FTabManager> TabManager = Editor->GetAssociatedTabManager();
							if (TabManager.IsValid())
							{
								// Check if this tab manager's active tab is in the foreground
								TSharedPtr<SDockTab> ActiveTab = TabManager->GetOwnerTab();
								if (ActiveTab.IsValid() && ActiveTab->IsForeground())
								{
									// This is the focused data table - return it immediately
									return DataTable;
								}
							}
						}
					}
				}
			}

			// Fallback: if no focused editor found, return the first matching DataTable
			// This maintains backward compatibility
			for (UObject* Asset : EditedAssets)
			{
				if (UDataTable* DataTable = Cast<UDataTable>(Asset))
				{
					if (DataTable->RowStruct == FLQGameplayAttributeDataTableRow::StaticStruct())
					{
						return DataTable;
					}
				}
			}
		}
	}
	return nullptr;
}

TSharedRef<SWidget> FLQAttributeTableEditorExtension::GeneratePopulateMenu()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	// Get all AttributeSet subclasses dynamically
	TArray<UClass*> AttributeSetClasses = GetAllAttributeSetClasses();

	MenuBuilder.BeginSection("AttributeSets", LOCTEXT("AttributeSetsSection", "Select AttributeSet"));
	for (UClass* Class : AttributeSetClasses)
	{
		// Count attributes for tooltip
		int32 AttributeCount = GetAttributeProperties(Class).Num();

		MenuBuilder.AddMenuEntry(
			FText::FromString(Class->GetName()),
			FText::Format(LOCTEXT("ClassTooltip", "{0} attributes"), FText::AsNumber(AttributeCount)),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateSP(this, &FLQAttributeTableEditorExtension::PopulateFromAttributeSet, Class))
		);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

TArray<UClass*> FLQAttributeTableEditorExtension::GetAllAttributeSetClasses() const
{
	TArray<UClass*> Result;

	// Get all derived classes of UAttributeSet
	GetDerivedClasses(UAttributeSet::StaticClass(), Result, true);

	// Filter to only include classes that have at least one FGameplayAttributeData property
	Result.RemoveAll([this](UClass* Class)
	{
		// Exclude abstract classes
		if (Class->HasAnyClassFlags(CLASS_Abstract))
		{
			return true;
		}

		// Exclude base UAttributeSet class
		if (Class == UAttributeSet::StaticClass())
		{
			return true;
		}

		// Check if it has any attribute properties
		return GetAttributeProperties(Class).Num() == 0;
	});

	// Sort alphabetically by class name
	Result.Sort([](const UClass& A, const UClass& B)
	{
		return A.GetName() < B.GetName();
	});

	return Result;
}

void FLQAttributeTableEditorExtension::PopulateFromAttributeSet(UClass* AttributeSetClass)
{
	UDataTable* DataTable = GetCurrentDataTable();
	if (!DataTable || !AttributeSetClass)
	{
		return;
	}

	// Get all attribute properties from the class
	TArray<FProperty*> AttributeProperties = GetAttributeProperties(AttributeSetClass);

	int32 AddedCount = 0;
	int32 SkippedCount = 0;

	// Begin transaction for undo support
	const FScopedTransaction Transaction(LOCTEXT("PopulateAttributeTable", "Populate Attribute Table"));
	DataTable->Modify();

	// Notify pre-change
	FDataTableEditorUtils::BroadcastPreChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);

	for (FProperty* Property : AttributeProperties)
	{
		// Check if this attribute already exists in the table
		if (DoesAttributeRowExist(DataTable, Property))
		{
			SkippedCount++;
			continue;
		}

		// Create a new row name from the property name
		FName RowName = FName(*Property->GetName());

		// Create the row data
		FLQGameplayAttributeDataTableRow NewRow;
		NewRow.Attribute = FGameplayAttribute(Property);
		NewRow.Value = 0.0f; // Default value

		// Add the row using the standard DataTable API
		DataTable->AddRow(RowName, NewRow);
		AddedCount++;
	}

	// Notify that the data table has changed
	FDataTableEditorUtils::BroadcastPostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);

	// Log result
	UE_LOG(LogTemp, Log, TEXT("Populated DataTable '%s' from '%s': Added %d rows, Skipped %d existing"),
		*DataTable->GetName(), *AttributeSetClass->GetName(), AddedCount, SkippedCount);
}

TArray<FProperty*> FLQAttributeTableEditorExtension::GetAttributeProperties(UClass* AttributeSetClass) const
{
	TArray<FProperty*> Result;

	if (!AttributeSetClass)
	{
		return Result;
	}

	// Iterate all properties including inherited ones
	for (TFieldIterator<FProperty> It(AttributeSetClass, EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		FProperty* Property = *It;

		// Check if this is a FGameplayAttributeData struct property
		if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			if (StructProperty->Struct == FGameplayAttributeData::StaticStruct())
			{
				Result.Add(Property);
			}
		}
	}

	return Result;
}

bool FLQAttributeTableEditorExtension::DoesAttributeRowExist(UDataTable* DataTable, FProperty* AttributeProperty) const
{
	if (!DataTable || !AttributeProperty)
	{
		return false;
	}

	// Check if a row with this attribute name already exists
	FName RowName = FName(*AttributeProperty->GetName());
	return DataTable->FindRowUnchecked(RowName) != nullptr;
}

#undef LOCTEXT_NAMESPACE
