#include "TaleWeaversNodeBase.h"

#include "SocialComponent.h"

UTaleWeaversNodeBase::UTaleWeaversNodeBase()
{
    ID = GetFName();
}

#if WITH_EDITOR

void UTaleWeaversNodeBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.MemberProperty)
	{
		//If we changed the ID, make sure it doesn't conflict with any other IDs in the quest
		if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UTaleWeaversNodeBase, ID))
		{
			EnsureUniqueID();
		}
	}
}

#endif

void UTaleWeaversNodeBase::ProcessEvents(APawn* Pawn, APlayerController* Controller, class USocialComponent* SocialComponent, const EEventRuntime Runtime)
{
	if (!SocialComponent)
	{
		UE_LOG(LogSocial, Warning, TEXT("Tried running events on node %s but Social Comp was null."), *GetNameSafe(this));
	}

	for (auto& Event : Events)
	{
		if (Event && (Event->EventRuntime == Runtime || Event->EventRuntime == EEventRuntime::Both))
		{
			 USocialComponent* CompToUse = /*Event->bUseSharedComponent && SocialComponent->SharedNarrativeComp ? SocialComponent->SharedNarrativeComp :*/SocialComponent;

			if (!CompToUse && Event->bUseSharedComponent)
			{
				UE_LOG(LogSocial, Warning, TEXT("Event %s on node %s uses bUseSharedComponent but SharedNarrativeComp was null. Falling back to local Social comp..."), *GetNameSafe(Event), *GetNameSafe(this));
				CompToUse = SocialComponent;
			}

			if (CompToUse)
			{
				if (Event)
				{
					Event->ExecuteEvent(Pawn, Controller, CompToUse);
				}
			}
			else
			{
				UE_LOG(LogSocial, Warning, TEXT("Tried running event %s on node %s but Social Comp was null."), *GetNameSafe(Event), *GetNameSafe(this));
			}
		}
	}
}

bool UTaleWeaversNodeBase::AreConditionsMet(APawn* Pawn, APlayerController* Controller, class USocialComponent* SocialComponent)
{

	if (!SocialComponent)
	{
		UE_LOG(LogSocial, Warning, TEXT("Tried running conditions on node %s but Social Comp was null."), *GetNameSafe(this));
	}

	//Ensure all conditions are met
	for (auto& Cond : Conditions)
	{	
		if (Cond)
		{
			
			//USocialComponent* CompToCheck = Cond->bCheckSharedComponent && SocialComponent->SharedNarrativeComp ? SocialComponent->SharedNarrativeComp : SocialComponent;

			USocialComponent* CompToCheck = SocialComponent;

			if (!CompToCheck /*&& Cond->bCheckSharedComponent*/)
			{
				UE_LOG(LogSocial, Warning, TEXT("Condition %s on node %s uses bCheckSharedComponent but SharedNarrativeComp was null. Falling back to local Narrative comp..."), *GetNameSafe(Cond), *GetNameSafe(this));
				CompToCheck = SocialComponent;
			}

			if (CompToCheck)
			{
				// Nếu có tick vào bNot thì trả về false nếu condition đó đúng
				if (Cond && Cond->CheckCondition(Pawn, Controller, CompToCheck) == Cond->bNot)
				{
					return false;
				}
			}
			else
			{
				UE_LOG(LogSocial, Warning, TEXT("Tried running condition %s on node %s but Social Comp was null."), *GetNameSafe(Cond), *GetNameSafe(this));
			}
		}
	}

	return true;
}