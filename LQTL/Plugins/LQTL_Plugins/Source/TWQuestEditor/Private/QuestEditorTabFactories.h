#pragma once

#include "CoreMinimal.h"

// Thư viện Unreal
#include "GraphEditor.h"
#include "EdGraph/EdGraph.h"
#include "Widgets/SWidget.h"
#include "Misc/Attribute.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

//Thư viện Plugin
#include "QuestGraphEditor.h"

struct FQuestGraphEditorSummoner : public FDocumentTabFactoryForObjects<UEdGraph>
{
public:

	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditorWidget, UEdGraph*);

	FQuestGraphEditorSummoner(TSharedPtr<class FQuestGraphEditor> InQuestEditorPtr);

	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;
	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override;

protected:
	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual void SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const override;

protected:
	TWeakPtr<class FQuestGraphEditor> QuestEditorPtr;
	FOnCreateGraphEditorWidget OnCreateGraphEditorWidget;

};


struct FQuestDetailsSummoner : public FWorkflowTabFactory
{
public:
	FQuestDetailsSummoner(TSharedPtr<class FQuestGraphEditor> InQuestEditorPtr);

	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<class FQuestGraphEditor> QuestEditorPtr;
};