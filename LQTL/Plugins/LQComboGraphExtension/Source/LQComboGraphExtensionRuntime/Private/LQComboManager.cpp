// Copyright Tale Weavers


#include "LQComboManager.h"
#include "GraphInstance/ComboGraphInstance_ASC.h"


// Sets default values for this component's properties
ULQComboManager::ULQComboManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	// You can turn these features off to improve performance if you don't need them.
	// ...
}

bool ULQComboManager::ActivateComboGraphFromAsset(const TSoftObjectPtr<UComboGraphAsset>& ComboGraphAsset)
{
	if (ComboGraphAsset.IsNull())
	{
		return false;
	}

	// Check if the combo graph instance already exists
	UComboGraphInstance* Instance = GetComboGraphInstanceByAsset(ComboGraphAsset);
	
	if (!Instance)
	{
		// Grant the combo graph if it hasn't been granted yet
		FComboGraphGrantParams GrantParams;
		GrantParams.ComboGraphAsset = ComboGraphAsset;
		GrantParams.GraphInstance = UComboGraphInstance_ASC::StaticClass();
		
		if (!GrantComboGraph(GrantParams))
		{
			return false;
		}
		
		// Get the instance again after granting
		Instance = GetComboGraphInstanceByAsset(ComboGraphAsset);
	}
	
	// Set the combo graph instance as active
	if (Instance && IsValid(Instance))
	{
		SetComboInstanceActive(Instance);
		return true;
	}
	
	return false;
}
