#include "ScriptManager.h"
#include "Drawer.h"
#include "offsets.h"
#include "globalVars.h"
#include <string>
#include "imgui.h"
#include "libs/MinHook.h"
#include <chrono>

bool firstTickMouseDown = false;

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

void ScriptManager::tick(LPDIRECT3DDEVICE9 pDevice) {
	gameTime = *(float*)(baseAddress + oGameTime);
	if (gameTime < 1) {
		return;
	}
	try {
		if (!(initHelpers && initLP && initAAMissileSpeed && initWnd && initStream && initHook)) {
			init(pDevice);
		}

		drawer.tick(pDevice); //Aggiornamento del pDevice

		gui.tick(pDevice);

		entities.tick(); //Aggiornamento delle entità

#pragma region Debug
	//gui.print("Heroes: %i, Minions: %i, Missiles: %i, Turrets: %i", entities.heroes.size(), entities.minions.size(), entities.missiles.size(), entities.turrets.size());
	/*if (myHero.useSpell) {
		for (int i = 0; i < entities.missiles.size(); i++)
			if (entities.missiles[i]->GetMissileSourceIndex() == myHero.LPObject->GetIndex()) {
				toClipboard(utils.stringf("%p ", entities.missiles[0]).c_str());
				MessageBoxA(NULL, utils.stringf("pointer: %p pos: %s", entities.missiles[0], entities.missiles[0]->GetPos().toString()).c_str(), "Debug", MB_OK);
				myHero.useSpell = false;
				break;
			}


	}*/
	/*if (myHero.LPObject && myHero.useSpell) {
		MessageBoxA(NULL, utils.stringf("CObj: %p AIMgr1: %p AIMgr2: %p", myHero.LPObject, myHero.LPObject->GetAIManager(), myHero.LPObject->GetAIManager2()).c_str(), "DEBUG", MB_OK);
		myHero.useSpell = false;
	}*/

#ifdef _DEBUG
		utils.drawDebug();
#endif
#pragma endregion

		using namespace std::chrono;
		auto start = high_resolution_clock::now();
		std::string chronoDbg = "";

		utils.drawBoundingBox();

		utils.dbgStream("drawEntities");
		start = high_resolution_clock::now();
		utils.drawEntities();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		utils.dbgStream("drawLastHittableMinions");
		start = high_resolution_clock::now();
		utils.drawLastHittableMinions();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		utils.dbgStream("drawActiveSpells");
		start = high_resolution_clock::now();
		utils.drawActiveSpells();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		utils.dbgStream("drawMissiles");
		start = high_resolution_clock::now();
		utils.drawMissiles();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		utils.dbgStream("drawPredictedPos");
		start = high_resolution_clock::now();
		utils.drawPredictedPos();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		utils.dbgStream("drawSpellCD");
		start = high_resolution_clock::now();
		utils.drawSpellCD();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		utils.dbgStream("ChampionCustomDraw");
		start = high_resolution_clock::now();
		utils.ChampionCustomDraw();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		utils.dbgStream("myHero");
		start = high_resolution_clock::now();
		myHero.tick();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		utils.dbgStreamChrono(chronoDbg);

		championScript.tick();

		orbWalker.tick();

		baseUlt.tick();

		inputManager.tick();

		if (GetKeyState(VK_NUMPAD1) & 0x8000)
			myHero.useSpell = true;
		if (GetKeyState(VK_NUMPAD2) & 0x8000)
			myHero.useSpell = false;
		if (myHero.useSpell)
			drawer.drawTextSmall({ 100,200, 0 }, "Using spell", 0xff00ff00);

		if (GetKeyState(VK_LBUTTON) & 0x8000 && !firstTickMouseDown) {
			leftButtonDown = true;
			firstTickMouseDown = true;
		}
		else {
			leftButtonDown = false;
			if (!(GetKeyState(VK_LBUTTON) & 0x8000))
				firstTickMouseDown = false;
		}

		if (leftButtonDown) {
			myHero.selectedTarget = targetSelector.getClickedChampion(GH.getMouseWorldPosition(), 150);
		}
	}
	catch (int e) {
		gui.print("Exception");
	}
}

void ScriptManager::init(LPDIRECT3DDEVICE9 pDevice) {
	if (!initWnd) {
		initWndProc();
		initWnd = true;
	}
	if (!initHelpers) {
		drawer = Drawer(pDevice);
		initHelpers = true;

		gui.init(pDevice);
		inputManager.resetInputs();
	}
	if (!initLP) {
		if (*(DWORD*)(baseAddress + oLocalPlayer) != NULL) {
			myHero = LocalPlayer((CObject*)*(DWORD*)(baseAddress + oLocalPlayer));
			gui.print("LocalPlayer: " + std::to_string(*(DWORD*)(baseAddress + oLocalPlayer)) + "\0");
			myHero.championName = myHero.LPObject->GetChampionName();
			gui.print("Hero: " + std::string(myHero.championName) + "\0");
			gui.print("AiMgr: %p\0", myHero.LPObject->GetAIManager());

			baseUlt.init();

			initLP = true;
		}
		else {
			return;
		}
	}
	if (!initAAMissileSpeed) {
		std::string temp = myHero.LPObject->GetChampionName() + std::string("BasicAttack");
		const char* test = temp.c_str();
		if (myHero.LPObject->GetActiveSpell() != NULL &&
			strcmp(myHero.LPObject->GetActiveSpell()->GetSpellInfo()->GetSpellData()->GetMissileName(), test) == 0) {

			myHero.AAMissileSpeed = myHero.LPObject->GetActiveSpell()->GetSpellInfo()->GetSpellData()->GetSpellSpeed();
			gui.print("Found AA missile speed: " + std::to_string(myHero.AAMissileSpeed) + "\0");
			initAAMissileSpeed = true;
		}
	}
	if (!initHook) {
		int a = MH_CreateHook((LPVOID)GetCursorPos, &HkGetCursorPos, (LPVOID*)&orig_GetCursorPos);
		int b = MH_EnableHook((LPVOID)GetCursorPos);

		initHook = true;
	}
}

//KeyManager

WNDPROC wndProc;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

LRESULT WINAPI WndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
	//if (ImGui_ImplWin32_WndProcHandler(hwnd, u_msg, w_param, l_param))
		//return true;

	switch (u_msg) {
	case WM_SETFOCUS:
		orbWalker.focused = true;
		break;

	case WM_KILLFOCUS:
		orbWalker.focused = false;
		break;
	}

	return CallWindowProcA(wndProc, hwnd, u_msg, w_param, l_param);
}

void ScriptManager::initWndProc() {
	HWND hwnd = FindWindowA(0, "League of Legends (TM) Client");
	wndProc = WNDPROC(SetWindowLongA(hwnd, GWL_WNDPROC, LONG_PTR(WndProc)));
}

BOOL WINAPI HkGetCursorPos(LPPOINT lpPoint) {
	BOOL ret = orig_GetCursorPos(lpPoint);

	if (inputManager.hookingMousePos) {
		lpPoint->x = inputManager.hookedMousePos.x;
		lpPoint->y = inputManager.hookedMousePos.y;

		gui.print("Injected GetCursorPos: %i, %i", lpPoint->x, lpPoint->y);
	}

	return ret;
}

