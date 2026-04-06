#pragma once
#include "TLogger.h"
#include <android/log.h>

class Hooks {
private:
  static inline std::vector<std::pair<std::string, void (*)()>> installFuncs;

public:
  static inline void AddInstallFunc(std::string name, void (*installFunc)()) {
    installFuncs.emplace_back(name, installFunc);
  }

  static inline void InstallHooks() {
    std::sort(installFuncs.begin(), installFuncs.end(),
              [](std::pair<std::string, void (*)()> const& a, std::pair<std::string, void (*)()> const& b) {
                return a.first < b.first;
              });
    for (auto const& installFunc : installFuncs) {
      installFunc.second();
    }
  }
};

#define TInstallHooks(func)                                                                                            \
  struct __TRegister##func {                                                                                           \
    __TRegister##func() {                                                                                              \
      Hooks::AddInstallFunc(#func, func);                                                                              \
      __android_log_print(ANDROID_LOG_DEBUG, "TInstallHooks", "Registered install func: " #func);                      \
    }                                                                                                                  \
  };                                                                                                                   \
  static __TRegister##func __TRegisterInstance##func;

void InstallAndRegisterAll();