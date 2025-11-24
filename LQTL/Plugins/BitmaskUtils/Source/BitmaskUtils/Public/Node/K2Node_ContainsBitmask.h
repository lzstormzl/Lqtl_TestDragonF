#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "SGraphPin.h"

#include "K2Node_ContainsBitmask.generated.h"

UCLASS()
class BITMASKUTILS_API UK2Node_ContainsBitmask : public UK2Node
{
	GENERATED_BODY()
	
public:
    inline virtual bool IsNodePure() const override {
        return true;
    }

    virtual void AllocateDefaultPins() override;

    virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

    virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

    virtual FText GetTooltipText() const override;
};
