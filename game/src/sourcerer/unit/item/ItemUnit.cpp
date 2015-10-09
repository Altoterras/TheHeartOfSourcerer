/***********************************************************************//**
 *	ItemUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ItemUnit.h"

// Friends
#include "../../episode/data/FixedData.h"
#include "../../item/ItemDefTbl.h"
#include "../../body/GameRenderCtx.h"
#include "../../status/CharStat.h"
#include "../../status/EleneStat.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ItemUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	代表名を得る（無い場合は NULL が返る）
**//*---------------------------------------------------------------------*/
const VcString* ItemUnit::getMainName() const
{
	if(_itmdefRef != 0L)
	{
		return _itmdefRef->getName(Env_getLangId());
	}

	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ItemUnit::ItemUnit(UnitType unittype)
	: Unit(unittype)
	, _cstat(0L)
	, _eestatRef(0L)
	, _itmdefRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ItemUnit::~ItemUnit()
{
	ASSERT(_cstat == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ItemUnit::create(u16 itmdid, EleneStat* eestatRef, u16 unitflags)
{
	// アイテム定義を自ら得る
	_itmdefRef = FixedData::getItemDefTable()->getDef(itmdid);

	// 影響反映用キャラクタステータスを作成する
	_eestatRef = eestatRef;
	_cstat = new CharStat();
	_cstat->setup(CharStat::CF_ITEM | CharStat::CF_IGNORE_ATTACK, false);
	_cstat->bind(this, _eestatRef, 0L);

	// 親クラスによる作成
	if(!Unit::create(0L, 0L, UNITFLAG_ITEM | unitflags))
	{
		return false;
	}

	// 仕様やパラメータ変更時の対応するための、
	// データパラメータ反映とベールの限界値クリップ
	reflectParameterFromDefData(true);
	_cstat->clipVeilLimitElene(EleneStat::VEIL_WP);
	_cstat->clipVeilLimitElene(EleneStat::VEIL_BR);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ItemUnit::destroy()
{
	// 影響反映用キャラクタステータスを削除する
	if(_cstat != 0L)
	{
		delete _cstat;
		_cstat = 0L;
	}

	_eestatRef = 0L;
	_itmdefRef = 0L;

	Unit::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ItemUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	if(isEnable())
	{
		// キャラクタステータス更新
		if(_cstat != 0L)
		{
			_cstat->exec(ec);
		}
	}
}

/*---------------------------------------------------------------------*//**
	定義データからパラメータを反映させる
**//*---------------------------------------------------------------------*/
void ItemUnit::reflectParameterFromDefData(bool isKeepVeil)
{
	if(_itmdefRef == 0L)	{	return;	}

	_eestatRef->setup(_itmdefRef->getOqlElene(), _itmdefRef->getOqlElene(), isKeepVeil);
	_eestatRef->updateResultValue(true);
}


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
