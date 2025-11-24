#pragma once

#include "CoreMinimal.h"

// Thư viện Unreal
#include "EdGraph/EdGraphNode.h"

//Thư viện Dialogue Plugins
#include "Dialogue/TWDialogueNode.h"

// Generate
#include "DialogueGraphNode.generated.h"

UCLASS()
class UDialogueGraphNode : public UEdGraphNode
{
    GENERATED_BODY()

public:

    UPROPERTY(Transient)
    class UDialogueGraphNode* ParentNode;

    UPROPERTY()
    TArray<UDialogueGraphNode*> SubNodes;

    UPROPERTY()
    class UTWDialogueNode* TWDialogueNode; // To do: Tạo ra info cho dialogue node này

    // Function
    // Lấy Title của Graph Node
    virtual FText GetNodeTitleText() const { return FText::GetEmpty(); }
    //
    virtual FText GetNodeText() const;

    // Override từ UEdGraphNode
    virtual void PostPlacedNewNode() override; // được gọi ra khi tạo node mới có thể set up thêm data ở đây

    // Tự động nối dây mới tạo với pin từ node khác
    virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
    virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
    //virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const override;

    // @return the input pin for this state
    virtual UEdGraphPin* GetInputPin(int32 InputIndex = 0) const;
    // @return the output pin for this state
    virtual UEdGraphPin* GetOutputPin(int32 InputIndex = 0) const;

    // virtual FLinearColor GetGraphNodeColor() const { return FLinearColor(0.15f, 0.15f, 0.15f); };


    UPROPERTY()
    	class UK2Node_CustomEvent* OnPlayedCustomNode;

    UFUNCTION()
    	void OnStartedOrFinished(UTWDialogueNode* Node, bool bStarted);
};