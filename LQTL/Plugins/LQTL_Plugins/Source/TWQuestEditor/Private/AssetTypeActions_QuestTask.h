
#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

class FAssetTypeActions_QuestTask : public FAssetTypeActions_Blueprint
{
public:

	// Constructor
	FAssetTypeActions_QuestTask(uint32 InAssetCategory);

	//Category
	uint32 Category;

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_QuestTask", "Task"); };
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;

};

//Just put conditions and events in here too 
class FAssetTypeActions_SocialCondition : public FAssetTypeActions_Blueprint
{
public:

	FAssetTypeActions_SocialCondition(uint32 InAssetCategory);

	uint32 Category;

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_SocialCondition", "Social Condition"); };
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;

	/** Return the factory responsible for creating this type of Blueprint */
	virtual UFactory* GetFactoryForBlueprintType(UBlueprint* InBlueprint) const;
};

// Tạo các function event để game Designer sử dụng (nhưng hiện tại design có thể tự tạo thông qua blueprint bằng cách gọi tên các function khác vào trong Event start/finished của 1 TaleWeaverNodeBase nên tạm thời chưa cần)

class FAssetTypeActions_NarrativeEvent : public FAssetTypeActions_Blueprint
{
public:

	FAssetTypeActions_NarrativeEvent(uint32 InAssetCategory);

	uint32 Category;

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_NarrativeEvent", "Narrative Event"); };
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;

	/** Return the factory responsible for creating this type of Blueprint */
	virtual UFactory* GetFactoryForBlueprintType(UBlueprint* InBlueprint) const;
};
