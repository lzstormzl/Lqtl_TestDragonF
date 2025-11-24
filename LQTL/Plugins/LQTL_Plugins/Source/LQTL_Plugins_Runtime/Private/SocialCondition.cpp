#include "SocialCondition.h"


bool USocialCondition::CheckCondition_Implementation(APawn* Pawn, APlayerController* Controller, class USocialComponent* SocialComponent)
{
	return true;
}

FString USocialCondition::GetGraphDisplayText_Implementation()
{
	return GetName();
}
