#include "UtilityFunctions.h"
#include "globalVars.h"
#include <vector>
#include <cstdarg>
#include <fstream>
#include <iostream>
#include "offsets.h"

#define M_PI           3.14159265358979323846

std::vector<std::string> missilesExported;
std::string missileFile = "E:\\Downloads\\Cheats\\Lol\\Missiles.txt";
std::vector<std::string> activeSpellsExported;
std::string activeSpellFile = "E:\\Downloads\\Cheats\\Lol\\ActiveSpells.txt";

void UtilityFunctions::init()
{
	std::fstream f;
	std::string l;
	f.open(missileFile);
	if (f.is_open()) {
		while (std::getline(f, l)) { //read data from file object and put it into string.
			missilesExported.push_back(l);
		}
		f.close();
	}

	f.open(activeSpellFile);
	if (f.is_open()) {
		while (std::getline(f, l)) { //read data from file object and put it into string.
			activeSpellsExported.push_back(l);
		}
		f.close();
	}

	//Cerco l'offset per chatOpen
	DWORD ChatClientBox = *(DWORD*)(baseAddress + oChatClientBox);
	DWORD Temp = *(DWORD*)(baseAddress + oChatHud);
	DWORD Temp2 = 0;
	if (Temp) {
		Temp = *(DWORD*)(Temp + 0x4);
		Temp2 = *(DWORD*)Temp;
		for(int i = 0; Temp2; i++){
			Temp2 = *(DWORD*)(Temp + (i << 2));
			if (Temp2 && abs((int)(Temp2 - ChatClientBox)) < 0x1000) {
				ChatOpenAddress = Temp2 + 0x48;
				break;
			}
		}
	}
	if (!ChatOpenAddress) {
		gui.print("Can't find isChatOpen offset");
	}
}

void UtilityFunctions::Draw() {
	if (debugging)
		utils.drawDebug();
	drawMyHero();
	drawEntities();
	drawLastHittableMinions();
	drawActiveSpells();
	drawMissiles();
	//drawPredictedPos();
	drawSpellCD();
	ChampionCustomDraw();
}

void UtilityFunctions::drawMyHero() {
	if (myHero.PCObject) {
		drawer.drawCircumference(myHero.Pos, myHero.BoundingRadius, 15, 0xff00ff00, 2);
	}
}

float UtilityFunctions::calcEffectiveDamage(float damage, float armor) { //Anche per la magicRes
	return damage * (100 / (100 + armor));
}

bool UtilityFunctions::isValidTarget(EntityBase* target) {
	return target->Visible && target->Targetable && GH.isAlive(target->PCObject);
}

void UtilityFunctions::drawEntities() {
	float r;
	EntityBase* temp;
	Vector3 vTemp;

	for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
		if (isValidTarget(temp)) {
			if (temp->Team != myHero.Team) {
				r = temp->AttackRange + temp->BoundingRadius;

				drawer.drawCircumference(temp->Pos, r, 30, 0x7fff0000, 2);

				if (temp->Pos.distTo(myHero.Pos) > 800 && temp->Pos.distTo(myHero.Pos) < 2500)
					drawer.drawLine(temp->Pos, myHero.Pos, 0xffff0000, 2);

				if (temp->Index == myHero.selectedTargetIndex) {
					drawer.drawCircumference(temp->Pos, 75, 20, 0xffffff00, 4);
				}
			}
		}
	}

	for (int i = 0; i < entitiesContainer.turretsIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.turretsIndex[i]];
		if (temp->Team != myHero.Team && temp->Visible && temp->Health > 1 && temp->Pos.isDrawable()) {
			drawer.drawCircumference(temp->Pos, 850, 30, 0xffff0000, 2);
		}
	}

	drawer.drawCircumference(myHero.Pos, myHero.AttackRange + myHero.BoundingRadius, 50, 0x7f00ff00, 2);
}

