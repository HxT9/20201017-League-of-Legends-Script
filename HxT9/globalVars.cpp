#include "globalVars.h"

#pragma region helpers
GameFunctions GH;
ScriptManager ticker;
Drawer drawer;
UtilityFunctions utils;
EntitiesContainer entitiesContainer;
IMGUI_Manager gui;
OrbWalker orbWalker;
TargetSelector targetSelector;
ChampionScript championScript;
BaseUlt baseUlt;
InputManager inputManager;
Minimap miniMap;
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