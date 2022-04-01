#include "Drawer.h"
#include "globalVars.h"

# define M_PI           3.14159265358979323846 

Drawer::Drawer() {

}
Drawer::Drawer(LPDIRECT3DDEVICE9 pDev) {
	pDevice = pDev;
	D3DXCreateLine(pDevice, &m_Line);
	D3DXCreateFont(pDevice, 14, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_FontSmall);
	D3DXCreateFont(pDevice, 18, 7, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_FontMedium);
}

void Drawer::tick(LPDIRECT3DDEVICE9 pDev) {
	pDevice = pDev;
	if (m_Line == NULL)
		D3DXCreateLine(pDevice, &m_Line);
	if (m_FontSmall == NULL)
		D3DXCreateFontW(pDevice, 14, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &m_FontSmall);
	if (m_FontMedium == NULL)
		D3DXCreateFontW(pDevice, 18, 7, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &m_FontMedium);
}

void Drawer::drawPolygon(Vector3 vectors[], int n, int d3dcolor, float lineWidth, bool isWorldPos) {
	Vector3 shortVectors[1000];
	Vector3 vDir;
	int nShort = 0, subVectors;
	float maxLength = 1000, rest;

	if (isWorldPos) {
		//////////////////////////SUBDIVIDING VECTORS
		//Suddivido le varie linee in linee più corte
		shortVectors[nShort] = vectors[0];
		nShort++;
		for (int i = 1; i < n; i++) {
			vDir = vectors[i] - vectors[i - 1];
			if (vDir.getMagnitude() > maxLength) {
				subVectors = (int)(vDir.getMagnitude() / maxLength);
				for (int j = 0; j < subVectors; j++) {
					shortVectors[nShort] = vectors[i].setRelativeMagnitude(vectors[i - 1], maxLength * (j + 1));
					nShort++;
				}
				shortVectors[nShort] = vectors[i].setRelativeMagnitude(vectors[i - 1], vDir.getMagnitude());
				nShort++;
			}
			else {
				shortVectors[nShort] = vectors[i];
				nShort++;
			}
		}
		if (n > 2) {
			vDir = vectors[n - 1] - vectors[0];
			if (vDir.getMagnitude() > maxLength) {
				subVectors = (int)(vDir.getMagnitude() / maxLength);
				for (int j = 0; j < subVectors; j++) {
					//shortVectors[nShort] = vectors[i].setRelativeMagnitude(shortVectors[nShort - 1], maxLength);
					shortVectors[nShort] = vectors[0].setRelativeMagnitude(vectors[n - 1], maxLength * (j + 1));
					nShort++;
				}
				shortVectors[nShort] = vectors[0].setRelativeMagnitude(vectors[n - 1], vDir.getMagnitude());
				nShort++;
			}
			else {
				shortVectors[nShort] = vectors[0];
				nShort++;
			}
		}
		//////////////////////////SUBDIVIDING VECTORS
		for (int i = 0; i < nShort; i++) {
			GH.worldToScreen(&shortVectors[i], &shortVectors[i]);
		}
	}
	else {
		nShort = n;
		for (int i = 0; i < n; i++) {
			shortVectors[i] = vectors[i];
		}
	}

	m_Line->SetWidth(lineWidth);
	m_Line->SetAntialias(true);
	m_Line->Begin();

	for (int i = 0; i < nShort-1; i++) {
		if (shortVectors[i].isDrawable() && shortVectors[i + 1].isDrawable()) {
			screenPoints[0] = { shortVectors[i].x, shortVectors[i].y };
			screenPoints[1] = { shortVectors[i + 1].x, shortVectors[i + 1].y };
			m_Line->Draw(screenPoints, 2, d3dcolor);
		}
	}

	m_Line->End();
}
void Drawer::drawLine(Vector3 vStart, Vector3 vEnd, int d3dcolor, float lineWidth) {
	Vector3 points[2] = { {0,0,0}, {0,0,0} };
	points[0] = vStart;
	points[1] = vEnd;
	drawPolygon(points, 2, d3dcolor, lineWidth, true);
}
void Drawer::drawRectangle(Vector3 vStart, Vector3 vEnd, float radius, int d3dcolor, float lineWidth) {
	Vector3 vDirection = Vector3(vEnd.x - vStart.x, 60, vEnd.z - vStart.z);

	Vector3 points[4] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };
	points[0] = vStart + Vector3(vDirection.z * -1, 60, vDirection.x * 1).setMagnitude(radius);
	points[1] = vStart + Vector3(vDirection.z * 1, 60, vDirection.x * -1).setMagnitude(radius);
	points[2] = vEnd + Vector3(vDirection.z * 1, 60, vDirection.x * -1).setMagnitude(radius);
	points[3] = vEnd + Vector3(vDirection.z * -1, 60, vDirection.x * 1).setMagnitude(radius);
	drawPolygon(points, 4, d3dcolor, lineWidth, true);
}
void Drawer::drawCircumference(Vector3 center, float radius, int precision, int d3dcolor, float lineWidth) {
	if (precision > 360)
		precision = 360;
	float angle = (2 * M_PI) / precision; //radianti
	Vector3 points[360];
	for (int i = 0; i < precision; i++) {
		points[i] = Vector3(center.x + radius * cosf(angle * i), center.y, center.z + radius * sinf(angle * i));
	}
	drawPolygon(points, precision, d3dcolor, lineWidth, true);
}
void Drawer::drawCircumferenceScreen(Vector3 center, float radius, int precision, int d3dcolor, float lineWidth) {
	if (precision > 360)
		precision = 360;
	float angle = (2 * M_PI) / precision; //radianti
	Vector3 points[360];
	for (int i = 0; i < precision; i++) {
		points[i] = Vector3(center.x + radius * cosf(angle * i), center.y + radius * sinf(angle * i), 0);
	}
	drawPolygon(points, precision, d3dcolor, lineWidth, false);
}
void Drawer::drawConic(Vector3 center, Vector3 vEnd, int angle, int precision, int d3dcolor, float lineWidth) {
	Vector3 points[360];
	Vector3 vDirection = vEnd - center;

	float radAngle = angle * (M_PI / 180);
	float castAngle = (vEnd - center).getAngle();

	if (precision > min(angle, 360))
		precision = min(angle, 360);

	while (castAngle < 0)
		castAngle += 2 * M_PI;

	float minAngle = castAngle - radAngle / 2;
	float maxAngle = castAngle + radAngle / 2;

	float angleStep = (maxAngle - minAngle) / precision;

	for (int i = 0; i < precision; i++) {
		points[i] = Vector3(center.x + vDirection.getMagnitude() * cosf(minAngle + angleStep * i), center.y, center.z + vDirection.getMagnitude() * sinf(minAngle + angleStep * i));
	}

	points[precision] = center;

	drawPolygon(points, precision + 1, d3dcolor, lineWidth, true);
}
void Drawer::drawTextSmall(Vector3 screenPosition, const char* text, int d3dcolor) {
	if (!screenPosition.isDrawable()) return;
	SetRect(&m_Rect, screenPosition.x, screenPosition.y, 1920, 1080);
	m_FontSmall->DrawTextA(NULL, text, -1, &m_Rect, 0, d3dcolor);
}
void Drawer::drawTextMedium(Vector3 screenPosition, const char* text, int d3dcolor) {
	if (!screenPosition.isDrawable()) return;
	SetRect(&m_Rect, screenPosition.x, screenPosition.y, 1920, 1080);
	m_FontMedium->DrawTextA(NULL, text, -1, &m_Rect, 0, d3dcolor);
}

