//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#include "EMSVersion.h"
#include "EMSActors.h"  
#include "EMSMisc.h"
#include "EMSPluginSettings.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"
#include "SaveGameSystem.h"
#include "PlatformFeatures.h"

FString FSaveVersion::GetGameVersion()
{
	const FString CustomVersion = FString::FromInt(UEMSPluginSettings::Get()->SaveGameVersion);
	return EMS::VerGame + CustomVersion;
}

FSaveVersionInfo FSaveVersion::MakeSaveFileVersion()
{
	const FString EmsVersion = EMS::VerPlugin + VERSION_STRINGIFY(EMS_VERSION_NUMBER);
	const FString GameVersion = GetGameVersion();
	const FSaveVersionInfo Info = FSaveVersionInfo(EmsVersion, GameVersion);

	return Info;
}

bool FSaveVersion::IsSaveGameVersionEqual(const FSaveVersionInfo& SaveVersion)
{
	const FString GameVersion = GetGameVersion();
	return EMS::EqualString(SaveVersion.Game, GameVersion);
}

FPackageFileVersion FSaveVersion::GetStaticOldPackageVersion()
{
	//Check hardcoded legacy package file versions. Print with GPackageFileUEVersion.ToValue()

	uint32 StaticPackageVersion = 1009;

	if (UEMSPluginSettings::Get()->MigratedSaveEngineVersion == EOldPackageEngine::EN_UE40)
	{
		StaticPackageVersion = 555;
	}
	else if (UEMSPluginSettings::Get()->MigratedSaveEngineVersion == EOldPackageEngine::EN_UE54)
	{
		StaticPackageVersion = 1012;
	}

	/*
	1013 - UE 5.5+ - Not required as this will definitely have a written version.
	For Multi-Level Saving(PerObjectPackageTag), it still uses the static package version, but it is backwards compatible on read.
	See UEMSObjectBase::SerializeFromBinary. It will then write out the new version when saving again.
	*/

	return FPackageFileVersion(StaticPackageVersion, EUnrealEngineObjectUE5Version(StaticPackageVersion));
}

bool FSaveVersion::RequiresPerObjectPackageTag(const UObject* Object)
{
	if (!UEMSPluginSettings::Get()->bMigratedSaveActorVersionCheck)
	{
		return false;
	}

	if (FSettingHelpers::IsStackBasedMultiLevelSave() || FSettingHelpers::IsStreamMultiLevelSave())
	{
		if (const AActor* Actor = Cast<AActor>(Object))
		{
			const EActorType Type = FActorHelpers::GetActorType(Actor);
			if (FActorHelpers::IsLevelActor(Type, true))
			{
				return true;
			}
		}
		else
		{
			//This is for components and raw objects. 
			return true;
		}
	}

	return false;
}

void FSaveVersion::WriteObjectPackageTag(TArray<uint8>& Data)
{
	const uint8* DataTag = EMS::UE_OBJECT_PACKAGE_TAG;
	Data.Append(DataTag, EMS_PKG_TAG_SIZE);
}

bool FSaveVersion::CheckObjectPackageTag(const TArray<uint8>& Data)
{
	const uint8* DataTag = EMS::UE_OBJECT_PACKAGE_TAG;
	const uint8 Len = EMS_PKG_TAG_SIZE;

	if (Data.Num() < Len)
	{
		return false;
	}

	//Compare the tag at the end of the array
	for (int32 i = 0; i < Len; ++i)
	{
		if (Data[Data.Num() - Len + i] != DataTag[i])
		{
			return false;
		}
	}

	return true;
}

uint8 FSaveVersion::UpdateArchiveVersion(FArchive& Ar)
{
	//Allows to update FArchive with new data by simple versioning
 
	//No version
	uint8 Version = 0;

	if (Ar.IsSaving())
	{
		uint32 Tag = EMS::ARCHIVE_DATA_TAG;
		uint8 Ver = EMS::ACTOR_DATA_VERSION;
		Ar << Tag; 
		Ar << Ver;
		Version = Ver;
	}
	else if (Ar.IsLoading())
	{
		const int64 StartPos = Ar.Tell();

		uint32 PeekTag = 0;
		Ar << PeekTag;
		
		//Impossible that an expected archive entry matches the tag exactly
		if (PeekTag == EMS::ARCHIVE_DATA_TAG)
		{
			uint8 ReadVersion = 0;
			Ar << ReadVersion;
			Version = ReadVersion;
		}
		else
		{
			//No version, reset pos
			Ar.Seek(StartPos);
		}	
	}

	return Version;
}

EFileValidity FSaveVersion::IsSaveFileValid(const FString& InSavePath, const bool bLog)
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (!SaveSystem)
	{
		return EFileValidity::FILE_VALID;
	}

	//Currently static, matching EMSObjectBase. Can be added as param later.
	const int32 PlayerIndex = 0;

	if (!SaveSystem->DoesSaveGameExist(*InSavePath, PlayerIndex))
	{
		if (bLog)
		{
			UE_LOG(LogEasyMultiSave, Log, TEXT("Save file missing (nothing to check) - %s"), *InSavePath);
		}
		return EFileValidity::FILE_MISSING;
	}

	TArray<uint8> Data;
	if (!SaveSystem->LoadGame(false, *InSavePath, PlayerIndex, Data))
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("Save file is empty or unreadable - %s"), *InSavePath);
		return EFileValidity::FILE_INVALID;
	}

	constexpr int32 MinBytesToInspect = 16;

	//Early reject if file is smaller than the minimum threshold.
	if (Data.Num() < MinBytesToInspect)
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("Save file too small (< %d bytes) - %s"), MinBytesToInspect, *InSavePath);
		return EFileValidity::FILE_INVALID;
	}

	//Check first N bytes aren't all zero.
	bool bAllNull = true;
	for (int32 i = 0; i < MinBytesToInspect; ++i)
	{
		if (Data[i] != 0)
		{
			bAllNull = false;
			break;
		}
	}

	if (bAllNull)
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("Save file starts with all NULLs - %s"), *InSavePath);
		return EFileValidity::FILE_INVALID;
	}

	if (bLog)
	{
		UE_LOG(LogEasyMultiSave, Log, TEXT("%s successfully passed integrity check."), *InSavePath);
	}

	return EFileValidity::FILE_VALID;
}
