#include "ScriptManager.h"
#include "Drawer.h"
#include "offsets.h"
#include "globalVars.h"
#include <string>
#include "imgui.h"

void ScriptManager::tick(LPDIRECT3DDEVICE9 pDevice) {
	gameTime = *(float*)(baseAddress + oGameTime);
	if (gameTime < 1) {
		return;
	}
	try {
		if (!(initHelpers && initLP && initAAMissileSpeed && initWnd))
			init(pDevice);

		drawer.tick(pDevice); //Aggiornamento del pDevice
		gui.tick(pDevice);

		entities.tick(); //Aggiornamento delle entità

		utils.drawBoundingBox();
		utils.drawEntitiesRange();
		utils.drawLastHittableMinions();
		utils.drawActiveSpells();
		utils.drawMissiles();
		utils.drawPredictedPos();

		//utils.drawDebug();

		/*bool detected = *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(utils.get_peb()) + 0xA00);
		/gui.print(detected ? "true":"false");
		bool detected2 = *(bool*)(__readfsdword(0x18) + 0x900);
		bool detected = *(bool*)(__readfsdword(0x18) + 0xA00);
		gui.print(utils.stringf("1) %d, 2) %d", detected, detected2));*/

		
		myHero.tick();
		championScript.tick();
		orbWalker.tick();
		baseUlt.tick();

		/*DA SPOSTARE*/
		
		if (GetKeyState(VK_NUMPAD1) & 0x8000)
			myHero.useSpell = true;
		if (GetKeyState(VK_NUMPAD2) & 0x8000)
			myHero.useSpell = false;
		if (myHero.useSpell)
			drawer.drawText({ 100,200, 0 }, "Using spell", 0xff00ff00);

		int zero;
		if (GetKeyState('i') & 0x8000 || GetKeyState('I') & 0x8000)
			gui.print(utils.stringf("Spellstate Q: %d                     ", GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero)));
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
			GH.printChat("Found AA missile speed: " + std::to_string(myHero.AAMissileSpeed));
			initAAMissileSpeed = true;
		}
	}
}

//KeyManager

WNDPROC wndProc;
bool justfocused = true;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

LRESULT WINAPI WndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
	//if (ImGui_ImplWin32_WndProcHandler(hwnd, u_msg, w_param, l_param))
		//return true;

	switch (u_msg) {
	case WM_SETFOCUS:
		orbWalker.focused = true;
		justfocused = true;
	case WM_KILLFOCUS:
		if (!justfocused)
			orbWalker.focused = false;
		justfocused = false;
	}

	return CallWindowProcA(wndProc, hwnd, u_msg, w_param, l_param);
}

void ScriptManager::initWndProc() {
	HWND hwnd = FindWindowA(0, "League of Legends (TM) Client");
	wndProc = WNDPROC(SetWindowLongA(hwnd, GWL_WNDPROC, LONG_PTR(WndProc)));
}