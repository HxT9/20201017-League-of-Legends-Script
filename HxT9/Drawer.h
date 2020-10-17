#pragma once
#include "d3d9.h"
#pragma comment(lib, "d3d9.lib")
#include "d3dx9.h"
#pragma comment(lib, "d3dx9.lib")

#include "Vector3.h"

class Drawer
{
public:
	LPDIRECT3DDEVICE9 pDevice;
	LPD3DXLINE m_Line;
	D3DXVECTOR2 screenPoints[2];
	LPD3DXFONT m_Font;
	RECT m_Rect;

	Drawer();
	Drawer(LPDIRECT3DDEVICE9 pDev);
	void tick(LPDIRECT3DDEVICE9 pDev);
	void drawPolygon(Vector3 vectors[], int n, int d3dcolor, float lineWidth);
	void drawLine(Vector3 vStart, Vector3 vEnd, int d3dcolor, float lineWidth);
	void drawRectangle(Vector3 vStart, Vector3 vEnd, float radius, int d3dcolor, float lineWidth); //Radius is half height
	void drawCircumference(Vector3 center, float radius, int precision, int d3dcolor, float lineWidth);
	void drawConic(Vector3 center, Vector3 vEnd, int angle, int precision, int d3dcolor, float lineWidth);
	void drawText(Vector3 screenPosition, const char* text, int d3dcolor);
	Vector3* getRectanglePoints(Vector3 vStart, Vector3 vEnd, float radius, int d3dcolor, float lineWidth);
	Vector3* getCircumferencePoints(Vector3 center, float radius, int precision, int d3dcolor, float lineWidth);
	Vector3* getConicPoints(Vector3 center, Vector3 vEnd, int angle, int precision, int d3dcolor, float lineWidth);
};