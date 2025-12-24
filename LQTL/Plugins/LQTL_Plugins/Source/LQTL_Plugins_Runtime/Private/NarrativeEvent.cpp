// Copyright Tale Weavers

#include "NarrativeEvent.h"

FString UNarrativeEvent::GetGraphDisplayText_Implementation()
{
	return GetName();
}

bool UNarrativeEvent::ExecuteEvent_Implementation(APawn* Pawn, APlayerController* Controller)
{
	return true;
}
