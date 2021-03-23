#include "Minimap.h"
#include "globalVars.h"
#include "offsets.h"

Vector3 Minimap::getMinimapPos()
{
	Vector3 ret;
	DWORD mmap = *(DWORD*)(baseAddress + oMinimapObj);
	DWORD mmapHud = *(DWORD*)(mmap + oMinimapObjectHud);
	ret.x = *(float*)(mmapHud + oMinimapPosition);
	ret.y = *(float*)(mmapHud + oMinimapPosition + 0x4);
	ret.z = 0;

	return ret;
}

Vector3 Minimap::getMinimapSize()
{
	Vector3 ret;
	DWORD mmap = *(DWORD*)(baseAddress + oMinimapObj);
	DWORD mmapHud = *(DWORD*)(mmap + oMinimapObjectHud);
	ret.x = *(float*)(mmapHud + oMinimapSize);
	ret.y = *(float*)(mmapHud + oMinimapSize + 0x4);
	ret.z = 0;

	return ret;
}

/*
Mouse RedSide
1750	- 1745	- 305
762 	- 687	- 687

Mouse Blue Size
1537	- 1463	- 22
974		- 970	- 970

Pos
1531	- 1454	- 13
754		- 677	- 677

Size
227		- 301	- 302
227		- 302	- 302


MouseSize - Pos % scale

Red Calcs
0,96475	- 0,96677 - 0,96688
0,03524	- 0,03311 - 0x03311

Blue Calcs
0,02643	- 0,02990 - 0,02980
0,96916	- 0,97019 - 0,97019
*/