#pragma once

#define oObjectsManager	0x24A4B90			//8B 0D ? ? ? ? 56 E8 ? ? ? ? 85 C0 74
#define oLocalPlayer	0x30F5BBC			//8B 3D ? ? ? ? 3B F7 75
#define oChatClientBox	0x30f5c94			//8B 0D ? ? ? ? 85 C9 74 0D 8B 41
#define oGameTime		0x30ECC8C			//F3 0F 5C 0D ? ? ? ? 0F 2F C1 F3
#define oMinimapObj		0x30ecccc			//8B 0D ? ? ? ? 66 0F 6E C6
#define oHudInstance	0x24a4bc0			//8B 0D ? ? ? ? FF 70 FC 8B 49 0C E8 ? ? ? ? C6

#define oGetAIManager		0x12eec0		//E8 ? ? ? ? 6A 00 6A 01 FF 74 24 14
#define oGetBoundingRadius	0x12c960		//FF 90 ? ? ? ? 8B C8 E8 ? ? ? ? D9 54 24 08 D9 5C 24 (mid function)

//Functions
#define oIsHero			0x147830			//E8 ? ? ? ? 83 C4 04 84 C0 74 61 85
#define oIsMinion		0x147890			//E8 ? ? ? ? 83 C4 04 B9 ? ? ? ? 84 C0 0F 44 CB
#define oIsTurret		0x147a40			//E8 ? ? ? ? 83 C4 04 84 C0 74 09 5F
#define oIsMissile		0x1478b0			//E8 ? ? ? ? 83 C4 04 84 C0 74 3F
#define oIsTroy			0x147bb0			//E8 ? ? ? ? 8B D8 83 C4 04 85 DB 74 63 8B 0B //E8 ? ? ? ? 8B E8 83 C4 04 85 ED 0F 84 ? ? ? ? 6A 08
#define oIsAlive		0x141050			//56 8B F1 8B 06 8B 80 ? ? ? ? FF D0 84 C0 74 19

#define oGetSpellState		0x4f0010		//83 EC 14 8B 44 24 1C 55
#define oGetAttackCastDelay	0x27b850		//83 EC 0C 53 8B 5C 24 14 8B CB 56 57 8B 03 FF 90
#define oGetAttackDelay		0x27b950		//8B 44 24 04 51 F3

#define oW2S		0xa41410				//83 EC 10 56 E8 ? ? ? ? 8B 08

#define oIssueClickNew	0x5fc850 //Come viene chiamato in 0x5d9f60 (8B 44 24 04 6A 01 FF 70 40 Attenzione, questa sig è per la chiamante, non la vera funzione)
#define oCastSpellNew	0x5e5050 //53 8B 1D ?? ?? ?? ?? 55 56 57 8B F9 85

//Chat offsets
#define oChatBoxChatOpened	0x754			

//MMap offsets
#define oMinimapObjectHud	0x110
#define oMinimapPosition	0x44
#define oMinimapSize		0x4C

