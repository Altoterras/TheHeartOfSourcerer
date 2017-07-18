/***********************************************************************//**
 *	EsResolver.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/05/12.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_RESOLVER_H_
#define _ES_ES_RESOLVER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsValue.h"

#include "../tfw/hash/HashEntry.h"
#include "../tfw/hash/HashOp.h"
#include "../tfw/hash/HashTable.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class HashTable;

TFW_END_NS

ES_BEGIN_NS

class EsObject;
class EsProgramCounter;

/*---------------------------------------------------------------------*//**
 *	リゾルバ
 *
**//*---------------------------------------------------------------------*/
class EsResolver
{
	//======================================================================
	// 定数
public:
	// 動作フラグ
	static const u32 OPF_SEARCH		= 0x01;	// 探す ⇒ JSRESFLAG_LOOKUP
	static const u32 OPF_WATCH		= 0x02;	// 参照 ⇒ JSRESFLAG_WATCH

	// 解決フラグ
	static const u8 RSLVF_QUALIFIED	= 0x01;	// 修飾されたアクセサ ID ⇒ JSRESOLVE_QUALIFIED
	static const u8 RSLVF_ASSIGNING	= 0x02;	// 等号の左辺 ⇒ JSRESOLVE_ASSIGNING
	static const u8 RSLVF_DETECTING	= 0x04;	// if(xx) もしくは xx ? xx : xx ⇒ JSRESOLVE_DETECTING
	static const u8 RSLVF_DECLARING	= 0x08;	// var, const, function ⇒ JSRESOLVE_DECLARING
	static const u8 RSLVF_CLASSNAME	= 0x10;	// コンストラクタ時のクラス名 ⇒ JSRESOLVE_CLASSNAME
	static const u8 RSLVF_WITH		= 0x20;	// with 文内 ⇒ JSRESOLVE_WITH
	static const u8 RSLVF_INFER		= 0xff;	// バイトコードから推察する ⇒ JSRESOLVE_INFER

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	自動解除フラグ
	 *
	**//*-----------------------------------------------------------------*/
	class AutoFlags
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		AutoFlags(EsResolver* rslv, u8 flagsRslv)
			: _rslv(rslv)
			, _flagsRslvBk(rslv->getResolvFlags())
		{
			_rslv->setResolvFlags(flagsRslv);
		}
		~AutoFlags()
		{
			_rslv->setResolvFlags(_flagsRslvBk);
		}

		//==================================================================
		// 変数
	public:
		EsResolver*		_rslv;
		u8				_flagsRslvBk;
	};

	/*-----------------------------------------------------------------*//**
	 *	リゾルバ キー
	 *
	**//*-----------------------------------------------------------------*/
	class EsResolverHashKey
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		EsResolverHashKey() : _obj(0L) {}

		inline void copy(const EsResolverHashKey* key)
		{
			_obj = key->_obj;
			_id = key->_id;
		}

		//==================================================================
		// 変数
	public:
		EsObject*		_obj;
		EsValue			_id;
	};

	/*-----------------------------------------------------------------*//**
	 *	リゾルバ ハッシュ エントリ
	 *
	**//*-----------------------------------------------------------------*/
	class EsResolverHashEntry : public HashEntry
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		const EsResolverHashKey* getKey() const { return &_key; }
		EsResolverHashKey* key() { return &_key; }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:
		EsResolverHashEntry() : _flags(0) {}
		void init(const EsResolverHashKey* key) { _key = *key; }

		//==================================================================
		// 変数
	public:
		u32 _flags;
		EsResolverHashKey _key;
	};

	/*-----------------------------------------------------------------*//**
	 *	リゾルバ ハッシュ演算 ⇒ resolving_dhash_ops
	 *
	**//*-----------------------------------------------------------------*/
	class EsResolverHashOp : public HashOp
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:
		virtual u32		getEntrySize();
		virtual void*	newEntryBuffer(u32 numEntry);
		virtual void	deleteEntryBuffer(void* entrybuf);
		virtual u32		hashCode(const void* key);
		virtual bool	matchEntry(HashEntry* entry, const void* key);
		virtual void	cleanupEntry(HashEntry* entry);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ハッシュテーブルのエントリ数を得る
	inline u32 getEntryCount() const { return _htblKeyTbl != 0L ? _htblKeyTbl->getEntryCount() : 0; }
	// テーブル生成カウンタを得る
	inline u32 getGenerationCount() const { return _htblKeyTbl != 0L ? _htblKeyTbl->getGenerationCount() : 0; }
	// 解決フラグを得る
	inline u8 getResolvFlags() const { return _flagsRslv; }

	// 解決フラグを設定する
	inline void setResolvFlags(u8 flagsRslv) { _flagsRslv = flagsRslv; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool startResolving(EsResolverHashEntry** hentOut, EsResolverHashKey* key, u32 optflags);
	void stopResolving(EsResolverHashKey* key, u32 optflags, EsResolverHashEntry* hentStartTime, u32 cntGenerateStartTime);

	inline EsResolverHashEntry* add(const EsResolverHashKey* key) { return _htblKeyTbl != 0L ? (EsResolverHashEntry*)_htblKeyTbl->add(key) : 0L; }
	inline void remove(const EsResolverHashKey* key) { if(_htblKeyTbl != 0L){ _htblKeyTbl->remove(key); } }

	static u32 inferFlags(EsContext* ctx, u32 flagsDefault);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsResolver();
	~EsResolver();
	void reset();

	//======================================================================
	// 変数
private:
	HashTable* _htblKeyTbl;		// ⇒ resolvingTable
	u8 _flagsRslv;				// 解決フラグ (RSLVF_*) ⇒ cx->resolveFlags
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_RESOLVER_H_
