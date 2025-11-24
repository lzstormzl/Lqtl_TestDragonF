#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "TriggerSequenceTap.generated.h"

/** 
UTriggerSequenceTap
Trigger fires a sequence of taps are performed.
*/
UCLASS(NotBlueprintable, meta = (DisplayName = "Sequence Tap"))
class LQTL_TT_DEVELOPMENT_API UTriggerSequenceTap : public UInputTriggerTimedBase
{
	GENERATED_BODY()

private:
    bool isInSequence = false;
    int32 tapCount = 0;
    double lastTriggerTime = 0.f;

    inline void ResetSequence() {
        isInSequence = false;
        tapCount = 0;
        lastTriggerTime = 0;
    }

protected:
    virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override;

public:
    virtual ETriggerEventsSupported GetSupportedTriggerEvents() const override { return ETriggerEventsSupported::Instant; }

    // How many taps must be detected before triggering
    UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category = "Trigger Settings", meta = (ClampMin = "2"))
    int32 sequences = 2;

    // Taps triggered winthin this time-frame are considered a sequence
    UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category = "Trigger Settings", meta = (ClampMin = "0"))
    float tapsInterval = 0.3f;

    // Release within this time-frame to trigger a tap
    UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category = "Trigger Settings", meta = (ClampMin = "0"))
    float tapReleaseTimeThreshold = 0.2f;
};
