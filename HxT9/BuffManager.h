#pragma once
#include "BuffInfo.h"

class BuffManager
{
public:
	int BuffCount();
	BuffInfo* getBuff(int index);
};

