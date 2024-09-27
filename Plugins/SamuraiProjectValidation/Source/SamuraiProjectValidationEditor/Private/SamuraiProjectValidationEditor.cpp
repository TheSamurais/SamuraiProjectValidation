#include "SamuraiProjectValidationEditor.h"

#include "ISettingsModule.h"
#include "SPVSettings.h"

#define LOCTEXT_NAMESPACE "FSamuraiProjectValidationEditorModule"

void FSamuraiProjectValidationEditorModule::StartupModule()
{
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->RegisterSettings("Project", "Plugins", "SamuraiProjectValidation",
			LOCTEXT("RuntimeSettingsName", "Samurai Project Validation"),
			LOCTEXT("RuntimeSettingsDescription", "Configure SPV"),
			GetMutableDefault<USPVSettings>()
		);
	}
}

void FSamuraiProjectValidationEditorModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "SamuraiProjectValidation");
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSamuraiProjectValidationEditorModule, SamuraiProjectValidationEditor)