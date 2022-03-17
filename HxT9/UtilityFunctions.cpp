#include "UtilityFunctions.h"
#include "globalVars.h"
#include <vector>
#include <cstdarg>
#include <fstream>

#define M_PI           3.14159265358979323846

void UtilityFunctions::drawBoundingBox() {
	if (myHero.LPObject) {
		drawer.drawCircumference(myHero.LPObject->GetPos(), myHero.boundingRadius, 10, 0xff00ff00, 2);
	}
}

float UtilityFunctions::calcEffectiveDamage(float damage, float armor) { //Anche per la magicRes
	return damage * (100 / (100 + armor));
}

bool UtilityFunctions::isValidTarget(CObject* target) {
	return target && target->IsVisible() && target->isTargetable() && GH.isAlive(target);
}

void UtilityFunctions::drawEntities() {
	float r;
	CObject* temp;
	for (int i = 0; i < entities.heroes.size(); i++) {
		temp = entities.heroes[i];
		if (isValidTarget(temp)) {
			if (temp->GetTeam() != myHero.LPObject->GetTeam()) {
				r = temp->GetAttackRange() + temp->GetBoundingRadius();

				drawer.drawCircumference(temp->GetPos(), r, 30, 0x7fff0000, 2);

				if (temp->GetPos().distTo(myHero.LPObject->GetPos()) > 800 && temp->GetPos().distTo(myHero.LPObject->GetPos()) < 2500)
					drawer.drawLine(temp->GetPos(), myHero.LPObject->GetPos(), 0xffff0000, 2);

				if (temp == myHero.selectedTarget) {
					drawer.drawCircumference(temp->GetPos(), 75, 50, 0x7fffff00, 4);
				}
			}
		}
	}
	for (int i = 0; i < entities.turrets.size(); i++) {
		temp = entities.turrets[i];
		if (temp->GetTeam() != myHero.LPObject->GetTeam() && temp->IsVisible() && temp->GetHealth() > 1) {
			drawer.drawCircumference(temp->GetPos(), 850, 30, 0xffff0000, 2);
		}
	}

	if(myHero.LPObject)
		drawer.drawCircumference(myHero.LPObject->GetPos(), myHero.LPObject->GetAttackRange() + myHero.boundingRadius, 50, 0x7f00ff00, 2);
}

void UtilityFunctions::drawLastHittableMinions() {
	float myDamage = myHero.LPObject->GetTotalAttackDamage();
	float effDamage;
	if (strcmp(myHero.championName, "kalista") == 0) {
		myDamage *= 0.9;
	}

	float dmgIncoming, AATimeNeeded;
	CObject *temp, *missile;
	for (int i = 0; i < entities.minions.size(); i++) {
		temp = entities.minions[i];
		if (!isValidTarget(temp)
			|| temp->GetTeam() == myHero.LPObject->GetTeam()
			|| temp->GetPos().distTo(myHero.LPObject->GetPos()) > 1500
			|| temp->GetMaxHealth() < 10)
			continue;


		effDamage = calcEffectiveDamage(myDamage, temp->GetArmor());
		AATimeNeeded = (temp->GetPos().distTo(myHero.LPObject->GetPos()) / myHero.AAMissileSpeed ? myHero.AAMissileSpeed : 2000) + myHero.AACastTime;
		dmgIncoming = 0;
		for (int j = 0; j < entities.missiles.size(); j++) {
			missile = entities.missiles[j];
			if (!missile) continue;
			if (missile->GetMissileTargetIndex() != temp->GetIndex()) continue;

			if (missile->GetPos().distTo(temp->GetPos()) / 300 < AATimeNeeded) {
				dmgIncoming += entities.GetCObjectFromIndex(missile->GetMissileSourceIndex()) ? entities.GetCObjectFromIndex(missile->GetMissileSourceIndex())->GetTotalAttackDamage() : 0;
			}
		}

		if (effDamage > temp->GetHealth()) {
			drawer.drawCircumference(temp->GetPos(), 50, 10, 0x7f00ff00, 2);
		}
		else if (effDamage > temp->GetHealth() - (dmgIncoming)) {
			drawer.drawCircumference(temp->GetPos(), 50, 10, 0x7fffff00, 2);
		}
		else {
			drawer.drawCircumference(temp->GetPos(), 50, 10, 0x7fff0000, 2);
		}
	}
}

void UtilityFunctions::drawDebug() {
	Vector3 pos;

	for (int i = 0; i < 5; i++) {
		std::vector<CObject*> ent = entities.getEntities(i);
		for (int j = 0; j < ent.size(); j++) {
			if (ent[j]) {
				GH.worldToScreen(&ent[j]->GetPos(), &pos);
				drawer.drawTextSmall(pos, utils.stringf("%p", ent[j]).c_str(), 0xffff0000);

				pos.y += 30;
				drawer.drawTextSmall(pos, utils.stringf("%i", ent[j]->GetIndex()).c_str(), 0xffff0000);
			}
		}
	}

	Path p = myHero.LPObject->GetPath();
	for (int i = 0; i < p.nPathPoints; i++) {
		drawer.drawCircumference(p.pathPoints[i], 30, 10, 0xffffff00, 2);

		if (i > 0)
			drawer.drawLine(p.pathPoints[i], p.pathPoints[i - 1], 0xffffffff, 2);
	}

	/*Path myPath;
	for (int j = 0; j < entities.iHeroes; j++) {
		if (entities.heroes[j] != NULL && entities.heroes[j]->IsVisible() && GH.isAlive(entities.heroes[j])) {
			myPath = entities.heroes[j]->GetPath();
			for (int i = 0; i < myPath.nPathPoints; i++) {
				drawer.drawCircumference(myPath.pathPoints[i], 50, 15, 0xffffff00, 3);
			}
		}
	}*/
}

__declspec(naked) void* __cdecl UtilityFunctions::get_peb()
{
	__asm {
		mov eax, fs:0x2C //I dont remember if is 0x18, 0x2c or 0x30
		mov eax, [eax + 0x30]
		retn
	}
}

void UtilityFunctions::drawPredictedPos() {
	CObject* temp;
	for (int i = 0; i < entities.heroes.size(); i++) {
		temp = entities.heroes[i];
		if (isValidTarget(temp)) {
			drawer.drawCircumference(getPredictedPos(temp, 1, 0, myHero.LPObject), 40, 15, 0xff00ffff, 2);
		}
	}
}

/*CObject* UtilityFunctions::getMissileSourceEntity(CObject* missile) {
	short id = missile->GetMissileSourceIndex();

	

	for (int i = 0; i < entities.minions.size(); i++)
		if (entities.minions[i]->GetIndex() == id)
			return entities.minions[i];

	for (int i = 0; i < entities.heroes.size(); i++)
		if (entities.heroes[i]->GetIndex() == id)
			return entities.heroes[i];

	for (int i = 0; i < entities.turrets.size(); i++)
		if (entities.turrets[i]->GetIndex() == id)
			return entities.turrets[i];

	return NULL;
}*/

