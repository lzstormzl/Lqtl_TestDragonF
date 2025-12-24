// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSets/LQCharacterAttributeSet.h"

#include "LQAbilitySystemTypes.h"

void ULQCharacterAttributeSet::InitFromMetaDataTable(const UDataTable* DataTable)
{
	static const FString Context = FString(TEXT("UAttribute::BindToMetaDataTable"));

	TArray<FLQGameplayAttributeDataTableRow*> OutRows;
	DataTable->GetAllRows(TEXT(""), OutRows);
	if (auto ASC = GetOwningAbilitySystemComponent())
	{
		for (FLQGameplayAttributeDataTableRow* Row : OutRows)
		{
			if (Row)
			{
				ASC->ApplyModToAttribute(Row->Attribute, EGameplayModOp::Override, Row->Value);
			}
		}
	}
	PrintDebug();
}
