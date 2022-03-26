#include "ScriptManager.h"
#include "Drawer.h"
#include "offsets.h"
#include "globalVars.h"
#include <string>
#include "imgui.h"
#include "libs/MinHook.h"
#include <chrono>

//typedef BOOL(WINAPI* myGetCursorPos)(LPPOINT lpPoint);
//myGetCursorPos originalGetCursorPos = NULL;
BOOL WINAPI HkGetCursorPos(LPPOINT lpPoint);
BOOL(WINAPI* orig_GetCursorPos)(LPPOINT) = GetCursorPos;

void toClipboard(const std::string& s) {
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size());
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size());
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

using namespace std::chrono;

void ScriptManager::Tick(LPDIRECT3DDEVICE9 pDevice) {
	std::string chronoDbg = "";
	gameTime = *(float*)(baseAddress + oGameTime);
	if (gameTime < 1) {
		return;
	}
	try {
		if (!(initLP && initHelpers)) {
			Init(pDevice);
		}

		drawer.tick(pDevice); //Aggiornamento del pDevice

		gui.tick(pDevice);

		auto start = high_resolution_clock::now();
		entitiesContainer.tick();
		//chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

#pragma region Debug
	/*if (myHero.useSpell) {
		for (int i = 0; i < entitiesContainer.missiles.size(); i++)
			if (entitiesContainer.missiles[i]->GetMissileSourceIndex() == myHero.LPCObject->getIndex()) {
				toClipboard(utils.stringf("%p ", entitiesContainer.missiles[0]).c_str());
				MessageBoxA(NULL, utils.stringf("pointer: %p pos: %s", entitiesContainer.missiles[0], entitiesContainer.missiles[0]->GetPos().toString()).c_str(), "Debug", MB_OK);
				myHero.useSpell = false;
				break;
			}
	}*/

#ifdef _DEBUG
		//start = high_resolution_clock::now();
		utils.drawDebug();
		//chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		for (int i = 0; i < entitiesContainer.missilesIndex.size(); i++) {
			gui.print("%d %s", entitiesContainer.entities[entitiesContainer.missilesIndex[i]]->Index, entitiesContainer.entities[entitiesContainer.missilesIndex[i]]->PCObject->GetMissileSpellInfo()->GetSpellData()->GetMissileName());
		}

		if (myHero.useSpell) {
			MessageBoxA(NULL, "", "DEBUG", MB_OK);
			myHero.useSpell = false;
		}
#endif
#pragma endregion

		//start = high_resolution_clock::now();
		utils.drawMyHero();
		//chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		//start = high_resolution_clock::now();
		utils.drawEntities();
		//chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		//start = high_resolution_clock::now();
		utils.drawLastHittableMinions();
		//chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		//start = high_resolution_clock::now();
		utils.drawActiveSpells();
		//chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		//start = high_resolution_clock::now();
		utils.drawMissiles();
		//chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		////start = high_resolution_clock::now();
		//utils.drawPredictedPos();
		////chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		//start = high_resolution_clock::now();
		utils.drawSpellCD();
		//chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		//utils.dbgStreamChrono(chronoDbg);

		utils.ChampionCustomDraw();

		myHero.tick();

		championScript.tick();

		orbWalker.tick();

		baseUlt.tick();

		inputManager.tick();
	}
	catch (int e) {
		gui.print("Exception");
	}
}

void ScriptManager::Init(LPDIRECT3DDEVICE9 pDevice) {
	if (!initHelpers) {
		drawer = Drawer(pDevice);
		gui.init(pDevice);
		utils.init();
		inputManager.resetInputs();
		entitiesContainer.Init();
		initHelpers = true;
	}

	if (!initLP) {
		if (*(DWORD*)(baseAddress + oLocalPlayer) != NULL) {
			myHero = LocalPlayer(*(CObject**)(baseAddress + oLocalPlayer));
			gui.print("LocalPlayer: %p", myHero.PCObject);
			gui.print("Hero: " + std::string(myHero.ChampionName));
			gui.print("AiMgr: %p\0", myHero.PCObject->GetAIManager());
			baseUlt.init();

			initLP = true;
			gui.print("initLP");

#ifdef _DEBUG
			myHero.useSpell = false;
#endif
		}
		else {
			return;
		}
	}
}


