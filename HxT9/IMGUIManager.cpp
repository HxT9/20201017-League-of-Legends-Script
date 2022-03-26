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

void IMGUIManager::tick(LPDIRECT3DDEVICE9 pDevice) {
	if (!Show) return;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	////////////////////////////

	ImGui::Begin("HxT9", &Show, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::SetWindowPos(ImVec2(100, 100), ImGuiCond_Once);
	ImGui::SetWindowSize(ImVec2(650, 500), ImGuiCond_Once);

	ImGui::BeginChild("Tab", ImVec2(0, 0), false);

	ImGui::BeginChildFrame(1, ImVec2(0, 40));
	{
		ImGui::SameLine();
		if (ImGui::Button("Settings", ImVec2(80, 25))) {
			selectedTab = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Log", ImVec2(40, 25))) {
			selectedTab = 1;
		}
	}
	ImGui::EndChildFrame();

	switch (selectedTab) {
	case 0:
		ImGui::BeginChildFrame(2, ImVec2(0, 0));
		ImGui::LabelText("To-Do", "To-Do");
		ImGui::EndChildFrame();
		break;
	case 1:
		ImGui::BeginChildFrame(2, ImVec2(0, 30));
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
		break;
	}
	ImGui::EndChild();
	ImGui::End();

	////////////////////////////
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	if (!Show)
		PostMessage(FindWindowA(0, "League of Legends (TM) Client"), WM_ACTIVATE, WA_ACTIVE, 0);
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