#include "EntityBase.h"
#include "globalVars.h"
#include "CObject.h"

float getNextUpdate(Interval UpdateInterval, int NInt, int Offset) {
	return gameTime + ((float)((1 * NInt) + Offset) / (float)UpdateInterval);
}

EntityBase::EntityBase()
{}

void EntityBase::Init(CObject* PObj, EntityType entityType)
{
	PCObject = PObj;
	Type	= entityType;
	Index	= PObj->GetIndex();
	Offset	= Index % 10;
}

void EntityBase::Delete() {
	PCObject = NULL;
	Type = EntityType::Undefined;
}

void EntityBase::UpdateAttributes()
{
	if (PCObject) {
		//NetworkID
		if (gameTime >= NetworkIDNextUpdate || DEBUGGING) {
			NetworkID = PCObject->GetNetworkID();
			NetworkIDNextUpdate = getNextUpdate(NetworkIDUpdateInterval, NetworkIDUpdateNInt, Offset);
		}

		//Team
		if (gameTime >= TeamNextUpdate || DEBUGGING) {
			Team = PCObject->GetTeam();
			TeamNextUpdate = getNextUpdate(TeamUpdateInterval, TeamUpdateNInt, Offset);
		}

		//Pos
		if (gameTime >= PosNextUpdate || DEBUGGING) {
			Pos = PCObject->GetPos();
			PosNextUpdate = getNextUpdate(PosUpdateInterval, PosUpdateNInt, Offset);
		}

		//Name
		if (gameTime >= NameNextUpdate || DEBUGGING) {
			Name = PCObject->GetName();
			NameNextUpdate = getNextUpdate(NameUpdateInterval, NameUpdateNInt, Offset);
		}

		if (Type == EntityType::Hero || Type == EntityType::Minion || Type == EntityType::Turret) {
			//Visible
			if (gameTime >= VisibleNextUpdate || DEBUGGING) {
				Visible = PCObject->IsVisible();
				VisibleNextUpdate = getNextUpdate(VisibleUpdateInterval, VisibleUpdateNInt, Offset);
			}

			//Health
			if (gameTime >= HealthNextUpdate || DEBUGGING) {
				Health = PCObject->GetHealth();
				MaxHealth = PCObject->GetMaxHealth();
				HealthNextUpdate = getNextUpdate(HealthUpdateInterval, HealthUpdateNInt, Offset);
			}

			//AttackDamage
			if (gameTime >= DamageNextUpdate || DEBUGGING) {
				BaseAttackDamage = PCObject->GetBaseAttackDamage();
				BonusAttackDamage = PCObject->GetBonusAttackDamage();
				AbilityPower = PCObject->GetAP();
				DamageNextUpdate = getNextUpdate(DamageUpdateInterval, DamageUpdateNInt, Offset);
			}

			//Armor
			if (gameTime >= ArmorNextUpdate || DEBUGGING) {
				Armor = PCObject->GetArmor();
				MagicResist = PCObject->GetMagicResist();
				ArmorNextUpdate = getNextUpdate(ArmorUpdateInterval, ArmorUpdateNInt, Offset);
			}

			//AttackRange
			if (gameTime >= AttackRangeNextUpdate || DEBUGGING) {
				AttackRange = PCObject->GetAttackRange();
				AttackRangeNextUpdate = getNextUpdate(AttackRangeUpdateInterval, AttackRangeUpdateNInt, Offset);
			}

			//BoundingRadius
			if (gameTime >= BoundingRadiusNextUpdate || DEBUGGING) {
				BoundingRadius = PCObject->GetBoundingRadius();
				BoundingRadiusNextUpdate = getNextUpdate(BoundingRadiusUpdateInterval, BoundingRadiusUpdateNInt, Offset);
			}

			//Targetable
			if (gameTime >= TargetableNextUpdate || DEBUGGING) {
				Targetable = PCObject->isTargetable();
				TargetableNextUpdate = getNextUpdate(TargetableUpdateInterval, TargetableUpdateNInt, Offset);
			}
		}

		if (Type == EntityType::Hero){
			//SpellCD
			if (gameTime >= SpellCDNextUpdate || DEBUGGING) {
				if (PCObject->GetSpellBook() &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::Q) &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::Q)->GetSpellLvl() > 0) {

					SpellQCD = PCObject->GetSpellBook()->GetSpellSlot(Spells::Q)->GetSpellReady() - gameTime;
					if (SpellQCD < 0) SpellQCD = 0;
				}
				else
					SpellQCD = -1;

				if (PCObject->GetSpellBook() &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::W) &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::W)->GetSpellLvl() > 0) {

					SpellWCD = PCObject->GetSpellBook()->GetSpellSlot(Spells::Q)->GetSpellReady() - gameTime;
					if (SpellWCD < 0) SpellWCD = 0;
				}
				else
					SpellWCD = -1;

				if (PCObject->GetSpellBook() &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::E) &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellLvl() > 0) {

					SpellECD = PCObject->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellReady() - gameTime;
					if (SpellECD < 0) SpellECD = 0;
				}
				else
					SpellECD = -1;

				if (PCObject->GetSpellBook() &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::R) &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl() > 0) {

					SpellRCD = PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellReady() - gameTime;
					if (SpellRCD < 0) SpellRCD = 0;
				}
				else
					SpellRCD = -1;

				if (PCObject->GetSpellBook() &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::Summoner1) &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::Summoner1)->GetSpellLvl() > 0) {

					SpellDCD = PCObject->GetSpellBook()->GetSpellSlot(Spells::Summoner1)->GetSpellReady() - gameTime;
					if (SpellDCD < 0) SpellDCD = 0;
				}
				else
					SpellDCD = -1;

				if (PCObject->GetSpellBook() &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::Summoner2) &&
					PCObject->GetSpellBook()->GetSpellSlot(Spells::Summoner2)->GetSpellLvl() > 0) {

					SpellFCD = PCObject->GetSpellBook()->GetSpellSlot(Spells::Summoner2)->GetSpellReady() - gameTime;
					if (SpellFCD < 0) SpellFCD = 0;
				}
				else
					SpellFCD = -1;

				SpellCDNextUpdate = getNextUpdate(SpellCDUpdateInterval, SpellCDUpdateNInt, Offset);
			}

			//SpellCD
			if (gameTime >= ActiveSpellNextUpdate || DEBUGGING) {
				ActiveSpell = PCObject->GetActiveSpell();
				ActiveSpellNextUpdate = getNextUpdate(ActiveSpellUpdateInterval, ActiveSpellUpdateNInt, Offset);
			}

			//Level
			if (gameTime >= LevelNextUpdate || DEBUGGING) {
				Level = PCObject->GetLevel();
				LevelNextUpdate = getNextUpdate(LevelUpdateInterval, LevelUpdateNInt, Offset);
			}

			//MovementSpeed
			if (gameTime >= MovementSpeedNextUpdate || DEBUGGING) {
				MovementSpeed = PCObject->GetMovementSpeed();
				MovementSpeedNextUpdate = getNextUpdate(MovementSpeedUpdateInterval, MovementSpeedUpdateNInt, Offset);
			}

			//ChampionName
			if (gameTime >= ChampionNameNextUpdate || DEBUGGING) {
				ChampionName = PCObject->GetChampionName();
				ChampionNameNextUpdate = getNextUpdate(ChampionNameUpdateInterval, ChampionNameUpdateNInt, Offset);
			}
		}

		if (Type == EntityType::Missile) {
			//Missile
			if (gameTime >= MissileNextUpdate || DEBUGGING) {
				SourceIndex	= PCObject->GetMissileSourceIndex();
				TargetIndex	= PCObject->GetMissileTargetIndex();
				SpellInfo	= PCObject->GetMissileSpellInfo();
				StartPos	= PCObject->GetMissileStartPos();
				EndPos		= PCObject->GetMissileEndPos();
				MissileName = SpellInfo->GetSpellData()->GetMissileName();
				TeamNextUpdate = getNextUpdate(MissileUpdateInterval, MissileUpdateNInt, Offset);
			}
		}
	}
}

float EntityBase::GetTotalAttackDamage()
{
	return BaseAttackDamage + BonusAttackDamage;
}

bool EntityBase::IsEnemyTo(EntityBase* eb)
{
	return (this->Team == 100 && eb->Team == 200) || (this->Team == 200 && eb->Team == 100);
}
