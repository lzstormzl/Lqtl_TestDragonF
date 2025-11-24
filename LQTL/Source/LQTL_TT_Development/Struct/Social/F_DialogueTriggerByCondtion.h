#pragma once

#include "CoreMinimal.h"

#include "SocialComponent.h"

#include "F_DialogueTriggerByCondtion.generated.h"

USTRUCT(BlueprintType)
struct FDialogueTriggerByCondition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TSubclassOf<UDialogue> DialogueClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName StartFromID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Conditions")
    TArray<class USocialCondition* > Conditions;
};