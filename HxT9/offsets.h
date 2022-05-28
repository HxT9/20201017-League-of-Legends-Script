﻿#define oObjectsManager 0x24b9b48
#define oLocalPlayer 0x31085ec
#define oChatClientBox 0x3108ec8
#define oChatHud 0x31085d0
#define oGameTime 0x3101d6c
#define oMinimapObj 0x3101dac
#define oHudInstance 0x186abb0

#define oGetAIManager 0x13f180
#define oGetBoundingRadius 0x13b5a0
#define oIsType 0x146de0
#define oIsHero 0x170960
#define oIsMinion 0x1709a0
#define oIsTurret 0x170a90
#define oIsMissile 0x1709c0
#define oIsTroy 0x170be0
#define oIsAlive 0x1636f0
#define oGetSpellState 0x4ea930
#define oGetAttackCastDelay 0x27d290
#define oGetAttackDelay 0x27d390
#define oW2S 0xa550f0
#define oIssueClickNew 0x601c60
#define oOnProcessSpell 0x501bd0

#define oChatHudChatOpened 0x48
#define oMinimapObjectHud 0x120
#define oMinimapPosition 0x44
#define oMinimapSize 0x4c

#define oObjIndex 0x8
#define oObjTeam 0x34
#define oObjName 0x54
#define oObjNetworkID 0xb4
#define oObjPos 0x1dc
#define oObjVisibility 0x274
#define oObjMana 0x29c
#define oObjMaxManaoObjMana 0x2ac
#define oObjSar 0x30c
#define oObjMaxSar 0x31c
#define oObjSarEnabled 0x32c
#define oObjTargetable 0xd04
#define oObjHP 0xd9c
#define oObjMaxHP 0xdac
#define oObjHPMaxPenalty 0xdbc
#define oObjAllShield 0xddc
#define oObjPhysicalShield 0xdec
#define oObjMagicalShield 0xdfc
#define oObjStats 0x1090
#define oObjBonusAtk 0x11e4
#define oObjAP 0x11f4
#define oObjBaseAtk 0x126c
#define oObjArmor 0x1294
#define oObjMagicResist 0x129c
#define oObjMoveSpeed 0x12ac
#define oObjAtkRange 0x12b4
#define oObjBuffMgr 0x2098
#define oObjSpellBook 0x2250
#define oObjObjectName 0x2ca8
#define oObjLevel 0x329c

#define oSpellBookActiveSpell 0x20
#define oActiveSpellSpellInfoPtr 0x8
#define oActiveSpellEndCastTime 0x10
#define oActiveSpellStartPos 0x84
#define oActiveSpellEndPos 0x84
#define oActiveSpellTargetArray 0xc0
#define oActiveSpellTargetArraySize 0xc4
#define oActiveSpellCastTime 0xcc
#define oActiveSpellChannelStartTime 0x13c
#define oActiveSpellChannelEndTime 0x140

#define oSpellBookSpellSlots 0x488
#define oSpellSlotSpellLvl 0x1c
#define oSpellSlotSpellReady 0x24
#define oSpellSlotSpellInfo 0x120

#define oSpellInfoSpellData 0x40
#define oSpellDataName 0x6c
#define oSpellDataSpellSpeed 0x460

#define oAIMGR_TargetPos 0x10
#define oAIMGR_IsMoving 0x1c0
#define oAIMGR_PassedWaypoints 0x1c4
#define oAIMGR_NavBegin 0x1e4
#define oAIMGR_NavEnd 0x1e8
#define oAIMGR_IsDashing 0x214
#define oAIMGR_CurrentPos 0x2ec
#define oAIMGR_Velocity 0x2f8

#define oMissileSpellInfo 0x260
#define oMissileSourceIndex 0x2c4
#define oMissileStartPos 0x2dc
#define oMissileEndPos oMissileStartPos + 0xc
#define oMissileTargetIndex 0x330
#define oMissileHasTarget 0x334

#define oBuffType 0x4
#define oBuffName 0x8
#define oBuffNameOffset 0x4
#define oBuffStartTime 0x10
#define oBuffEndTime 0x14
#define oBuffCountAlt 0x24
#define oBuffIntCount 0x74
#define oBuffFloatCount 0x130