void UtilityFunctions::drawLastHittableMinions() {
	float myDamage = myHero.GetBasicAttackDamage(0, 0);
	float effDamage;

	float dmgIncoming, AATimeNeeded;
	EntityBase *temp, *missile;
	for (int i = 0; i < entitiesContainer.minionsIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.minionsIndex[i]];
		if (!isValidTarget(temp)
			|| !temp->IsEnemyTo(&myHero)
			|| temp->Pos.distTo(myHero.Pos) > 1500
			|| temp->MaxHealth < 10)
			continue;

		effDamage = calcEffectiveDamage(myDamage, temp->Armor);
		
		AATimeNeeded = (temp->Pos.distTo(myHero.Pos) / myHero.AAMissileSpeed ? myHero.AAMissileSpeed : 2000) + myHero.AACastTime;

		dmgIncoming = temp->IncomingDamage(AATimeNeeded);

		if (effDamage > temp->Health) {
			drawer.drawCircumference(temp->Pos, 50, 10, 0x7f00ff00, 2);
		}
		else if (effDamage > temp->Health - dmgIncoming) {
			drawer.drawCircumference(temp->Pos, 50, 10, 0x7fffff00, 2);
		}
		else {
			drawer.drawCircumference(temp->Pos, 50, 10, 0x7fff0000, 2);
		}
	}
}

void UtilityFunctions::drawDebug() {
	Vector3 pos;
	EntityBase* temp;
	std::string str;
	for (int i = 0; i < entitiesContainer.MaxIndex; i++) {
		temp = entitiesContainer.entities[i];
		if (temp->PCObject) {
			pos = temp->Pos.ToScreen();
			pos.y += 10;

			str = utils.stringf("%p\r\n%i\r\n%s\r\n", temp->PCObject, temp->Index, temp->ObjectName.c_str());

			if (temp->BuffMgr)
				for (int j = 0; j < temp->BuffMgr->BuffCount(); j++) {
					if (!temp->BuffMgr->getBuff(j)->IsValid()) continue;
					str += temp->BuffMgr->getBuff(j)->GetBuffName() + std::string("\r\n");
				}

			drawer.drawTextSmall(pos, utils.stringf("%s", str.c_str()).c_str(), 0xffffffff);
		}
	}

	Path p = myHero.PCObject->GetPath();
	for (int i = 0; i < p.nPathPoints; i++) {
		drawer.drawCircumference(p.pathPoints[i], 30, 10, 0xffffff00, 2);

		if (i > 0)
			drawer.drawLine(p.pathPoints[i], p.pathPoints[i - 1], 0xffffffff, 2);
	}

	for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
		drawer.drawCircumference(getPredictedPos(entitiesContainer.entities[entitiesContainer.heroesIndex[i]], 0.5, 145), 30, 10, 0xffffff00, 2);
	}
}

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
	EntityBase* temp;
	int collision = 0;
	for (int i = 0; i < entitiesContainer.minionsIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.minionsIndex[i]];
		if (!isValidTarget(temp) || temp->Team == myHero.Team || start.distTo(temp->Pos) > start.distTo(end))
			continue;

		if (isCollision(start, end, width, temp->Pos, temp->BoundingRadius))
			collision++;
	}
	return collision;
}

int UtilityFunctions::heroesColliding(Vector3 start, Vector3 end, float width) {
	float length = start.distTo(end);
	int collision = 0;
	EntityBase* temp;
	for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
		if (!isValidTarget(temp) || temp->Team == myHero.Team || start.distTo(temp->Pos) > start.distTo(end))
			continue;

		if (isCollision(start, end, width, temp->Pos, temp->BoundingRadius))
			collision++;
	}
	return collision;
}

