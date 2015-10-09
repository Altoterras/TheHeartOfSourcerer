/***********************************************************************//**
 *	CharIdDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_CHAR_ID_DEF_H_
#define _SRCR_EPISODE_CHAR_CHAR_ID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	キャラクタ ID の定義
 *
**//*---------------------------------------------------------------------*/
class CharIdDef
{
	//======================================================================
	// 定数
public:
	static const u16	CHARID_PC1					= 1101;		// アサ
	static const u16	CHARID_PC2					= 1102;		// コルポ
	static const u16	CHARID_PC3					= 1103;		// コツメ

	static const u16	CHARID_NPC_MOTHER			= 1106;		// アサの母

	static const u16	CHARID_ENEMY_UNGETSUIWA		= 1401;		// 雲月岩
	static const u16	CHARID_ENEMY_H_SARU			= 1403;		// サルナハ
	static const u16	CHARID_ENEMY_BATTLETUTO		= 1404;		// 戦闘（対決）チュートリアルの敵
	static const u16	CHARID_ENEMY_KAGEIWA		= 1405;		// 影岩
	static const u16	CHARID_ENEMY_H_KABUTOMUSHI	= 1406;		// カブナハ
	static const u16	CHARID_ENEMY_USAGI			= 1502;		// ウサナ
	static const u16	CHARID_ENEMY_SHIKA			= 1504;		// シカナ
	static const u16	CHARID_ENEMY_KUMA			= 1505;		// コグナ
	static const u16	CHARID_ENEMY_ABU			= 1506;		// アブナ

	static const u16	CHARID_IDO_DOOR_OPENED		= 1811;		// 井戸舎の開き扉
	static const u16	CHARID_IDO_DOOR_CLOSED		= 1812;		// 井戸舎の閉じ扉
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_CHAR_ID_DEF_H_