Vector3* Drawer::getRectanglePoints(Vector3 vStart, Vector3 vEnd, float radius, int d3dcolor, float lineWidth) {
	Vector3 vDirection = Vector3(vEnd.x - vStart.x, 60, vEnd.z - vStart.z);

	Vector3 points[4] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };
	points[0] = vStart + Vector3(vDirection.z * -1, 60, vDirection.x * 1).setMagnitude(radius);
	points[1] = vStart + Vector3(vDirection.z * 1, 60, vDirection.x * -1).setMagnitude(radius);
	points[2] = vEnd + Vector3(vDirection.z * 1, 60, vDirection.x * -1).setMagnitude(radius);
	points[3] = vEnd + Vector3(vDirection.z * -1, 60, vDirection.x * 1).setMagnitude(radius);
	return points;
}
Vector3* Drawer::getCircumferencePoints(Vector3 center, float radius, int precision, int d3dcolor, float lineWidth) {
	if (precision > 360)
		precision = 360;
	float angle = (2 * M_PI) / precision; //radianti
	Vector3 points[360];
	for (int i = 0; i < precision; i++) {
		points[i] = Vector3(center.x + radius * cosf(angle * i), center.y, center.z + radius * sinf(angle * i));
	}
	return points;
}
Vector3* Drawer::getConicPoints(Vector3 center, Vector3 vEnd, int angle, int precision, int d3dcolor, float lineWidth) {
	Vector3 points[360];
	Vector3 vDirection = vEnd - center;

	float radAngle = angle * (M_PI / 180);
	float castAngle = (vEnd - center).getAngle();

	if (precision > min(angle, 360))
		precision = min(angle, 360);

	while (castAngle < 0)
		castAngle += 2 * M_PI;

	float minAngle = castAngle - radAngle / 2;
	float maxAngle = castAngle + radAngle / 2;

	float angleStep = (maxAngle - minAngle) / precision;

	for (int i = 0; i < precision; i++) {
		points[i] = Vector3(center.x + vDirection.getMagnitude() * cosf(minAngle + angleStep * i), center.y, center.z + vDirection.getMagnitude() * sinf(minAngle + angleStep * i));
	}

	points[precision] = center;

	return points;
}