//HeroOffsets
#define oObjIndex 0x20						//39 48 20 0F 84 ? ? ? ? 8B CF C6 45 11 00 E8 ? ? ? ?
#define oObjTeam 0x4C						//3B 53 4C 0F 84 ? ? ? ? 3B F2 75 3D F3 0F 11 02 F3 0F 10 44 24 ? F3 0F 11 42 ? 
#define oObjName 0x6C						//8D 41 6C C3
#define oObjNetworkID 0xCC //4000001D		//8D BB ? ? ? ? 83 EF 18 8B CF E8 ? ? ? ? 83 EE 01 75 F1 8D 4B 0C
#define oObjPos 0x1F4						//8D 8F ? ? ? ? E8 ? ? ? ? 8B 83 ? ? ? ? 05 ? ? ? ? 89 44 24 20 83 C0 0C 89 44 24 4C 83 38 00 75 20 E8 ? ? ? ? 8B 4C 24 20
#define oObjVisibility 0x28C				//8D 8F ? ? ? ? E8 ? ? ? ? 8D 4C 24 34 51 8B C8 8B 10 FF 92 ? ? ? ? 6A 00 8B CF
#define oObjMana 0x2B4						//8D B3 ? ? ? ? F3 0F 11 83 ? ? ? ? BF ? ? ? ? C6 83 ? ? ? ? ? 8B CE E8 ? ? ? ? 83 C6 70 83 EF 01 75 F1 0F 31
#define oObjMaxMana 0x2C4
#define oObjTargetable	0xD1C				//8D B0 ? ? ? ? 56 FF 74 24 20 E8 ? ? ? ? 8A 5C 24 1C 8B 7C 24 20 8B 4C 24 18 "mIsTargetable" + 0x9
#define oObjHealth 0xDB4					//8D 8B ? ? ? ? C6 83 ? ? ? ? ? 50 8D 83 ? ? ? ? C7 83 ? ? ? ? ? ? ? ? 50
#define oObjMaxHealth 0xDC4
#define oObjBonusAtk 0x11FC					//89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86 ? ? ? ? F3 0F 10 86 ? ? ? ? F3 0F 11 44 24 ? 8B 44 24 18
#define oObjAP 0x120C						//89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86 ? ? ? ? 8B 86 ? ? ? ? 89 86 ? ? ? ? F3 0F 10 86 ? ? ? ? F3 0F 11 44 24 ? 8B 44 24 18
#define oObjBaseAtk 0x1284					//8B 81 ? ? ? ? 81 C1 ? ? ? ? 89 47 04
#define oObjArmor 0x12AC					//89 8E ? ? ? ? 8B 88 ? ? ? ? 89 8E ? ? ? ? 8B CE FF 92 ? ? ? ?
#define oObjMagicResist 0x12B4				//8B 81 ? ? ? ? 89 42 0C 8B 81 ? ? ? ? 89 42 08 F3 0F 58 86 ? ? ? ? 5F 5E F3 0F 11 42 ? 59 C2 04 00
#define oObjMoveSpeed 0x12C4				//89 8E ? ? ? ? 8B 88 ? ? ? ? 89 8E ? ? ? ? 8B 88 ? ? ? ? 89 8E ? ? ? ? 8B CE FF 92 ? ? ? ?
#define oObjAtkRange 0x12CC					//D9 83 ? ? ? ? 5F 5E 5B 83 C4 08 C3			
#define oObjBuffMgr 0x2180					//???? //C7 85 ? ? ? ? ? ? ? ? 8D 45 20 //8D 8B ? ? ? ? FF 30 FF 73 20 //0x14ca80 //P.S. Non va deferenziato e a 0x10 si trova l'array
#define oObjSpellBook 0x2338				//8B 84 83 ? ? ? ? EB 06 8B 83 ? ? ? ? 85 C0 0F 84 ? ? ? ? 53 8B CF E8 ? ? ? ? 8B C8 8B 10 FF 52 18 8B F0
#define oObjObjectName 0x2BAC
#define oObjLevel 0x33A0

#define oSpellBookActiveSpell 0x20 //ObjSpellBook + oObjCastingSpell //MissileObject being created
#define oActiveSpellSpellInfoPtr 0x8 //Dopo questa si va a +0x38 su spelldata
#define oActiveSpellEndCastTime 0x10 //ObjCastingSpell]+...
#define oActiveSpellStartPos 0x84
#define oActiveSpellEndPos 0x90
#define oActiveSpellTargetArray 0xc0 //Si entra nell'array e trovi la lista (di solito l'ultimo è il target)
#define oActiveSpellTargetArraySize 0xc4 //Penso sia sempre 1
#define oActiveSpellCastTime 0xCC
#define oActiveSpellChannelStartTime 0x13C //ObjCastingSpell]+...
#define oActiveSpellChannelEndTime 0x140

#define oSpellBookSpellSlots 0x488 //localPlayer + objSpellBook + objSpellSlots + spellslotnumber
#define oSpellSlotSpellLvl 0x1C //SpellSlot + 0x20 (int)
#define oSpellSlotSpellReady 0x24 //SpellSlot + 0x28 (float) (CD = SpellReady - gameTime)
#define oSpellSlotSpellInfo 0x120 //E8 ? ? ? ? C3 8B 41 24 + 0x24

#define oSpellInfoSpellData 0x44
#define oSpellDataMissileName 0x6C
#define oSpellDataSpellName 0x90
#define oSpellDataCastTime 0x264 
#define oSpellDataSpellMaxCooldown 0x288 //oObjSpellData + oSpellDataSpellMaxCooldown + 4 * SpellLvl (No cdr)
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
#define oBuffNameOffset 0x4
#define oBuffFloatCount 0x130
#define oBuffIntCount 0x74
#define oBuffCountAlt 0x24