/***********************************************************************//**
 *	CharLogicDataTbl.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/28.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_DATA_NPC_LOGIC_DATA_TBL_H_
#define _SRCR_EPISODE_DATA_NPC_LOGIC_DATA_TBL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "CharLogicData.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	NPC ロジックデータ テーブル
 *
**//*---------------------------------------------------------------------*/
class CharLogicDataTbl
{
	//======================================================================
	// 定数
private:
	static const s32	NUM_CHAR_MAX	= 128;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 固定パラメータを得る
	const CharLogicData* getData(u16 chrdid) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CharLogicDataTbl();
	bool init(u32 fileidData);
	void cleanup();

	//======================================================================
	// 変数
private:
	CharLogicData _cldat[NUM_CHAR_MAX];
	s32 _numData;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_DATA_NPC_LOGIC_DATA_TBL_H_
