/***********************************************************************//**
 *	FocusArray.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/01.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FocusArray.h"

// Friends
#include "../common/CalcUtils.h"
#include "../unit/Unit.h"

// External
#include "../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// FocusArray メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ユニット数を得る
**//*---------------------------------------------------------------------*/
s32 FocusArray::getCount() const
{
	s32 cnt = 0;
	for(int i = 0; i < _numMax; i++)
	{
		if(_farr[i]._unit != 0L)
		{
			cnt++;
		}
	}
	return cnt;
}

/*---------------------------------------------------------------------*//**
	ユニット追加
**//*---------------------------------------------------------------------*/
void FocusArray::setUnit(s32 index, Unit* unit)
{
	if((index < 0) || (_numMax < index))
	{
		TRACE("{FocusArray::setUnit} over index: %d/%d\n", index, _numMax);
		return;
	}
	ASSERT((0 <= index) && (index < _numMax));
	_farr[index]._unit = unit;
}

/*---------------------------------------------------------------------*//**
	ユニット追加
**//*---------------------------------------------------------------------*/
void FocusArray::removeUnit(s32 index)
{
	if((index < 0) || (_numMax < index))
	{
		TRACE("{FocusArray::removeUnit} over index: %d/%d\n", index, _numMax);
		return;
	}
	ASSERT((0 <= index) && (index < _numMax));
	for(int i = index; i < _numMax; i++)
	{
		if((i + 1) < _numMax)
		{
			_farr[i].copy(&_farr[i + 1]);
		}
		else
		{
			_farr[i]._unit = 0L;
		}
	}
}

/*---------------------------------------------------------------------*//**
	スクリーン座標の無効化（未計算化）
**//*---------------------------------------------------------------------*/
void FocusArray::invalidateScreenPos(s32 index)
{
	if((index < 0) || (_numMax < index))
	{
		TRACE("{FocusArray::invalidateScreenPos} over index: %d/%d\n", index, _numMax);
		return;
	}
	ASSERT((0 <= index) && (index < _numMax));
	_farr[index]._posScr->_v[0] = F32_PMAX;
}

/*---------------------------------------------------------------------*//**
	スクリーン座標が有効か（計算済か）を得る
**//*---------------------------------------------------------------------*/
bool FocusArray::isValidScreenPos(s32 index) const
{
	if((index < 0) || (_numMax < index))
	{
		TRACE("{FocusArray::isValidScreenPos} over index: %d/%d\n", index, _numMax);
		return false;
	}
	ASSERT((0 <= index) && (index < _numMax));
	return _farr[index]._posScr->_v[0] != F32_PMAX;
}

/*---------------------------------------------------------------------*//**
	計算済みのスクリーン座標を設定する
**//*---------------------------------------------------------------------*/
void FocusArray::setCalculatedScreenPos(s32 index, const Vector2F* posScr)
{
	if((index < 0) || (_numMax < index))
	{
		TRACE("{FocusArray::setCalculatedScreenPos} over index: %d/%d\n", index, _numMax);
		return;
	}
	ASSERT((0 <= index) && (index < _numMax));
	_farr[index]._posScr->copy(posScr);
}

/*---------------------------------------------------------------------*//**
	スクリーン座標を計算する
**//*---------------------------------------------------------------------*/
void FocusArray::calcScreenPos(s32 index)
{
	if((index < 0) || (_numMax < index))
	{
		TRACE("{FocusArray::calcScreenPos} over index: %d/%d\n", index, _numMax);
		return;
	}
	///ASSERT(_farr[index]._unit != 0L);
	if(_farr[index]._unit != 0L)
	{
		CalcUtils::calc3dPosToScreenPos(_farr[index]._posScr, _farr[index]._unit->getCenterPos());
	}
}

/*---------------------------------------------------------------------*//**
	全てのフォーカスのスクリーン座標を計算する
**//*---------------------------------------------------------------------*/
void FocusArray::calcScreenPosAll()
{
	for(int i = 0; i < _numMax; i++)
	{
		if(_farr[i]._unit != 0L)
		{
			CalcUtils::calc3dPosToScreenPos(_farr[i]._posScr, _farr[i]._unit->getCenterPos());
		}
	}
}

/*---------------------------------------------------------------------*//**
	有効性の更新
**//*---------------------------------------------------------------------*/
void FocusArray::updateValidity()
{
	int i = 0;
	while(i < _numMax)
	{
		if((_farr[i]._unit != 0L) && !_farr[i]._unit->isEnable())
		{
			removeUnit(i);
			continue;
		}
		i++;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FocusArray::FocusArray(u16 numMax, bool isCalcScreenPos)
	: _farr(0L)
	, _numMax(numMax)
	, _flags(0)
{
	TFW_SET_FLAG(_flags, F_CALC_SCR_POS, isCalcScreenPos);

	_farr = new Focus[_numMax];
	for(int i = 0; i < _numMax; i++)
	{
		_farr[i].create(isCalcScreenPos);
	}
}

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
**//*---------------------------------------------------------------------*/
FocusArray::FocusArray(const FocusArray& src)
	: _farr(0L)
	, _numMax(0)
	, _flags(0)
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FocusArray::~FocusArray()
{
	clear();
	ASSERT(_farr == 0L);
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void FocusArray::copy(const FocusArray* src)
{
	recreate(src->_numMax);

	_flags = src->_flags;
	for(int i = 0; i < _numMax; i++)
	{
		_farr[i].copy(&src->_farr[i]);
	}
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void FocusArray::clear()
{
	for(int i = 0; i < _numMax; i++)
	{
		_farr[i].destroy();
	}

	delete[] _farr;
	_farr = 0L;
	_numMax = 0;
	_flags = 0;
}

/*---------------------------------------------------------------------*//**
	再作成
**//*---------------------------------------------------------------------*/
bool FocusArray::recreate(u16 numMax)
{
	clear();

	bool isCalcScreenPos = isCalculateScreenPos();
	_numMax = numMax;
	_farr = new Focus[_numMax];
	if(_farr == 0L)	{	return false;	}
	for(int i = 0; i < _numMax; i++)
	{
		_farr[i].create(isCalcScreenPos);
	}

	return true;
}

//==========================================================================
// FocusArray::FaElm メソッド

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void FocusArray::Focus::copy(const FocusArray::Focus* src)
{
	_unit = src->_unit;

	if(src->_posScr != 0L)
	{
		if(_posScr != 0L)
		{
			_posScr->copy(src->_posScr);
		}
		else
		{
			_posScr = new Vector2F(*src->_posScr);
		}
	}
}

/*---------------------------------------------------------------------*//**
	生成
**//*---------------------------------------------------------------------*/
bool FocusArray::Focus::create(bool isCalcScreenPos)
{
	_posScr = new Vector2F();

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void FocusArray::Focus::destroy()
{
	delete _posScr;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
