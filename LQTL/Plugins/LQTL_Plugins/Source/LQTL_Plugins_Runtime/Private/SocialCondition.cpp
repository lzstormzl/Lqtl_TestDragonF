#include "SocialCondition.h"

bool USocialCondition::CheckCondition_Implementation(APawn* Pawn, APlayerController* Controller)
{
	return true;
}


FString USocialCondition::GetGraphDisplayText_Implementation()
{
	return GetName();
}
