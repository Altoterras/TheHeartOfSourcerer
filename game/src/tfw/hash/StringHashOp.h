/***********************************************************************//**
 *	StringHashOp.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/10/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_HASH_STRING_HASH_OP_H_
#define _TFW_HASH_STRING_HASH_OP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "HashOp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	文字列ハッシュ演算
 *
**//*---------------------------------------------------------------------*/
class StringHashOp : public HashOp
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	StringHashOp();
	virtual ~StringHashOp();
	virtual u32		getEntrySize();
	virtual void*	newEntryBuffer(u32 numEntry);
	virtual void	deleteEntryBuffer(void* entrybuf);
	virtual u32		hashCode(const void* key);
	virtual bool	matchEntry(HashEntry* entry, const void* key);

	//======================================================================
	// メンバ変数
private:
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_HASH_STRING_HASH_OP_H_
