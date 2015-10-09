/***********************************************************************//**
 *	ItemDefTbl.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/11.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_ITEM_ITEM_DEF_TBL_H_
#define _SRCR_ITEM_ITEM_DEF_TBL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "ItemDef.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	アイテム定義テーブル
 *	
**//*---------------------------------------------------------------------*/
class ItemDefTbl
{
	//======================================================================
	// 定数
private:
	static const s32	NUM_ITEMDEF_MAX	= 256;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// アイテム定義の最大数を得る
	inline s32 getMaxNum() const { return NUM_ITEMDEF_MAX; }
	// アイテム定義を得る
	const ItemDef* getDef(u16 itmdid) const;
	// インデックスよりアイテム定義を得る
	const ItemDef* getDefFromIndex(s32 index) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ItemDefTbl();
	~ItemDefTbl();
	bool init(u32 fileidData, u32 fileidStr);
	void cleanup();

	//======================================================================
	// 変数
private:
	ItemDef _itmdefarr[NUM_ITEMDEF_MAX];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_ITEM_ITEM_DEF_TBL_H_
