//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  


#include "EMSMisc.h"
#include "EMSPluginSettings.h"
#include "EMSAsyncLoadGame.h"
#include "EMSAsyncSaveGame.h"
#include "EMSAsyncStream.h"
#include "Engine/World.h"
#include "UObject/UObjectIterator.h"

/**
FSettingHelpers
**/

bool FSettingHelpers::IsNormalMultiLevelSave()
{
	return UEMSPluginSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Normal;
}

bool FSettingHelpers::IsStreamMultiLevelSave()
{
	return UEMSPluginSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Stream;
}

bool FSettingHelpers::IsFullMultiLevelSave()
{
	return UEMSPluginSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Full;
}

bool FSettingHelpers::IsStackBasedMultiLevelSave()
{
	return IsFullMultiLevelSave() || IsNormalMultiLevelSave();
}

bool FSettingHelpers::IsContainingStreamMultiLevelSave()
{
	return IsFullMultiLevelSave() || IsStreamMultiLevelSave();
}

bool FSettingHelpers::IsDynamicLevelStreaming()
{
	return IsContainingStreamMultiLevelSave() && UEMSPluginSettings::Get()->bDynamicLevelStreaming;
}

bool FSettingHelpers::IsMemoryOnlySave()
{
	return UEMSPluginSettings::Get()->WorldPartitionSaving == EWorldPartitionMethod::MemoryOnly;
}

bool FSettingHelpers::IsConsoleFileSystem()
{
	return UEMSPluginSettings::Get()->FileSaveMethod == EFileSaveMethod::FM_Console;
}

bool FSettingHelpers::IsMultiThreadSaving()
{
	return UEMSPluginSettings::Get()->bMultiThreadSaving && FPlatformProcess::SupportsMultithreading();
}

bool FSettingHelpers::IsMultiThreadLoading()
{
	return UEMSPluginSettings::Get()->LoadMethod == ELoadMethod::LM_Thread && FPlatformProcess::SupportsMultithreading();
}

bool FSettingHelpers::IsDeferredLoading()
{
	return UEMSPluginSettings::Get()->LoadMethod == ELoadMethod::LM_Deferred;
}

uint32 FSettingHelpers::GetLoadBatchSize()
{
	return FMath::Max(1, UEMSPluginSettings::Get()->DeferredLoadStackSize);
}

/**
Async Node Helpers
**/

template<class T>
bool FAsyncSaveHelpers::CheckLoadIterator(const T& It, const ESaveGameMode Mode, const bool bLog, const FString& DebugString)
{
	if (It && It->IsActive() && (It->Mode == Mode || Mode == ESaveGameMode::MODE_All))
	{
		if (bLog)
		{
			UE_LOG(LogEasyMultiSave, Warning, TEXT("%s is active while trying to save or load."), *DebugString);
		}

		return true;
	}

	return false;
}

bool FAsyncSaveHelpers::IsAsyncSaveOrLoadTaskActive(const UWorld* InWorld, const ESaveGameMode Mode, const EAsyncCheckType CheckType, const bool bLog)
{
	//This will prevent the functions from being executed at all during pause.
	if (InWorld->IsPaused())
	{
		if (bLog)
		{
			UE_LOG(LogEasyMultiSave, Warning, TEXT("Async save or load called during pause. Operation was canceled."));
		}

		return true;
	}

	if (CheckType == EAsyncCheckType::CT_Both || CheckType == EAsyncCheckType::CT_Load)
	{
		for (TObjectIterator<UEMSAsyncLoadGame> It; It; ++It)
		{
			if (CheckLoadIterator(It, Mode, bLog, "Load Game Actors"))
			{
				return true;
			}
		}

		for (TObjectIterator<UEMSAsyncStream> It; It; ++It)
		{
			if (CheckLoadIterator(It, Mode, bLog, "Load Stream Level Actors"))
			{
				return true;
			}
		}
	}

	if (CheckType == EAsyncCheckType::CT_Both || CheckType == EAsyncCheckType::CT_Save)
	{
		for (TObjectIterator<UEMSAsyncSaveGame> It; It; ++It)
		{
			if (CheckLoadIterator(It, Mode, bLog, "Save Game Actors"))
			{
				return true;
			}
		}
	}

	return false;
}

bool FAsyncSaveHelpers::IsStreamAutoLoadActive(const ULevel* InLevel)
{
	for (TObjectIterator<UEMSAsyncStream> It; It; ++It)
	{
		if (It && It->IsActive() && It->StreamingLevel == InLevel)
		{
			return true;
		}
	}

	return false;
}

void FAsyncSaveHelpers::DestroyStreamAutoLoadTask(const ULevel* InLevel)
{
	for (TObjectIterator<UEMSAsyncStream> It; It; ++It)
	{
		if (It && It->IsActive() && It->StreamingLevel == InLevel)
		{
			It->ForceDestroy();
		}
	}
}

void FAsyncSaveHelpers::DestroyAsyncLoadLevelTask()
{
	for (TObjectIterator<UEMSAsyncLoadGame> It; It; ++It)
	{
		if (It && It->IsActive())
		{
			It->ForceDestroy();
		}
	}
}

ESaveGameMode FAsyncSaveHelpers::GetMode(const int32 Data)
{
	if (Data & ENUM_TO_FLAG(ESaveTypeFlags::SF_Player))
	{
		if (Data & ENUM_TO_FLAG(ESaveTypeFlags::SF_Level))
		{
			return ESaveGameMode::MODE_All;
		}
		else
		{
			return ESaveGameMode::MODE_Player;
		}
	}

	return ESaveGameMode::MODE_Level;
}
