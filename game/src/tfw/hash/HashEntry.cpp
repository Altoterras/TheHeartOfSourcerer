/***********************************************************************//**
 *	HashEntry.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/10/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HashEntry.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HashEntry メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HashEntry::HashEntry()
{
	clear();
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void HashEntry::clear()
{
	_hashkey = 0;
}

/*---------------------------------------------------------------------*//**
	ハッシュコードからハッシュキーを計算する
**//*---------------------------------------------------------------------*/
u32 HashEntry::calcHashKey(u32 hashcode)
{
	u32 hashkey = hashcode * RATIO_MAGIC_HASH;							// 値を補正する
	if(hashkey < HASHKEY_ACTIVE_TS) { hashkey -= HASHKEY_ACTIVE_TS; }	// ハッシュ値の 0（空）, 1（衝突）を除外
	hashkey &= ~FLAG_COLL;												// 衝突フラグ分を空ける
	return hashkey;
}

/*---------------------------------------------------------------------*//**
	衝突フラグ付きハッシュキーを計算する
**//*---------------------------------------------------------------------*/
u32 HashEntry::collidedHashKey(u32 hashkey)
{
	return hashkey | FLAG_COLL;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
