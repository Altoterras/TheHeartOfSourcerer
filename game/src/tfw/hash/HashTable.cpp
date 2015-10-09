/***********************************************************************//**
 *	HashTable.cpp
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
#include "HashTable.h"

// Friends
#include "HashEntry.h"
#include "HashOp.h"

// External

// Library
#include "../lib/Calc.h"

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HashTable メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ ⇒ JS_DHashTableInit
**//*---------------------------------------------------------------------*/
HashTable::HashTable(HashOp* opEntr, u32 numEntryMax)
	: _op(opEntr)
	, _sizeEntry(0)
	, _numEntryMax(numEntryMax)
	, _shiftHashToIdx(0)
	, _bufEntry(0L)
	, _cntEntry(0)
	, _cntRemovedEntry(0)
	, _cntGenerate(0)
{
	// エントリのサイズを得る
	_sizeEntry = _op->getEntrySize();

	// エントリ最大数の補正
	u32 numEntryLog2 = 0;
	adjustTableSize(&_numEntryMax, &numEntryLog2, _numEntryMax);

	// ハッシュ値をバッファアドレスに変換するシフト値を求める
	_shiftHashToIdx = BITS_HASH - numEntryLog2;

	// エントリバッファの確保
	_bufEntry = _op->newEntryBuffer(_numEntryMax);
	if(_bufEntry == 0L)	{	return;	}	// 失敗
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HashTable::~HashTable()
{
	// エントリの削除
	for(u32 i = 0; i < _numEntryMax; i++)
	{
		HashEntry* entry = (HashEntry*)((u8*)_bufEntry + (i * _sizeEntry));
		if(entry->getHashKey() != 0)
		{
			_op->cleanupEntry(entry);
		}
	}

	// エントリバッファの削除
	_op->deleteEntryBuffer(_bufEntry);

	// 演算インスタンスを削除する
	delete _op;
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void HashTable::reset()
{
	// エントリの削除
	for(u32 i = 0; i < _numEntryMax; i++)
	{
		HashEntry* entry = (HashEntry*)((u8*)_bufEntry + (i * _sizeEntry));
		if(entry->getHashKey() != 0)
		{
			_op->cleanupEntry(entry);
		}
	}

	// 値のリセット
	_cntEntry = 0;
	_cntRemovedEntry = 0;
	_cntGenerate = 0;
}

/*---------------------------------------------------------------------*//**
	要素追加 ⇒ JS_DHashTableOperate, JS_DHASH_ADD
**//*---------------------------------------------------------------------*/
HashEntry* HashTable::add(const void* key)
{
	// テーブル再生成
	if((_cntEntry + _cntRemovedEntry) >= _numEntryMax)
	{
		if(!renewTable((_cntRemovedEntry >= (_numEntryMax / 4)) ? 0 : 1))
		{
			return 0L;
		}
	}

	// エントリ検索
	u32 hashkey = calcHashKey(key);	// ⇒ table->ops->hashKey(table, key) ～ keyHash &= ~COLLISION_FLAG;
	HashEntry* entry = findEntry(hashkey, key, true);
	if(entry->isActive())	// 有効なハッシュエントリ
	{
		return entry;
	}
	// 無効なハッシュエントリ

	// 削除エントリの再利用
	if(entry->isRemove())
	{
		_cntRemovedEntry--;
		hashkey = HashEntry::collidedHashKey(hashkey);
	}

	// エントリの初期化
	if(!_op->initEntry(entry, key))
	{
		return 0L;	// 失敗
	}

	// 有効化して返す
	entry->setHashKey(hashkey);
	_cntEntry++;

	return entry;
}

/*---------------------------------------------------------------------*//**
	要素検索 ⇒ JS_DHashTableOperate, JS_DHASH_LOOKUP
**//*---------------------------------------------------------------------*/
HashEntry* HashTable::find(const void* key)
{
	u32 hashkey = calcHashKey(key);

	HashEntry* entry = findEntry(hashkey, key, false);
	if(entry->isActive())	// 有効なハッシュエントリ
	{
		return entry;
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	要素検索 ⇒ JS_DHashTableOperate, JS_DHASH_REMOVE
**//*---------------------------------------------------------------------*/
void HashTable::remove(const void* key)
{
	// エントリ検索
	u32 hashkey = calcHashKey(key);	// ⇒ table->ops->hashKey(table, key) ～ keyHash &= ~COLLISION_FLAG;
	HashEntry* entry = findEntry(hashkey, key, true);

	if(entry->isActive())	// 有効なハッシュエントリ
	{
		removeRaw(entry);	// 削除にマーク

		// テーブルサイズの縮小
		u32 sizeMin = 1 << (32 - _shiftHashToIdx);
		if(	(_numEntryMax > TABLE_SIZE_MIN) &&
			(_cntEntry <= sizeMin)	)
		{
			renewTable(sizeMin - _numEntryMax);
		}
	}
}

/*---------------------------------------------------------------------*//**
	削除 ⇒ JS_DHashTableRawRemove
**//*---------------------------------------------------------------------*/
void HashTable::removeRaw(HashEntry* entry)
{
	u32 hashkeyBk = entry->getHashKey();
	::memset(entry, 0, _sizeEntry);	// ⇒ table->ops->clearEntry(table, entry);
	if(TFW_IS_FLAG(hashkeyBk, HashEntry::FLAG_COLL))
	{
		entry->setRemove(true);
		_cntRemovedEntry++;
	}
	else
	{
		entry->setFree(true);
	}
	_cntEntry--;
}

/*---------------------------------------------------------------------*//**
	テーブルサイズを最適化する
**//*---------------------------------------------------------------------*/
void HashTable::adjustTableSize(u32* sizeNew, u32* sizeLog2, const u32 sizeOrg)
{
	*sizeNew = sizeOrg;
	if(*sizeNew < TABLE_SIZE_MIN)	{	*sizeNew = TABLE_SIZE_MIN;	}

	// ２の累乗のサイズにする
	*sizeLog2 = Calc::log2Ceiling32(*sizeNew);
	*sizeNew = 1 << *sizeLog2;

	if(*sizeNew > TABLE_SIZE_MAX)	{	*sizeNew = TABLE_SIZE_MAX;	}
}

/*---------------------------------------------------------------------*//**
	テーブルサイズを再生成する ⇒ ChangeTable
**//*---------------------------------------------------------------------*/
bool HashTable::renewTable(s32 numAddSize)
{
	// 新しいエントリ最大数
	u32 numEntryMaxNew = _numEntryMax + numAddSize;
	u32 numEntryLog2 = 0;
	adjustTableSize(&numEntryMaxNew, &numEntryLog2, numEntryMaxNew);

	// ハッシュ値をバッファアドレスに変換するシフト値を求める
	u32 shiftHashToIdxNew = BITS_HASH - numEntryLog2;

	// エントリバッファの確保
	void* bufEntryNew = _op->newEntryBuffer(numEntryMaxNew);
	if(bufEntryNew == 0L)	{	return false;	}	// 失敗

	// 新しいバッファをクリア
	::memset(bufEntryNew, 0, _sizeEntry * numEntryMaxNew);

	// 新しい値に付け替える
	void* bufEntryOld = _bufEntry;
	u32 numEntryMaxOld = _numEntryMax;
	_bufEntry = bufEntryNew;
	_numEntryMax = numEntryMaxNew;
	_shiftHashToIdx = shiftHashToIdxNew;
	_cntRemovedEntry = 0;	// 削除エントリはない（下記コピーループで解消される）
	_cntGenerate++;			// 生成カウント増算

	// 有効なエントリを新しいバッファにコピーする
	HashEntry* entryOld = (HashEntry*)bufEntryOld;
	for(u32 i = 0; i < numEntryMaxOld; i++)
	{
		if(entryOld->isActive())	// 有効なハッシュエントリのみ
		{
			entryOld->setCollision(false);
			HashEntry* entryNew = findFreeEntry(entryOld->getHashKey());
			::memcpy(entryNew, entryOld, _sizeEntry);	// ⇒ moveEntry(table, oldEntry, newEntry);
			entryNew->setHashKey(entryOld->getHashKey());
		}
		entryOld = (HashEntry*)(((u8*)entryOld) + _sizeEntry);
	}

	// 古いバッファを削除
	_op->deleteEntryBuffer(bufEntryOld);

	return true;
}

/*---------------------------------------------------------------------*//**
	ハッシュ値を計算する
**//*---------------------------------------------------------------------*/
u32 HashTable::calcHashKey(const void* key)
{
	return HashEntry::calcHashKey( _op->hashCode(key) );
}

/*---------------------------------------------------------------------*//**
	ハッシュ値を算出する（１）⇒ HASH1
**//*---------------------------------------------------------------------*/
u32 HashTable::calcHash1(u32 hashkey)
{
	return hashkey >> _shiftHashToIdx;
}

/*---------------------------------------------------------------------*//**
	ハッシュ値を算出する（２）⇒ HASH2
**//*---------------------------------------------------------------------*/
u32 HashTable::calcHash2(u32 hashkey, u32 log2)
{
	return ((hashkey << log2) >> _shiftHashToIdx) | 1;
}

/*---------------------------------------------------------------------*//**
	ハッシュ値からエントリを得る ⇒ ADDRESS_ENTRY
**//*---------------------------------------------------------------------*/
HashEntry* HashTable::getEntryFromHashKey(u32 hashidx)
{
	ASSERT(sizeof(HashEntry) <= _sizeEntry);
	return (HashEntry*)((u8*)_bufEntry + (hashidx * _sizeEntry));
}

/*---------------------------------------------------------------------*//**
	エントリ検索 ⇒ SearchTable
**//*---------------------------------------------------------------------*/
HashEntry* HashTable::findEntry(u32 hashkey, const void* key, bool isAddCase)
{
	// ハッシュ値からエントリを得る
	u32 hash1 = calcHash1(hashkey);
	HashEntry* entry = getEntryFromHashKey(hash1);

	// 空のエントリ
	if(entry->isFree())
	{
		return entry;
	}

	// 完全一致判定
	if(entry->getHashKeyNoFlag() == hashkey)	// ハッシュ値が一致
	{
		if(_op->matchEntry(entry, key))	// 実際の値も一致
		{
			return entry;
		}
	}

	// ハッシュ値が衝突した
	u32 sizeLog2 = 32 - _shiftHashToIdx;
	u32 hash2 = calcHash2(hashkey, sizeLog2);
	u32 maskSize = (1 << sizeLog2) - 1;
	HashEntry* entryFirstRemoved = 0L;
	while(true)
	{
		if(entry->isRemove())
		{
			if(entryFirstRemoved == 0L)
			{
				entryFirstRemoved = entry;
			}
		}
		else
		{
			if(isAddCase)
			{
				entry->setCollision(true);
			}
		}

		// 新たなハッシュ値からエントリを得る
		hash1 -= hash2;
		hash1 &= maskSize;
		entry = getEntryFromHashKey(hash1);

		// 空のエントリであれば成功
		if(entry->isFree())
		{
			if(isAddCase && (entryFirstRemoved != 0L))
			{
				return entryFirstRemoved;
			}
			else
			{
				return entry;
			}
		}

		// 完全一致判定
		if(entry->getHashKeyNoFlag() == hashkey)	// ハッシュ値が一致
		{
			if(_op->matchEntry(entry, key))	// 実際の値も一致
			{
				return entry;
			}
		}
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	空きエントリ検索 ⇒ FindFreeEntry
**//*---------------------------------------------------------------------*/
HashEntry* HashTable::findFreeEntry(u32 hashkey)
{
	// ハッシュ値からエントリを得る
	u32 hash1 = calcHash1(hashkey);
	HashEntry* entry = getEntryFromHashKey(hash1);

	// 空きエントリならばそのまま返す
	if(entry->isFree())
	{
		return entry;
	}

	// ハッシュ値が衝突した
	u32 sizeLog2 = 32 - _shiftHashToIdx;
	u32 hash2 = calcHash2(hashkey, sizeLog2);
	u32 maskSize = (1 << sizeLog2) - 1;
	//HashEntry* entryFirstRemoved = 0L;
	while(true)
	{
		ASSERT(!entry->isRemove());
		entry->setCollision(true);

		// 新たなハッシュ値からエントリを得る
		hash1 -= hash2;
		hash1 &= maskSize;
		entry = getEntryFromHashKey(hash1);

		// 空のエントリであれば成功
		if(entry->isFree())
		{
			return entry;
		}
	}

	return 0L;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
