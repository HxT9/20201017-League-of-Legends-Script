#pragma once

#include <Windows.h>
#include "functionDefinitions.h"
#include "GameFunctions.h"
#include "LocalPlayer.h"
#include "ScriptManager.h"
#include "Drawer.h"
#include "EntitiesContainer.h"
#include "IMGUI_Manager.h"
#include "UtilityFunctions.h"
#include "OrbWalker.h"
#include "TargetSelector.h"
#include "ChampionScript.h"
#include "BaseUlt.h"
#include "InputManager.h"
#include "Minimap.h"

#pragma region helpers
extern GameFunctions GH;
extern ScriptManager ticker;
extern Drawer drawer;
extern EntitiesContainer entitiesContainer;
extern IMGUI_Manager gui;
extern UtilityFunctions utils;
extern OrbWalker orbWalker;
extern TargetSelector targetSelector;
extern ChampionScript championScript;
extern BaseUlt baseUlt;
extern InputManager inputManager;
extern Minimap miniMap;
#pragma endregion

#pragma region baseVars
extern HMODULE thisDll;
extern DWORD baseAddress;
extern LocalPlayer myHero;
extern float gameTime;
#pragma endregion

#pragma region functions
extern Prototype_Present originalPresent;
#pragma endregion