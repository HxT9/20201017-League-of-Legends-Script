#pragma once
enum class SpellState
{
	Ready = 0,
	NotAvailable = 4,
	Surpressed = 8,
	NotLearned = 12,
	Processing = 24,
	Cooldown = 32,
	NoMana = 64,
	Unknown
};

enum class Behaviour
{
	Nothing = 0,
	Combo = 1,
	Push = 2,
	LastHit = 3,
	Flee = 4
};

enum class Spells
{
	Q = 0,
	W = 1,
	E = 2,
	R = 3,
	Summoner1 = 4,
	Summoner2 = 5,
	Item1 = 6,
	Item2 = 7,
	Item3 = 8,
	Item4 = 9,
	Item5 = 10,
	Item6 = 11,
	Trinket = 12,
	Recall = 13,
	ApheliosCrescendum = 45,
	AA = 46
};

enum class SkillShotType
{
	Linear = 0,
	Circular = 1,
	Conic = 2,
	Threeway = 3,
	Rectangular = 4,
	Polygon = 5
};

enum class ObjectTypeFlags {
	GameObject = (1 << 0),
	NeutralCamp = (1 << 1),  
	DeadObject = (1 << 4),  
	InvalidObject = (1 << 5),  
	AIBaseCommon = (1 << 7),  
	AttackableUnit = (1 << 9),  
	AI = (1 << 10), 
	Minion = (1 << 11), 
	Hero = (1 << 12), 
	Turret = (1 << 13), 
	Unknown0 = (1 << 14), 
	Missile = (1 << 15), 
	Unknown1 = (1 << 16), 
	Building = (1 << 17), 
	Unknown2 = (1 << 18), 
};

enum class EntityType {
	Undefined = -1,
	Hero,
	Minion,
	Turret,
	Missile,
	Troy
};

enum class Interval {
	ms	= 1000,
	cs	= 100,
	ds	= 10,
	s	= 1,
};