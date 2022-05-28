#include "IMGUIManager.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <Windows.h>
#include "globalVars.h"
#include "UtilityFunctions.h"
#include "BaseUlt.h"
#include "InputManager.h"

namespace IMGUIManager {
	bool ShowLog = false;
	bool ShowMain = false;
	int MaxConsoleLines = 1000;
	std::vector<std::string> ConsoleLines;

	bool scrollToBottom = true;
	bool stopLogging = false;
	int selectedTab = 0;

	void Init(LPDIRECT3DDEVICE9 pDevice) {
		HWND hwnd = FindWindowA(0, "League of Legends (TM) Client");

		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX9_Init(pDevice);

		ResetConsole();
	}

	__inline void createWindow(const char* name, bool* openedStatus, ImGuiWindowFlags flags, ImVec2 Pos = ImVec2(0, 0), ImVec2 Size = ImVec2(100, 100)) {
		ImGui::Begin(name, openedStatus, ImGuiWindowFlags_NoSavedSettings | flags);
		ImGui::SetWindowPos(Pos, ImGuiCond_Once);
		ImGui::SetWindowSize(Size, ImGuiCond_Once);
	}

	void renderLogWindow() {
		int id = 1000;
		createWindow("Log", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse, ImVec2(100, 100), ImVec2(650, 500));
		ImGui::BeginChild("#Log", ImVec2(0, 0), false);

		ImGui::BeginChildFrame(id++, ImVec2(0, 30));
		ImGui::SameLine();
		ImGui::Checkbox("Keep scrolling to bottom", &scrollToBottom);
		ImGui::SameLine();
		ImGui::Checkbox("Stop Loggin", &stopLogging);
		ImGui::EndChildFrame();

		ImGui::BeginChildFrame(3, ImVec2(0, 0));
		for (int i = 0; i < MaxConsoleLines; i++) {
			ImGui::Text(ConsoleLines[i].c_str());
		}

		if (scrollToBottom) {
			ImGui::SetScrollHereY(1.f);
		}
		ImGui::EndChildFrame();

		ImGui::EndChild();
		ImGui::End();
	}

