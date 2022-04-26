#include "ControlWindow.h"
#include <Windows.h>
#include <atlstr.h>

#include "d3d9.h"
#include "d3dx9.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);;
void DirectXInit(HWND hWnd);
void Render();

char wndName[256] = "HxT9 Control Panel";
int width = 800, height = 600;
IDirect3D9Ex* pObject = 0;
IDirect3DDevice9Ex* pDevice = 0;
D3DPRESENT_PARAMETERS pParams;

void ControlWindow::Init() {
	ATOM registeredClass;
	WNDCLASSEXA wClass;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.lpfnWndProc = WinProc;
	wClass.lpszClassName = wndName;
	wClass.lpszMenuName = wndName;

	registeredClass = RegisterClassExA(&wClass);

	WindowHWND = CreateWindowExA(NULL, wndName, wndName, WS_VISIBLE, 1, 1, width, height, 0, 0, 0, 0);
	DirectXInit(WindowHWND);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_PAINT:
		Render();
		break;

	case WM_DESTROY:
		PostQuitMessage(1);
		return 0;

	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void DirectXInit(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &pObject)))
		CloseWindow(hWnd);

	ZeroMemory(&pParams, sizeof(pParams));
	pParams.Windowed = TRUE;
	pParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pParams.hDeviceWindow = hWnd;
	pParams.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	pParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	pParams.BackBufferWidth = width;
	pParams.BackBufferHeight = height;
	pParams.EnableAutoDepthStencil = TRUE;
	pParams.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(pObject->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pParams, 0, &pDevice))) {
		CloseWindow(hWnd);
		return;
	}
}

void Render()
{
	pDevice->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	pDevice->BeginScene();
	pDevice->EndScene();
	pDevice->PresentEx(0, 0, 0, 0, 0);
}