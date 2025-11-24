//

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Blueprint.h"
#include "Dialogue/Dialogue.h"
#include "DialogueBlueprint.generated.h"

// class UDialogue;

/**
 * A special type of blueprint that includes a node based dialogue editor.
 */
UCLASS(BlueprintType)
class UDialogueBlueprint : public UBlueprint 
{
	GENERATED_BODY()
	
public:

	UDialogueBlueprint();

public:

	/** Graph for dialogue asset */
	UPROPERTY()
	class UEdGraph* DialogueGraph; // Dialogue Graph

    // To Do
	UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
	UDialogue* DialogueTemplate;

	//If set, the factory will use this legacy asset as a template for the new asset
	// UPROPERTY()
	// TObjectPtr<class UDialoguesAsset> LegacyAsset;

	virtual UClass* GetBlueprintClass() const override; //	Loại generate class nào ?

	// Asset có thể được tạo thông qua Default Blueprint Factory không?
	virtual bool SupportedByDefaultBlueprintFactory() const override
	{ 
		return false;
	}

	// Có thể compile trong quá trình chơi không?
	virtual bool CanAlwaysRecompileWhilePlayingInEditor() const override { return true; }

	virtual void PostLoad() override;

	// Để false bắt buộc full compile để tránh lỗi runtime
	virtual bool IsValidForBytecodeOnlyRecompile() const override { return false; }


	// Khi một Blueprint được compile, engine sẽ tạo ra 1 UBlueprintGenenratedClass (class runtime)
	// Hàm này được gọi ra để xác thực rằng InClass (class vừa được sinh ra) có đúng kiểu và hợp lệ so với loại Blueprint đó không
	// Nếu không hợp lệ -> Compile sẽ fail, báo lỗi, hoặc asset coi như "corrupted"
	static bool ValidateGeneratedClass(const UClass* InClass);
};