Vector3 UtilityFunctions::getPredictedPos(EntityBase* hero, float seconds, float width = 0, SkillShotType spellType) {
	int firstPoint = 0;
	float distanceTraveled = hero->MovementSpeed * seconds; //Alla distanza che percorre tolgo metà width della spell

	Path path = hero->PCObject->GetPath();
	Vector3 curPos = hero->Pos;
	Vector3 finalPoint, preFinalPoint;

	if (!seconds || !path.nPathPoints || (distanceTraveled < width / 2 && spellType == SkillShotType::Circular)) {
		return hero->Pos;
	}

	//if(UseCustomPrediction)
		//distanceTraveled -= (width / 2);

	firstPoint = hero->PCObject->getAIMgrPassedWaypoints() - 1;
	
	if (firstPoint < 0 || firstPoint >= path.nPathPoints) return curPos;

	path.pathPoints[firstPoint] = curPos;

	for (int i = firstPoint; i < path.nPathPoints - 1; i++) {
		if (distanceTraveled < path.pathPoints[i].distTo(path.pathPoints[i + 1])) {
			finalPoint = path.pathPoints[i + 1].setRelativeMagnitude(path.pathPoints[i], distanceTraveled);
			preFinalPoint = path.pathPoints[i];
			distanceTraveled = 0;
			break;
		}

		distanceTraveled -= path.pathPoints[i].distTo(path.pathPoints[i + 1]);
	}
	if (distanceTraveled > 0) {
		finalPoint = path.pathPoints[path.nPathPoints - 1];
		if (path.nPathPoints > 1)
			preFinalPoint = path.pathPoints[path.nPathPoints - 2];
		else
			preFinalPoint = finalPoint;
	}

	if (UseCustomPrediction) {
		//In questo caso tengo conto della spell width per far cadere il centro prima dell'ultimo punto

		//Metto la distanza dei punti a quella maggiore
		preFinalPoint = preFinalPoint.setRelativeMagnitude(myHero.Pos, max(preFinalPoint.distTo(myHero.Pos), finalPoint.distTo(myHero.Pos)));
		finalPoint = finalPoint.setRelativeMagnitude(myHero.Pos, max(preFinalPoint.distTo(myHero.Pos), finalPoint.distTo(myHero.Pos)));

		//Adesso tolgo width/2 nella distanza tra preFinal e final
		finalPoint = finalPoint.setRelativeMagnitude(preFinalPoint, finalPoint.distTo(preFinalPoint) > width / 2 ? preFinalPoint.distTo(finalPoint) - width / 2 : preFinalPoint.distTo(finalPoint) / 2);
	}

	return finalPoint;
}

void UtilityFunctions::ChampionCustomDraw() {
	float dmg;
	int spellLvl;
	Vector3 screenPos;
	EntityBase* temp;
	if (myHero.ObjectName == "Xerath") {
		spellLvl = myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
		dmg = (2 + spellLvl) * (150 + 50 * spellLvl + myHero.AbilityPower * 0.45);

		for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
			temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
			if (isValidTarget(temp) && temp->IsEnemyTo(&myHero) && temp->Health < calcEffectiveDamage(dmg, temp->MagicResist)) {
				GH.worldToScreen(&temp->Pos, &screenPos);
				drawer.drawTextSmall(screenPos, "!!! KILLABLE WITH ULT !!!", 0xffffff00);
			}
		}
	}
	else if (myHero.ObjectName == "Kalista") {
		for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
			temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
			dmg = championScript.getKalistaSpearDamage(temp);
			if (dmg > 0) {
				if (isValidTarget(temp) && temp->IsEnemyTo(&myHero) && temp->Health > 0) {
					drawer.drawTextSmall(temp->Pos.ToScreen(), stringf("%d%%", (int)(dmg * 100 / temp->Health)).c_str(), 0xffffffff);
				}
			}
		}
	}
}

bool UtilityFunctions::isChatOpen()
{
	if (ChatOpenAddress)
		return *(bool*)ChatOpenAddress;

	return false;
}

