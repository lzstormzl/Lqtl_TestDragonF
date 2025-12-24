//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#pragma once

#include "EMSData.h"
#include "EMSMisc.h"
#include "EMSLevel.h"
#include "EMSVersion.h"
#include "EMSPluginSettings.h"
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Misc/EngineVersion.h"
#include "EMSObjectBase.generated.h"

class UTextureRenderTarget2D;
class UTexture2D;
class UBlueprintAsyncActionBase;
class UEMSInfoSaveGame;
class UEMSCustomSaveGame;
class FBufferArchive;
class FMemoryReader;
class FMemoryWriter;

UCLASS(config = EmsUser, configdonotcheckdefaults, abstract, NotBlueprintType, NotBlueprintable)
class EASYMULTISAVE_API UEMSObjectBase : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UEMSObjectBase();

/** Variables  */

protected:

	const uint32 PlayerIndex = 0;

	uint8 bLoadFromMemory : 1;

private:

	FSaveVersionInfo LastReadVersion;
	FPackageFileVersion LoadedPackageVersion;
	FEngineVersion LoadedEngineVersion;

private:

	UPROPERTY(config)
	FString CurrentSaveGameName;

	UPROPERTY(config)
	FString CurrentSaveUserName;

	float LastSlotSaveTime = 0.f;

	UPROPERTY(Transient)
	TMap<FString, TObjectPtr<UEMSInfoSaveGame>> CachedSaveSlots;

	UPROPERTY(Transient)
	TMap<FString, TObjectPtr<UEMSCustomSaveGame>> CachedCustomSaves;

	TArray<FName> MultiSaveLevels;

/** Default Implementations  */

public:

	UWorld* GetWorld() const override;
	const bool HasValidWorld() const;

protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool UnpackActorArchive(const EDataLoadType LoadType, FMemoryReader& FromBinary) { return false; }

	inline void ClearLoadFromMemory()
	{
		//This is only used in EMSObject, but implemented here
		bLoadFromMemory = false;
	}

/** Save Object Creation  */

private:

	template <class TSaveGame>
	TSaveGame* CreateNewSaveObject(const FString& FullSavePath, const FSoftClassPath& InClassName);

/** Custom Saves  */

public:

	UEMSCustomSaveGame* GetCustomSave(const TSubclassOf<UEMSCustomSaveGame>& SaveGameClass, const FString& InSlotName, const FString& InFileName);
	bool SaveCustom(UEMSCustomSaveGame* SaveGame);
	bool SaveAllCustomObjects();

	FCustomSaveInfo GetCustomSaveInfo(const UEMSCustomSaveGame* SaveGame) const;
	void DeleteCustomSave(UEMSCustomSaveGame* SaveGame);
	void ResetCustomSave(UEMSCustomSaveGame* SaveGame, const EResetCustomSaveType Type);

	bool DoesCustomSaveExist(const FString& InSlotName, const FString& InFileName) const;

private:

	void ClearCustomSaveByRef(const FString& CachedRefName);
	void ClearCachedCustomSaves();
	void ClearCustomSavesDesktop(const FString& SaveGameName);
	void ClearCustomSavesConsole(const FString& SaveGameName, const bool bAddFiles, TArray<FString>& OutFiles);

/** Save Slot and User Names  */

public:

	FString GetCurrentSaveGameName() const;
	void SetCurrentSaveGameName(const FString& SaveGameName);
	TArray<FString> GetSortedSaveSlots() const;

	UEMSInfoSaveGame* GetSlotInfoObject(const FString& SaveGameName = FString());
	TArray<FSaveSlotInfo> GetSlotInfos(const TArray<FString>& SaveGameNames) const;
	TArray<FString> GetAllSaveGames() const;

	void ClearMultiSaveLevels();

	bool DoesSaveGameExist(const FString& SaveGameName) const;
	bool DoesFullSaveGameExist(const FString& SaveGameName) const;
	void DeleteAllSaveDataForSlot(const FString& SaveGameName);
	void SaveSlotInfoObject(const FString& SaveGameName);

	FString GetCurrentSaveUserName() const;
	void SetCurrentSaveUserName(const FString& UserName);
	void DeleteAllSaveDataForUser(const FString& UserName);
	TArray<FString> GetAllSaveUsers() const;

protected:

	bool HasSaveUserName() const;
	UEMSInfoSaveGame* MakeSlotInfoObject(const FString& SaveGameName = FString());
	void ClearCachedSlots();
	virtual void ClearUserData();

/** Save and Load Archive Functions  */

protected:

	bool SaveObject(const FString& FullSavePath, UObject* SaveGameObject) const;
	bool LoadObject(FMemoryReader& FromBinary, UObject* SaveGameObject);

	bool LoadBinaryData(const FString& FullSavePath, TArray<uint8>& OutBinaryData) const;
	bool SaveBinaryData(const TArray<uint8>& SavedData, const FString& FullSavePath) const;
	void PerformAutoBackup(const FString& FullSavePath) const;

	bool SaveBinaryArchive(FBufferArchive& BinaryData, const FString& FullSavePath) const;
	bool LoadBinaryArchive(const EDataLoadType LoadType, const FString& FullSavePath, const FSaveObjects& Objects = FSaveObjects(), const bool bReadVersion = false);

