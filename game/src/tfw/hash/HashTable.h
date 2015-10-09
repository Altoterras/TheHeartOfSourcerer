/***********************************************************************//**
 *	HashTable.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/10/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_HASH_HASH_TABLE_H_
#define _TFW_HASH_HASH_TABLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class HashEntry;
class HashOp;

/*---------------------------------------------------------------------*//**
 *	ハッシュテーブル
 *
**//*---------------------------------------------------------------------*/
class HashTable
{
	//======================================================================
	// 定数
public:
	static const u32	TABLE_SIZE_MIN		= 16;		// ⇒ JS_DHASH_MIN_SIZE
	static const u32	TABLE_SIZE_MAX		= 1 << 24;	// ⇒ JS_DHASH_SIZE_LIMIT

public:
	static const u32	BITS_HASH			= 32;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 有効かどうかを得る
	inline bool isAvailable() const { return _bufEntry != 0L; }
	// エントリ数を得る
	inline u32 getEntryCount() const { return _cntEntry; }
	// 最大エントリ数を得る
	inline u32 getMaxEntryNum() const { return _numEntryMax; }
	// エントリ削除カウンタを得る
	inline u32 getRemovedEntryCount() const { return _cntRemovedEntry; }
	// テーブル生成カウンタを得る
	inline u32 getGenerationCount() const { return _cntGenerate; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	HashTable(HashOp* opEntr, u32 numEntryMax);
	virtual ~HashTable();
	void			reset();

	u32				calcHashKey(const void* key);
	HashEntry*		add(const void* key);
	HashEntry*		find(const void* key);
	void			remove(const void* key);
	void			removeRaw(HashEntry* entry);

private:
	static void		adjustTableSize(u32* sizeNew, u32* sizeLog2, const u32 sizeOrg);
	bool			renewTable(s32 numAddSize);
	u32				calcHash1(u32 hashkey);
	u32				calcHash2(u32 hashkey, u32 log2);
	HashEntry*		getEntryFromHashKey(u32 hashidx);
	HashEntry*		findEntry(u32 hashkey, const void* key, bool isAddCase);
	HashEntry*		findFreeEntry(u32 hashkey);

	//======================================================================
	// メンバ変数
private:
	HashOp*			_op;
	u32				_sizeEntry;
	u32				_numEntryMax;
	u32				_shiftHashToIdx;
	void*			_bufEntry;
	u32				_cntEntry;
	u32				_cntRemovedEntry;	// ⇒ removedCount
	u32				_cntGenerate;		// ⇒ generation
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_HASH_HASH_TABLE_H_
