/***********************************************************************//**
 *  TxtPos.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/11/27.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TxtPos.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	比較 this > x ～を超える
**//*---------------------------------------------------------------------*/
bool TxtPos::gt(const TxtPos* x) const
{
	return this->_indexRaw > x->_indexRaw;
}

/*---------------------------------------------------------------------*//**
	比較 this < x ～に満たない
**//*---------------------------------------------------------------------*/
bool TxtPos::lt(const TxtPos* x) const
{
	return this->_indexRaw < x->_indexRaw;
}

/*---------------------------------------------------------------------*//**
	比較 this >= x ～以上
**//*---------------------------------------------------------------------*/
bool TxtPos::ge(const TxtPos* x) const
{
	return this->_indexRaw >= x->_indexRaw;
}

/*---------------------------------------------------------------------*//**
	比較 this <= x ～以下
**//*---------------------------------------------------------------------*/
bool TxtPos::le(const TxtPos* x) const
{
	return this->_indexRaw <= x->_indexRaw;
}

/*---------------------------------------------------------------------*//**
	比較 this == x ～と同じ
**//*---------------------------------------------------------------------*/
bool TxtPos::eq(const TxtPos* x) const
{
	return this->_indexRaw == x->_indexRaw;
}

/*---------------------------------------------------------------------*//**
	設定
**//*---------------------------------------------------------------------*/
void TxtPos::set(u32 indexRaw, u32 indexChar)
{
	_indexRaw = indexRaw;
	_indexChar = indexChar;
}

/*---------------------------------------------------------------------*//**
	無効化
**//*---------------------------------------------------------------------*/
void TxtPos::invalidate()
{
	_indexRaw = INDEX_INVALID;
	_indexChar = INDEX_INVALID;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TxtPos::TxtPos()
	: _indexRaw(INDEX_INVALID)
	, _indexChar(INDEX_INVALID)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TxtPos::TxtPos(u32 indexRaw, u32 indexChar)
{
	set(indexRaw, indexChar);
}

/*---------------------------------------------------------------------*//**
	コピー コンストラクタ
**//*---------------------------------------------------------------------*/
TxtPos::TxtPos(const TxtPos& src)
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void TxtPos::copy(const TxtPos* src)
{
	_indexRaw = src->_indexRaw;
	_indexChar = src->_indexChar;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS