// Fill out your copyright notice in the Description page of Project Settings.


#include "Node/K2Node_ContainsBitmask.h"

#include "KismetCompiler.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_CallFunction.h"
#include "Kismet/KismetMathLibrary.h"
#include "EdGraphSchema_K2.h"

#define LOCTEXT_NAMESPACE "K2Node"



void UK2Node_ContainsBitmask::AllocateDefaultPins()
{
    // Enum Pin
    UEdGraphPin* EnumTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, TEXT("Enum Type"));
    EnumTypePin->bHidden = false; // Show it, but override with custom UI
    EnumTypePin->bAdvancedView = false;

    // Bitmask Pin
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, TEXT("Bitmask A"));

    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, TEXT("Bitmask B"));

    // Advanced Controls Pin
    auto ContainsAll = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, TEXT("Contains All"));
    ContainsAll->bAdvancedView = true;

    // Output Pin
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, TEXT("Result"));
}



void UK2Node_ContainsBitmask::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    UK2Node::ExpandNode(CompilerContext, SourceGraph);

    const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
    UEdGraphPin* BitmaskAPin = FindPin(TEXT("BitmaskA"));
    UEdGraphPin* BitmaskBPin = FindPin(TEXT("BitmaskB"));
    UEdGraphPin* OutputPin = FindPin(TEXT("Result"));

    // A & B
    auto* AndNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    AndNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, And_IntInt), UKismetMathLibrary::StaticClass());
    AndNode->AllocateDefaultPins();

    Schema->TryCreateConnection(BitmaskAPin, AndNode->FindPin(TEXT("A")));
    Schema->TryCreateConnection(BitmaskBPin, AndNode->FindPin(TEXT("B")));

    // (A & B) == B
    auto* EqualNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    EqualNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, EqualEqual_IntInt), UKismetMathLibrary::StaticClass());
    EqualNode->AllocateDefaultPins();

    Schema->TryCreateConnection(AndNode->GetReturnValuePin(), EqualNode->FindPin(TEXT("A")));
    Schema->TryCreateConnection(BitmaskBPin, EqualNode->FindPin(TEXT("B")));

    CompilerContext.MovePinLinksToIntermediate(*OutputPin, *EqualNode->GetReturnValuePin());

    BreakAllNodeLinks();
}



FText UK2Node_ContainsBitmask::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("CompareBitmask", "Contains Bitmask");
}



FText UK2Node_ContainsBitmask::GetTooltipText() const
{
    return LOCTEXT("CompareBitmaskTooltip", "Returns true if (A & B) == B");
}



void UK2Node_ContainsBitmask::GetMenuActions(FBlueprintActionDatabaseRegistrar& Registrar) const
{
    if (Registrar.IsOpenForRegistration(GetClass()))
    {
        auto* Spawner = UBlueprintNodeSpawner::Create(GetClass());
        Registrar.AddBlueprintAction(GetClass(), Spawner);
    }
}



#undef LOCTEXT_NAMESPACE