#pragma once

#define oObjectsManager	0x1879830 
#define oLocalPlayer	0x31168d4 
#define oChatClientBox	0x3116f60
#define oGameTime		0x310df84 
#define oMinimapObj		0x310f288
#define oHudInstance	0x1879860

#define oGetFirstCObject	0x279b50
#define oGetNextCObject		0x27a760

#define oGetAIManager		0x127550
#define oGetBoundingRadius	0x125160

//Functions
#define oIsHero			0x13e420
#define oIsMinion		0x13e480
#define oIsTurret		0x13e630
#define oIsMissile		0x13e4a0
#define oIsTroy			0x13e7a0
#define oIsAlive		0x13f8d0

#define oGetSpellState		0x4e1780
#define oGetAttackCastDelay	0x276d60
#define oGetAttackDelay		0x276e60

#define oW2S		0xa34420

#define oIssueClickNew 0x5ed0c0 //Come viene chiamato in 0x5d9f60 (8B 44 24 04 6A 01 FF 70 40 Attenzione, questa sig è per la chiamante, non la vera funzione)
//#define oCastSpellNew 0x5cf310


//Chat offsets
#define oChatBoxChatOpened	0x73C

//MMap offsets
#define oMinimapObjectHud	0x110
#define oMinimapPosition	0x44
#define oMinimapSize		0x4C

//HeroOffsets
#define oObjIndex 0x20
#define oObjTeam 0x4C
#define oObjName 0x6C
#define oObjNetworkID 0xCC //4000001D
#define oObjPos 0x1F4
#define oObjVisibility 0x28C
#define oObjMana 0x2B4
#define oObjMaxMana 0x2C4
#define oObjTargetable	0xD1C
#define oObjHealth 0xdb4
#define oObjMaxHealth 0xdc4
#define oObjBonusAtk 0x1234
#define oObjAP 0x1244
#define oObjBaseAtk 0x12BC
#define oObjArmor 0x12E4
#define oObjMagicResist 0x12EC
#define oObjMoveSpeed 0x12FC
#define oObjAtkRange 0x1304
#define oObjBuffMgr 0x21B8 //P.S. Non va deferenziato e a 0x10 si trova l'array
#define oObjSpellBook 0x2370
#define oObjChampionName 0x2BE4
#define oObjLevel 0x33A4

#define oSpellBookActiveSpell 0x20 //ObjSpellBook + oObjCastingSpell //MissileObject being created
#define oActiveSpellSpellInfoPtr 0x8 //Dopo questa si va a +0x38 su spelldata
#define oActiveSpellCastTime 0x10 //ObjCastingSpell]+...
#define oActiveSpellStartPos 0x84
#define oActiveSpellEndPos 0x90
#define oActiveSpellTargetArray 0xc0 //Si entra nell'array e trovi la lista (di solito l'ultimo è il target)
#define oActiveSpellTargetArraySize 0xc4 //Penso sia sempre 1
#define oActiveSpellCastTime 0xCC
#define oActiveSpellChannelStartTime 0x13C //ObjCastingSpell]+...
#define oActiveSpellChannelEndTime 0x140

#define oSpellBookSpellSlots 0x488 //localPlayer + objSpellBook + objSpellSlots + spellslotnumber
#define oSpellSlotSpellLvl 0x20 //SpellSlot + 0x20 (int)
#define oSpellSlotSpellReady 0x28 //SpellSlot + 0x28 (float) (CD = SpellReady - gameTime)
#define oSpellSlotSpellInfo 0x13C

#define oSpellInfoSpellData 0x44 //SpellInfo + 0x38 (ptr)
#define oSpellDataMissileName 0x6C //SpellData + 0x58
#define oSpellDataSpellName 0x90 //SpellData + 0x58
#define oSpellDataCastTime 0x264 
#define oSpellDataSpellMaxCooldown 0x28C //oObjSpellData + oSpellDataSpellMaxCooldown + 4 * SpellLvl (No cdr)
#define oSpellDataSpellSpeed 0x460 //SpellData + 0x418
#define oSpellDataSpellWidth 0x494

#define oSpellBookManaCost 0x58 //SpellBook + 0x58 + 0x10 * SpellSlot (float)

#define oAIMGR_TargetPos 0x10 //Vec3
#define oAIMGR_IsMoving 0x1c0 //Bool
#define oAIMGR_PassedWaypoints 0x1c4 //int
#define oAIMGR_NavBegin 0x1E4 //[AIMGR + NavBegin] Vector3
#define oAIMGR_NavEnd 0x1E8 //([AIMGR + NavEnd] - 0xC) Vector3 //Tra NavBegin e NavEnd ci sono tutti i Vec3 del path
#define oAIMGR_IsDashing 0x214 //Bool
#define oAIMGR_CurrentPos 0x2E4 //vec3
#define oAIMGR_Velocity 0x2F0 //Vec3 (Probably delta coords from Currentpos per frame)

#define oMissileSpellInfo 0x278
#define oMissileSourceIndex 0x2DC
#define oMissileStartPos 0x2F4
#define oMissileEndPos 0x300
#define oMissileTargetIndex 0x330 //Puntatore al target
#define oMissileHasTarget 0x334

#define oBuffStartTime 0x0C
#define oBuffEndTime 0x10
#define oBuffType 0x4
#define oBuffName 0x8
#define oBuffFloatCount 0x130
#define oBuffIntCount 0x74
#define oBuffCountAlt 0x24
#define oBuffCountAlt2 0x20