#pragma once
#include "offsets.h"
#include "Vector3.h"

class CastingStruct {
public:
	void setStartIndex(short value);
	void setTargetIndex(short value);
	void setStartPosition(Vector3 pos);
	void setEndPosition(Vector3 pos);
};