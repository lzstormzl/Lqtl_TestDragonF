#pragma once

#include "Modules/ModuleManager.h"

class FBitmaskUtilsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;

    virtual void ShutdownModule() override;
};