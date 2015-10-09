/***********************************************************************//**
 *	Item.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Item.h"
#include "ItemDefTbl.h"

// Friends

// External
#include "../../tfw/serialize/Serializer.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Item メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	未設定かどうかを調べる
**//*---------------------------------------------------------------------*/
bool Item::isEmpty() const
{
	return _itmdf == 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Item::Item()
{
	clear();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Item::~Item()
{
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void Item::clear()
{
	_itmgid = 0;
	_flags = 0;
	_eestat.clear();
	_itmdf = 0L;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void Item::copy(const Item* src)
{
	_itmgid = src->_itmgid;
	_flags = src->_flags;
	_eestat.copy(&src->_eestat);
	_itmdf = src->_itmdf;
}

/*---------------------------------------------------------------------*//**
	セットアップ
**//*---------------------------------------------------------------------*/
void Item::setup(u16 itmgid, const ItemDef* itmdf, u16 flags)
{
	_itmgid = itmgid;
	_flags = flags;
	_itmdf = itmdf;

	reflectParameterFromDefData(false);
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool Item::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!storeToStructure(&svst)) { return false; }
		if(!ser->store(&svst, sizeof(SaveStructure))) { return false; }
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	デシリアイズ
**//*---------------------------------------------------------------------*/
bool Item::deserialize(Serializer* ser, const ItemDefTbl* itmdftbl)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!ser->restore(&svst, sizeof(SaveStructure))) { return false; }
		return restoreFromStructure(&svst, itmdftbl);
	}
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャに保存する
**//*---------------------------------------------------------------------*/
bool Item::storeToStructure(SaveStructure* svst) const
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	svst->_itmgid = _itmgid;
	if(_itmgid > 0)
	{
		svst->_itmdid = _itmdf->getItemDefId();
		svst->_flags = _flags;
		_eestat.storeToStructure(&svst->_eestat);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool Item::restoreFromStructure(const SaveStructure* svst, const ItemDefTbl* itmdftbl)
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	if(svst->_itmgid > 0)
	{
		_itmgid = svst->_itmgid;
		_itmdf = itmdftbl->getDef(svst->_itmdid); ASSERT(_itmdf != 0L);
		reflectParameterFromDefData(false);	// 基本パラメーター読み込み
		_flags = svst->_flags;
		_eestat.restoreFromStructure(&svst->_eestat);
	}
	else
	{
		clear();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	定義データからパラメータを反映させる
**//*---------------------------------------------------------------------*/
void Item::reflectParameterFromDefData(bool isKeepVeil)
{
	if(_itmgid <= 0)	{	return;	}
	if(_itmdf == 0L)	{	return;	}

	_eestat.setup(_itmdf->getOqlElene(), _itmdf->getOqlElene(), isKeepVeil);
	_eestat.updateResultValue(true);
}


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
