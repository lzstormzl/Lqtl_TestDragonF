// Copyright Tale Weavers


#include "LQComboActionPass_ActivateAbility.h"

#include "AbilitySystemGlobals.h"
#include "LQAbilitySystemComponent.h"
#include "LQAbilitySystemLibrary.h"
#include "LQGameplayAbilitiesSettings.h"
#include "LQGameplayAbility.h"
#include "ComboManager/ComboManagerComponent.h"
#include "GraphInstance/ComboGraphInstance.h"

bool ULQComboActionPass_ActivateAbility::ExecutePass_Implementation(UComboGraphInstance* GraphInstance) const
{
	if (!IsValid(GraphInstance) || !GraphInstance->IsValidInstance())
	{
		return false;
	}

	if (auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GraphInstance->GetManagerComponent()->GetOwner()))
	{
		return ULQAbilitySystemLibrary::TryActivateAbilityWithPayload(ASC, AbilityClass.Get(), Payload);
	}
	return false;
}

TArray<FSoftObjectPath> ULQComboActionPass_ActivateAbility::GetPreviewAssets() const
{
	if (Payload.IsValid())
	{
		const FLQCombatAnimationAbilityPayload& AnimationPayload = Payload.Get<FLQCombatAnimationAbilityPayload>();
		return {AnimationPayload.Montage.ToSoftObjectPath()};
	}
	return Super::GetPreviewAssets();
}

FString ULQComboActionPass_ActivateAbility::GetPassInformation_Implementation() const
{
	return FString::Format(TEXT("Activate LQ.Ability:\n----\n{0}"), {IsValid(AbilityClass) ? AbilityClass->GetName() : "None"});
}
#if WITH_EDITOR
void ULQComboActionPass_ActivateAbility::SetAttackTypeMutipllierOnChanged()
{
	if (Payload.IsValid())
	{
		auto& Temp = Payload.GetMutable<FLQCombatAnimationAbilityPayload>();
		if (Temp.ExternalAttackData.IsValid() && Temp.ExternalAttackData.GetScriptStruct()->IsChildOf(FPreApplyDamageData::StaticStruct()))
		{
			if (auto Setting = GetDefault<ULQGameplayAbilitiesSettings>())
			{
				auto AttackTypeMultiplier = Setting->AttackTypeDamageMutipliers.FindRef(Temp.AbilityTag);
				auto PreApplyDamageData = Temp.ExternalAttackData.GetMutablePtr<FPreApplyDamageData>();
				PreApplyDamageData->AttackTypeMultiplier = AttackTypeMultiplier;
				PreApplyDamageData->MyDataSource = EExternalAttackDataSource::EADS_ComboGraph;
			}
		}
	}
}

void ULQComboActionPass_ActivateAbility::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ULQComboActionPass_ActivateAbility, Payload))
	{
		SetAttackTypeMutipllierOnChanged();
	}
}
#endif
