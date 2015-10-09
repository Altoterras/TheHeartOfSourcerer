/***********************************************************************//**
 *	CharUnitSaveStructure.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_CHAR_CHAR_UNIT_SAVE_STRUCTURE_H_
#define _SRCR_UNIT_CHAR_CHAR_UNIT_SAVE_STRUCTURE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../UnitSaveStructure.h"
#include "../../status/CharStat.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	キャラユニットのセーブストラクチャ
 *
**//*---------------------------------------------------------------------*/
class CharUnitSaveStructure
{
	//======================================================================
	// 定数
public:
	static const s32 SIZE_THIS = 176;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CharUnitSaveStructure()	{ TFW_ZERO_MEMORY(this, sizeof(CharUnitSaveStructure)); }

	//======================================================================
	// 変数
public:
	// [  0]
	u16 _ctrlevid;						// [  2] 制御イベント ID
	u8 __pad_0010[ 2];					// [  2]
	UnitSaveStructure _usvst;			// [ 16] ユニットのセーブストラクチャ
	CharStat::SaveStructure _cssvst;	// [144] キャラステータスのセーブストラクチャ
	Vector3F _dir;						// [ 12] 向き
	// [176]
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_CHAR_CHAR_UNIT_SAVE_STRUCTURE_H_
