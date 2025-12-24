// Copyright(c) Aurora Devs 2022-2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#ifndef AURORA_DEVS_UGC
#define AURORA_DEVS_UGC
#endif

DECLARE_LOG_CATEGORY_EXTERN(AuroraUGC, Log, All);
#define UGC_LOG(Verbosity, Format, ...) UE_LOG(AuroraUGC, Verbosity, Format,  ##__VA_ARGS__)
#define UGC_LOG_ONCE(LogId, Verbosity, Format, ...)\
do\
{\
        static bool bLogged##LogId##Already = false; \
        if (!bLogged##LogId##Already)\
        {\
                UE_LOG(AuroraUGC, Verbosity, Format, ##__VA_ARGS__); \
                bLogged##LogId##Already = true; \
        } \
} while (0)

class FAuroraDevs_UGCModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
