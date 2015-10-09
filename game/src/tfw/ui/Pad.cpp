/***********************************************************************//**
 *	Pad.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Pad.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Pad メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	継続押下取得
**//*---------------------------------------------------------------------*/
bool Pad::isOn(int key) const
{
	ASSERT(key < _numKeyKind);
	return _swOnCur[key];
}

/*---------------------------------------------------------------------*//**
	トリガ押下取得
**//*---------------------------------------------------------------------*/
bool Pad::isTrigger(int key) const
{
	ASSERT(key < _numKeyKind);
	return _swTrgCur[key];
}

/*---------------------------------------------------------------------*//**
	トリガ押上取得
**//*---------------------------------------------------------------------*/
bool Pad::isRelease(int key) const
{
	ASSERT(key < _numKeyKind);
	return _swTrgRelCur[key];
}

/*---------------------------------------------------------------------*//**
	トリガ押下取得
**//*---------------------------------------------------------------------*/
bool Pad::isRepeatTrigger(int key) const
{
	ASSERT(key < _numKeyKind);
	return (_cntRepOn[key] == 0) || _swTrgCur[key];
}

/*---------------------------------------------------------------------*//**
	いずれかのボタンの継続押下取得
**//*---------------------------------------------------------------------*/
bool Pad::isOnAny() const
{
	return _swOnAny;
}

/*---------------------------------------------------------------------*//**
	継続押下設定
**//*---------------------------------------------------------------------*/
void Pad::setOn(int key, bool is)
{
	ASSERT(key < _numKeyKind);
	_swOnDirect[key] = is;
}

/*---------------------------------------------------------------------*//**
	リピートカウントの設定
**//*---------------------------------------------------------------------*/
void Pad::setRepeatCount(short cnt)
{
	_cntRepMax = cnt;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Pad::Pad()
	: _numKeyKind(0)
	, _swOnDirect(0L)
	, _swOnCur(0L)
	, _swOnPrev(0L)
	, _swTrgCur(0L)
	, _swTrgRelCur(0L)
	, _swOnAny(false)
	, _cntRepOn(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Pad::~Pad()
{
	delete[] _swOnDirect;
	delete[] _swOnCur;
	delete[] _swOnPrev;
	delete[] _swTrgCur;
	delete[] _swTrgRelCur;
	delete[] _cntRepOn;
}

/*---------------------------------------------------------------------*//**
	構築
**//*---------------------------------------------------------------------*/
bool Pad::create(int numKeyKind, short cntRepMax)
{
	#if 0	// あまり必要とも思えないので除去
		// 最低限固定キー分は最低限必要
		if(numKeyKind < NUM_FIXED_KEY)
		{
			numKeyKind = NUM_FIXED_KEY;
		}
	#endif

	// キー配列確保
	_swOnDirect = new bool[numKeyKind];
	_swOnCur = new bool[numKeyKind];
	_swOnPrev = new bool[numKeyKind];
	_swTrgCur = new bool[numKeyKind];
	_swTrgRelCur = new bool[numKeyKind];
	_cntRepOn = new short[numKeyKind];

	// 初期化
	::memset(_swOnDirect, 0, sizeof(bool) * numKeyKind);
	::memset(_swOnCur, 0, sizeof(bool) * numKeyKind);
	::memset(_swOnPrev, 0, sizeof(bool) * numKeyKind);
	::memset(_swTrgCur, 0, sizeof(bool) * numKeyKind);
	::memset(_swTrgRelCur, 0, sizeof(bool) * numKeyKind);
	::memset(_cntRepOn, 0, sizeof(short) * numKeyKind);
	_numKeyKind = numKeyKind;
	_cntRepMax = cntRepMax;

	
	return true;
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void Pad::exec()
{
	// フラグ更新
	_swOnAny = false;
	for(int i = 0; i < _numKeyKind; i++)
	{
		_swOnPrev[i] = _swOnCur[i];
		_swOnCur[i] = _swOnDirect[i];
		_swTrgCur[i] = (!_swOnPrev[i]) && (_swOnCur[i]);
		_swTrgRelCur[i] = (_swOnPrev[i]) && (!_swOnCur[i]);
		if(_swOnCur[i])	{	_swOnAny = true;	}

		if(_swOnCur[i])
		{
			if(_cntRepOn[i] == -1)		{	_cntRepOn[i] = _cntRepMax;	}
			else if(_cntRepOn[i] == 0)	{	_cntRepOn[i] = 1;			}
			else						{	_cntRepOn[i]--;				}
		}
		else
		{
			_cntRepOn[i] = -1;
		}
	}

	/*
	// UI ハンドラ
	if(_listener != 0L)
	{
		if(_swTrgRelCur[KEY_OK])
		{
			_listener->onUiClick(KEY_OK, 0, 0, 0, 0);
		}
	}
	*/
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS