#include "IMGUI_Manager.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <Windows.h>
#include "globalVars.h"
#include "UtilityFunctions.h"

LRESULT WINAPI WndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

void IMGUI_Manager::init(LPDIRECT3DDEVICE9 pDevice) {
	HWND hwnd = FindWindowA(0, "League of Legends (TM) Client"); //Ok

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(pDevice);

	resetConsole();
}

void IMGUI_Manager::tick(LPDIRECT3DDEVICE9 pDevice) {
	if (GetKeyState(VK_LSHIFT) & 0x8000) {
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		////////////////////////////

		ImGui::Begin("HxT9");
		ImGui::SetWindowSize({ 1000.f, MaxConsoleLines * 18.f });

		for (int i = 0; i < MaxConsoleLines; i++) {
			ImGui::Text(ConsoleLines[i].c_str());
		}
		ImGui::End();

		////////////////////////////
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
}
void IMGUI_Manager::resetConsole() {
	ConsoleLines.clear();

	for (int i = 0; i < MaxConsoleLines; i++) {
		ConsoleLines.push_back(std::string(""));
	}
}
void IMGUI_Manager::print(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	std::string buf = utils.vformat(fmt, ap);
	va_end(ap);
	print(buf);
}
void IMGUI_Manager::print(std::string in) {
	for (int i = 0; i < MaxConsoleLines - 1; i++) {
		ConsoleLines[i] = ConsoleLines[i + 1];
	}
	ConsoleLines[MaxConsoleLines - 1] = in;
}
void IMGUI_Manager::destroy() {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}