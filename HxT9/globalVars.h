#pragma once

#include <Windows.h>
#include "LocalPlayer.h"
#include "DriverRWInterface.h"


extern DriverRWInterface driver;
extern HMODULE thisDll;
extern DWORD baseAddress;
extern LocalPlayer myHero;
extern float gameTime;
extern bool unloaded;
extern bool debugging;