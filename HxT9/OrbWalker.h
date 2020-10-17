#pragma once
class OrbWalker
{
public:
	bool focused = true;
	float dmg;

	void tick();
	void ComboLogic();
	void PushLogic();
	void LastHitLogic();
	void FleeLogic();
};