/*
bool isColliding(Vector3 start, Vector3 end, float SpellWidth, Vector3 center, float radius) {
	Equation line = start.lineEquation(end);
	float dist = abs(line.a * center.x + line.b * center.z + line.c) / sqrt(line.a * line.a + line.b * line.b);
	if (dist <= radius + SpellWidth / 2) {
		return true;
	}
	else {
		//Controllo se è dentro il rettangolo (Raddrizzo il rettangolo sul punto start - width/2)
		
		//3---end----2
		//|		   |
		//|		   |
		//0---start--1
		//Calcolo dei vertici
		Vector3 vertici[4];
		vertici[0] = start.setRelativeMagnitude(start, SpellWidth / 2);
		vertici[1] = vertici[0].rotatePoint(start, M_PI);
		vertici[2] = end.setRelativeMagnitude(end, SpellWidth / 2).rotatePoint(end, M_PI / -2);
		vertici[3] = vertici[2].rotatePoint(end, M_PI);

		float angle = atan2f(start.z - end.z, start.x - end.x);
		angle = (angle > M_PI / 2 ? -1 : 1) * abs(angle - M_PI / 2);

		//Rotazione
		vertici[1] = vertici[1].rotatePoint(vertici[0], angle);
		vertici[2] = vertici[2].rotatePoint(vertici[0], angle);
		vertici[3] = vertici[3].rotatePoint(vertici[0], angle);
		center = center.rotatePoint(vertici[0], angle);

		//Calcolo gli estremi assoluti del rettangolo
		float b = vertici[0].z, t = vertici[0].z, l = vertici[0].x, r = vertici[0].x;
		for (int i = 0; i < 4; i++) {
			if (vertici[i].z < b) b = vertici[i].z;
			if (vertici[i].z > t) t = vertici[i].z;
			if (vertici[i].x < l) l = vertici[i].x;
			if (vertici[i].x > r) r = vertici[i].x;
		}
		
		//Vedo se il cerchio è dentro
		if (center.x >= l && center.x <= r && center.z >= b && center.z <= t)
			return true;

		return false;
	}
}
*/

bool UtilityFunctions::isCollision(Vector3 start, Vector3 end, float SpellWidth, Vector3 center, float radius)
{
	Vector3 vecEndStart = end - start;
	Vector3 vecCircleStart = center - start;
	vecEndStart.y = 60;
	vecCircleStart.y = 60;

	Vector3 vecProjection = vecCircleStart.ProjectionOn(vecEndStart);
	vecProjection = vecProjection.sameDirectionAs(vecEndStart);

	if (vecProjection.getMagnitude() > vecEndStart.getMagnitude())
	{
		vecProjection = vecEndStart;
		return (end.distTo(center) < radius);
	}
	else
	{
		Vector3 vecCircleToProjection = (vecCircleStart - vecProjection);
		float distFromLineToCircle = vecCircleToProjection.getMagnitude();

		return SpellWidth / 2 + radius >= distFromLineToCircle;
	}
}

int UtilityFunctions::minionsColliding(Vector3 start, Vector3 end, float width) {
	float length = start.distTo(end);
	CObject* temp;
	int collision = 0;
	for (int i = 0; i < entities.minions.size(); i++) {
		temp = entities.minions[i];
		if (!isValidTarget(temp) || temp->GetTeam() == myHero.LPObject->GetTeam() || start.distTo(temp->GetPos()) > start.distTo(end))
			continue;

		if (isCollision(start, end, width, temp->GetPos(), temp->GetBoundingRadius()))
			collision++;
	}
	return collision;
}

int UtilityFunctions::heroesColliding(Vector3 start, Vector3 end, float width) {
	float length = start.distTo(end);
	int collision = 0;
	CObject* temp;
	for (int i = 0; i < entities.heroes.size(); i++) {
		temp = entities.heroes[i];
		if (!isValidTarget(temp) || temp->GetTeam() == myHero.LPObject->GetTeam() || start.distTo(temp->GetPos()) > start.distTo(end))
			continue;

		if (isCollision(start, end, width, temp->GetPos(), temp->GetBoundingRadius()))
			collision++;
	}
	return collision;
}

Vector3 UtilityFunctions::getPredictedPos(CObject* hero, float seconds, float width, CObject* sender) {
	int firstPoint = 0;
	float distance = hero->GetMovementSpeed() * seconds;
	Path path = hero->GetPath();
	Vector3 curPos = hero->GetPos();
	Vector3 result;

	if (!seconds || !path.nPathPoints) {
		return hero->GetPos();
	}

	firstPoint = hero->getAIMgrPassedWaypoints() - 1;
	
	if (firstPoint < 0 || firstPoint >= path.nPathPoints) return curPos;

	path.pathPoints[firstPoint] = curPos;

	for (int i = firstPoint; i < path.nPathPoints - 1; i++) {
		if (distance < path.pathPoints[i].distTo(path.pathPoints[i + 1])) {
			result = path.pathPoints[i + 1].setRelativeMagnitude(path.pathPoints[i], distance);
			distance = 0;
			break;
		}

		distance -= path.pathPoints[i].distTo(path.pathPoints[i + 1]);
	}
	if (distance > 0) {
		result = path.pathPoints[path.nPathPoints - 1];
	}

	return result;
}

void UtilityFunctions::ChampionCustomDraw() {
	float dmg;
	int spellLvl;
	Vector3 screenPos;
	CObject* temp;
	if (strcmp(myHero.championName, "Xerath") == 0) {
		spellLvl = myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
		dmg = (2 + spellLvl) * (150 + 50 * spellLvl + myHero.LPObject->GetAP() * 0.45);

		for (int i = 0; i < entities.heroes.size(); i++) {
			temp = entities.heroes[i];
			if (isValidTarget(temp) && temp->IsEnemyTo(myHero.LPObject) && temp->GetHealth() < calcEffectiveDamage(dmg, temp->GetMagicResist())) {
				GH.worldToScreen(&temp->GetPos(), &screenPos);
				drawer.drawTextSmall(screenPos, "!!! KILLABLE WITH ULT !!!", 0xffffff00);
			}
		}
	}
	else if (strcmp(myHero.championName, "Kalista") == 0) {
		for (int i = 0; i < entities.heroes.size(); i++) {
			temp = entities.heroes[i];
			dmg = championScript.getKalistaSpearDamage(entities.heroes[i]);
			if (dmg > 0) {
				if (isValidTarget(temp) && temp->IsEnemyTo(myHero.LPObject) && temp->GetHealth() > 0) {
					GH.worldToScreen(&temp->GetPos(), &screenPos);
					drawer.drawTextMedium(screenPos, stringf("%d%", (int)(dmg * 100 / temp->GetHealth())).c_str(), 0xffffff00);
				}
			}
		}
	}
}

