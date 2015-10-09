/***********************************************************************//**
 *	FixedSourceDefTbl.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/24.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOURCE_FIXED_SOURCE_DEF_TBL_H_
#define _SRCR_SOURCE_FIXED_SOURCE_DEF_TBL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "FixedSourceDef.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	固定ソース定義テーブル
 *		※　～DefTbl 系の実装手法については、CharDefTbl のコメント参照
 *	
**//*---------------------------------------------------------------------*/
class FixedSourceDefTbl
{
	//======================================================================
	// 定数
private:
	static const s32	NUM_FIXEDSRCDEF_MAX	= 64;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 固定ソース定義を得る
	const FixedSourceDef* getDef(u16 fsrcdid) const;
	// 固定ソース定義をインデックスから得る
	const FixedSourceDef* getDefFromIndex(s32 index) const { return &_fsrcdefarr[index]; }
	// 固定ソース定義の最大数を得る
	s32 getMaxNum() const { return NUM_FIXEDSRCDEF_MAX; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FixedSourceDefTbl();
	~FixedSourceDefTbl();
	bool init(u32 fileidData, u32 fileidStr);
	void cleanup();

	//======================================================================
	// 変数
private:
	FixedSourceDef _fsrcdefarr[NUM_FIXEDSRCDEF_MAX];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOURCE_FIXED_SOURCE_DEF_TBL_H_
