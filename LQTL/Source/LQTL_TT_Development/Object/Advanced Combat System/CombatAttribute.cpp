#include "CombatAttribute.h"

#if WITH_EDITOR
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#endif

#if WITH_EDITOR
void UCombatAttributeSet::PreEditChange(FProperty* _PropertyAboutToChange) {
	Super::PreEditChange(_PropertyAboutToChange);

	// On attribute set updated.
	if (_PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, Attributes)) {
		CachedAttributes = Attributes;
	}
}

void UCombatAttributeSet::PostEditChangeProperty(FPropertyChangedEvent& _PropertyChangedEvent) {
	// On attribute set updated.
	if (_PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCombatAttributeSet, Attributes)) {
		TArray<TSubclassOf<UCombatAttribute>> attributeClasses;

		bool nullOccupied = false;
		for (auto attribute : Attributes) {
			// If contains more than 1 "None" class within attribute list
			if (attribute == nullptr) {
				if (nullOccupied) {
					FNotificationInfo Info(FText::FromString("An Attribute class need to be selected before new attributes can be added"));
					Info.ExpireDuration = 3.0f;
					FSlateNotificationManager::Get().AddNotification(Info);
					
					Attributes = CachedAttributes;
					break;
				}

				if (!nullOccupied) nullOccupied = true;

				continue;
			}

			// If contains more than 1 of the same class within attribute list
			if (attributeClasses.Find(attribute->GetClass()) != INDEX_NONE) {
				FNotificationInfo Info(FText::FromString("An AttributeSet can NOT contains duplicated Attribute class"));
				Info.ExpireDuration = 3.0f;
				FSlateNotificationManager::Get().AddNotification(Info);

				Attributes = CachedAttributes;
				break;
			}

			attributeClasses.Add(attribute->GetClass());
		}
	}

	Super::PostEditChangeProperty(_PropertyChangedEvent);
}

void UCombatAttributeSet::PostLoad() {
	Super::PostLoad();

	AttributeClassLookup.Empty();
	AttributeTagLookup.Empty();

	for (auto attribute : Attributes) {
		if (attribute == nullptr) continue;

		for (auto tag : attribute->AttributeTags.GetGameplayTagArray()) {
			auto arr = AttributeTagLookup.Find(tag);
			arr->Add(attribute);
		}
	}
}
#endif

void UCombatAttributeSet::SetAttribute(TObjectPtr<UCombatAttribute> _Attribute, float _Value)
{
	float orgValue = _Attribute->GetValue();
	_Attribute->SetValue(_Value);

	OnAttributeModified.Broadcast(FCombatAttributeModifyCallback{
		_Attribute,
		orgValue,
		_Attribute->GetValue()
		});
}

bool UCombatAttributeSet::AddAttributeByClass(TSubclassOf<UCombatAttribute> _AttributeClass, float _Value)
{
	auto attributePtr = AttributeClassLookup.Find(_AttributeClass);
	if (attributePtr == nullptr) return false;

	auto attribute = attributePtr->Get(); 
	
	SetAttribute(attribute, attribute->GetValue() + _Value);

	return true;
}

bool UCombatAttributeSet::AddAttributeByTag(FGameplayTag _AttributeTag, float _Value)
{
	auto attArr = AttributeTagLookup.Find(_AttributeTag);
	if (attArr == nullptr) return false;

	for (auto attribute : *attArr) {
		SetAttribute(attribute, attribute->GetValue() + _Value);
	}

	return true;
}

bool UCombatAttributeSet::MultAttributeByClass(TSubclassOf<UCombatAttribute> _AttributeClass, float _Value)
{
	auto attributePtr = AttributeClassLookup.Find(_AttributeClass);
	if (attributePtr == nullptr) return false;

	auto attribute = attributePtr->Get();

	SetAttribute(attribute, attribute->GetValue() + _Value);

	return true;
}

bool UCombatAttributeSet::MultAttributeByTag(FGameplayTag _AttributeTag, float _Value)
{
	auto attArr = AttributeTagLookup.Find(_AttributeTag);
	if (attArr == nullptr) return false;

	for (auto attribute : *attArr) {
		SetAttribute(attribute, attribute->GetValue() * _Value);
	}

	return true;
}
