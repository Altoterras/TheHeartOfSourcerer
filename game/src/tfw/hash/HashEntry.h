/***********************************************************************//**
 *	HashEntry.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/10/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_HASH_HASH_ENTRY_H_
#define _TFW_HASH_HASH_ENTRY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ハッシュエントリ
 *		それぞれの使用用途に従って、このクラスを継承すること
 *
**//*---------------------------------------------------------------------*/
class HashEntry
{
	//======================================================================
	// 定数
public:
	static const u32	RATIO_MAGIC_HASH	= 0x9E3779B9U;
	static const u32	FLAG_COLL			= 1;
	static const u32	HASHKEY_FREE		= 0;
	static const u32	HASHKEY_REMOVE		= 1;
	static const u32	HASHKEY_ACTIVE_TS	= 2;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	u32			getHashKey() const			{	return _hashkey;							}
	u32			getHashKeyNoFlag() const	{	return _hashkey & ~FLAG_COLL;				}
	bool		isFree() const				{	return _hashkey == HASHKEY_FREE;			}
	bool		isActive() const			{	return _hashkey >= HASHKEY_ACTIVE_TS;		}
	bool		isRemove() const			{	return _hashkey == HASHKEY_REMOVE;			}
	bool		isCollision() const			{	return TFW_IS_FLAG(_hashkey, FLAG_COLL);	}

	void		setHashKey(u32 hkey)		{	_hashkey = hkey;							}
	void		setFree(bool is)			{	_hashkey = is ? HASHKEY_FREE : 0;			}
	void		setRemove(bool is)			{	_hashkey = is ? HASHKEY_REMOVE : 0;			}
	void		setCollision(bool is)		{	TFW_SET_FLAG(_hashkey, FLAG_COLL, is);		}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	HashEntry();
	void clear();
	static u32	calcHashKey(u32 hashcode);
	static u32	collidedHashKey(u32 hashkey);

	//======================================================================
	// メンバ変数
private:
	u32			_hashkey;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_HASH_HASH_ENTRY_H_
