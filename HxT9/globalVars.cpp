#include "globalVars.h"

DriverRWInterface driver = DriverRWInterface("\\\\.\\HxT9", GetCurrentProcessId());
HMODULE thisDll;
DWORD baseAddress;
LocalPlayer myHero;
float gameTime;
bool unloaded = false;
bool debugging = false;