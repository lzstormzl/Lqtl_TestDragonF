#include "NarrativeEvent.h"


bool UNarrativeEvent::ExecuteEvent_Implementation(APawn* Pawn, APlayerController* Controller, class USocialComponent* SocialComponent)
{
	return true;
}

FString UNarrativeEvent::GetGraphDisplayText_Implementation()
{
	return GetName();
}