void UtilityFunctions::drawSpellCD() {
	EntityBase* temp;
	Vector3 screenPos;
	std::string text;

	for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
		if (temp->Index != myHero.Index && temp->Visible && GH.isAlive(temp->PCObject)) {
			GH.worldToScreen(&temp->Pos, &screenPos);
			screenPos.y -= 100;

			text = "";

			text = stringf("[%d]", (int)ceilf(temp->SpellQCD));
			screenPos.x += 25;
			drawer.drawTextMedium(screenPos, text.c_str(), !temp->SpellQCD ? 0xff00ff00 : 0xffff0000);

			text = stringf("[%d]", (int)ceilf(temp->SpellWCD));
			screenPos.x += 35;
			drawer.drawTextMedium(screenPos, text.c_str(), !temp->SpellWCD ? 0xff00ff00 : 0xffff0000);

			text = stringf("[%d]", (int)ceilf(temp->SpellECD));
			screenPos.x += 35;
			drawer.drawTextMedium(screenPos, text.c_str(), !temp->SpellECD ? 0xff00ff00 : 0xffff0000);

			text = stringf("[%d]", (int)ceilf(temp->SpellRCD));
			screenPos.x += 35;
			drawer.drawTextMedium(screenPos, text.c_str(), !temp->SpellRCD ? 0xff00ff00 : 0xffff0000);

			text = stringf("[%d]", (int)ceilf(temp->SpellDCD));
			screenPos.x += 35;
			drawer.drawTextMedium(screenPos, text.c_str(), !temp->SpellDCD ? 0xff00ffff : 0xffffff00);

			text = stringf("[%d]", (int)ceilf(temp->SpellFCD));
			screenPos.x += 35;
			drawer.drawTextMedium(screenPos, text.c_str(), !temp->SpellFCD ? 0xff00ffff : 0xffffff00);
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

void UtilityFunctions::MB(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	std::string buf = vformat(fmt, ap);
	va_end(ap);

	MessageBoxA(NULL, buf.c_str(), "DEBUG", MB_OK);
}

char* UtilityFunctions::GetString(DWORD Address) {
	if(*(int*)(Address + 0x14) > 0x10){
		return *(char**)(Address);
	}
	else {
		return (char*)Address;
	}
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
	EntityBase* temp;
	for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
		if (temp != NULL && temp->ActiveSpell && 
			temp->ActiveSpell->GetTargetIndex() == NULL
			&& (temp->Team != myHero.Team || debugging)
			) {
			char* spellName = temp->ActiveSpell->GetSpellInfo()->GetSpellData()->GetName();
			Vector3 vEnd = temp->ActiveSpell->GetEndPos();
			Vector3 vStart = temp->ActiveSpell->GetStartPos();
				
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

			if (std::find(activeSpellsExported.begin(), activeSpellsExported.end(), spellName) == activeSpellsExported.end()) {
				std::fstream ActiveSpells;
				ActiveSpells.open(activeSpellFile, std::ofstream::app);
				ActiveSpells << spellName << std::endl;
				ActiveSpells.close();

				activeSpellsExported.push_back(spellName);
			}
		}
	}
}

void UtilityFunctions::drawMissiles() {
	EntityBase* temp;
	for (int i = 0; i < entitiesContainer.missilesIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.missilesIndex[i]];
		if (temp != NULL &&
				entitiesContainer.GetEntityFromIndex(temp->SourceIndex)->Type == EntityType::Hero &&
				temp->TargetIndex == NULL
			&& (entitiesContainer.GetEntityFromIndex(temp->SourceIndex)->Team != myHero.Team || debugging)
			) {

			char* spellName = temp->SpellInfo->GetSpellData()->GetName();
			Vector3 vStart = temp->StartPos;
			Vector3 vPos = temp->Pos;
			Vector3 vEnd = temp->EndPos;
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

			if (std::find(missilesExported.begin(), missilesExported.end(), spellName) == missilesExported.end()) {
				std::fstream ActiveSpells;
				ActiveSpells.open(missileFile, std::ofstream::app);
				ActiveSpells << spellName << std::endl;
				ActiveSpells.close();

				missilesExported.push_back(spellName);
			}
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

void UtilityFunctions::dbgStreamFile(std::string fileName, std::string msg) {
	std::fstream DebugStream;
	DebugStream.open(("E:\\Downloads\\Cheats\\Lol\\" + fileName).c_str(), std::ofstream::app);
	DebugStream << msg << std::endl;
	DebugStream.close();
}