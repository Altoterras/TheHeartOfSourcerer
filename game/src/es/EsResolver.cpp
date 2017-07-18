/***********************************************************************//**
 *	EsResolver.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/05/12.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsResolver.h"

// Friends
#include "EsContext.h"
#include "EsOpcode.h"
#include "EsProgramCounter.h"
#include "EsStackFrame.h"
#include "EsStackRegister.h"

// External
#include "../tfw/hash/HashTable.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// EsResolver クラス

//==========================================================================
// 定数

#define JS_DHASH_MIN_SIZE

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	解決開始 ⇒ js_StartResolving
**//*---------------------------------------------------------------------*/
bool EsResolver::startResolving(EsResolverHashEntry** hentOut, EsResolverHashKey* key, u32 optflags)
{
	if(_htblKeyTbl == 0L)
	{
		_htblKeyTbl = new HashTable(new EsResolverHashOp(), HashTable::TABLE_SIZE_MIN);	// ⇒ table = JS_NewDHashTable(&resolving_dhash_ops, NULL, sizeof(JSResolvingEntry), JS_DHASH_MIN_SIZE);
		if(_htblKeyTbl == 0L)
		{
			return false;
		}
	}

	EsResolverHashEntry* hent = (EsResolverHashEntry*)_htblKeyTbl->add(key);
	if(hent == 0L)
	{
		return false;
	}

	// 再入防止
	if(TFW_IS_FLAG(hent->_flags, optflags))
	{
		*hentOut = 0L;
	}
	else
	{
		if(hent->getKey()->_obj == 0L)
		{
			hent->key()->copy(key);
		}
		TFW_SET_FLAG(hent->_flags, optflags, true);
		*hentOut = hent;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	解決終了 ⇒ js_StopResolving
**//*---------------------------------------------------------------------*/
void EsResolver::stopResolving(EsResolverHashKey* key, u32 optflags, EsResolverHashEntry* hentStartTime, u32 cntGenerateStartTime)
{
	if(_htblKeyTbl == 0L)
	{
		ASSERT(false);
		return;
	}

	// 開始時のエントリが無い場合や、テーブルが再生成された場合はエントリを取得しなおす
	if((hentStartTime == 0L) || (_htblKeyTbl->getGenerationCount() != cntGenerateStartTime))
	{
		hentStartTime = (EsResolverHashEntry*)_htblKeyTbl->find(key);
	}
	// エントリのフラグをクリアする
	TFW_SET_FLAG(hentStartTime->_flags, optflags, false);
	if(hentStartTime->_flags == 0)
	{
		return;
	}

	if(_htblKeyTbl->getRemovedEntryCount() < (_htblKeyTbl->getMaxEntryNum() / 4))
	{
		_htblKeyTbl->removeRaw(hentStartTime);
	}
	else
	{
		_htblKeyTbl->remove(key);
	}
}

/*---------------------------------------------------------------------*//**
	現在実行中のバイトコードから解決フラグを推察する ⇒ InferFlags
**//*---------------------------------------------------------------------*/
u32 EsResolver::inferFlags(EsContext* ctx, u32 flagsDefault)
{
	EsStackFrame* sfrm = ctx->getTopStackFrame();
	if(sfrm == 0L)						{	return flagsDefault;	}
	if(sfrm->getStackRegister() == 0L)	{	return flagsDefault;	}

	EsProgramCounter pcWk(*sfrm->getStackRegister()->pc());
	EsOpcodeType ot = (EsOpcodeType)pcWk.get(0);			// ⇒ js_GetOpcode(cx, fp->script, pc)
	const EsOpcode::Spec* ospec = EsOpcode::getSpec(ot);	// ⇒ js_CodeSpec

	u32 flags = 0;
	if((ospec->_format & EsOpcode::OPFMT_MODEMASK) != EsOpcode::OPFMT_NAME)
	{
		TFW_SET_FLAG(flags, RSLVF_QUALIFIED, true);
	}
	if(	TFW_IS_FLAG(ospec->_format, EsOpcode::OPFMT_SET) ||
		TFW_IS_FLAG(ospec->_format, EsOpcode::OPFMT_FOR) ||
		TFW_IS_FLAG(sfrm->getFlags(), EsStackFrame::F_ASSIGNING)	)
	{
		TFW_SET_FLAG(flags, RSLVF_ASSIGNING, true);
	}
	else
	{
		if(pcWk.isSafeOffset(ospec->_length) && pcWk.checkDetecting(ctx, ospec->_length))
		{
			TFW_SET_FLAG(flags, RSLVF_DETECTING, true);
		}
	}
	if(TFW_IS_FLAG(ospec->_format, EsOpcode::OPFMT_DECLARING))
	{
		TFW_SET_FLAG(flags, RSLVF_DECLARING, true);
	}
	return flags;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsResolver::EsResolver()
	: _htblKeyTbl(0L)
	, _flagsRslv(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsResolver::~EsResolver()
{
	if(_htblKeyTbl != 0L)
	{
		delete _htblKeyTbl;
		_htblKeyTbl = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void EsResolver::reset()
{
	if(_htblKeyTbl != 0L)
	{
		_htblKeyTbl->reset();
	}
}

//==========================================================================
// EsResolver::EsResolverHashOp メソッド

/*---------------------------------------------------------------------*//**
	エントリのサイズを返す
**//*---------------------------------------------------------------------*/
u32 EsResolver::EsResolverHashOp::getEntrySize()
{
	return sizeof(EsResolverHashEntry);
}

/*---------------------------------------------------------------------*//**
	エントリバッファを確保する
**//*---------------------------------------------------------------------*/
void* EsResolver::EsResolverHashOp::newEntryBuffer(u32 numEntry)
{
	EsResolverHashEntry* arr = new EsResolverHashEntry[numEntry];
	return arr;
}

/*---------------------------------------------------------------------*//**
	エントリバッファを削除する
**//*---------------------------------------------------------------------*/
void EsResolver::EsResolverHashOp::deleteEntryBuffer(void* entrybuf)
{
	EsResolverHashEntry* arr = (EsResolverHashEntry*)entrybuf;
	delete[] arr;
}

/*---------------------------------------------------------------------*//**
	ハッシュコードを算出する ⇒ resolving_HashKey
**//*---------------------------------------------------------------------*/
u32 EsResolver::EsResolverHashOp::hashCode(const void* key)
{
	const EsResolverHashKey* rkey = (const EsResolverHashKey*)key;
#if _64BIT
	u64 addrObj = reinterpret_cast<u64>(rkey->_obj);
	u32 h = static_cast<u32>(addrObj) ^ rkey->_id.calcHashCode();
#else
	u32 h = reinterpret_cast<u32>(rkey->_obj) ^ rkey->_id.calcHashCode();
#endif
	return h;
}

/*---------------------------------------------------------------------*//**
	エントリが同一かどうか判定する ⇒ resolving_MatchEntry
**//*---------------------------------------------------------------------*/
bool EsResolver::EsResolverHashOp::matchEntry(HashEntry* entry, const void* key)
{
	const EsResolverHashKey* key1 = ((EsResolverHashEntry*)entry)->getKey();
	const EsResolverHashKey* key2 = (EsResolverHashKey*)key;
	return (key1->_obj == key2->_obj) && key1->_id.equals(&key2->_id);
}

/*---------------------------------------------------------------------*//**
	エントリ破棄
**//*---------------------------------------------------------------------*/
void EsResolver::EsResolverHashOp::cleanupEntry(HashEntry* entry)
{
	EsResolverHashEntry* ae = (EsResolverHashEntry*)entry;
	entry->clear();
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
