#include "BuffInfo.h"
#include <Windows.h>
#include "offsets.h"

bool BuffInfo::IsValid(){
	if (this == NULL || (DWORD)this <= 0x1000)
		return false;

	return strcmp(GetBuffName(), "NULL") && GetBuffCountAlt() > 0;
}
unsigned char BuffInfo::GetBuffType(){
	return *(unsigned char*)((DWORD)this + oBuffType);
}
float BuffInfo::GetBuffStartTime(){
	return *(float*)((DWORD)this + oBuffStartTime);
}
float BuffInfo::GetBuffEndTime(){
	return *(float*)((DWORD)this + oBuffEndTime);
}
int BuffInfo::GetBuffCountAlt(){
	return (*(int*)((DWORD)this + oBuffCountAlt));
}
float BuffInfo::GetBuffCountFloat(){
	return *(float*)((DWORD)this + oBuffFloatCount);
}
int BuffInfo::GetBuffCountInt(){
	return *(int*)((DWORD)this + oBuffIntCount);
}
const char* BuffInfo::GetBuffName(){
	DWORD aux = *(DWORD*)((DWORD)this + oBuffName);
	if (aux == NULL)
		return "NULL";

	if (*(DWORD*)(aux + oBuffName) == NULL)
		return "NULL";

	return (char*)(aux + oBuffNameOffset);
}