/***********************************************************************//**
 *	ItemUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_ITEM_ITEM_UNIT_H_
#define _SRCR_UNIT_ITEM_ITEM_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Unit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class CharStat;
class ItemDef;

/*---------------------------------------------------------------------*//**
 *	アイテム ユニット
 *
**//*---------------------------------------------------------------------*/
class ItemUnit : public Unit
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 影響反映用キャラクタステータスを得る
	inline const CharStat* getCharStat() const { return _cstat; }
	inline CharStat* charStat() { return _cstat; }
	// 元素エネルギーを得る
	const EleneStat* getEleneStat() const { return _eestatRef; }
	EleneStat* eleneStat() { return _eestatRef; }
	// アイテム定義を得る（無い場合は NULL が返る）
	inline const ItemDef* getItemDef() const { return _itmdefRef; }
	// 代表名を得る（無い場合は NULL が返る）
	const VcString* getMainName() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ItemUnit(UnitType unittype);
	~ItemUnit();
	bool create(u16 itmdid, EleneStat* eestatRef, u16 unitflags);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void reflectParameterFromDefData(bool isKeepVeil);

protected:

	//======================================================================
	// メンバ変数
protected:
	CharStat*			_cstat;					// 影響反映用キャラクタステータス
	EleneStat*			_eestatRef;				// エネルギー状態
	const ItemDef*		_itmdefRef;				// アイテム定義
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_ITEM_ITEM_UNIT_H_
