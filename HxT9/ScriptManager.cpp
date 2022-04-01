#include "ScriptManager.h"
#include "Drawer.h"
#include "offsets.h"
#include "globalVars.h"
#include <string>
#include "imgui.h"
#include "libs/MinHook.h"

//#define HXT9_USECHRONO

#ifdef HXT9_USECHRONO
#include <chrono>
#endif

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

#ifdef HXT9_USECHRONO
		using namespace std::chrono;
		auto start = high_resolution_clock::now();
		entitiesContainer.tick();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		gui.tick();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		if (this->Debugging)
			utils.drawDebug();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		utils.drawMyHero();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		utils.drawEntities();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		utils.drawLastHittableMinions();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		utils.drawActiveSpells();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		utils.drawMissiles();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		//start = high_resolution_clock::now();
		//utils.drawPredictedPos();
		//chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		utils.drawSpellCD();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		utils.ChampionCustomDraw();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		myHero.tick();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		championScript.tick();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		orbWalker.tick();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		baseUlt.tick();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		inputManager.tick();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		utils.dbgStreamChrono(chronoDbg);
#else
		entitiesContainer.tick();
		gui.tick();
		if (this->Debugging)
			utils.drawDebug();
		utils.drawMyHero();
		utils.drawEntities();
		utils.drawLastHittableMinions();
		utils.drawActiveSpells();
		utils.drawMissiles();
		//utils.drawPredictedPos();
		utils.drawSpellCD();
		utils.ChampionCustomDraw();
		myHero.tick();
		championScript.tick();
		orbWalker.tick();
		baseUlt.tick();
		inputManager.tick();
#endif
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
		}
		else {
			return;
		}
	}
}


