#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "DialogueAssetFactory.generated.h"

UCLASS()
class UDialogueAssetFactory : public UFactory
{
    GENERATED_BODY()

public:
    UDialogueAssetFactory();

    // UFactory interface implementation
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
    
    virtual bool CanCreateNew() const override { return true; }

    virtual FString GetDefaultNewAssetName() const override
    {
        return FString(TEXT("NewDialogue"));
    }

    virtual FText GetDisplayName() const override { return FText::FromString("Dialogue"); }

    UPROPERTY(EditAnywhere, Category = "Dialogue Asset")
    TSubclassOf<class UDialogueBlueprint> ParentClass;
};