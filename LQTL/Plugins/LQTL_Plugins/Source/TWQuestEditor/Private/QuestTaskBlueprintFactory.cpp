#include "QuestTaskBlueprintFactory.h"
#include "Quest/QuestTask.h"
#include "SocialCondition.h"
#include "NarrativeEvent.h"
#include "QuestTaskBlueprint.h"
#include <Kismet2/KismetEditorUtilities.h>

UQuestTaskBlueprintFactory::UQuestTaskBlueprintFactory()
{
	SupportedClass = UQuestTaskBlueprint::StaticClass();
	ParentClass = UQuestTask::StaticClass();
	bSkipClassPicker = true;
}

UObject* UQuestTaskBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	// Make sure we are trying to factory a blueprint, then create and init one
	check(Class->IsChildOf(UBlueprint::StaticClass()));

	return FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BPTYPE_Normal, UQuestTaskBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext);
}

UObject* UQuestTaskBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

UNarrativeEventBlueprintFactory::UNarrativeEventBlueprintFactory()
{
	SupportedClass = UNarrativeEvent::StaticClass();//Parent class will actually be UBlueprint, we need to follow Factory selector
	ParentClass = UNarrativeEvent::StaticClass();
	bSkipClassPicker = true;
}

UObject* UNarrativeEventBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	// Make sure we are trying to factory a blueprint, then create and init one
	//check(Class->IsChildOf(UBlueprint::StaticClass()));

	return FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext);
}

UObject* UNarrativeEventBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

USocialConditionFactory::USocialConditionFactory()
{
	SupportedClass = USocialCondition::StaticClass();//Parent class will actually be UBlueprint, we need to follow Factory selector
	ParentClass = USocialCondition::StaticClass();
	bSkipClassPicker = true;
}

UObject* USocialConditionFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	// Make sure we are trying to factory a blueprint, then create and init one
	//check(Class->IsChildOf(UBlueprint::StaticClass()));

	return FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext);
}

UObject* USocialConditionFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}
