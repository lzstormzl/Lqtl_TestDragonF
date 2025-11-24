#pragma once

#include "CoreMinimal.h"
#include "QuestGraphNode.h"
#include "QuestGraphNode_Branch.generated.h"

class UQuestBranch;

/**
 *  
 */
UCLASS()
class UQuestGraphNode_Branch : public UQuestGraphNode
{
	GENERATED_BODY()
	
public:

	virtual void AllocateDefaultPins() override;
	virtual void DestroyNode() override;

	virtual FLinearColor GetGraphNodeColor() const override;

	UPROPERTY(EditAnywhere, Category = "Node")
	UQuestBranch* Branch;

};
