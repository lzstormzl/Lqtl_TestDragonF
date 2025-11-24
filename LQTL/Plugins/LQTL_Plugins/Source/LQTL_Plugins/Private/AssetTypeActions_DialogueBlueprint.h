#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

// Class này dùng để định nghĩa Asset này cho engine biết

class FAssetTypeActions_DialogueBlueprint : public FAssetTypeActions_Blueprint
{

public:
    FAssetTypeActions_DialogueBlueprint(uint32 InAssetCategory); // Constructor

    uint32 Category; // Category (số thứ tự)

    // IAssetTypeAction Implementation

    virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_DialogueBlueprint", "Dialogue Blueprint");}
    virtual FColor GetTypeColor() const override { return FColor(255, 55, 120); }
    virtual UClass* GetSupportedClass() const override; // class được support -> hiện tại sẽ là UDialogueBlueprint

    // Function khi mở Asset
    virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

    virtual uint32 GetCategories() override; // Lấy category
};