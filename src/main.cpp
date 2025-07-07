#include "TLogger.h"
#include "THooks.h"
#include "Animation/Events.h"

extern "C" void set_panic_callback(void (*callback)(char const*));

extern "C" void panic_callback(char const* message) {
  TLogger::Logger.error("Panic callback called: {}", message);
  // You can add more handling here if needed
}

extern "C" void setup(CModInfo* info) {
  info->id = "Tracks";
  info->version = VERSION;
  info->version_long = 0;

  set_panic_callback(panic_callback);
}

extern "C" void late_load() {
  // Force load to ensure order
  auto cjdModInfo = CustomJSONData::modInfo.to_c();
  modloader_require_mod(&cjdModInfo, CMatchType::MatchType_IdOnly);

  Hooks::InstallHooks();
  Events::AddEventCallbacks();
}