#include "IMGUI_Manager.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <Windows.h>
#include "globalVars.h"

LRESULT WINAPI WndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

void IMGUI_Manager::init(LPDIRECT3DDEVICE9 pDevice) {
	HWND hwnd = FindWindowA(0, "League of Legends (TM) Client"); //Ok

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(pDevice);
}

void IMGUI_Manager::tick(LPDIRECT3DDEVICE9 pDevice) {
	if (GetKeyState(VK_LSHIFT) & 0x8000) {
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		////////////////////////////

		ImGui::Begin("HxT9");
		ImGui::SetWindowSize({ 700, 200 });
		for (int i = 0; i < 10; i++) {
			if (text[i] != NULL)
				ImGui::Text(text[i]);
		}
		ImGui::End();

		////////////////////////////
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
}
void IMGUI_Manager::resetConsole() {
	for (int i = 0; i < 10; i++) {
		text[i] = "";
	}
}
void IMGUI_Manager::print(const char* in) {
	for (int i = 0; i < 9; i++) {
		text[i] = text[i + 1];
	}
	text[9] = in;
}
void IMGUI_Manager::print(std::string in) {
	for (int i = 0; i < 9; i++) {
		text[i] = text[i + 1];
	}
	char* out = new char[in.size() + 1];
	copy(in.begin(), in.end(), out);
	text[9] = out;
	delete[] out;
}
void IMGUI_Manager::destroy() {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}