#pragma once

#include "CoreMinimal.h"

// Thư viện Unreal
#include "Blueprint/UserWidget.h"

#include "Dialogue/TWDialogueNode.h"
#include "Dialogue/Dialogue.h"

// Generated
#include "TWDialogueNodeUserWidget.generated.h"

UCLASS()
class UTWDialogueNodeUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    void InitializeFromNode(class UTWDialogueNode* InNode, class UDialogue* InDialogue);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Node")        // UPROPERTY của Node Widget: Node
	class UTWDialogueNode* Node;                                    


public:

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Dialogue Node")
    class UVerticalBox* LeftPinBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Dialogue Node")
    class UVerticalBox* RightPinBox;


};
