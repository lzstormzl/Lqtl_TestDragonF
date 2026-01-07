// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSets/LQCharacterAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "LQAbilitySystemComponent.h"
#include "LQAbilitySystemTypes.h"

void ULQCharacterAttributeSet::InitFromMetaDataTable(const UDataTable* DataTable)
{
	static const FString Context = FString(TEXT("UAttribute::BindToMetaDataTable"));

	for (TFieldIterator<FProperty> It(GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		FProperty* Property = *It;
		FString RowNameStr = FString::Printf(TEXT("%s"), *Property->GetName());
		FLQGameplayAttributeDataTableRow* MetaData = DataTable->FindRow<FLQGameplayAttributeDataTableRow>(FName(*RowNameStr), Context, false);
		if (MetaData)
		{
			FStructProperty* StructProperty = CastField<FStructProperty>(Property);
			check(StructProperty);
			FGameplayAttributeData* DataPtr = StructProperty->ContainerPtrToValuePtr<FGameplayAttributeData>(this);
			check(DataPtr);
			DataPtr->SetBaseValue(MetaData->Value);
			DataPtr->SetCurrentValue(MetaData->Value);
		}
	}
	PrintDebug();
}

bool ULQCharacterAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetCurrentHPAttribute() && GetCurrentHP() > 0.f)
	{
		if (auto ASC = Cast<ULQAbilitySystemComponent>(&Data.Target))
		{
			ASC->NotifyBeforeApplyDamageOnSelf(Data.EffectSpec);
		}
	}
	return Super::PreGameplayEffectExecute(Data);
}


void ULQCharacterAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Magnitude <= 0.f && Data.EvaluatedData.Attribute.IsValid() && Data.EvaluatedData.Attribute == ULQCharacterAttributeSet::GetCurrentHPAttribute())
	{
		if (auto ASC = Cast<ULQAbilitySystemComponent>(GetOwningAbilitySystemComponent()))
		{
			ASC->NotifyPostApplyDamageOnSelf(FMath::Abs(Data.EvaluatedData.Magnitude), Data.EffectSpec);
		}
	}
}
