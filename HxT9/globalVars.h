#pragma once

#include <Windows.h>
#include "GameFunctions.h"
#include "LocalPlayer.h"
#include "ScriptManager.h"
#include "Drawer.h"
#include "EntitiesContainer.h"
#include "IMGUIManager.h"
#include "UtilityFunctions.h"
#include "OrbWalker.h"
#include "TargetSelector.h"
#include "ChampionScript.h"
#include "BaseUlt.h"
#include "InputManager.h"
#include "Minimap.h"
#include "HookManager.h"

#pragma region helpers
extern GameFunctions GH;
extern ScriptManager scriptManager;
extern Drawer drawer;
extern EntitiesContainer entitiesContainer;
extern IMGUIManager gui;
extern UtilityFunctions utils;
extern OrbWalker orbWalker;
extern TargetSelector targetSelector;
extern ChampionScript championScript;
extern BaseUlt baseUlt;
extern InputManager inputManager;
extern Minimap miniMap;
extern HookManager hookManager;
#pragma endregion

#pragma region baseVars
extern HMODULE thisDll;
extern DWORD baseAddress;
extern LocalPlayer myHero;
extern float gameTime;
extern bool unloaded;
extern bool debugging;
#pragma endregion