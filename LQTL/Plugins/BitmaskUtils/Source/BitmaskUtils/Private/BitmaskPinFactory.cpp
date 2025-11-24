#include "BitmaskPinFactory.h"

#include "Pin/SGraphPin_BitmaskEnum.h"
#include "Pin/SGraphPin_EnumSelector.h"

TSharedPtr<SGraphPin> FBitmaskPinFactory::CreatePin(UEdGraphPin* Pin) const
{
    if (Pin->PinName == TEXT("Enum Type"))
    {
        return SNew(SGraphPin_EnumSelector, Pin);
    }

    if (Pin->PinName == TEXT("Bitmask A") || Pin->PinName == TEXT("Bitmask B"))
    {
        return SNew(SGraphPin_BitmaskEnum, Pin);
    }

    return nullptr;
}