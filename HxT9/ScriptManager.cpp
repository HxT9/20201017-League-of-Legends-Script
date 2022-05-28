#include "ScriptManager.h"
#include "offsets.h"
#include "globalVars.h"
#include "D3DDrawer.h"
#include "HookManager.h"
#include "OrbWalker.h"
#include "ChampionScript.h"
#include "IMGUIManager.h"
#include "EntitiesContainer.h"
#include "UtilityFunctions.h"
#include "BaseUlt.h"
#include "InputManager.h"
#include "GameFunctions.h"

namespace ScriptManager {
	bool InitLP = false;
	bool InitHelpers = false;

	DWORD WINAPI Unload(LPVOID lpParam) {
		Sleep(500);
		FreeLibraryAndExitThread(thisDll, 0);

		MessageBoxA(NULL, "Can't unload the dll. Restarting script", "HxT9", MB_OK);
		InitLP = false;
		InitHelpers = false;
		HookManager::Init();
	}

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

	void Tick(LPDIRECT3DDEVICE9 pDevice) {
		std::string chronoDbg = "";


		gameTime = *(float*)(baseAddress + oGameTime);
		if (gameTime < 1) {
			return;
		}
		try {
			if (!(InitLP && InitHelpers)) {
				Init(pDevice);
			}

			if (unloaded) {
				CallWindowProcA(HookManager::NewWndProc, FindWindowA(0, "League of Legends (TM) Client"), WM_ACTIVATE, WA_ACTIVE, 0);

				IMGUIManager::Dispose();
				HookManager::Dispose();
				driver.Dispose();
				CreateThread(NULL, 0, &Unload, NULL, 0, NULL);
				return;
			}

			D3DDrawer::Tick(pDevice);
			IMGUIManager::Tick();

			EntitiesContainer::Tick();
			UtilityFunctions::Draw();
			myHero.Tick();
			ChampionScript::Tick();
			OrbWalker::Tick();
			BaseUlt::Tick();
			InputManager::Tick();
		}
		catch (int e) {
			IMGUIManager::Print("Exception");
		}
	}

	void Init(LPDIRECT3DDEVICE9 pDevice) {
		if (!InitHelpers) {
			D3DDrawer::Init(pDevice);
			IMGUIManager::Init(pDevice);
			UtilityFunctions::Init();
			InputManager::ResetInputs();
			EntitiesContainer::Init();
			GameFunctions::Init();
			InitHelpers = true;

			int driverTester = 35;
			if (driver.ReadVirtualMemory<int>((UINT64)&driverTester) != driverTester) {
				IMGUIManager::Print("DRIVER NOT INITIALIZED!");
			}
		}

		if (!InitLP) {
			if (*(DWORD*)(baseAddress + oLocalPlayer) != NULL) {
				myHero = LocalPlayer(*(CObject**)(baseAddress + oLocalPlayer));
				IMGUIManager::Print("LocalPlayer: %p", myHero.PCObject);
				IMGUIManager::Print("Hero: %s", std::string(myHero.ObjectName).c_str());
				IMGUIManager::Print("AiMgr: %p", myHero.PCObject->GetAIManager());
				BaseUlt::Init();

				InitLP = true;
			}
			else {
				return;
			}
		}
	}
}