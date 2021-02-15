#include "ScriptManager.h"
#include "Drawer.h"
#include "offsets.h"
#include "globalVars.h"
#include <string>
#include "imgui.h"

bool firstTickMouseDown = false;

void ScriptManager::tick(LPDIRECT3DDEVICE9 pDevice) {
	gameTime = *(float*)(baseAddress + oGameTime);
	if (gameTime < 1) {
		return;
	}
	try {
		if (!(initHelpers && initLP && initAAMissileSpeed && initWnd && initStream)) {
			utils.dbgStream("Init");
			init(pDevice);
		}

		utils.dbgStream("drawer.tick");
		drawer.tick(pDevice); //Aggiornamento del pDevice
		utils.dbgStream("gui.tick");
		gui.tick(pDevice);

		utils.dbgStream("entities.tick");
		entities.tick(); //Aggiornamento delle entità

		utils.dbgStream("drawBoundingBox");
		utils.drawBoundingBox();
		utils.dbgStream("drawEntitiesRange");
		utils.drawEntitiesRange();
		utils.dbgStream("drawLastHittableMinions");
		utils.drawLastHittableMinions();
		utils.dbgStream("drawActiveSpells");
		utils.drawActiveSpells();
		utils.dbgStream("drawMissiles");
		utils.drawMissiles();
		utils.dbgStream("drawPredictedPos");
		utils.drawPredictedPos();
		utils.dbgStream("ChampionCustomDraw");
		utils.ChampionCustomDraw();

		//utils.drawDebug();

		//bool detected = *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(utils.get_peb()) + 0xA00);
		//gui.print(detected ? "true":"false");
		//bool detected2 = *(bool*)(__readfsdword(0x18) + 0x900);
		//bool detected = *(bool*)(__readfsdword(0x18) + 0xA00);
		//gui.print(utils.stringf("1) %d, 2) %d", detected, detected2));


		utils.dbgStream("myHero.tick");
		myHero.tick();
		utils.dbgStream("championScript.tick");
		championScript.tick();
		utils.dbgStream("OrbWalker.tick");
		orbWalker.tick();
		//baseUlt.tick();
		
		if (GetKeyState(VK_NUMPAD1) & 0x8000)
			myHero.useSpell = true;
		if (GetKeyState(VK_NUMPAD2) & 0x8000)
			myHero.useSpell = false;
		if (myHero.useSpell)
			drawer.drawText({ 100,200, 0 }, "Using spell", 0xff00ff00);

		if (GetKeyState(VK_LBUTTON) & 0x8000 && !firstTickMouseDown) {
			leftButtonDown = true;
			firstTickMouseDown = true;
		}
		else {
			leftButtonDown = false;
			if(!(GetKeyState(VK_LBUTTON) & 0x8000))
				firstTickMouseDown = false;
		}

		if (leftButtonDown) {
			myHero.selectedTarget = targetSelector.getClickedChampion(GH.getMouseWorldPosition(), 150);
		}
	}
	catch (int e) {

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
	}
	if (!initLP) {
		if (*(DWORD*)(baseAddress + oLocalPlayer) != NULL) {
			myHero = LocalPlayer((CObject*)*(DWORD*)(baseAddress + oLocalPlayer));
			gui.print("LocalPlayer: " + std::to_string(*(DWORD*)(baseAddress + oLocalPlayer)) + "                                      ");
			myHero.championName = myHero.LPObject->GetChampionName();
			gui.print("Hero: " + std::string(myHero.championName) + "                                 ");

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
			gui.print("Found AA missile speed: " + std::to_string(myHero.AAMissileSpeed));
			initAAMissileSpeed = true;
		}
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