// Copyright Tale Weavers


#include "LQAbilitySystemTypes.h"

#include "DataTableEditorUtils.h"
#include "DataTableEditorUtils.h"
#include "Kismet/KismetStringLibrary.h"
LQ_GAMEPLAYABILITIESSYSTEM_API UE_DEFINE_GAMEPLAY_TAG(LQSetByCaller::ComboAttackMultiplier, "SetByCaller.LQ.ComboAttackMultiplier");
LQ_GAMEPLAYABILITIESSYSTEM_API UE_DEFINE_GAMEPLAY_TAG(LQSetByCaller::AttackTypeMultiplier, "SetByCaller.LQ.AttackTypeMultiplier");
LQ_GAMEPLAYABILITIESSYSTEM_API UE_DEFINE_GAMEPLAY_TAG(LQSetByCaller::AbsoluteDamage, "SetByCaller.LQ.AbsoluteDamage");


void FPreApplyDamageData::CollectSetByCallerData(EExternalAttackDataSource InSource, TMap<FGameplayTag, float>& OutMap) const
{
	if (InSource > MyDataSource)
	{
		OutMap.FindOrAdd(LQSetByCaller::ComboAttackMultiplier, ComboAttackMultiplier);
		OutMap.FindOrAdd(LQSetByCaller::AttackTypeMultiplier, AttackTypeMultiplier);
		OutMap.FindOrAdd(LQSetByCaller::AbsoluteDamage, AbsoluteDamage);
	}
}

void FPreApplyDamageData::PrintDebug() const
{
	UE_LOG(LogTemp, Log, TEXT("PreApplyDamageData: Source: %d, ComboAttackMultiplier: %f, AttackTypeMultiplier: %f, AbsoluteDamage: %f"), (int)MyDataSource, ComboAttackMultiplier,
	       AttackTypeMultiplier,
	       AbsoluteDamage)
}

void FPostApplyDamageDataContainer::ExecutePostApplyDamage(AActor* VictimActor, float AppliedDamage, const FGameplayEffectContextHandle& EffectContext) const
{
	for (const FPostApplyDamageData& PostApplyDamageData : PostApplyDamageDatas)
	{
		PostApplyDamageData.ExecutePostApplyDamage(VictimActor, AppliedDamage, EffectContext);
	}
}

void FLQGameplayAttributeDataTableRow::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
#if WITH_EDITOR
	UDataTable* DataTable = const_cast<UDataTable*>(InDataTable);
	if (DataTable)
	{
		if (auto Found = DataTable->FindRow<FLQGameplayAttributeDataTableRow>(InRowName, TEXT(""), false))
		{
			FDataTableEditorUtils::RenameRow(DataTable, InRowName, (FName)Attribute.GetUProperty()->GetName());
		}
	}
#endif
}
