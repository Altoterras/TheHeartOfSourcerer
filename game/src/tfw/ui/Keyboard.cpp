/***********************************************************************//**
 *	Keyboard.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2010/05/25.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Keyboard.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Keyboard メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	継続押下取得
**//*---------------------------------------------------------------------*/
bool Keyboard::isOn(int key) const
{
	ASSERT(key < NUM_KEY);
	return TFW_IS_FLAG(_ki[key]._swflags, SWF_ON_CUR);
}

/*---------------------------------------------------------------------*//**
	トリガ押下取得
**//*---------------------------------------------------------------------*/
bool Keyboard::isTrigger(int key) const
{
	ASSERT(key < NUM_KEY);
	return TFW_IS_FLAG(_ki[key]._swflags, SWF_TRG_CUR);
}

/*---------------------------------------------------------------------*//**
	トリガ押上取得
**//*---------------------------------------------------------------------*/
bool Keyboard::isRelease(int key) const
{
	ASSERT(key < NUM_KEY);
	return TFW_IS_FLAG(_ki[key]._swflags, SWF_TRG_REL_CUR);
}

/*---------------------------------------------------------------------*//**
	トリガ押下取得
**//*---------------------------------------------------------------------*/
bool Keyboard::isRepeatTrigger(int key) const
{
	ASSERT(key < NUM_KEY);
	return (_ki[key]._repcnt == 0) || TFW_IS_FLAG(_ki[key]._swflags, SWF_TRG_CUR);
}

/*---------------------------------------------------------------------*//**
	いずれかのボタンの継続押下取得
**//*---------------------------------------------------------------------*/
bool Keyboard::isOnAny() const
{
	return _swOnAny;
}

/*---------------------------------------------------------------------*//**
	継続押下設定
**//*---------------------------------------------------------------------*/
void Keyboard::setOn(int key, bool is)
{
	if((key < 0) || (NUM_KEY <= key))	{	return;	}
	ASSERT(key < NUM_KEY);
	TFW_SET_FLAG(_ki[key]._swflags, SWF_ON_DIRECT, is);
}

/*---------------------------------------------------------------------*//**
	１フレームのみの押下設定
**//*---------------------------------------------------------------------*/
void Keyboard::setOnOnlyOneFrame(int key)
{
	if((key < 0) || (NUM_KEY <= key))	{	return;	}
	TFW_SET_FLAG(_ki[key]._swflags, SWF_ON_DIRECT, true);
	TFW_SET_FLAG(_ki[key]._cflags, CF_ON_ONE_SHOT, true);
}

/*---------------------------------------------------------------------*//**
	リピートカウントの設定
**//*---------------------------------------------------------------------*/
void Keyboard::setRepeatCount(short cntFirst, short cntAfter)
{
	_cntRepFirst = cntFirst;
	_cntRepAfter = cntAfter;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Keyboard::Keyboard()
	: _swOnAny(false)
	, _cntRepFirst(REPCNT_FIRST_DEFAULT)
	, _cntRepAfter(REPCNT_AFTER_DEFAULT)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Keyboard::~Keyboard()
{
}

/*---------------------------------------------------------------------*//**
	構築
**//*---------------------------------------------------------------------*/
bool Keyboard::create()
{
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Keyboard::destroy()
{
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void Keyboard::exec()
{
	// フラグ更新
	_swOnAny = false;
	for(int i = 0; i < NUM_KEY; i++)
	{
		KeyInfo* ki = &_ki[i];
		
		bool isOnPrev = TFW_IS_FLAG(ki->_swflags, SWF_ON_CUR);
		TFW_SET_FLAG(ki->_swflags, SWF_ON_PREV, isOnPrev);
		bool isOnCur = TFW_IS_FLAG(ki->_swflags, SWF_ON_DIRECT);
		TFW_SET_FLAG(ki->_swflags, SWF_ON_CUR, isOnCur);
		TFW_SET_FLAG(ki->_swflags, SWF_TRG_CUR, !isOnPrev && isOnCur);
		TFW_SET_FLAG(ki->_swflags, SWF_TRG_REL_CUR, isOnPrev && !isOnCur);

		if(isOnCur)
		{
			_swOnAny = true;

			if(TFW_IS_FLAG(ki->_cflags, CF_ON_ONE_SHOT))
			{
				TFW_SET_FLAG(ki->_swflags, SWF_ON_DIRECT, false);
				TFW_SET_FLAG(ki->_cflags, CF_ON_ONE_SHOT, false);
			}

			if(ki->_repcnt == -1)		{	ki->_repcnt = _cntRepFirst;	}
			else if(ki->_repcnt == 0)	{	ki->_repcnt = _cntRepAfter;	}
			else						{	ki->_repcnt--;				}
		}
		else
		{
			if(TFW_IS_FLAG(ki->_cflags, CF_ON_ONE_SHOT))
			{
				TFW_SET_FLAG(ki->_cflags, CF_ON_ONE_SHOT, false);
			}

			ki->_repcnt = -1;
		}
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS