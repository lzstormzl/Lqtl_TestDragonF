// Copyright Narrative Tools 2022. 

#pragma once

#include "CoreMinimal.h"
#include "KismetCompiler.h"
#include "DialogueBlueprint.h"

#include "KismetCompilerModule.h"

//////////////////////////////////////////////////////////////////////////
// FDialogueBlueprintCompiler 
class LQTL_PLUGINS_API FDialogueBlueprintCompiler : public IBlueprintCompiler
{

public:

	FDialogueBlueprintCompiler();

	bool CanCompile(const UBlueprint* Blueprint) override;
	void PreCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions) override;
	void Compile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions, FCompilerResultsLog& Results) override;
	void PostCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions) override;
	virtual bool GetBlueprintTypesForClass(UClass* ParentClass, UClass*& OutBlueprintClass, UClass*& OutBlueprintGeneratedClass) const override;

};


/**
 * old blueprint generated class to the new regenerated class after a compile happens  
 */
class LQTL_PLUGINS_API FDialogueBlueprintCompilerContext : public FKismetCompilerContext
{
public:

FDialogueBlueprintCompilerContext(UDialogueBlueprint* SourceDialogueBP, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions);
virtual ~FDialogueBlueprintCompilerContext();

protected:

	UDialogueBlueprint* GetDialogueBlueprint() const { return Cast<UDialogueBlueprint>(Blueprint); }

	typedef FKismetCompilerContext Super;

	// FKismetCompilerContext
	//virtual void CreateFunctionList() override;

	/*
	 	* SpawnNewClass() là một hàm ảo (virtual) của FKismetCompilerContext
		* Tạo ra UBlueprintGeneratedClass mới cho Blueprint đang được compile.
		* Nó được gọi ở đầu quy trình compile để sinh ra “GeneratedClass” mới — cái class mà Blueprint của bạn sẽ build vào.
	*/
	virtual void SpawnNewClass(const FString& NewClassName) override; 
	virtual void OnNewClassSet(UBlueprintGeneratedClass* ClassToUse) override;
	virtual void PrecompileFunction(FKismetFunctionContext& Context, EInternalCompilerFlags InternalFlags) override;
	virtual void CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOutOldCDO) override;
	virtual void SaveSubObjectsFromCleanAndSanitizeClass(FSubobjectCollection& SubObjectsToSave, UBlueprintGeneratedClass* ClassToClean) override;
	virtual void EnsureProperGeneratedClass(UClass*& TargetClass) override;
	virtual void CopyTermDefaultsToDefaultObject(UObject* DefaultObject);
	virtual void FinishCompilingClass(UClass* Class) override;
	virtual bool ValidateGeneratedClass(UBlueprintGeneratedClass* Class) override;
	virtual bool IsNodePure(const UEdGraphNode* Node) const;
	// End FKismetCompilerContext

protected:

	void FixAbandonedDialogueTemplate(UDialogueBlueprint* DialogueBP);

	class UDialogueBlueprintGeneratedClass* NewDialogueBlueprintClass;

	class UDialogueGraphSchema* DialogueSchema;

};
