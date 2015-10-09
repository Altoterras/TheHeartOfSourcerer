/***********************************************************************//**
 *	EsUpvarInfo.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/31.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_UPVAR_INFO_H_
#define _ES_ES_UPVAR_INFO_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsAtom.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	上域変数情報
 *
**//*---------------------------------------------------------------------*/
class EsUpvarInfo
{
	//======================================================================
	// 定数
public:
	static const u32	DEPTH_FREE_STATIC	= 0x3fff;		// ⇒ FREE_STATIC_LEVEL
	static const u32	SLOTIDX_CALEE_UPVAR	= 0xffff;		// ⇒ CALLEE_UPVAR_SLOT

	// 無効な Upvar スロットインデックス／スキップレベル ⇒ FREE_UPVAR_COOKIE
	static const u16	SKIPDEP_FREE		= 0xffff;
	static const u16	SLOTIDX_FREE		= 0xffff;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// スキップ深さを取得する
	inline u16 getSkipDepth() const { return _skipdep; }
	// スロットインデックスを取得する
	inline u16 getSlotIndex() const { return _slotidx; }
	// スキップ深さを設定する
	inline void setSkipDepth(u16 skipdep) { _skipdep = skipdep; }
	// スロットインデックスを設定する
	inline void setSlotIndex(u16 slotidx) { _slotidx = slotidx; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	inline bool isFree() const { return (_skipdep == SKIPDEP_FREE) && (_slotidx == SLOTIDX_FREE); }
	inline void setFree() { _skipdep = SKIPDEP_FREE, _slotidx = SLOTIDX_FREE; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsUpvarInfo() : _skipdep(0), _slotidx(0) {}
	explicit EsUpvarInfo(u16 skipdep, u16 slotidx) : _skipdep(skipdep), _slotidx(slotidx) {}
	EsUpvarInfo(const EsUpvarInfo& src) { copy(&src); }
	inline EsUpvarInfo& operator=(const EsUpvarInfo& src) { copy(&src);	return *this; }
	inline void copy(const EsUpvarInfo* src) { _skipdep = src->_skipdep, _slotidx = src->_slotidx; }

	//======================================================================
	// 変数
public:
	u16 _skipdep;	// スキップ深さ
	u16 _slotidx;	// スロットインデックス
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_UPVAR_INFO_H_