	void renderMainWindow() {
		int id = 2000;
		createWindow("Main", &ShowMain, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse, ImVec2(400, 100), ImVec2(800, 600));

		ImGui::BeginChild("#Main", ImVec2(0, 0), false);

		ImGui::BeginChildFrame(id++, ImVec2(0, 40));
		{
			ImGui::SameLine();
			if (ImGui::Button("Settings", ImVec2(80, 25))) {
				selectedTab = 0;
			}
			ImGui::SameLine();
			if (ImGui::Button("Heroes", ImVec2(80, 25))) {
				selectedTab = 1;
			}
			ImGui::SameLine();
			if (ImGui::Button("Buffs", ImVec2(80, 25))) {
				selectedTab = 2;
			}
			ImGui::SameLine();
			if (ImGui::Button("SpellSlots", ImVec2(80, 25))) {
				selectedTab = 3;
			}
			ImGui::SameLine();
			if (ImGui::Button("ActiveSpell", ImVec2(80, 25))) {
				selectedTab = 4;
			}
		}
		ImGui::EndChildFrame();

		switch (selectedTab) {
		case 0:
			ImGui::BeginChildFrame(id++, ImVec2(0, 0));
			ImGui::Checkbox("Debugging", &debugging);
			ImGui::Checkbox("Use Custom Prediction (WIP)", &UtilityFunctions::UseCustomPrediction);
			ImGui::Checkbox("Use Input ScanCode", &InputManager::UseScan);
			ImGui::Checkbox("Use Spells (Num1 On, Num2 Off)", &myHero.UseSpell);
			ImGui::Checkbox("Use AA in Combo", &myHero.UseAAInCombo);
			ImGui::Checkbox("BaseUlt Enabled", &BaseUlt::Enabled);
			ImGui::SliderFloat("Action Delay (s)", &myHero.ActionDelay, 0, 0.500);
			ImGui::SliderInt("Humanizer Min (ms)", &myHero.HumanizerMin, 0, 500);
			ImGui::SliderInt("Humanizer Max (ms)", &myHero.HumanizerMax, 0, 500);

			ImGui::Spacing();
			if (ImGui::Button("Unload Script")) {
				unloaded = true;
			}
			ImGui::EndChildFrame();
			break;
		case 1:
			ImGui::BeginChildFrame(id++, ImVec2(0, 0));
			for (int i = 0; i < EntitiesContainer::HeroesIndex.size(); i++) {
				EntityBase* temp = EntitiesContainer::Entities[EntitiesContainer::HeroesIndex[i]];
				ImGui::BeginChildFrame(id++, ImVec2(0, 40));
				ImGui::Text(UtilityFunctions::Stringf("%s - %p index %d, visible %d, health %f, pos %s", temp->ObjectName.c_str(), temp->PCObject, temp->Index, temp->Visible, temp->Health, temp->Pos.toString().c_str()).c_str());
				ImGui::EndChildFrame();
			}
			ImGui::EndChildFrame();
			break;
		case 2:
			BuffInfo * bi;
			ImGui::BeginChildFrame(id++, ImVec2(0, 0));
			for (int i = 0; i < myHero.BuffMgr->BuffCount(); i++) {
				bi = myHero.BuffMgr->getBuff(i);
				if (!bi->IsValid()) continue;

				ImGui::BeginChildFrame(id++, ImVec2(0, 40));
				ImGui::Text(UtilityFunctions::Stringf("%d Name: \"%s\", Type: %d, Start: %f, End: %f, Count Int: %d, Alt: %d, Float: %f",
					i,
					bi->GetBuffName(),
					bi->GetBuffType(),
					bi->GetBuffStartTime(),
					bi->GetBuffEndTime(),
					bi->GetBuffCountInt(),
					bi->GetBuffCountAlt(),
					bi->GetBuffCountFloat()).c_str());
				ImGui::EndChildFrame();
			}
			ImGui::EndChildFrame();
			break;
		case 3:
			SpellSlot * ss;
			ImGui::BeginChildFrame(id++, ImVec2(0, 0));
			for (int i = 0; i < 64; i++) {
				ss = myHero.SpellBk->GetSpellSlot((Spells)i);
				if (!(ss && ss->GetSpellInfo())) continue;

				ImGui::BeginChildFrame(id++, ImVec2(0, 40));
				ImGui::Text(UtilityFunctions::Stringf("[%d] Spell name: \"%s\" Level: %d",
					i,
					ss->GetSpellInfo()->GetSpellData()->GetName(),
					ss->GetSpellLvl()
				).c_str()
				);
				ImGui::EndChildFrame();
			}
			ImGui::EndChildFrame();
			break;
		case 4:
			ImGui::BeginChildFrame(id++, ImVec2(0, 0));
			if (myHero.ActiveSpell) {
				ImGui::BeginChildFrame(id++, ImVec2(0, 40));
				ImGui::Text(UtilityFunctions::Stringf("Spell name: \"%s\"",
					myHero.ActiveSpell->GetSpellInfo()->GetSpellData()->GetName()
				).c_str());
				ImGui::EndChildFrame();

				ImGui::BeginChildFrame(id++, ImVec2(0, 40));
				ImGui::Text(UtilityFunctions::Stringf("Casting start time: %f, Casting end time: %f, Cast time: %f",
					myHero.ActiveSpell->GetCastingStartTime(),
					myHero.ActiveSpell->GetCastingEndTime(),
					myHero.ActiveSpell->GetCastTime()
				).c_str());
				ImGui::EndChildFrame();

				ImGui::BeginChildFrame(id++, ImVec2(0, 40));
				ImGui::Text(UtilityFunctions::Stringf("Channel start time: %f, Channel end time: %f, Channeling time: %f",
					myHero.ActiveSpell->GetChannelStartTime(),
					myHero.ActiveSpell->GetChannelEndTime(),
					myHero.ActiveSpell->GetChannelingTime()
				).c_str());
				ImGui::EndChildFrame();

				ImGui::BeginChildFrame(id++, ImVec2(0, 40));
				ImGui::Text(UtilityFunctions::Stringf("Start Pos: %s, End Pos: %s",
					myHero.ActiveSpell->GetStartPos().toString().c_str(),
					myHero.ActiveSpell->GetEndPos().toString().c_str()
				).c_str());
				ImGui::EndChildFrame();

				ImGui::BeginChildFrame(id++, ImVec2(0, 40));
				ImGui::Text(UtilityFunctions::Stringf("Target Index: %d",
					myHero.ActiveSpell->GetTargetIndex()
				).c_str());
				ImGui::EndChildFrame();
			}
			ImGui::EndChildFrame();
			break;
		}
		ImGui::EndChild();
		ImGui::End();

		if (!ShowMain)
			PostMessage(FindWindowA(0, "League of Legends (TM) Client"), WM_ACTIVATE, WA_ACTIVE, 0);
	}

	void renderOverlay() {
		createWindow("Overlay", NULL, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar, { 100, 200 }, { 300, 200 });
		ImGui::BeginChild("#Overlay", ImVec2(0, 0), false);
		ImGui::TextColored(myHero.UseSpell ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), "Using Spell");
		ImGui::EndChild();
		ImGui::End();
	}

	void Tick() {
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		////////////////////////////

		renderOverlay();
		if (ShowLog) renderLogWindow();
		if (ShowMain) renderMainWindow();

		////////////////////////////
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	void ResetConsole() {
		ConsoleLines.clear();

		for (int i = 0; i < MaxConsoleLines; i++) {
			ConsoleLines.push_back(std::string(""));
		}
	}

	void Print(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		std::string buf = UtilityFunctions::VFormat(fmt, ap);
		va_end(ap);
		Print(buf);
	}

	void Print(std::string in) {
		if (stopLogging) return;
		for (int i = 0; i < MaxConsoleLines - 1; i++) {
			ConsoleLines[i] = ConsoleLines[i + 1];
		}
		ConsoleLines[MaxConsoleLines - 1] = in;
	}

	void Dispose() {
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX9_Shutdown();
		ImGui::DestroyContext(ImGui::GetCurrentContext());
	}
}