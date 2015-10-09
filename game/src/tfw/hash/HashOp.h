/***********************************************************************//**
 *	HashOp.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/10/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_HASH_HASH_OP_H_
#define _TFW_HASH_HASH_OP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "HashEntry.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class HashEntry;

/*---------------------------------------------------------------------*//**
 *	ハッシュ演算
 *		ハッシュテーブルがハッシュ値（キー）を求めるのに
 *		どのような演算を行うかを定義するクラス
 *
**//*---------------------------------------------------------------------*/
class HashOp
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual u32		getEntrySize() = 0;
	virtual void*	newEntryBuffer(u32 numEntry) = 0;
	virtual void	deleteEntryBuffer(void* entrybuf) = 0;
	virtual u32		hashCode(const void* key) = 0;
	virtual bool	matchEntry(HashEntry* entry, const void* key) = 0;
	virtual bool	initEntry(HashEntry* entry, const void* key)		{	return true;	}	// 継承必須ではない
	virtual void	cleanupEntry(HashEntry* entry)						{	entry->clear();	}	// 継承必須ではない

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	virtual ~HashOp() {}

	//======================================================================
	// メンバ変数
private:
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_HASH_HASH_OP_H_
