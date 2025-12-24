#pragma once

#include "EdGraphUtilities.h"
#include "SGraphPin.h"

class FBitmaskPinFactory : public FGraphPanelPinFactory
{
public:
    virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override;
};