#pragma once 

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraph/EdGraph.h"
#include "Dialogue/Dialogue.h"
#include "TWDialogueGraphSchema.generated.h"

USTRUCT()
struct FDialogueSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	/** Template of node we want to create */
	UPROPERTY()
	class UDialogueGraphNode* NodeTemplate;

	FDialogueSchemaAction_NewNode()
		: FEdGraphSchemaAction()
		, NodeTemplate(nullptr)
	{}

	FDialogueSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
		, NodeTemplate(nullptr)
	{}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End FEdGraphSchemaAction Interface

	template <typename NodeType>
	static NodeType* SpawnNodeFromTemplate(class UEdGraph* ParentGraph, NodeType* InTemplateNode, const FVector2D Location)
	{
		FDialogueSchemaAction_NewNode Action;
		Action.NodeTemplate = InTemplateNode;

		return Cast<NodeType>(Action.PerformAction(ParentGraph, nullptr, Location));
	}

};

USTRUCT()
struct FDialogueSchemaAction_AddComment : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FDialogueSchemaAction_AddComment() : FEdGraphSchemaAction() {}
	FDialogueSchemaAction_AddComment(FText InDescription, FText InToolTip)
		: FEdGraphSchemaAction(FText(), MoveTemp(InDescription), MoveTemp(InToolTip), 0)
	{
	}

	// FEdGraphSchemaAction interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override final;
	// End of FEdGraphSchemaAction interface
};

UCLASS()
class UTWDialogueGraphSchema : public UEdGraphSchema
{
    GENERATED_BODY()

    public:
    // Tạo node default cho graph
    virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override; 

    // Tạo quy luật nối pin giữa các node
    virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* NodeA, const UEdGraphPin* NodeB) const override;

    virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

    static TSharedPtr<FDialogueSchemaAction_NewNode> AddNewNodeAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip);
	static TSharedPtr<FDialogueSchemaAction_AddComment> AddCommentAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& MenuDesc, const FText& Tooltip);
    
};