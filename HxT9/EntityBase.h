#pragma once
#include <windows.h>
#include "CObject.h"

class EntityBase
{
	Interval	NetworkIDUpdateInterval			= Interval::s;
	int			NetworkIDUpdateNInt				= 5;

	Interval	TeamUpdateInterval				= Interval::s;
	int			TeamUpdateNInt					= 10;

	Interval	PosUpdateInterval				= Interval::ms;
	int			PosUpdateNInt					= 1;

	Interval	NameUpdateInterval				= Interval::s;
	int			NameUpdateNInt					= 10;

	Interval	VisibleUpdateInterval			= Interval::ms;
	int			VisibleUpdateNInt				= 1;

	Interval	HealthUpdateInterval			= Interval::ms;
	int			HealthUpdateNInt				= 1;

	Interval	AttackRangeUpdateInterval		= Interval::cs;
	int			AttackRangeUpdateNInt			= 2;

	Interval	DamageUpdateInterval			= Interval::ds;
	int			DamageUpdateNInt				= 1;

	Interval	ArmorUpdateInterval				= Interval::ds;
	int			ArmorUpdateNInt					= 5;

	Interval	BoundingRadiusUpdateInterval	= Interval::s;
	int			BoundingRadiusUpdateNInt		= 1;

	Interval	TargetableUpdateInterval		= Interval::cs;
	int			TargetableUpdateNInt			= 1;

	Interval	SpellCDUpdateInterval			= Interval::ds;
	int			SpellCDUpdateNInt				= 2;

	Interval	MissileUpdateInterval			= Interval::cs;
	int			MissileUpdateNInt				= 5;

	Interval	SpellBookUpdateInterval			= Interval::s;
	int			SpellBookUpdateNInt				= 5;

	Interval	ActiveSpellUpdateInterval		= Interval::ms;
	int			ActiveSpellUpdateNInt			= 5;

	Interval	BuffManagerUpdateInterval		= Interval::s;
	int			BuffManagerUpdateNInt			= 5;

	Interval	LevelUpdateInterval				= Interval::ds;
	int			LevelUpdateNInt					= 5;

	Interval	MovementSpeedUpdateInterval		= Interval::ds;
	int			MovementSpeedUpdateNInt			= 5;

	Interval	ObjectNameUpdateInterval		= Interval::s;
	int			ObjectNameUpdateNInt			= 10;

	float	NetworkIDNextUpdate			= 0;
	float	TeamNextUpdate				= 0;
	float	PosNextUpdate				= 0;
	float	NameNextUpdate = 0;
	float	VisibleNextUpdate			= 0;
	float	HealthNextUpdate			= 0;
	float	DamageNextUpdate			= 0;
	float	ArmorNextUpdate				= 0;
	float	AttackRangeNextUpdate		= 0;
	float	BoundingRadiusNextUpdate	= 0;
	float	TargetableNextUpdate		= 0;
	float	MissileNextUpdate			= 0;
	float	SpellCDNextUpdate			= 0;
	float	SpellBookNextUpdate = 0;
	float	ActiveSpellNextUpdate		= 0;
	float	BuffManagerNextUpdate		= 0;
	float	LevelNextUpdate				= 0;
	float	MovementSpeedNextUpdate		= 0;
	float	ObjectNameNextUpdate		= 0;

public:
	bool DEBUGGING = true;

	CObject*	PCObject			= NULL;
	EntityType	Type				= EntityType::Undefined;
	int			Index				= 0;
	int			Offset				= 0;

	DWORD		NetworkID			= 0;
	int			Team				= 0;
	Vector3		Pos					= Vector3();
	std::string	Name				= "";
	bool		Visible				= 0;
	float		Health				= 0;
	float		MaxHealth			= 0;
	float		BaseAttackDamage	= 0;
	float		BonusAttackDamage	= 0;
	float		AbilityPower		= 0;
	float		Armor				= 0;
	float		MagicResist			= 0;
	float		AttackRange			= 0;
	float		BoundingRadius		= 0;
	bool		Targetable			= 0;
	SpellBook*	SpellBk				= NULL;
	ActiveSpell* ActiveSpell		= NULL;
	BuffManager* BuffMgr			= NULL;
	int			Level				= 0;
	float		MovementSpeed		= 0;
	std::string	ObjectName			= "";

	//Heroes
	float			SpellQCD			= 0;
	float			SpellWCD			= 0;
	float			SpellECD			= 0;
	float			SpellRCD			= 0;
	float			SpellDCD			= 0;
	float			SpellFCD			= 0;

	//Missile
	int			SourceIndex			= 0;
	int			TargetIndex			= 0;
	SpellInfo*	SpellInfo			= NULL;
	Vector3		StartPos			= Vector3();
	Vector3		EndPos				= Vector3();
	std::string	MissileName			= "";


	EntityBase();
	void	Init(CObject* PObj, EntityType Type);
	void	Delete();
	void	UpdateAttributes();
	float	GetTotalAttackDamage();
	bool	IsEnemyTo(EntityBase* eb);
	float	HasBuff(std::string BuffName);
	float	IncomingDamage(float seconds);
};