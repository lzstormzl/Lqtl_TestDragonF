#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditAction.h"

//Thư viện Dialogue Plugins
#include "DialogueGraphNode.h"

#include "TWDialogueGraph.generated.h"

UCLASS()
class UTWDialogueGraph : public UEdGraph
{
    GENERATED_BODY()

    public:

    virtual void OnCreated() {}
    virtual void OnLoaded() {}
    virtual void Initialize() {}
    virtual void NotifyGraphChanged(const FEdGraphEditAction& Action);


    // Chạy khi có 1 pin được nối với pin khác
    void PinRewired(UDialogueGraphNode* Node, UEdGraphPin* Pin);

    // Thêm node (kiểm tra node thêm là loại gì để thêm vào)
    void NodeAdded(UEdGraphNode* AddedNode);

protected:

	class UDialogueNode_NPC* MakeNPCReply(class UDialogueGraphNode_NPC* Node, class UDialogue* Dialogue);
    class UDialogueNode_Player* MakePlayerReply(class UDialogueGraphNode_Player* Node, class UDialogue* Dialogue);

};