// Copyright Tale Weavers

#include "FloatingTextWidget.h"

void UFloatingTextWidget::InitializeWithData_Implementation(const TInstancedStruct<FFloatingTextDataBase>& InData)
{
	CachedData = InData;
}

void UFloatingTextWidget::OnActivated_Implementation()
{
	// Override in Blueprint for custom activation logic (animations, etc.)
}

void UFloatingTextWidget::OnDeactivated_Implementation()
{
	// Override in Blueprint for custom deactivation logic
}

void UFloatingTextWidget::ResetWidget()
{
	CachedData.Reset();
}
