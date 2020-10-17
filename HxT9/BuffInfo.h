#pragma once
class BuffInfo
{
public:
	bool IsValid();
	unsigned char GetBuffType();
	float GetBuffStartTime();
	float GetBuffEndTime();
	int GetBuffCountAlt();
	float GetBuffCountFloat();
	int GetBuffCountInt();
	const char* GetBuffName();
};

