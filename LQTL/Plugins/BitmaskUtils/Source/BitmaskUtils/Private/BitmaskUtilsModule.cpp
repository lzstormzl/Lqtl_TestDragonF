#include "BitmaskUtilsModule.h"

#include "EdGraphUtilities.h"
#include "Modules/ModuleManager.h"
#include "BitmaskPinFactory.h"


static TSharedPtr<FBitmaskPinFactory> BitmaskPinFactoryHandle;

void FBitmaskUtilsModule::StartupModule()
{
    BitmaskPinFactoryHandle = MakeShared<FBitmaskPinFactory>();
    FEdGraphUtilities::RegisterVisualPinFactory(BitmaskPinFactoryHandle.ToSharedRef());
}

void FBitmaskUtilsModule::ShutdownModule()
{
    BitmaskPinFactoryHandle.Reset();
}


IMPLEMENT_MODULE(FBitmaskUtilsModule, BitmaskUtils)