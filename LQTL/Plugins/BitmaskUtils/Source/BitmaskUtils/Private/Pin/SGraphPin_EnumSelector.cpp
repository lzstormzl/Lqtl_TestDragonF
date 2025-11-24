#include "Pin/SGraphPin_EnumSelector.h"

#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "EdGraph/EdGraphPin.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"
#include "ScopedTransaction.h"



void SGraphPin_EnumSelector::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
    this->GraphPinObj = InPin;
    this->SetCursor(EMouseCursor::Default);
    this->bShowLabel = true;

    this->ChildSlot
        [
            GenerateEnumDropdown()
        ];
}



TArray<UEnum*> GetAllBitmaskEnums()
{
    TArray<UEnum*> Result;

    for (TObjectIterator<UEnum> It; It; ++It)
    {
        UEnum* Enum = *It;
        if (Enum->HasMetaData(TEXT("Bitflags")))
        {
            Result.Add(Enum);
        }
    }

    Result.Sort([](const UEnum& A, const UEnum& B)
        {
            return A.GetName() < B.GetName();
        });

    return Result;
}

TSharedRef<SWidget> SGraphPin_EnumSelector::GenerateEnumDropdown()
{
    EnumOptions.Reset();

    TArray<UEnum*> EnumList = GetAllBitmaskEnums();

    for (UEnum* Enum : EnumList)
    {
        FString Name = Enum->GetName();
        TSharedPtr<FString> Item = MakeShared<FString>(Name);
        EnumOptions.Add(Item);

        if (GraphPinObj->GetDefaultAsString().Contains(Name))
        {
            SelectedEnumName = Item;
        }
    }

    if (!SelectedEnumName.IsValid() && EnumOptions.Num() > 0)
    {
        SelectedEnumName = EnumOptions[0];
    }

    return SNew(SComboBox<TSharedPtr<FString>>)
        .OptionsSource(&EnumOptions)
        .OnSelectionChanged_Lambda([this](TSharedPtr<FString> NewItem, ESelectInfo::Type)
            {
                if (NewItem.IsValid())
                {
                    SelectedEnumName = NewItem;
                    FString WrappedName = FString::Printf(TEXT("\"%s\""), **NewItem);
                    GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, WrappedName);
                }
            })
        .OnGenerateWidget_Lambda([](TSharedPtr<FString> InItem)
            {
                return SNew(STextBlock).Text(FText::FromString(*InItem));
            })
        .InitiallySelectedItem(SelectedEnumName)
        .Content()
        [
            SNew(STextBlock).Text_Lambda([this]()
                {
                    return SelectedEnumName.IsValid() ? FText::FromString(*SelectedEnumName) : FText::FromString("Select Enum");
                })
        ];
}