void UtilityFunctions::drawSpellCD() {
	CObject* temp;
	Vector3 screenPos;
	float cdQ, cdW, cdE, cdR, cdS1, cdS2;
	std::string text;
	for (int i = 0; i < entities.heroes.size(); i++) {
		temp = entities.heroes[i];
		if (temp != myHero.LPObject && temp->IsVisible() && GH.isAlive(temp) && temp->GetSpellBook()) {
			GH.worldToScreen(&temp->GetPos(), &screenPos);
			screenPos.y -= 100;

			text = "";

			if (temp->GetSpellBook()->GetSpellSlot(Spells::Q) &&
				temp->GetSpellBook()->GetSpellSlot(Spells::Q)->GetSpellLvl() > 0) {
				cdQ = temp->GetSpellBook()->GetSpellSlot(Spells::Q)->GetSpellReady() - gameTime;
				if (cdQ < 0) cdQ = 0;
			}
			else
				cdQ = -1;

			if (temp->GetSpellBook()->GetSpellSlot(Spells::W) &&
				temp->GetSpellBook()->GetSpellSlot(Spells::W)->GetSpellLvl() > 0) {
				cdW = temp->GetSpellBook()->GetSpellSlot(Spells::W)->GetSpellReady() - gameTime;
				if (cdW < 0) cdW = 0;
			}
			else
				cdW = -1;

			if (temp->GetSpellBook()->GetSpellSlot(Spells::E) &&
				temp->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellLvl() > 0) {
				cdE = temp->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellReady() - gameTime;
				if (cdE < 0) cdE = 0;
			}
			else
				cdE = -1;

			if (temp->GetSpellBook()->GetSpellSlot(Spells::R) &&
				temp->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl() > 0) {
				cdR = temp->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellReady() - gameTime;
				if (cdR < 0) cdR = 0;
			}
			else
				cdR = -1;

			cdS1 = temp->GetSpellBook()->GetSpellSlot(Spells::Summoner1)->GetSpellReady() - gameTime;
			if (cdS1 < 0) cdS1 = 0;
			cdS2 = temp->GetSpellBook()->GetSpellSlot(Spells::Summoner2)->GetSpellReady() - gameTime;
			if (cdS2 < 0) cdS2 = 0;

			text = stringf("[%d]", (int)ceilf(cdQ));
			screenPos.x += 25;
			drawer.drawTextMedium(screenPos, text.c_str(), cdQ == 0 ? 0xff00ff00 : 0xffff0000);

			text = stringf("[%d]", (int)ceilf(cdW));
			screenPos.x += 35;
			drawer.drawTextMedium(screenPos, text.c_str(), cdW == 0 ? 0xff00ff00 : 0xffff0000);

			text = stringf("[%d]", (int)ceilf(cdE));
			screenPos.x += 35;
			drawer.drawTextMedium(screenPos, text.c_str(), cdE == 0 ? 0xff00ff00 : 0xffff0000);

			text = stringf("[%d]", (int)ceilf(cdR));
			screenPos.x += 35;
			drawer.drawTextMedium(screenPos, text.c_str(), cdR == 0 ? 0xff00ff00 : 0xffff0000);

			text = stringf("[%d]", (int)ceilf(cdS1));
			screenPos.x += 35;
			drawer.drawTextMedium(screenPos, text.c_str(), cdS1 == 0 ? 0xff00ffff : 0xffffff00);

			text = stringf("[%d]", (int)ceilf(cdS2));
			screenPos.x += 35;
			drawer.drawTextMedium(screenPos, text.c_str(), cdS2 == 0 ? 0xff00ffff : 0xffffff00);
		}
	}
}

std::string UtilityFunctions::vformat(const char* fmt, va_list ap)
{
	// Allocate a buffer on the stack that's big enough for us almost
	// all the time.
	size_t size = 2048;
	char buf[2048];

	// Try to vsnprintf into our buffer.
	va_list apcopy;
	va_copy(apcopy, ap);
	int needed = vsnprintf(&buf[0], size, fmt, ap);
	// NB. On Windows, vsnprintf returns -1 if the string didn't fit the
	// buffer.  On Linux & OSX, it returns the length it would have needed.

	if (needed <= size && needed >= 0) {
		// It fit fine the first time, we're done.
		return std::string(&buf[0]);
	}
	else {
		// vsnprintf reported that it wanted to write more characters
		// than we allotted.  So do a malloc of the right size and try again.
		// This doesn't happen very often if we chose our initial size
		// well.
		std::vector <char> buf;
		size = needed;
		buf.resize(size);
		needed = vsnprintf(&buf[0], size, fmt, apcopy);
		return std::string(&buf[0]);
	}
}
std::string UtilityFunctions::stringf(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	std::string buf = vformat(fmt, ap);
	va_end(ap);
	return buf;
}

void drawRectangle(Vector3 vStart, Vector3 vEnd, float radius) {
	drawer.drawRectangle(vStart, vEnd, radius, 0xffffffff, 2);
}

void drawCircular(Vector3 vCenter, float radius) {
	drawer.drawCircumference(vCenter, radius, 15, 0xffffffff, 2);
}

void drawConic(Vector3 vCenter, Vector3 vEnd, int angle) {
	drawer.drawConic(vCenter, vEnd, angle, 15, 0xffffffff, 2);
}

