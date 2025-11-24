#pragma once

#include "CoreMinimal.h"

// Thư viện Unreal
#include "UObject/NoExportTypes.h"

// Thư viện Dialog Plugins
#include "SocialCondition.h"
#include "NarrativeEvent.h"

//Generate
#include "TaleWeaversNodeBase.generated.h"

UCLASS()
class LQTL_PLUGINS_RUNTIME_API UTaleWeaversNodeBase : public UObject
{
    GENERATED_BODY()

public:
    UTaleWeaversNodeBase();

#if WITH_EDITOR

    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

    // Cần biến này vì sẽ phải sort các node các dựa trên vị trí của node đó
    UPROPERTY()
    FVector2D NodePos;

    UFUNCTION(BlueprintCallable, Category = "Events & Conditions")
	void ProcessEvents(APawn* Pawn, APlayerController* Controller, class USocialComponent* SocialComponent, const EEventRuntime Runtime);

    UFUNCTION(BlueprintCallable, Category = "Events & Conditions")
	bool AreConditionsMet(APawn* Pawn, APlayerController* Controller, class USocialComponent* SocialComponent);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Events & Conditions")
	TArray<class UNarrativeEvent*> Events;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Events & Conditions")
	TArray<class USocialCondition*> Conditions;


    void SetID(const FName& NewID) 
	{
		ID = NewID;
		EnsureUniqueID();
	};

    FORCEINLINE FName GetID() const {return ID;};

protected:
    virtual void EnsureUniqueID(){};

	/**An optional ID for this node, can be left empty*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Details", meta = (DisplayPriority = 0))
	FName ID;
};