#include "TriggerSequenceTap.h"
#include "EnhancedPlayerInput.h"

ETriggerState UTriggerSequenceTap::UpdateState_Implementation(const UEnhancedPlayerInput* _PlayerInput, FInputActionValue _ModifiedValue, float _DeltaTime)
{
    // Updates HeldDuration
    float lastHeldDuration = HeldDuration;

    ETriggerState State = Super::UpdateState_Implementation(_PlayerInput, _ModifiedValue, _DeltaTime);

    // Trigger tap when released before tap release threshold
    if (IsActuated(LastValue) && State == ETriggerState::None && lastHeldDuration < tapReleaseTimeThreshold)
    {
        // Reset sequence if tap interval duration is exceeded
        float currentTriggerTime = bAffectedByTimeDilation ? _PlayerInput->GetWorld()->GetUnpausedTimeSeconds() : _PlayerInput->GetWorld()->GetRealTimeSeconds();

        if (currentTriggerTime - lastTriggerTime > tapsInterval) {
            ResetSequence();
        }

        isInSequence = true;
        tapCount++;

        lastTriggerTime = currentTriggerTime;

        // Only trigger if tap reaches the required tap count
        // Note: setting State = ETriggerState::OnGoing is not necessary as the trigger does not account for OnGoing trigger event.
        if (tapCount == sequences) {
            State = ETriggerState::Triggered;

            ResetSequence();
        }
    }
    else if (HeldDuration >= tapReleaseTimeThreshold)
    {
        // Once we pass the threshold halt all triggering until released
        State = ETriggerState::None;
    }

    return State;
}