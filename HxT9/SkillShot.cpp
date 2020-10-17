#include "SkillShot.h"

SkillShot::SkillShot(Vector3 startPos, Vector3 endPos, Vector3 centerPos, float spd, float ang, SkillShotType type, float start, float dur)
{
	startPosition = startPos; endPosition = endPos; centerPosition = centerPos; speed = spd; angle = ang; skillShotType = type; startTime = start, duration = dur;
}

SkillShot SkillShot::linearSkillShot(Vector3 startPos, Vector3 endPos, float spd)
{
	return SkillShot(startPos, endPos, Vector3(), spd, 0, SkillShotType::Linear, 0, 0);
}

SkillShot SkillShot::circularSkillShot(Vector3 centerPos, float start, float dur)
{
	return SkillShot(Vector3(), Vector3(), centerPos, 0, 0, SkillShotType::Circular, start, dur);
}

SkillShot SkillShot::conicSkillShot(Vector3 centerPos, float ang)
{
	return SkillShot(Vector3(), Vector3(), centerPos, 0, ang, SkillShotType::Conic, 0, 0);
}