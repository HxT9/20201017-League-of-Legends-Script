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
	createWindow("Main", &ShowMain, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse, ImVec2(500, 100), ImVec2(650, 500));

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
		if (ImGui::Button("Missiles", ImVec2(80, 25))) {
			selectedTab = 2;
		}
	}
	ImGui::EndChildFrame();

	switch (selectedTab) {
	case 0:
		ImGui::BeginChildFrame(id++, ImVec2(0, 0));
		ImGui::Checkbox("Debugging", &scriptManager.Debugging);
		ImGui::EndChildFrame();
		break;
	case 1:
		ImGui::BeginChildFrame(id++, ImVec2(0, 0));
		for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
			EntityBase* temp = entitiesContainer.entities[i];
			ImGui::BeginChildFrame(id++, ImVec2(0, 40));
			ImGui::Text(utils.stringf("Hero index %d, visible %b, health %f, pos %s", temp->Index, temp->Visible, temp->Health, temp->Pos.toString()).c_str());
			ImGui::EndChildFrame();
		}
		ImGui::EndChildFrame();
		break;
	case 2:
		ImGui::BeginChildFrame(id++, ImVec2(0, 0));
		for (int i = 0; i < entitiesContainer.missilesIndex.size(); i++) {
			EntityBase* temp = entitiesContainer.entities[i];
			ImGui::BeginChildFrame(id++, ImVec2(0, 40));
			ImGui::Text(utils.stringf("Missile index %d, source %d, target %d, StartPos %s, EndPos %s, Name %s", temp->Index, temp->SourceIndex, temp->TargetIndex, temp->StartPos.toString(), temp->EndPos.toString(), temp->MissileName).c_str());
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

void IMGUIManager::tick() {
	if (!(ShowLog || ShowMain)) return;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	////////////////////////////

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