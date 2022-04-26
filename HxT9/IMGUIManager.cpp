#include "IMGUIManager.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <Windows.h>
#include "globalVars.h"
#include "UtilityFunctions.h"

bool scrollToBottom = true;
int selectedTab = 0;

void IMGUIManager::init(LPDIRECT3DDEVICE9 pDevice) {
	HWND hwnd = FindWindowA(0, "League of Legends (TM) Client"); //Ok

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(pDevice);

	resetConsole();
}

__inline void createWindow(const char* name, bool* openedStatus, ImGuiWindowFlags flags, ImVec2 Pos = ImVec2(0, 0), ImVec2 Size = ImVec2(100,100)) {
	ImGui::Begin(name, openedStatus, ImGuiWindowFlags_NoSavedSettings | flags);
	ImGui::SetWindowPos(Pos, ImGuiCond_Once);
	ImGui::SetWindowSize(Size, ImGuiCond_Once);
}

void IMGUIManager::renderLogWindow() {
	int id = 1000;
	createWindow("Log", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse, ImVec2(100, 100), ImVec2(650, 500));
	ImGui::BeginChild("#Log", ImVec2(0, 0), false);

	ImGui::BeginChildFrame(id++, ImVec2(0, 30));
	ImGui::Checkbox("Keep scrolling to bottom", &scrollToBottom);
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

void IMGUIManager::renderMainWindow() {
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
		ImGui::Checkbox("Use Custom Prediction (WIP)", &utils.UseCustomPrediction);
		ImGui::Checkbox("Use Input ScanCode", &inputManager.useScan);
		ImGui::Checkbox("Use Spells (Num1 On, Num2 Off)", &myHero.useSpell);
		ImGui::Checkbox("Use AA in Combo", &myHero.useAAInCombo);
		ImGui::Checkbox("BaseUlt Enabled", &baseUlt.enabled);
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
		for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
			EntityBase* temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
			ImGui::BeginChildFrame(id++, ImVec2(0, 40));
			ImGui::Text(utils.stringf("%s index %d, visible %d, health %f, pos %s", temp->ObjectName.c_str(), temp->Index, temp->Visible, temp->Health, temp->Pos.toString().c_str()).c_str());
			ImGui::EndChildFrame();
		}
		ImGui::EndChildFrame();
		break;
	case 2:
		BuffInfo* bi;
		ImGui::BeginChildFrame(id++, ImVec2(0, 0));
		for (int i = 0; i < myHero.BuffMgr->BuffCount(); i++) {
			bi = myHero.BuffMgr->getBuff(i);
			if (!bi->IsValid()) continue;

			ImGui::BeginChildFrame(id++, ImVec2(0, 40));
			ImGui::Text(utils.stringf("Name: \"%s\", Type: %d, Start: %f, End: %f, Count Int: %d, Alt: %d, Float: %f",
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
		SpellSlot* ss;
		ImGui::BeginChildFrame(id++, ImVec2(0, 0));
		for (int i = 0; i < 64; i++) {
			ss = myHero.SpellBk->GetSpellSlot((Spells)i);
			if (!(ss && ss->GetSpellInfo())) continue;

			ImGui::BeginChildFrame(id++, ImVec2(0, 40));
			ImGui::Text(utils.stringf("[%d] Missile name: \"%s\", Spell name: \"%s\"",
				i,
				ss->GetSpellInfo()->GetSpellData()->GetMissileName(),
				ss->GetSpellInfo()->GetSpellData()->GetSpellName()).c_str());
			ImGui::EndChildFrame();
		}
		ImGui::EndChildFrame();
		break;
	case 4:
		ImGui::BeginChildFrame(id++, ImVec2(0, 0));
		if (myHero.ActiveSpell) {
			ImGui::BeginChildFrame(id++, ImVec2(0, 40));
			ImGui::Text(utils.stringf("Missile name: \"%s\", Spell name: \"%s\"",
				myHero.ActiveSpell->GetSpellInfo()->GetSpellData()->GetMissileName(),
				myHero.ActiveSpell->GetSpellInfo()->GetSpellData()->GetSpellName()
			).c_str());
			ImGui::EndChildFrame();

			ImGui::BeginChildFrame(id++, ImVec2(0, 40));
			ImGui::Text(utils.stringf("Casting start time: %f, Casting end time: %f, Cast time: %f",
				myHero.ActiveSpell->GetCastingStartTime(),
				myHero.ActiveSpell->GetCastingEndTime(),
				myHero.ActiveSpell->GetCastTime()
			).c_str());
			ImGui::EndChildFrame();

			ImGui::BeginChildFrame(id++, ImVec2(0, 40));
			ImGui::Text(utils.stringf("Channel start time: %f, Channel end time: %f, Channeling time: %f",
				myHero.ActiveSpell->GetChannelStartTime(),
				myHero.ActiveSpell->GetChannelEndTime(),
				myHero.ActiveSpell->GetChannelingTime()
			).c_str());
			ImGui::EndChildFrame();

			ImGui::BeginChildFrame(id++, ImVec2(0, 40));
			ImGui::Text(utils.stringf("Start Pos: %s, End Pos: %s",
				myHero.ActiveSpell->GetStartPos().toString().c_str(),
				myHero.ActiveSpell->GetEndPos().toString().c_str()
			).c_str());
			ImGui::EndChildFrame();

			ImGui::BeginChildFrame(id++, ImVec2(0, 40));
			ImGui::Text(utils.stringf("Target Index: %d",
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
	createWindow("Overlay", NULL, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar, { 100, 200 }, {300, 200});
	ImGui::BeginChild("#Overlay", ImVec2(0, 0), false);
	ImGui::TextColored(myHero.useSpell ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), "Using Spell");
	ImGui::EndChild();
	ImGui::End();
}

void IMGUIManager::tick() {
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
void IMGUIManager::resetConsole() {
	ConsoleLines.clear();

	for (int i = 0; i < MaxConsoleLines; i++) {
		ConsoleLines.push_back(std::string(""));
	}
}
void IMGUIManager::print(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	std::string buf = utils.vformat(fmt, ap);
	va_end(ap);
	print(buf);
}
void IMGUIManager::print(std::string in) {
	for (int i = 0; i < MaxConsoleLines - 1; i++) {
		ConsoleLines[i] = ConsoleLines[i + 1];
	}
	ConsoleLines[MaxConsoleLines - 1] = in;
}
void IMGUIManager::destroy() {
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX9_Shutdown();
	ImGui::DestroyContext(ImGui::GetCurrentContext());
}