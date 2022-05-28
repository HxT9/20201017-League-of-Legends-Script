#pragma once
#include "d3d9.h"
#include "d3dx9.h"

#include "Vector3.h"

namespace D3DDrawer
{
	void Init(LPDIRECT3DDEVICE9 pDev);
	void Tick(LPDIRECT3DDEVICE9 pDev);
	void DrawPolygon(Vector3 vectors[], int n, int d3dcolor, float lineWidth, bool isWorldPos);
	void DrawLine(Vector3 vStart, Vector3 vEnd, int d3dcolor, float lineWidth);
	void DrawRectangle(Vector3 vStart, Vector3 vEnd, float radius, int d3dcolor, float lineWidth); //Radius is half height
	void DrawCircumference(Vector3 center, float radius, int precision, int d3dcolor, float lineWidth);
	void DrawCircumferenceScreen(Vector3 center, float radius, int precision, int d3dcolor, float lineWidth);
	void DrawConic(Vector3 center, Vector3 vEnd, int angle, int precision, int d3dcolor, float lineWidth);
	void DrawTextSmall(Vector3 screenPosition, const char* text, int d3dcolor);
	void DrawTextMedium(Vector3 screenPosition, const char* text, int d3dcolor);
	Vector3* GetRectanglePoints(Vector3 vStart, Vector3 vEnd, float radius, int d3dcolor, float lineWidth);
	Vector3* GetCircumferencePoints(Vector3 center, float radius, int precision, int d3dcolor, float lineWidth);
	Vector3* GetConicPoints(Vector3 center, Vector3 vEnd, int angle, int precision, int d3dcolor, float lineWidth);
};