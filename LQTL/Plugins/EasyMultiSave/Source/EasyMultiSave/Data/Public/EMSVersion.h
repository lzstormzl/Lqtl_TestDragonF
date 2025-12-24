//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#pragma once

#include "EMSTypes.h"
#include "CoreMinimal.h"
#include "EMSVersion.generated.h"

USTRUCT()
struct FSaveVersionInfo
{
	GENERATED_BODY()

public:

	FSaveVersionInfo()
	{
		Plugin = FString();
		Game = FString();
	}

	FSaveVersionInfo(const FString& InPlugin, const FString& InGame)
	{
		Plugin = InPlugin;
		Game = InGame;
	}

	friend FArchive& operator<<(FArchive& Ar, FSaveVersionInfo& VersionArchive)
	{
		Ar << VersionArchive.Plugin;
		Ar << VersionArchive.Game;
		return Ar;
	}

	bool operator!=(const FSaveVersionInfo& InInfo) const
	{
		return (EMS::EqualString(InInfo.Game, Game) && EMS::EqualString(InInfo.Plugin, Plugin)) == false;
	}

public:

	FString Plugin;
	FString Game;
};

class EASYMULTISAVE_API FSaveVersion
{

public:

	static FString GetGameVersion();
	static FSaveVersionInfo MakeSaveFileVersion();
	static bool IsSaveGameVersionEqual(const FSaveVersionInfo& SaveVersion);

	static FPackageFileVersion GetStaticOldPackageVersion();
	static bool RequiresPerObjectPackageTag(const UObject* Object);

	static void WriteObjectPackageTag(TArray<uint8>& Data);
	static bool CheckObjectPackageTag(const TArray<uint8>& Data);

	static uint8 UpdateArchiveVersion(FArchive& Ar);

	static EFileValidity IsSaveFileValid(const FString& InSavePath, const bool bLog = true);
};
