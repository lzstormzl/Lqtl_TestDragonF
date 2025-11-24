#pragma once

#include "CoreMinimal.h"

#include "SocialComponent.h"
#include "NarrativeEvent.h"

#include "F_EventPhase.generated.h"

USTRUCT(BlueprintType)
struct FEventPhase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Condition")
    TArray<class USocialCondition* > Conditions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Event")
    TArray<class UNarrativeEvent* > Event;
};