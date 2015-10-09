/***********************************************************************//**
 *	UnitArray.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/22.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "UnitArray.h"

// Friends
#include "Unit.h"

// External
#include "../../tfw/collection/List.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// UnitArray メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ユニット取得
**//*---------------------------------------------------------------------*/
const Unit* UnitArray::getUnit(int index) const
{
	if(_listUnit == 0)
	{
		return 0L;
	}
	if((index < 0) || (_listUnit->getCount() <= (u32)index))
	{
		return 0L;
	}
	return _listUnit->getObject(index);
}

/*---------------------------------------------------------------------*//**
	ユニット取得
**//*---------------------------------------------------------------------*/
Unit* UnitArray::unit(int index) const
{
	if(_listUnit == 0)
	{
		return 0L;
	}
	if((index < 0) || (_listUnit->getCount() <= (u32)index))
	{
		return 0L;
	}
	return _listUnit->getObject(index);
}

/*---------------------------------------------------------------------*//**
	ユニット数を得る
**//*---------------------------------------------------------------------*/
int UnitArray::getCount() const
{
	if(_listUnit == 0)
	{
		return 0;
	}
	return _listUnit->getCount();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	追加
**//*---------------------------------------------------------------------*/
void UnitArray::add(const UnitArray* src)
{
	if(src->_listUnit != 0L)
	{
		for(ListIterator<Unit*> ita = src->_listUnit->iterator(); ita.has(); ita.next())
		{
			// 重複チェック
			bool isDup = false;
			for(ListIterator<Unit*> itc = _listUnit->iterator(); itc.has(); itc.next())
			{
				if(itc.object() == ita.object())
				{
					isDup = true;
					break;	// 既にある
				}
			}

			if(!isDup)
			{
				_listUnit->addTail(ita.object());
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	ユニット追加
**//*---------------------------------------------------------------------*/
void UnitArray::addUnit(Unit* unit)
{
	if(unit == 0)
	{
		return;
	}
	if(_listUnit == 0)
	{
		_listUnit = new List<Unit*>();
	}
	if(_listUnit == 0)
	{
		ASSERT(_listUnit != 0L);
		return;
	}

	// 重複チェック
	for(ListIterator<Unit*> it = _listUnit->iterator(); it.has(); it.next())
	{
		if(it.object() == unit)
		{
			return;	// 既にある
		}
	}

	_listUnit->addTail(unit);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
UnitArray::UnitArray()
	: _listUnit(0L)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
UnitArray::UnitArray(Unit* unit1)
	: _listUnit(new List<Unit*>())
{
	ASSERT(_listUnit != 0L);
	if(unit1 != 0L)	{	_listUnit->addTail(unit1);	}
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
UnitArray::UnitArray(Unit* unit1, Unit* unit2)
	: _listUnit(new List<Unit*>())
{
	ASSERT(_listUnit != 0L);
	if(unit1 != 0L)	{	_listUnit->addTail(unit1);	}
	if(unit2 != 0L)	{	_listUnit->addTail(unit2);	}
}

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
**//*---------------------------------------------------------------------*/
UnitArray::UnitArray(const UnitArray& src)
	: _listUnit(0L)
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
UnitArray::~UnitArray()
{
	clear();
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void UnitArray::copy(const UnitArray* src)
{
	clear();
	if(src->_listUnit != 0L)
	{
		_listUnit = new List<Unit*>(*src->_listUnit);
	}
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void UnitArray::clear()
{
	delete _listUnit;
	_listUnit = 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
