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

using namespace std::chrono;

void ScriptManager::tick(LPDIRECT3DDEVICE9 pDevice) {
	std::string chronoDbg = "";
	gameTime = *(float*)(baseAddress + oGameTime);
	if (gameTime < 1) {
		return;
	}
	try {
		if (!(initLP && initHelpers)) {
			init(pDevice);
		}

		drawer.tick(pDevice); //Aggiornamento del pDevice

		gui.tick(pDevice);

		auto start = high_resolution_clock::now();
		entitiesContainer.tick(); //Aggiornamento delle entità
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

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
		start = high_resolution_clock::now();
		utils.drawDebug();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		for (int i = 0; i < entitiesContainer.missilesIndex.size(); i++) {
			gui.print("%d %s", entitiesContainer.entities[entitiesContainer.missilesIndex[i]]->Index, entitiesContainer.entities[entitiesContainer.missilesIndex[i]]->PCObject->GetMissileSpellInfo()->GetSpellData()->GetMissileName());
		}

		if (myHero.useSpell) {
			MessageBoxA(NULL, "", "DEBUG", MB_OK);
			myHero.useSpell = false;
		}
#endif
#pragma endregion

		start = high_resolution_clock::now();
		utils.drawBoundingBox();
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

		/*
#ifdef _DEBUG
		start = high_resolution_clock::now();
#endif
		utils.drawPredictedPos();
#ifdef _DEBUG
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));
#endif*/

		start = high_resolution_clock::now();
		utils.drawSpellCD();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		utils.ChampionCustomDraw();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		start = high_resolution_clock::now();
		myHero.tick();
		chronoDbg += utils.stringf("%f;", duration<double>(high_resolution_clock::now() - start));

		//utils.dbgStreamChrono(chronoDbg);

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
			myHero.selectedTargetIndex = -1;
			EntityBase* temp = targetSelector.getClickedChampion(GH.getMouseWorldPosition(), 150);
			if (temp)
				myHero.selectedTargetIndex = temp->Index;
		}
	}
	catch (int e) {
		gui.print("Exception");
	}
}

void ScriptManager::init(LPDIRECT3DDEVICE9 pDevice) {
	if (!initHelpers) {
		drawer = Drawer(pDevice);
		gui.init(pDevice);
		utils.init();
		inputManager.resetInputs();
		initWndProc();
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

	MH_CreateHook((LPVOID)GetCursorPos, &HkGetCursorPos, (LPVOID*)&orig_GetCursorPos);
	MH_EnableHook((LPVOID)GetCursorPos);
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

