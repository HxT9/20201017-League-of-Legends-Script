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

			//SpellBook
			if (gameTime >= SpellBookNextUpdate || DEBUGGING) {
				SpellBk = PCObject->GetSpellBook();
				SpellBookNextUpdate = getNextUpdate(SpellBookUpdateInterval, SpellBookUpdateNInt, Offset);
			}

			//ActiveSpell
			if (gameTime >= ActiveSpellNextUpdate || DEBUGGING) {
				ActiveSpell = PCObject->GetActiveSpell();
				ActiveSpellNextUpdate = getNextUpdate(ActiveSpellUpdateInterval, ActiveSpellUpdateNInt, Offset);
			}

			//BuffManager
			if (gameTime >= BuffManagerNextUpdate || DEBUGGING) {
				BuffMgr = PCObject->getBuffManager();
				BuffManagerNextUpdate = getNextUpdate(BuffManagerUpdateInterval, BuffManagerUpdateNInt, Offset);
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
			if (gameTime >= ObjectNameNextUpdate || DEBUGGING) {
				ObjectName = PCObject->GetObjectName();
				ObjectNameNextUpdate = getNextUpdate(ObjectNameUpdateInterval, ObjectNameUpdateNInt, Offset);
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
	float Dmg = BaseAttackDamage + BonusAttackDamage;

	return Dmg;
}

float EntityBase::GetBasicAttackDamage(float armor, float magicRes) {
	int lvl = 0;
	float Dmg = GetTotalAttackDamage();
	BuffInfo* bi;

	if (ObjectName == "Kalista") {
		Dmg *= 0.9;
	}

	if (ObjectName == "Draven" && GetBuff("DravenSpinningAttack")) {
		lvl = SpellBk->GetSpellSlot(Spells::Q)->GetSpellLvl();
		Dmg += 35 + (5 * lvl) + (BonusAttackDamage * (60 + 10 * lvl) / 100);
	}
	
	if (bi = GetBuff("6672buff"), bi ? bi->GetBuffCountInt() == 2 : false) {
		Dmg += 60 + BonusAttackDamage * 0.45;
	}

	return Dmg;
}

bool EntityBase::IsEnemyTo(EntityBase* eb)
{
	return (this->Team == 100 && eb->Team == 200) || (this->Team == 200 && eb->Team == 100);
}

/*
* Check if entity has buff and return number of it in a float variable
*/
BuffInfo* EntityBase::GetBuff(std::string BuffName) {
	BuffInfo* bi = NULL;
	for (int i = 0; i < BuffMgr->BuffCount(); i++) {
		bi = BuffMgr->getBuff(i);
		if (bi && bi->IsValid() && std::string(bi->GetBuffName()) == BuffName)
			return bi;
	}
	return NULL;
}

float EntityBase::IncomingDamage(float seconds) {
	float totalDamage = 0;
	float missileApplyDamageDelay = 0.1;
	EntityBase* temp;

	//Ciclo i missili che ce l'hanno come target
	for (int i = 0; i < entitiesContainer.missilesIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.missilesIndex[i]];

		if (temp->TargetIndex == Index
			&& entitiesContainer.entities[temp->SourceIndex]
			&& temp->Pos.distTo(Pos) / temp->SpellInfo->GetSpellData()->GetSpellSpeed() + missileApplyDamageDelay < seconds
			&& temp->MissileName.find("BasicAttack") != std::string::npos) {

			totalDamage += utils.calcEffectiveDamage(entitiesContainer.entities[temp->SourceIndex]->GetTotalAttackDamage(), Armor);
		}
	}

	//Ciclo i minion che fanno il melee
	for (int i = 0; i < entitiesContainer.minionsIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.minionsIndex[i]];

		if (temp->ObjectName.find("MinionMelee") != std::string::npos
			&& temp->ActiveSpell
			&& temp->ActiveSpell->GetTargetIndex() == Index) {

			if (gameTime + seconds > temp->ActiveSpell->GetChannelEndTime()) {
				totalDamage += utils.calcEffectiveDamage(entitiesContainer.entities[temp->SourceIndex]->GetTotalAttackDamage(), Armor);
			}
		}
	}

	//Ciclo i player che fanno il melee
	for (int i = 0; i < entitiesContainer.minionsIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.minionsIndex[i]];
		if (temp->ActiveSpell
			&& temp->Pos.distTo(Pos) < 400
			&& temp->ActiveSpell->GetTargetIndex() == Index
			&& std::string(temp->ActiveSpell->GetSpellInfo()->GetSpellData()->GetMissileName()).find("BasicAttack") != std::string::npos) {

			if (gameTime + seconds > temp->ActiveSpell->GetChannelEndTime()) {
				totalDamage += utils.calcEffectiveDamage(entitiesContainer.entities[temp->SourceIndex]->GetTotalAttackDamage(), Armor);
			}
		}
	}

	//Ritorno una percentuale perchè ogni tanto ritorna troppo
	return totalDamage * 0.7;
}