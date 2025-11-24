#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "SGraphNode.h"
#include "DialogueGraphNode.h"

class LQTL_PLUGINS_API STWDialogueGraphNode : public SGraphNode
{
public:

    SLATE_BEGIN_ARGS(STWDialogueGraphNode) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UDialogueGraphNode* InNode);

    //~ Begin SGraphNode Interface
    virtual void CreatePinWidgets() override;
    virtual void UpdateGraphNode() override;
	//virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	//virtual void MoveTo( const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty /* = true */ );
	//~ End SGraphNode Interface

protected:

    TSharedPtr<SWidget> DialogueNodeWidgetRef;
};