#pragma once

#include "SGraphPin.h"

class SGraphPin_EnumSelector : public SGraphPin
{
public:
    SLATE_BEGIN_ARGS(SGraphPin_EnumSelector) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

private:
    TSharedRef<SWidget> GenerateEnumDropdown();

    TSharedPtr<FString> SelectedEnumName;
    TArray<TSharedPtr<FString>> EnumOptions;
};