void UtilityFunctions::drawActiveSpells() {
	CObject* temp;
	for (int i = 0; i < entities.heroes.size(); i++) {
		temp = entities.heroes[i];
		if (temp != NULL && temp->GetActiveSpell() != NULL && 
			temp->GetActiveSpell()->GetTargetIndex() == NULL
#ifndef _DEBUG
			&& temp->GetTeam() != myHero.LPObject->GetTeam()
#endif
			) {
			char* spellName = temp->GetActiveSpell()->GetSpellInfo()->GetSpellData()->GetMissileName();
			Vector3 vEnd = temp->GetActiveSpell()->GetEndPos();
			Vector3 vStart = temp->GetActiveSpell()->GetStartPos();
				
			//Aatrox
			if (strcmp(spellName, "AatroxQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 650);
				drawRectangle(vStart, vEnd, 130);
				return;
			}
			if (strcmp(spellName, "AatroxQ3") == 0) {
				drawCircular(vEnd, 300);
				return;
			}
			if (strcmp(spellName, "AatroxW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 825);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Ahri
			if (strcmp(spellName, "AhriOrbofDeception") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 880);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			if (strcmp(spellName, "AhriSeduce") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 975);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Akali
			if (strcmp(spellName, "AkaliQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 550);
				drawConic(vStart, vEnd, 50);
				return;
			}
			if (strcmp(spellName, "AkaliE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 825);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "AkaliR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 525);
				drawRectangle(vStart, vEnd, 65);
				return;
			}
			if (strcmp(spellName, "AkaliRb") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 525);
				drawRectangle(vStart, vEnd, 65);
				return;
			}
			//Alistar
			if (strcmp(spellName, "Pulverize") == 0) {
				drawCircular(vEnd, 365);
				return;
			}
			//Amumu
			if (strcmp(spellName, "BandageToss") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			if (strcmp(spellName, "CurseoftheSadMummy") == 0) {
				drawCircular(vEnd, 550);
				return;
			}
			//Anivia
			if (strcmp(spellName, "FlashFrostSpell") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 110);
				return;
			}
			//Annie
			if (strcmp(spellName, "AnnieW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 600);
				drawConic(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "AnnieR") == 0) {
				drawCircular(vEnd, 290);
				return;
			}
			//Aphelios
			if (strcmp(spellName, "ApheliosCalibrumQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1450);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "ApheliosInfernumQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawConic(vStart, vEnd, 50);
				return;
			}
			if (strcmp(spellName, "ApheliosR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1600);
				drawRectangle(vStart, vEnd, 125);
				return;
			}
			//Ashe
			if (strcmp(spellName, "Volley") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1200);
				drawConic(vStart, vEnd, 45);
				return;
			}
			if (strcmp(spellName, "EnchantedCrystalArrow") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 25000);
				drawRectangle(vStart, vEnd, 130);
				return;
			}
			//AurelionSol
			if (strcmp(spellName, "AurelionSolQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1075);
				drawRectangle(vStart, vEnd, 110);
				return;
			}
			if (strcmp(spellName, "AurelionSolR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1500);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Azir
			if (strcmp(spellName, "AzirQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, min(740, vEnd.distTo(vStart)));
				drawCircular(vEnd, 370);
				return;
			}
			if (strcmp(spellName, "AzirR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 500);
				drawRectangle(vStart, vEnd, 250);
				return;
			}
			//Bard
			if (strcmp(spellName, "BardQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "BardR") == 0) {
				drawCircular(vEnd, 350);
				return;
			}
			//Blitzcrank
			if (strcmp(spellName, "RocketGrab") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1150);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "StaticField") == 0) {
				drawCircular(vEnd, 600);
				return;
			}
			//Brand
			if (strcmp(spellName, "BrandQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "BrandW") == 0) {
				drawCircular(vEnd, 250);
				return;
			}
			//Braum
			if (strcmp(spellName, "BraumQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "BraumR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1250);
				drawRectangle(vStart, vEnd, 115);
				return;
			}
			//Caitlyn
			if (strcmp(spellName, "CaitlynPiltoverPeacemaker") == 0 || strcmp(spellName, "CaitlynPiltoverPeacemaker2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1250);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			if (strcmp(spellName, "CaitlynEntrapment") == 0 || strcmp(spellName, "CaitlynEntrapmentMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 750);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Camille
			if (strcmp(spellName, "CamilleE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "CamilleEDash2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 400);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Cassiopeia
			if (strcmp(spellName, "CassiopeiaQ") == 0) {
				drawCircular(vEnd, 150);
				return;
			}
			if (strcmp(spellName, "CassiopeiaW") == 0) {
				drawCircular(vEnd, 160);
				return;
			}
			if (strcmp(spellName, "CassiopeiaR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 825);
				drawConic(vStart, vEnd, 85);
				return;
			}
			//Chogath
			if (strcmp(spellName, "Rupture") == 0) {
				drawCircular(vEnd, 250);
				return;
			}
			if (strcmp(spellName, "FeralScream") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 650);
				drawConic(vStart, vEnd, 60);
				return;
			}
			//Corki
			if (strcmp(spellName, "PhosphorusBomb") == 0) {
				drawCircular(vEnd, 250);
				return;
			}
			if (strcmp(spellName, "MissileBarrageMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			if (strcmp(spellName, "MissileBarrageMissile2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1500);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			//Darius
			if (strcmp(spellName, "DariusAxeGrabCone") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 535);
				drawConic(vStart, vEnd, 50);
				return;
			}
			//Diana
			if (strcmp(spellName, "DianaQ") == 0) {
				drawCircular(vEnd, 185);
				return;
			}
			//Draven
			if (strcmp(spellName, "DravenDoubleShot") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 130);
				return;
			}
			if (strcmp(spellName, "DravenRCast") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 25000);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			//DrMundo
			if (strcmp(spellName, "InfectedCleaverMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 975);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Ekko
			if (strcmp(spellName, "EkkoQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1175);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "EkkoW") == 0) {
				drawCircular(vEnd, 400);
				return;
			}
			//Elise
			if (strcmp(spellName, "EliseHumanE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1075);
				drawRectangle(vStart, vEnd, 55);
				return;
			}
			//Evelynn
			if (strcmp(spellName, "EvelynnQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "EvelynnR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 450);
				drawConic(vStart, vEnd, 180);
				return;
			}
			//Ezreal
			if (strcmp(spellName, "EzrealQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1150);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "EzrealW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1150);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "EzrealE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, min(475, vEnd.distTo(vStart)));
				drawCircular(vEnd, 20);
				return;
			}
			if (strcmp(spellName, "EzrealR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 25000);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			//Fiora
			if (strcmp(spellName, "FioraW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 750);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Fizz
			if (strcmp(spellName, "FizzR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, min(1300, vEnd.distTo(vStart)));
				drawRectangle(vStart, vEnd, 150);
				drawCircular(vEnd.setRelativeMagnitude(vStart, 1300), 150);
				return;
			}
			//Galio
			if (strcmp(spellName, "GalioQ") == 0) {
				drawCircular(vEnd, 235);
				return;
			}
			if (strcmp(spellName, "GalioE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 650);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			//Gnar
			if (strcmp(spellName, "GnarQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1125);
				drawRectangle(vStart, vEnd, 55);
				return;
			}
			if (strcmp(spellName, "GnarBigQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1125);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			if (strcmp(spellName, "GnarBigW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 575);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			if (strcmp(spellName, "GnarE") == 0) {
				drawCircular(vEnd, 160);
				return;
			}
			if (strcmp(spellName, "GnarBigE") == 0) {
				drawCircular(vEnd, 375);
				return;
			}
			if (strcmp(spellName, "GnarR") == 0) {
				drawCircular(vEnd, 475);
				return;
			}
			//Gragas
			if (strcmp(spellName, "GragasQ") == 0) {
				drawCircular(vEnd, 275);
				return;
			}
			if (strcmp(spellName, "GragasE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 600);
				drawRectangle(vStart, vEnd, 170);
				return;
			}
			if (strcmp(spellName, "GragasR") == 0) {
				drawCircular(vEnd, 400);
				return;
			}
			//Hecarim
			if (strcmp(spellName, "HecarimUlt") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1650);
				drawRectangle(vStart, vEnd, 280);
				return;
			}
			//Heimerdinger
			if (strcmp(spellName, "HeimerdingerW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1325);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			if (strcmp(spellName, "HeimerdingerE") == 0) {
				drawCircular(vEnd, 250);
				return;
			}
			if (strcmp(spellName, "HeimerdingerEUlt") == 0) {
				drawCircular(vEnd, 250);
				return;
			}
			//Illaoi
			if (strcmp(spellName, "IllaoiQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			if (strcmp(spellName, "IllaoiE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			//Irelia
			if (strcmp(spellName, "IreliaW2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 825);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			if (strcmp(spellName, "IreliaR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			//Ivern
			if (strcmp(spellName, "IvernQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1075);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Janna
			if (strcmp(spellName, "HowlingGaleSpell") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1750);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			//JarvanIV
			if (strcmp(spellName, "JarvanIVDragonStrike") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 770);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "JarvanIVDemacianStandard") == 0) {
				drawCircular(vEnd, 175);
				return;
			}
			//Jayce
			if (strcmp(spellName, "JayceShockBlast") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "JayceShockBlastWallMis") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1600);
				drawRectangle(vStart, vEnd, 115);
				return;
			}
			//Jhin
			if (strcmp(spellName, "JhinW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 2550);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			if (strcmp(spellName, "JhinE") == 0) {
				drawCircular(vEnd, 130);
				return;
			}
			if (strcmp(spellName, "JhinRShot") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 3500);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Jinx
			if (strcmp(spellName, "JinxWMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1450);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "JinxR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 25000);
				drawRectangle(vStart, vEnd, 140);
				return;
			}
			//Kaisa
			if (strcmp(spellName, "KaisaW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 3000);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			//Kalista
			if (strcmp(spellName, "KalistaMysticShot") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1150);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			//Karma
			if (strcmp(spellName, "KarmaQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "KarmaQMantra") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Karthus
			if (strcmp(spellName, "KarthusLayWasteA1") == 0) {
				drawCircular(vEnd, 175);
				return;
			}
			if (strcmp(spellName, "KarthusLayWasteA2") == 0) {
				drawCircular(vEnd, 175);
				return;
			}
			if (strcmp(spellName, "KarthusLayWasteA3") == 0) {
				drawCircular(vEnd, 175);
				return;
			}
			//Kassadin
			if (strcmp(spellName, "ForcePulse") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 600);
				drawConic(vStart, vEnd, 85);
				return;
			}
			if (strcmp(spellName, "RiftWalk") == 0) {
				drawCircular(vEnd, 250);
				return;
			}
			//Kayle
			if (strcmp(spellName, "KayleQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Kayn
			if (strcmp(spellName, "KaynQ") == 0) {
				drawCircular(vEnd, 350);
				return;
			}
			if (strcmp(spellName, "KaynW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 700);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			//Kennen
			if (strcmp(spellName, "KennenShurikenHurlMissile1") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			//Khazix
			if (strcmp(spellName, "KhazixW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "KhazixWLong") == 0) {
			}
			//Kled
			if (strcmp(spellName, "KledQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 45);
				return;
			}
			if (strcmp(spellName, "KledRiderQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 700);
				drawConic(vStart, vEnd, 30);
				return;
			}
			if (strcmp(spellName, "KledEDash") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 550);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			//KogMaw
			if (strcmp(spellName, "KogMawQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1175);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "KogMawVoidOozeMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1360);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			if (strcmp(spellName, "KogMawLivingArtillery") == 0) {
				drawCircular(vEnd, 200);
				return;
			}
			//Leblanc
			if (strcmp(spellName, "LeblancE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 55);
				return;
			}
			if (strcmp(spellName, "LeblancRE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 55);
				return;
			}
			//LeeSin
			if (strcmp(spellName, "BlindMonkQOne") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Leona
			if (strcmp(spellName, "LeonaZenithBlade") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 875);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "LeonaSolarFlare") == 0) {
				drawCircular(vEnd, 300);
				return;
			}
			//Lissandra
			if (strcmp(spellName, "LissandraQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 750);
				drawRectangle(vStart, vEnd, 75);
				return;
			}
			if (strcmp(spellName, "LissandraEMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1025);
				drawRectangle(vStart, vEnd, 125);
				return;
			}
			//Lucian
			if (strcmp(spellName, "LucianQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 65);
				return;
			}
			if (strcmp(spellName, "LucianW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Lulu
			if (strcmp(spellName, "LuluQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Lux
			if (strcmp(spellName, "LuxLightBinding") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "LuxLightStrikeKugel") == 0) {
				drawCircular(vEnd, 300);
				return;
			}
			if (strcmp(spellName, "LuxMaliceCannon") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 3340);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			if (strcmp(spellName, "LuxMaliceCannonMis") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 3340);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Malphite
			if (strcmp(spellName, "Landslide") == 0) {
				drawCircular(vEnd, 400);
				return;
			}
			if (strcmp(spellName, "UFSlash") == 0) {
				drawCircular(vEnd, 300);
				return;
			}
			//Maokai
			if (strcmp(spellName, "MaokaiQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 600);
				drawRectangle(vStart, vEnd, 110);
				return;
			}
			//MissFortune
			if (strcmp(spellName, "MissFortuneBulletTime") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1400);
				drawConic(vStart, vEnd, 40);
				return;
			}
			//Mordekaiser
			if (strcmp(spellName, "MordekaiserQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 625);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			if (strcmp(spellName, "MordekaiserE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 200);
				return;
			}
			//Morgana
			if (strcmp(spellName, "MorganaQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1250);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Nami
			if (strcmp(spellName, "NamiQ") == 0) {
				drawCircular(vEnd, 180);
				return;
			}
			if (strcmp(spellName, "NamiRMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 2750);
				drawRectangle(vStart, vEnd, 250);
				return;
			}
			//Nautilus
			if (strcmp(spellName, "NautilusAnchorDragMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1200);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			//Neeko
			if (strcmp(spellName, "NeekoQ") == 0) {
				drawCircular(vEnd, 200);
				return;
			}
			if (strcmp(spellName, "NeekoE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Nidalee
			if (strcmp(spellName, "JavelinToss") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1500);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			if (strcmp(spellName, "Bushwhack") == 0) {
				drawCircular(vEnd, 85);
				return;
			}
			if (strcmp(spellName, "Swipe") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 350);
				drawConic(vStart, vEnd, 180);
				return;
			}
			//Nocturne
			if (strcmp(spellName, "NocturneDuskbringer") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1200);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Nunu
			if (strcmp(spellName, "NunuR") == 0) {
				drawCircular(vEnd, 650);
				return;
			}
			//Olaf
			if (strcmp(spellName, "OlafAxeThrowCast") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, min(1000, vEnd.distTo(vStart)));
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			//Ornn
			if (strcmp(spellName, "OrnnQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 65);
				return;
			}
			if (strcmp(spellName, "OrnnE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 150);
				return;
			}
			if (strcmp(spellName, "OrnnRCharge") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 2500);
				drawRectangle(vStart, vEnd, 200);
				return;
			}
			//Pantheon
			if (strcmp(spellName, "PantheonQTap") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 575);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			if (strcmp(spellName, "PantheonQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1200);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "PantheonR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1350);
				drawRectangle(vStart, vEnd, 250);
				return;
			}
			//Poppy
			if (strcmp(spellName, "PoppyQSpell") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 430);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			if (strcmp(spellName, "PoppyRSpell") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1200);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			//Pyke
			if (strcmp(spellName, "PykeQMelee") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 110);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "PykeQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "PykeR") == 0) {
				drawCircular(vEnd, 100);
				return;
			}
			//Qiyana
			if (strcmp(spellName, "QiyanaQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 500);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "QiyanaQ_Grass") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "QiyanaQ_Rock") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "QiyanaQ_Water") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "QiyanaR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 190);
				return;
			}
			//Quinn
			if (strcmp(spellName, "QuinnQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1025);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Rakan
			if (strcmp(spellName, "RakanQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawRectangle(vStart, vEnd, 65);
				return;
			}
			if (strcmp(spellName, "RakanW") == 0) {
				drawCircular(vEnd, 265);
				return;
			}
			//RekSai
			if (strcmp(spellName, "RekSaiQBurrowed") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1625);
				drawRectangle(vStart, vEnd, 65);
				return;
			}
			//Renekton
			if (strcmp(spellName, "RenektonSliceAndDice") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 450);
				drawRectangle(vStart, vEnd, 65);
				return;
			}
			//Rengar
			if (strcmp(spellName, "RengarE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Riven
			if (strcmp(spellName, "RivenIzunaBlade") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawConic(vStart, vEnd, 80);
				return;
			}
			//Rumble
			if (strcmp(spellName, "RumbleGrenade") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Ryze
			if (strcmp(spellName, "RyzeQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 55);
				return;
			}
			//Samira
			if (strcmp(spellName, "SamiraQGun") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Sejuani
			if (strcmp(spellName, "SejuaniR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Senna
			if (strcmp(spellName, "SennaQCast") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1400);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			if (strcmp(spellName, "SennaW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "SennaR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 25000);
				drawRectangle(vStart, vEnd, 180);
				return;
			}
			//Shen
			if (strcmp(spellName, "ShenE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 600);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Shyvana
			if (strcmp(spellName, "ShyvanaFireball") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "ShyvanaFireballDragon2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 975);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "ShyvanaTransformLeap") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawRectangle(vStart, vEnd, 150);
				return;
			}
			//Sion
			if (strcmp(spellName, "SionQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 650);
				drawRectangle(vStart, vEnd, 200);
				return;
			}
			if (strcmp(spellName, "SionE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Sivir
			if (strcmp(spellName, "SivirQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1250);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			//Skarner
			if (strcmp(spellName, "SkarnerFractureMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Sona
			if (strcmp(spellName, "SonaR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 140);
				return;
			}
			//Soraka
			if (strcmp(spellName, "SorakaQ") == 0) {
				drawCircular(vEnd, 235);
				return;
			}
			//Swain
			if (strcmp(spellName, "SwainQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 725);
				drawConic(vStart, vEnd, 65);
				return;
			}
			if (strcmp(spellName, "SwainW") == 0) {
				drawCircular(vEnd, 300);
				return;
			}
			if (strcmp(spellName, "SwainE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawRectangle(vStart, vEnd, 85);
				return;
			}
			//Sylas
			if (strcmp(spellName, "SylasQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 775);
				drawCircular(vEnd, 180);
				return;
			}
			if (strcmp(spellName, "SylasE2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Syndra
			if (strcmp(spellName, "SyndraQSpell") == 0) {
				drawCircular(vEnd, 200);
				return;
			}
			if (strcmp(spellName, "SyndraW") == 0) {
				drawCircular(vEnd, 225);
				return;
			}
			if (strcmp(spellName, "SyndraE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 700);
				drawConic(vStart, vEnd, 45);
				return;
			}
			if (strcmp(spellName, "SyndraESphereMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			//TahmKench
			if (strcmp(spellName, "TahmKenchQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Taliyah
			if (strcmp(spellName, "TaliyahQMis") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			if (strcmp(spellName, "TaliyahWVC") == 0) {
				drawCircular(vEnd, 150);
				return;
			}
			if (strcmp(spellName, "TaliyahE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawConic(vStart, vEnd, 85);
				return;
			}
			if (strcmp(spellName, "TaliyahR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 3000);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Talon
			if (strcmp(spellName, "TalonW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 650);
				drawConic(vStart, vEnd, 30);
				return;
			}
			if (strcmp(spellName, "TalonW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 650);
				drawConic(vStart, vEnd, 30);
				return;
			}
			//Thresh
			if (strcmp(spellName, "ThreshQMissile") == 0 || strcmp(spellName, "ThreshQInternal") == 0) {
				//vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				//drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "ThreshEFlay") == 0) {
			}
			//Tristana
			if (strcmp(spellName, "TristanaW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawCircular(vEnd, 300);
				return;
			}
			//Tryndamere
			if (strcmp(spellName, "TryndamereE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 660);
				drawRectangle(vStart, vEnd, 225);
				return;
			}
			//Urgot
			if (strcmp(spellName, "UrgotQ") == 0) {
				drawCircular(vEnd, 180);
				return;
			}
			if (strcmp(spellName, "UrgotE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 475);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			if (strcmp(spellName, "UrgotR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1600);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Varus
			if (strcmp(spellName, "VarusQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1525);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "VarusE") == 0) {
				drawCircular(vEnd, 260);
				return;
			}
			if (strcmp(spellName, "VarusR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1200);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Veigar
			if (strcmp(spellName, "VeigarBalefulStrike") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "VeigarDarkMatter") == 0) {
				drawCircular(vEnd, 200);
				return;
			}
			//Velkoz
			if (strcmp(spellName, "VelkozQMissileSplit") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 45);
				return;
			}
			if (strcmp(spellName, "VelkozQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			if (strcmp(spellName, "VelkozW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 87.5);
				return;
			}
			if (strcmp(spellName, "VelkozE") == 0) {
				drawCircular(vEnd, 185);
				return;
			}
			//Vi
			if (strcmp(spellName, "ViQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 725);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			//Viktor
			if (strcmp(spellName, "ViktorGravitonField") == 0) {
				drawCircular(vEnd, 270);
				return;
			}
			if (strcmp(spellName, "ViktorDeathRayMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 700);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Warwick
			if (strcmp(spellName, "WarwickR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 3000);
				drawRectangle(vStart, vEnd, 55);
				return;
			}
			//Xayah
			if (strcmp(spellName, "XayahQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 45);
				return;
			}
			//Yone
			if (strcmp(spellName, "YoneQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 450);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			if (strcmp(spellName, "YoneQ3") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			if (strcmp(spellName, "YoneW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 600);
				drawConic(vStart, vEnd, 80);
				return;
			}
			if (strcmp(spellName, "YoneR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 180);
				return;
			}
			//Xerath
			if (strcmp(spellName, "XerathArcanopulse2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1400);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			if (strcmp(spellName, "XerathArcaneBarrage2") == 0) {
				drawCircular(vEnd, 235);
				return;
			}
			if (strcmp(spellName, "XerathMageSpear") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "XerathLocusPulse") == 0) {
				drawCircular(vEnd, 200);
				return;
			}
			//XinZhao
			if (strcmp(spellName, "XinZhaoW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			//Yasuo
			if (strcmp(spellName, "YasuoQ3Mis") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			//Zac
			if (strcmp(spellName, "ZacQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Zed
			if (strcmp(spellName, "ZedQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			//Ziggs
			if (strcmp(spellName, "ZiggsQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, min(850, vEnd.distTo(vStart)));
				drawCircular(vEnd, 75);
				return;
			}
			if (strcmp(spellName, "ZiggsW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, min(1000, vEnd.distTo(vStart)));
				drawCircular(vEnd, 325);
				return;
			}
			if (strcmp(spellName, "ZiggsE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, min(900, vEnd.distTo(vStart)));
				drawCircular(vEnd, 325);
				return;
			}
			if (strcmp(spellName, "ZiggsR") == 0) {
				drawCircular(vEnd, 480);
				return;
			}
			//Zilean
			if (strcmp(spellName, "ZileanQ") == 0) {
				drawCircular(vEnd, 150);
				return;
			}
			//Zoe
			if (strcmp(spellName, "ZoeQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			if (strcmp(spellName, "ZoeQMis2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1600);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "ZoeE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			//Zyra
			if (strcmp(spellName, "ZyraQ") == 0) {
			}
			if (strcmp(spellName, "ZyraE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "ZyraR") == 0) {
				drawCircular(vEnd, 500);
				return;
			}

			/*std::fstream ActiveSpells;
			ActiveSpells.open("E:\\Downloads\\Cheats\\Lol\\ActiveSpells.txt", std::ofstream::app);
			ActiveSpells << spellName << std::endl;
			ActiveSpells.close();*/
		}
	}
}

void UtilityFunctions::drawMissiles() {
	CObject* temp;
	for (int i = 0; i < entities.missiles.size(); i++) {
		temp = entities.missiles[i];
		if (temp != NULL &&
				entities.GetCObjectFromIndex(temp->GetMissileSourceIndex()) != NULL &&
				GH.isHero(entities.GetCObjectFromIndex(temp->GetMissileSourceIndex())) &&
				temp->GetMissileTargetIndex() == NULL
#ifndef _DEBUG
			&& entities.GetCObjectFromIndex(temp->GetMissileSourceIndex())->GetTeam() != myHero.LPObject->GetTeam()
#endif
			) {

			char* spellName = temp->GetMissileSpellInfo()->GetSpellData()->GetMissileName();
			Vector3 vStart = temp->GetMissileStartPos();
			Vector3 vPos = temp->GetPos();
			Vector3 vEnd = temp->GetMissileEndPos();
			vPos.y = vStart.y = vEnd.y;

			//Ahri
			if (strcmp(spellName, "AhriOrbMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 880);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			if (strcmp(spellName, "AhriSeduceMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 975);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Anivia
			if (strcmp(spellName, "FlashFrostSpell") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 110);
				return;
			}
			//Aphelios
			if (strcmp(spellName, "ApheliosCalibrumQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1450);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "ApheliosR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1600);
				drawRectangle(vStart, vEnd, 125);
				return;
			}
			if (strcmp(spellName, "EnchantedCrystalArrow") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 25000);
				drawRectangle(vStart, vEnd, 130);
				return;
			}
			//Bard
			if (strcmp(spellName, "BardQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "BardR") == 0) {
				drawCircular(vEnd, 350);
				return;
			}
			//Blitzcrank
			if (strcmp(spellName, "RocketGrab") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1150);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Brand
			if (strcmp(spellName, "BrandQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Braum
			if (strcmp(spellName, "BraumQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "BraumR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1250);
				drawRectangle(vStart, vEnd, 115);
				return;
			}
			//Caitlyn
			if (strcmp(spellName, "CaitlynPiltoverPeacemaker") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1250);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			if (strcmp(spellName, "CaitlynEntrapment") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 750);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Corki
			if (strcmp(spellName, "MissileBarrageMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			if (strcmp(spellName, "MissileBarrageMissile2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1500);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			//Draven
			if (strcmp(spellName, "DravenDoubleShotMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 130);
				return;
			}
			if (strcmp(spellName, "DravenR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 25000);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			//DrMundo
			if (strcmp(spellName, "InfectedCleaverMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 975);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Ekko
			if (strcmp(spellName, "EkkoQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1175);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Elise
			if (strcmp(spellName, "EliseHumanE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1075);
				drawRectangle(vStart, vEnd, 55);
				return;
			}
			//Evelynn
			if (strcmp(spellName, "EvelynnQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Ezreal
			if (strcmp(spellName, "EzrealQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1150);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "EzrealW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1150);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "EzrealR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 25000);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			//Fizz
			if (strcmp(spellName, "FizzRMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, min(1300, vEnd.distTo(vStart)));
				drawRectangle(vStart, vEnd, 150);
				drawCircular(vEnd.setRelativeMagnitude(vStart, 1300), 150);
				return;
			}
			//Gnar
			if (strcmp(spellName, "GnarQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1125);
				drawRectangle(vStart, vEnd, 55);
				return;
			}
			if (strcmp(spellName, "GnarBigQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1125);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			//Gragas
			if (strcmp(spellName, "GragasR") == 0) {
				drawCircular(vEnd, 400);
				return;
			}
			if (strcmp(spellName, "HeimerdingerE") == 0) {
				drawCircular(vEnd, 250);
				return;
			}
			if (strcmp(spellName, "HeimerdingerEUlt") == 0) {
				drawCircular(vEnd, 250);
				return;
			}
			//Irelia
			if (strcmp(spellName, "IreliaR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			//Ivern
			if (strcmp(spellName, "IvernQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1075);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Janna
			if (strcmp(spellName, "HowlingGaleSpell") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1750);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			//Jayce
			if (strcmp(spellName, "JayceShockBlast") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "JayceShockBlastWallMis") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1600);
				drawRectangle(vStart, vEnd, 115);
				return;
			}
			//Jhin
			if (strcmp(spellName, "JhinRShot") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 3500);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Jinx
			if (strcmp(spellName, "JinxWMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1450);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "JinxR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 25000);
				drawRectangle(vStart, vEnd, 140);
				return;
			}
			//Kaisa
			if (strcmp(spellName, "KaisaW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 3000);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			//Kalista
			if (strcmp(spellName, "KalistaMysticShot") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1150);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			//Karma
			if (strcmp(spellName, "KarmaQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "KarmaQMantra") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Kennen
			if (strcmp(spellName, "KennenShurikenHurlMissile1") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			//Khazix
			if (strcmp(spellName, "KhazixW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//KogMaw
			if (strcmp(spellName, "KogMawQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1175);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "KogMawVoidOozeMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1360);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			if (strcmp(spellName, "KogMawLivingArtillery") == 0) {
				drawCircular(vEnd, 200);
				return;
			}
			//LeeSin
			if (strcmp(spellName, "BlindMonkQOne") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vPos, vEnd, 60);
				return;
			}
			//Leona
			if (strcmp(spellName, "LeonaZenithBladeMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 875);
				drawRectangle(vPos, vEnd, 70);
				return;
			}
			//Lillia
			if (strcmp(spellName, "LilliaE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 700);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Lissandra
			if (strcmp(spellName, "LissandraQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 750);
				drawRectangle(vStart, vEnd, 75);
				return;
			}
			if (strcmp(spellName, "LissandraEMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1025);
				drawRectangle(vStart, vEnd, 125);
				return;
			}
			//Lucian
			if (strcmp(spellName, "LucianW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Lulu
			if (strcmp(spellName, "LuluQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Lux
			if (strcmp(spellName, "LuxLightBinding") == 0 || strcmp(spellName, "LuxLightBindingDummy") == 0 || strcmp(spellName, "LuxLightBindingMis") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "LuxLightStrikeKugel") == 0) {
				drawCircular(vEnd, 310);
				return;
			}
			if (strcmp(spellName, "LuxMaliceCannon") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 3340);
				drawRectangle(vStart, vEnd, 200);
				return;
			}
			if (strcmp(spellName, "LuxRVfxMis") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 3400);
				drawRectangle(vStart, vEnd, 200);
				return;
			}
			//Mordekaiser
			if (strcmp(spellName, "MordekaiserEMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 200);
				return;
			}
			//Morgana
			if (strcmp(spellName, "MorganaQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1250);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Nami
			if (strcmp(spellName, "NamiQ") == 0) {
				drawCircular(vEnd, 180);
				return;
			}
			if (strcmp(spellName, "NamiRMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 2750);
				drawRectangle(vStart, vEnd, 250);
				return;
			}
			//Nautilus
			if (strcmp(spellName, "NautilusAnchorDragMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1200);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			//Neeko
			if (strcmp(spellName, "NeekoE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Nidalee
			if (strcmp(spellName, "JavelinToss") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1500);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			//Nocturne
			if (strcmp(spellName, "NocturneDuskbringer") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1200);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Pantheon
			if (strcmp(spellName, "PantheonQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1200);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Pyke
			if (strcmp(spellName, "PykeQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 400);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Qiyana
			if (strcmp(spellName, "QiyanaQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 500);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "QiyanaQ_Grass") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "QiyanaQ_Rock") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "QiyanaQ_Water") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "QiyanaR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 190);
				return;
			}
			//Quinn
			if (strcmp(spellName, "QuinnQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1025);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Rakan
			if (strcmp(spellName, "RakanQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawRectangle(vStart, vEnd, 65);
				return;
			}
			//RekSai
			if (strcmp(spellName, "RekSaiQBurrowed") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1625);
				drawRectangle(vStart, vEnd, 65);
				return;
			}
			//Rengar
			if (strcmp(spellName, "RengarE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Rumble
			if (strcmp(spellName, "RumbleGrenade") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Ryze
			if (strcmp(spellName, "RyzeQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 55);
				return;
			}
			//Samira
			if (strcmp(spellName, "SamiraQGun") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Sejuani
			if (strcmp(spellName, "SejuaniR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Senna
			if (strcmp(spellName, "SennaW") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "SennaR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 25000);
				drawRectangle(vStart, vEnd, 180);
				return;
			}
			//Shyvana
			if (strcmp(spellName, "ShyvanaFireball") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 925);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "ShyvanaFireballDragon2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 975);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Sivir
			if (strcmp(spellName, "SivirQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1250);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			//Skarner
			if (strcmp(spellName, "SkarnerFractureMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Sona
			if (strcmp(spellName, "SonaR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 140);
				return;
			}
			//Swain
			if (strcmp(spellName, "SwainE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawRectangle(vStart, vEnd, 85);
				return;
			}
			//Sylas
			if (strcmp(spellName, "SylasE2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 850);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			//Syndra
			if (strcmp(spellName, "SyndraESphereMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1300);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			//Taliyah
			if (strcmp(spellName, "TaliyahQMis") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 100);
				return;
			}
			//Thresh
			if (strcmp(spellName, "ThreshQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//TwistedFate
			if (strcmp(spellName, "WildCards") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1450);
				drawRectangle(vStart, vEnd, 40);
				return;
			}
			//Twitch
			if (strcmp(spellName, "TwitchSprayAndPrayAttack") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Urgot
			if (strcmp(spellName, "UrgotR") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1600);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Varus
			if (strcmp(spellName, "VarusQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1525);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "VarusRMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1200);
				drawRectangle(vStart, vEnd, 120);
				return;
			}
			//Veigar
			if (strcmp(spellName, "VeigarBalefulStrike") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			//Velkoz
			if (strcmp(spellName, "VelkozQMissileSplit") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 45);
				return;
			}
			if (strcmp(spellName, "VelkozQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			if (strcmp(spellName, "VelkozWMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 87.5);
				return;
			}
			//Viktor
			if (strcmp(spellName, "ViktorDeathRayMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 700);
				drawRectangle(vStart, vEnd, 80);
				return;
			}
			//Xayah
			if (strcmp(spellName, "XayahQ") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 45);
				return;
			}
			//Xerath
			if (strcmp(spellName, "XerathMageSpear") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1050);
				drawRectangle(vStart, vEnd, 60);
				return;
			}
			if (strcmp(spellName, "XerathLocusPulse") == 0) {
				drawCircular(vEnd, 200);
				return;
			}
			//Yasuo
			if (strcmp(spellName, "YasuoQ3Mis") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1000);
				drawRectangle(vStart, vEnd, 90);
				return;
			}
			//Yone
			if (strcmp(spellName, "YoneQ3Missile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 950);
				drawRectangle(vStart, vEnd, 160);
				return;
			}
			//Zed
			if (strcmp(spellName, "ZedQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 900);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			//Ziggs
			if (strcmp(spellName, "ZiggsR") == 0) {
				drawCircular(vEnd, 480);
				return;
			}
			//Zoe
			if (strcmp(spellName, "ZoeQMissile") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			if (strcmp(spellName, "ZoeQMis2") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1600);
				drawRectangle(vStart, vEnd, 70);
				return;
			}
			if (strcmp(spellName, "ZoeE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 800);
				drawRectangle(vStart, vEnd, 50);
				return;
			}
			//Zyra
			if (strcmp(spellName, "ZyraE") == 0) {
				vEnd = vEnd.setRelativeMagnitude(vStart, 1100);
				drawRectangle(vStart, vEnd, 70);
				return;
			}

			/*std::fstream ActiveSpells;
			ActiveSpells.open("E:\\Downloads\\Cheats\\Lol\\Missiles.txt", std::ofstream::app);
			ActiveSpells << spellName << std::endl;
			ActiveSpells.close();*/
		}
	}
}

void UtilityFunctions::dbgStream(std::string msg) {
	std::fstream DebugStream;
	DebugStream.open("E:\\Downloads\\Cheats\\Lol\\Debug.txt", std::ofstream::app);
	DebugStream << msg << std::endl;
	DebugStream.close();
}

void UtilityFunctions::dbgStreamChrono(std::string msg) {
	std::fstream DebugStream;
	DebugStream.open("E:\\Downloads\\Cheats\\Lol\\Chrono.txt", std::ofstream::app);
	DebugStream << msg << std::endl;
	DebugStream.close();
}