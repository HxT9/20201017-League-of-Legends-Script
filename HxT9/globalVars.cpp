#include "globalVars.h"

#pragma region helpers
GameFunctions GH;
ScriptManager scriptManager;
Drawer drawer;
UtilityFunctions utils;
EntitiesContainer entitiesContainer;
IMGUIManager gui;
OrbWalker orbWalker;
TargetSelector targetSelector;
ChampionScript championScript;
BaseUlt baseUlt;
InputManager inputManager;
Minimap miniMap;
HookManager hookManager;
#pragma endregion

#pragma region baseVars
HMODULE thisDll;
DWORD baseAddress;
LocalPlayer myHero;
float gameTime;
bool unloaded = false;
bool debugging = false;
#pragma endregion