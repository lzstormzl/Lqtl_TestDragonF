#include "Pin/SGraphPin_BitmaskEnum.h"

#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/AppStyle.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"


void SGraphPin_BitmaskEnum::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
    SetCursor(EMouseCursor::Default);
    bShowLabel = true;
    GraphPinObj = InPin;

    this->ChildSlot
        [
            GenerateEnumBitmaskWidget()
        ];
}



TSharedRef<SWidget> SGraphPin_BitmaskEnum::GetDefaultValueWidget()
{
    return SNew(STextBlock).Text(FText::FromString("Bitmask Editor"));
}

TSharedRef<SWidget> SGraphPin_BitmaskEnum::GenerateEnumBitmaskWidget()
{
    TSharedRef<SVerticalBox> Box = SNew(SVerticalBox);
    UEnum* Enum = GetResolvedEnum();

    if (!Enum)
    {
        return SNew(STextBlock).Text(FText::FromString("Invalid enum"));
    }

    int32 CurrentValue = FCString::Atoi(*GraphPinObj->GetDefaultAsString());

    for (int32 i = 0; i < Enum->NumEnums(); ++i)
    {
        const int64 FlagValue = Enum->GetValueByIndex(i);
        const FString EnumName = Enum->GetNameStringByIndex(i);

        if (EnumName.Contains(TEXT("MAX")))
            continue;

        Box->AddSlot()
            .AutoHeight()
            [
                SNew(SCheckBox)
                    .IsChecked_Lambda([=, this]() {
                        return (CurrentValue & FlagValue) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    .OnCheckStateChanged_Lambda([=, this](ECheckBoxState State) {
                        int32 NewValue = FCString::Atoi(*GraphPinObj->GetDefaultAsString());
                        if (State == ECheckBoxState::Checked)
                            NewValue |= FlagValue;
                        else
                            NewValue &= ~FlagValue;

                        GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, FString::FromInt(NewValue));
                    })
                    .Content()
                    [
                        SNew(STextBlock).Text(FText::FromString(EnumName))
                    ]
            ];
    }

    return Box;
}

UEnum* SGraphPin_BitmaskEnum::GetResolvedEnum() const
{
    return Cast<UEnum>(GraphPinObj->PinType.PinSubCategoryObject.Get());
}