#pragma once
#define oSpoof			0xe6ee6b

#define oObjectsManager	0x16f8678 
#define oLocalPlayer	0x2f9512c 
#define oChatClientBox	0x2f951f8
#define oGameTime		0x2f8d1dc 
#define oMinimapObj		0x2f94b0c
#define oHudInstance	0x16f86b0

#define oGetFirstCObject	0x27f470
#define oGetNextCObject		0x2809f0

//Functions
#define oIsHero			0x19c4d0
#define oIsMinion		0x19c510
#define oIsTurret		0x19c740 
#define oIsMissile		0x19c530
#define oIsAlive		0x18c350 
#define oIsTargetable	0x1c50f0 

#define oOldCastSpell		0x4ec8e0 
#define oUpdateChargeableSpell 0x240dd0
#define oGetSpellState		0x4e3110 
#define oIssueOrder			0x15f070
#define oGetAttackCastDelay	0x27bd40 
#define oGetAttackDelay		0x27be40

#define oW2S		0x960a30 

//Chat offsets
#define oChatBoxChatOpened	0x650

//MMap offsets
#define oMinimapObjectHud	0x88
#define oMinimapPosition	0x60
#define oMinimapSize		0x68

//HeroOffsets
#define oObjIndex 0x20
#define oObjTeam 0x4C
#define oObjName 0x6C
#define oObjNetworkID 0xCC //4000001D
#define oObjPos 0x1D8
#define oObjVisibility 0x270
#define oObjMana 0x298 
#define oObjMaxMana 0x2A8 
#define oObjHealth 0xd98
#define oObjMaxHealth 0xda8
#define oObjBonusAtk 0x11F0
#define oObjAP 0x1200
#define oObjBaseAtk 0x1274
#define oObjArmor 0x129C
#define oObjMagicResist 0x12A4
#define oObjMoveSpeed 0x12B4
#define oObjAtkRange 0x12BC
#define oObjBuffMgr 0x2178
#define oObjSpellBook 0x2720
#define oObjChampionName 0x314C
#define oObjLevel 0x36D4

#define oSpellBookActiveSpell 0x20 //ObjSpellBook + oObjCastingSpell //MissileObject being created
#define oActiveSpellSpellInfoPtr 0x8 //Dopo questa si va a +0x38 su spelldata
#define oActiveSpellCastingStartTime 0x10 //ObjCastingSpell]+...
#define oActiveSpellStartPos 0x80
#define oActiveSpellEndPos 0x8c
#define oActiveSpellTargetIndex 0xc0 //ObjCastingSpell]+objTarget (short, 2 byte)
//#define oActiveSpellTargetPosition 0x98 //ObjCastingSpell]+objTarget (Vector3)
#define oActiveSpellCastTime 0x4C0
#define oActiveSpellChannelTime 0x4D0
#define oActiveSpellSpellCooldown 0x4d4
#define oActiveSpellIsSpell 0x4dc //bool
#define oActiveSpellIsAA 0x4e0 //bool
#define oActiveSpellManaCost 0x4f0

#define oSpellBookSpellSlots 0x478 //localPlayer + objSpellBook + objSpellSlots + spellslotnumber
#define oSpellSlotSpellLvl 0x20 //SpellSlot + 0x20 (int)
#define oSpellSlotSpellReady 0x28 //SpellSlot + 0x28 (float) (CD = SpellReady - gameTime)
#define oSpellSlotCastingStruct 0x130
#define oSpellSlotSpellInfo 0x13C

#define oCastingStructStartIndex 0x08
#define oCastingStructTargetIndex 0x0C 
#define oCastingStructStartPos 0x10 
#define oCastingStructSpellEndPos 0x1C 
#define oCastingStructEndPos 0x28 

#define oSpellInfoSpellData 0x44 //SpellInfo + 0x38 (ptr)
#define oSpellDataMissileName 0x64 //SpellData + 0x58
#define oSpellDataSpellName 0x88 //SpellData + 0x58
#define oSpellDataSpellWidth 0x44C
#define oSpellDataSpellSpeed 0x424 //SpellData + 0x418
#define oSpellDataSpellMaxCooldown 0x25C //oObjSpellData + 0x25C + 4*SpellLvl (No cdr)

#define oSpellBookManaCost 0x58 //SpellBook + 0x58 (float) //old

#define oAIMGR_TargetPos 0x10 //Vec3
#define oAIMGR_IsMoving 0x198 //Bool
#define oAIMGR_IsDashing 0x1E8 //Bool
#define oAIMGR_NavBegin 0x1BC //[AIMGR + NavBegin] Vector3
#define oAIMGR_NavEnd 0x1C0 //([AIMGR + NavEnd] - 0xC) Vector3 //Tra NavBegin e NavEnd ci sono tutti i Vec3 del path
#define oAIMGR_CurrentPos 0x2B4 //vec3
#define oAIMGR_Velocity 0x2C0 //Vec3 (Probably delta coords from Currentpos per frame)

#define oMissileSpellInfo 0x230
#define oMissileSourceIndex 0x290
#define oMissileTargetIndex 0x2E8
#define oMissileStartPos 0x2A8
#define oMissileEndPos 0x2B4

#define oCasterName 0x0C

#define oBuffStartTime 0x0C
#define oBuffEndTime 0x10
#define oBuffType 0x4
#define oBuffName 0x8
#define oBuffFloatCount 0x130
#define oBuffIntCount 0x74
#define oBuffCountAlt 0x24
#define oBuffCountAlt2 0x20




#define oRetnAddr		0xe40939
#define oIsTroy		0x1956d0 
#define oPrintChat	0x546f30   
#define oChatClientPtr	0x22ff400  
#define oChatClientChatOpened	0x7C