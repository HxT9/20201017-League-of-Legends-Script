#include "CastingStruct.h"
#include "offsets.h"

void CastingStruct::setStartIndex(short value)
{
	*(short*)(this + oCastingStructStartIndex) = value;
}

void CastingStruct::setTargetIndex(short value)
{
	*(short*)(this + oCastingStructTargetIndex) = value;
}

void CastingStruct::setStartPosition(Vector3 pos)
{
	*(Vector3*)(this + oCastingStructStartPos) = pos;
}

void CastingStruct::setEndPosition(Vector3 pos)
{
	*(Vector3*)(this + oCastingStructEndPos) = pos;
}
