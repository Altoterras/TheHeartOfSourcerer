/***********************************************************************//**
 *	SourceManager.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SourceManager.h"

// Friends
#include "FixedSourceDefTbl.h"
#include "Source.h"
#include "../body/Game.h"
#include "../episode/save/SaveStructure.h"

// External
#include "../../tfw/lib/QuickSort.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SourceManager メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ソースを得る
**//*---------------------------------------------------------------------*/
const Source* SourceManager::getSource(s32 index) const
{
	return &_srcarr[index];
}

/*---------------------------------------------------------------------*//**
	ソースを得る
**//*---------------------------------------------------------------------*/
Source* SourceManager::source(s32 index)
{
	return &_srcarr[index];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ソースの新規追加
	
	@return	インデックス（-1 は失敗）
**//*---------------------------------------------------------------------*/
s32 SourceManager::newSource(ReservedKind rsvkind)
{
	// 空きインデックスを得る
	s32 idx = getEmptySourceIndex(rsvkind);
	if(idx == -1)		{	return -1;	}

	// ソース作成
	Source* source = &_srcarr[idx];
	source->clear();

	return idx;
}

/*---------------------------------------------------------------------*//**
	未使用の名前かどうかをチェックする
**//*---------------------------------------------------------------------*/
bool SourceManager::checkUnusedSourceName(const VcString* name) const
{
	for(s32 i = 0; i < _num; i++)
	{
		if(!_srcarr[i].isEmpty())
		{
			if(name->equals(_srcarr[i].getName()))
			{
				return false;
			}
		}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	空きソースのインデックスを得る
	
	@return	インデックス（-1 は失敗）
**//*---------------------------------------------------------------------*/
s32 SourceManager::getEmptySourceIndex(ReservedKind rsvkind, s32 indexSearchStart, bool isReverse) const
{
	if(isReverse)	// 逆順
	{
		if(indexSearchStart >= _num)
		{
			return -1;
		}
		for(s32 i = indexSearchStart; i >= 0; i--)
		{
			if(isEmptySourceIndex(rsvkind, i))
			{
				return i;
			}
		}
	}
	else
	{
		for(s32 i = indexSearchStart; i < _num; i++)
		{
			if(isEmptySourceIndex(rsvkind, i))
			{
				return i;
			}
		}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	空きソースのインデックスかを得る
**//*---------------------------------------------------------------------*/
bool SourceManager::isEmptySourceIndex(ReservedKind rsvkind, s32 index) const
{
	ASSERT((0 <= index) && (index < NUM_SOURCE_MAX));
	if((index < 0) || (NUM_SOURCE_MAX <= index))	{	return false;	}

	const Source* source = &_srcarr[index];
	if(!source->isEmpty())
	{
		return false;
	}

	switch(rsvkind)
	{
	default:
		return !source->isReserved();
	case RSVK_FIXED:
		return source->isReservedForFixed();
	case RSVK_USER:
		return source->isReservedForUser();
	}
}

/*---------------------------------------------------------------------*//**
	固定ソースを追加する
**//*---------------------------------------------------------------------*/
const FixedSourceDef* SourceManager::addFixedSource(u16 fsrcid)
{
	for(int i = 0; i < _fsrcdftblRef->getMaxNum(); i++)
	{
		// 固定ソース定義を得る
		const FixedSourceDef* fsrcdf = _fsrcdftblRef->getDefFromIndex(i);
		if(fsrcdf->getFixedSourceDefId() == 0)	// 未定義
		{
			break;	// 以降はない
		}
		if(fsrcdf->getFixedSourceDefId() != fsrcid)
		{
			continue;
		}

		// 重複チェック
		for(s32 j = 0; j < _num; j++)
		{
			if(!_srcarr[j].isEmpty() && (_srcarr[j].getFixedSourceDef() != 0L))
			{
				if(_srcarr[j].getFixedSourceDef()->getFixedSourceDefId() == fsrcid)
				{
					return 0L;	// 既にある
				}
			}
		}

		// ソース追加
		s32 idxSrcNew = newSource(RSVK_FIXED);
		ASSERT(idxSrcNew >= 0);
		Source* s = source(idxSrcNew);
		s->setup(fsrcdf, TFW_IS_FLAG(Game::getGame()->getOptionFlags(), Game::OPTF_SOUMA_DEFAULT_LANG_JS) ? SourceDef::SRCKIND_JS : SourceDef::SRCKIND_SS);

		// 最初に現れた予約ソースと置換
		s32 idxRepMin = S32_MAX;
		for(s32 i = 0; i < _num; i++)
		{
			if(i == idxSrcNew)				{	continue;	}
			if(!_srcarr[i].isEmpty())		{	continue;	}
			if(!_srcarr[i].isReserved())	{	continue;	}
			idxRepMin = i;
			break;
		}
		if(idxRepMin < idxSrcNew)
		{
			Source::swap(&_srcarr[idxRepMin], &_srcarr[idxSrcNew]);
			idxSrcNew = idxRepMin;
		}

		// 更に、最初に現れたフリーソースと置換
		idxRepMin = S32_MAX;
		for(s32 i = 0; i < _num; i++)
		{
			if(i == idxSrcNew)				{	continue;	}
			if(_srcarr[i].isReserved())		{	continue;	}
			if(!_srcarr[i].isEmpty())		{	continue;	}
			idxRepMin = i;
			break;
		}
		if(idxRepMin < idxSrcNew)
		{
			Source::swap(&_srcarr[idxRepMin], &_srcarr[idxSrcNew]);
		}

		#if defined(_DEBUG) && 0
			for(s32 i = 0; i < _num; i++)
			{
				s = &_srcarr[i];
				TRACE("{SourceManager::addFixedSource} [%d] kind=%d, fix_id=%d, rsv_fix=%d, rsv_usr=%d, empty=%d\n",
					i,
					s->getSourceKind(),
					(s->getFixedSourceDef() != 0L) ? s->getFixedSourceDef()->getFixedSourceDefId() : 0,
					s->isReservedForFixed(),
					s->isReservedForUser(),
					s->isEmpty()	);
			}
		#endif

		return fsrcdf;
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	初期ソース予約（ここでいう予約数は新規の数ではなく設定したい総数）
**//*---------------------------------------------------------------------*/
void SourceManager::reserveSourceFirst(s32 numReserveForUser, s32 numReserveForFixed)
{
	// 既存の予約数確認
	for(int i = 0; i < _num; i++)
	{
		if(_srcarr[i].isReservedForFixed() || (_srcarr[i].getFixedSourceDef() != 0L))
		{
			numReserveForFixed--;
		}
		else if(_srcarr[i].isReservedForUser())
		{
			numReserveForUser--;
		}
		if((numReserveForFixed <= 0) && (numReserveForUser <= 0))
		{
			return;
		}
	}

	// 予約処理
	for(int i = _num - 1; i >= 0; i--)
	{
		if((numReserveForFixed > 0) && !_srcarr[i].isReservedForFixed() && (_srcarr[i].getFixedSourceDef() == 0L))
		{
			_srcarr[i].setReservedForFixed(true);
			numReserveForFixed--;
		}
		else if((numReserveForUser > 0) && !_srcarr[i].isReservedForUser())
		{
			_srcarr[i].setReservedForUser(true);
			numReserveForUser--;
		}
		else
		{
			break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	予約ユーザーソースを解放する
**//*---------------------------------------------------------------------*/
s32 SourceManager::releaseReservedUserSource(s32 numRelease, s32 numMaxFree)
{
	// 現在のユーザーフリーソース数を得る
	s32 numFree = 0;
	s32 numReservedForUser = 0;
	for(int i = 0; i < _num; i++)
	{
		if(!_srcarr[i].isReserved() && (_srcarr[i].getFixedSourceDef() == 0L))
		{
			numFree++;
		}
		else if(_srcarr[i].isReservedForUser())
		{
			numReservedForUser++;
		}
	}

	// 解放数をクリップ
	if(numRelease > numReservedForUser)
	{
		numRelease = numRelease;
	}
	if((numRelease + numFree) > numMaxFree)
	{
		numRelease = numMaxFree - numFree;
	}
	if(numRelease <= 0)
	{
		return 0;	// 不要
	}

	// 解放処理
	s32 cnt = 0;
	for(int i = 0; i < _num; i++)
	{
		if(_srcarr[i].isReservedForUser())
		{
			_srcarr[i].setReservedForUser(false);

			numRelease--;
			cnt++;
			if(numRelease <= 0)
			{
				return cnt;
			}
		}
	}

	return cnt;
}

/*---------------------------------------------------------------------*//**
	ソートする
**//*---------------------------------------------------------------------*/
void SourceManager::sortSource()
{
	class QuickSortForEmptySource : public QuickSort
	{
		s32 compare(void* arr, s32 unitsize, s32 idxA, s32 idxB)
		{
			const Source* a = (Source*)((addr_size)arr + (unitsize * idxA));
			const Source* b = (Source*)((addr_size)arr + (unitsize * idxB));
			// 使用されているかで判定
			bool bA = !a->isEmpty();
			bool bB = !b->isEmpty();
			if(bA && bB)
			{
				const FixedSourceDef* fsrcdfA = a->getFixedSourceDef();
				const FixedSourceDef* fsrcdfB = b->getFixedSourceDef();
				bA = (fsrcdfA == 0L);
				bB = (fsrcdfB == 0L);
				if(bA && bB)	{	return 0;	}
				if(bA)			{	return -1;	}
				if(bB)			{	return 1;	}
				if(fsrcdfA->getFixedSourceDefId() < fsrcdfB->getFixedSourceDefId())	{	return -1;	}
				if(fsrcdfA->getFixedSourceDefId() > fsrcdfB->getFixedSourceDefId())	{	return 1;	}
				return 0;
			}
			if(bA)			{	return -1;	}
			if(bB)			{	return 1;	}
			// ユーザーフリー領域かで判定
			bA = !a->isReserved();
			bB = !b->isReserved();
			if(bA && bB)	{	return 0;	}
			if(bA)			{	return -1;	}
			if(bB)			{	return 1;	}
			// ユーザー予約領域かで判定
			bA = a->isReservedForUser();
			bB = b->isReservedForUser();
			if(bA && bB)	{	return 0;	}
			if(bA)			{	return -1;	}
			if(bB)			{	return 1;	}
			return 0;
		}

		void swap(void* arr, s32 unitsize, s32 idxA, s32 idxB)
		{
			Source* a = (Source*)((addr_size)arr + (unitsize * idxA));
			Source* b = (Source*)((addr_size)arr + (unitsize * idxB));
			Source::swap(a, b);
		}
	};

	QuickSortForEmptySource qsort;
	qsort.run(_srcarr, sizeof(Source), NUM_SOURCE_MAX);
}

/*---------------------------------------------------------------------*//**
	全てのソースの種別を変更する
**//*---------------------------------------------------------------------*/
void SourceManager::changeAllSourceKind(s32 srckind)
{
	for(s32 i = 0; i < _num; i++)
	{
		if(_srcarr[i].getSourceKind() == SourceDef::SRCKIND_NULL)
		{
			continue;
		}
		else if(_srcarr[i].getSourceKind() == srckind)
		{
			continue;
		}

		if(srckind == SourceDef::SRCKIND_JS)
		{
			_srcarr[i].convToJs();
		}
		else if(srckind == SourceDef::SRCKIND_SS)
		{
			_srcarr[i].convToSs();
		}
		else
		{
			ASSERT(false);
		}
	}
}

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	全ての固定ソースを追加（デバッグ用）
**//*---------------------------------------------------------------------*/
void SourceManager::debug_addFixedSourceAll()
{
	Game::getGame()->setOptionFlags(Game::OPTF_SOUMA_DEFAULT_LANG_JS, true);	// JS で追加

	for(int i = 0; i < _fsrcdftblRef->getMaxNum(); i++)
	{
		const FixedSourceDef* fsrcdf = _fsrcdftblRef->getDefFromIndex(i);
		if(fsrcdf->getFixedSourceDefId() == 0)	// 未定義
		{
			break;	// 以降はない
		}

		addFixedSource(fsrcdf->getFixedSourceDefId());
	}

	sortSource();
}
#endif

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	全ての固定ソースを更新（デバッグ用）
**//*---------------------------------------------------------------------*/
void SourceManager::debug_updateFixedSources()
{
	for(int i = 0; i < _num; i++)
	{
		Source* source = &_srcarr[i];
		const FixedSourceDef* fsrcdf = source->getFixedSourceDef();
		if(fsrcdf != 0L)
		{
			source->setup(fsrcdf, TFW_IS_FLAG(Game::getGame()->getOptionFlags(), Game::OPTF_SOUMA_DEFAULT_LANG_JS) ? SourceDef::SRCKIND_JS : SourceDef::SRCKIND_SS);
		}
	}
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SourceManager::SourceManager()
	: _srcarr(0L)
	, _fsrcdftblRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SourceManager::~SourceManager()
{
	ASSERT(_srcarr == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SourceManager::create(const FixedSourceDefTbl* fsrcdftblRef)
{
	// ソース配列の作成
	_num = NUM_SOURCE_MAX;
	_srcarr = new Source[_num];

	// 固定ソース定義テーブルへの参照を保存
	_fsrcdftblRef = fsrcdftblRef;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SourceManager::destroy()
{
	if(_srcarr != 0L)
	{
		delete[] _srcarr;
		_srcarr = 0;
	}
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool SourceManager::serialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		ASSERT(SaveStructure::NUM_SOURCE_MAX >= _num);
		for(int i = 0; i < _num; i++)
		{
			const Source* source = &_srcarr[i];
			source->storeToStructure(&svst->_srcsvst[i], ser->getString());
		}
	}
	else						// ダイレクトモード
	{
		for(int i = 0; i < _num; i++)
		{
			const Source* source = &_srcarr[i];
			source->serialize(ser);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool SourceManager::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		ASSERT(SaveStructure::NUM_SOURCE_MAX >= _num);
		for(int i = 0; i < _num; i++)
		{
			Source* source = &_srcarr[i];
			source->restoreFromStructure(&svst->_srcsvst[i], ser->getString(), _fsrcdftblRef);

			#if 0
				TRACE("{SourceManager::deserialize} [%d] kind=%d, fix_id=%d, rsv_fix=%d, rsv_usr=%d, empty=%d\n",
					i,
					source->getSourceKind(),
					(source->getFixedSourceDef() != 0L) ? source->getFixedSourceDef()->getFixedSourceDefId() : 0,
					source->isReservedForFixed(),
					source->isReservedForUser(),
					source->isEmpty()	);
			#endif
		}
	}
	else						// ダイレクトモード
	{
		for(int i = 0; i < _num; i++)
		{
			Source* source = &_srcarr[i];
			source->deserialize(ser, _fsrcdftblRef);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