/** Base Serialize Functions  */

protected:

	void SerializeToBinary(UObject* Object, TArray<uint8>& OutData) const;
	void SerializeFromBinary(UObject* Object, const TArray<uint8>& InData);

/** Versioning Functions  */

public:

	EIntegrityCheckResult CheckSaveGameIntegrity(const EDataLoadType Type, const FString& FullSavePath, const bool bComplexCheck);

protected:

	void ReadGameVersionInfo(FMemoryReader& FromBinary);
	void WriteGameVersionInfo(FBufferArchive& ToBinary) const;
	void WritePackageInfo(FBufferArchive& ToBinary) const;
	void ReadPackageInfo(FMemoryReader& MemoryReader, const bool bSeekInitialVersion = false);


/** Object Collection */

public:

	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Experimental")
	bool SaveObjectCollection(const TArray<FRawObjectSaveData>& Objects, const bool bUseSlot);

	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Experimental")
	bool LoadObjectCollection(const TArray<FRawObjectSaveData>& Objects, const bool bUseSlot);
	
private:

	bool ProcessObjectCollection(FMemoryReader& FromBinary, const FSaveObjects& Objects);

/** File System and Path Names  */

public:

	FString SlotInfoSaveFile(const FString& SaveGameName = FString()) const;
	FString CustomSaveFile(const FString& CustomSaveName, const FString& SlotName) const;
	FString ActorSaveFile(const FString& SaveGameName = FString()) const;
	FString PlayerSaveFile(const FString& SaveGameName = FString())  const;

protected:

	bool VerifyOrCreateDirectory(const FString& NewDir) const;

	static FString SaveUserDir();
	FString UserSubDir() const;
	FString BaseSaveDir() const;
	static FString ConsoleSaveDir();
	static FString GetThumbnailFormat();

	FString GetFolderOrFile() const;
	FString FullSaveDir(const FString& DataType, FString SaveGameName = FString()) const;

	FString ThumbnailSaveFile(const FString& SaveGameName) const;
	FString SlotFilePath(const FString& SaveGameName = FString()) const;

	FString GetObjectCollectionPath(const bool bUseSlot, const FString& CollectionSaveName = FString()) const;

	bool DoesFileExist(const FString& InFullFilePath) const;

/** Thumbnails  */

public:

	UTexture2D* ImportSaveThumbnail(const FString& SaveGameName);
	void ExportSaveThumbnail(UTextureRenderTarget2D* TextureRenderTarget, const FString& SaveGameName);

/** Settings Helpers  */

public:

	inline static bool IsNormalMultiLevelSave()
	{
		return FSettingHelpers::IsNormalMultiLevelSave();
	}

	inline static bool IsStreamMultiLevelSave()
	{
		return FSettingHelpers::IsStreamMultiLevelSave();
	}

	inline static bool IsFullMultiLevelSave()
	{
		return FSettingHelpers::IsFullMultiLevelSave();
	}

	inline static bool IsStackBasedMultiLevelSave()
	{
		return FSettingHelpers::IsStackBasedMultiLevelSave();
	}

	inline static bool IsContainingStreamMultiLevelSave()
	{
		return FSettingHelpers::IsContainingStreamMultiLevelSave();
	}

	inline static bool IsConsoleFileSystem()
	{
		return FSettingHelpers::IsConsoleFileSystem();
	}

	inline static bool IsMultiThreadLoading()
	{
		return FSettingHelpers::IsMultiThreadLoading();
	}

	inline static bool IsDeferredLoading()
	{
		return FSettingHelpers::IsDeferredLoading();
	}

/** Save Helpers  */

public:

	inline FName GetLevelName() const
	{
		return FLevelHelpers::GetWorldLevelName(GetWorld());
	}

	inline static TArray<uint8> BytesFromString(const FString& String)
	{
		return FSaveHelpers::BytesFromString(String);
	}

	inline static FString StringFromBytes(const TArray<uint8>& Bytes)
	{
		return FSaveHelpers::StringFromBytes(Bytes);
	}

	inline static bool CompareIdentifiers(const TArray<uint8>& ArrayId, const FString& StringId)
	{
		return FSaveHelpers::CompareIdentifiers(ArrayId, StringId);
	}

	inline bool IsAsyncSaveOrLoadTaskActive(const ESaveGameMode Mode = ESaveGameMode::MODE_All, const EAsyncCheckType CheckType = EAsyncCheckType::CT_Both, const bool bLog = true) const
	{
		return FAsyncSaveHelpers::IsAsyncSaveOrLoadTaskActive(GetWorld(), Mode, CheckType, bLog);
	}

	inline static EFileValidity IsSaveFileValid(const FString& FullSavePath, const bool bLog = true)
	{
		return FSaveVersion::IsSaveFileValid(FullSavePath, bLog);
	}

	inline bool IsPaused() const
	{
		return GetWorld()->IsPaused();	
	}

};
