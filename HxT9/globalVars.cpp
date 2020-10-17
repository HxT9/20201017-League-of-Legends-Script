#include "globalVars.h"

#pragma region helpers
GameFunctions GH;
ScriptManager ticker;
Drawer drawer;
EntitiesContainer entities;
IMGUI_Manager gui;
UtilityFunctions utils;
OrbWalker orbWalker;
TargetSelector targetSelector;
ChampionScript championScript;
BaseUlt baseUlt;
#pragma endregion

#pragma region baseVars
HMODULE thisDll;
DWORD baseAddress;
LocalPlayer myHero;
float gameTime;
#pragma endregion

#pragma region functions
Prototype_Present originalPresent;
#pragma endregion