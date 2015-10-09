/***********************************************************************//**
 *	CharDefTbl.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CHAR_CHAR_DEF_TBL_H_
#define _SRCR_CHAR_CHAR_DEF_TBL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "CharDef.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	キャラクター定義テーブル
 *		※　ちなみに ～DefTbl 系はソース中に複数あり、
 *			C++ のセオリーからすると抽象化・継承化すべきのように思われるが、
 *			仮想関数テーブルによるサイズ増量、デバッグの容易性、
 *			固定数配列の扱い、特殊形式のデータ取り扱い…
 *			などにおいて、あまり優位性を見出せなかったので
 *			それぞれの実装になっている．
 *			そもそも、これぐらい単純なクラスだと、抽象化というよりは、
 *			デザインパターンのようなものだ（かテンプレート？）、とする．
 *	
**//*---------------------------------------------------------------------*/
class CharDefTbl
{
	//======================================================================
	// 定数
private:
	static const s32	NUM_CHARDEF_MAX	= 256;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	const CharDef* getDef(u16 chrdid) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CharDefTbl();
	~CharDefTbl();
	bool init(u32 fileidData, u32 fileidStr);
	void cleanup();

	//======================================================================
	// 変数
private:
	CharDef _chrdefarr[NUM_CHARDEF_MAX];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CHAR_CHAR_DEF_TBL_H_
