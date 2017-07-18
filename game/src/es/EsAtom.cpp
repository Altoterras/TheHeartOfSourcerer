/***********************************************************************//**
 *	EsAtom.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsAtom.h"

// Friends
#include "EsValue.h"

// External
#include "../tfw/hash/HashTable.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsAtom メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsAtom::EsAtom(const VcString* strRef)
	: _flags(0)
	, _atype(ATYPE_STRING)
{
	_data._str = strRef;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsAtom::EsAtom(const VcString* str, u16 flags)
	: _flags(flags)
	, _atype(ATYPE_STRING)
{
	_data._str = str;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsAtom::EsAtom(const f64* dval)
	: _flags(0)
	, _atype(ATYPE_DOUBLE)
{
	_data._dval = new F64(*dval);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsAtom::EsAtom(const EsValue* val)
	: _flags(0)
	, _atype(ATYPE_VALUE)
{
	_data._val = new EsValue(*val);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsAtom::~EsAtom()
{
	if(_atype == ATYPE_STRING)
	{
		if(TFW_IS_FLAG(_flags, F_DEL_DATA))
		{
			delete _data._str;
		}
	}
	else if(_atype == ATYPE_DOUBLE)
	{
		delete _data._dval;
	}
	else if(_atype == ATYPE_VALUE)
	{
		delete _data._val;
	}
}

/*---------------------------------------------------------------------*//**
	比較 ⇒ SEARCH_PAIRS
**//*---------------------------------------------------------------------*/
bool EsAtom::equals(const EsValue* val) const
{
	if(val->isString())
	{
		const VcString* str = val->getString();
		return isSameString(str);
	}
	else if(val->isDouble())
	{
		if(_atype != ATYPE_DOUBLE)
		{
			return false;
		}
		f64 fval = val->getDouble();
		return getDouble()->v() == fval;
	}
	else
	{
		if(_atype != EsAtom::ATYPE_VALUE)
		{
			return false;
		}
		const EsValue* valT = getValue();
		return valT->equals(val);
	}
}

/*---------------------------------------------------------------------*//**
	同じ文字列かどうかを判定する
**//*---------------------------------------------------------------------*/
bool EsAtom::isSameString(const VcString* str) const
{
	if(_atype != ATYPE_STRING)	{	return false;	}
	return str->equals(_data._str);
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
