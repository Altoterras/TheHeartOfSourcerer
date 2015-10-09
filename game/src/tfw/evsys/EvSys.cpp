/***********************************************************************//**
 *	EvSys.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EvSys.h"

// Friends
#include "EvData.h"
#include "EvMatter.h"

// External
#include "../collection/List.h"
#include "../file/IdFile.h"
#include "../lib/XmlParser.h"
#include "../string/VcString.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EventSys メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ
	
/*---------------------------------------------------------------------*//**
	稼働中イベントの数を取得
**//*---------------------------------------------------------------------*/
int EvSys::getBegunEventCount() const
{
	int cnt = 0;
	for(int i = 0; i < _evdataRef->getMatterNum(); i++)
	{
		EvMatter* matter = _evdataRef->getMatter(i);
		if(matter->isBegin())
		{
			cnt++;
		}
	}
	return cnt;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	イベントの発動
	 
	@param evid イベント ID
	@param paramBegin 発動パラメータ
	@param flagsCond 条件フラグ（EvMatter::FLG_*）
	@param cause 発動原因
**//*---------------------------------------------------------------------*/
EvMatter* EvSys::begin(u16 evid, u32 paramBegin, u32 flagsCond, const EvCause* cause)
{
	if(_evdataRef == 0L)	{	return 0L;	}

	// 登録されたイベントマターの中から該当するものを起動する
	u16 evidConcurrent = 0;	// 併発起動イベント ID （現在は同時に１つまで）
	EvMatter* matterCauseForConcurrent = 0L;
	EvMatter* matterBegan = 0L;
	for(int i = 0; i < _evdataRef->getMatterNum(); i++)
	{
		EvMatter* matter = _evdataRef->getMatter(i);
		
		// 既に起動しているかどうか
		if(matter->isBegin())							{	continue;	}
		
		// 起動条件フラグ判定
		if(!matter->judgeBeginFlags(evid, flagsCond))	{	continue;	}	// 条件を満たしていない

		// 併発起動イベントの保存
		matterCauseForConcurrent = matter;
		evidConcurrent = matter->getConcurrentEvId();
		
		// 派生クラスによる起動判定
		if(!matter->judgeBeginCustom())					{	continue;	}

		// 起動
		TRACE("{EvSys::begin} evid=%d, paramBegin=%d, flagscond=%x\n", evid, paramBegin, flagsCond);
		matter->begin(paramBegin);
		matter->setCause(cause);
		matterBegan = matter;

		// 操作ロックフラグ更新
		if(matter->isLock())
		{
			_isOpLock = true;
		}
		
		// 派生クラスによるマター開始処理
		matter->markBegin();

		break;	// 成功したので終了する
	}
	
	// 併発起動（元イベントが起動しなかった場合でも起動する）
	if(evidConcurrent != 0)
	{
		EvCause evcauseFromMatter(matterCauseForConcurrent);
		EvCause evcauseFirstConcr(EvCause::CID_CONCURRENT);
		matterBegan = begin(evidConcurrent, paramBegin, 0, (matterCauseForConcurrent != 0L) ? &evcauseFromMatter : &evcauseFirstConcr);
	}

	// 起動したマターがある
	if(matterBegan != 0L)
	{
		if(!_isRunMatter)
		{
			_isRunMatter = true;
		}
	}
	
	return matterBegan;
}

/*---------------------------------------------------------------------*//**
	イベントの簡易発動
	 
	@param	evid		イベント ID
**//*---------------------------------------------------------------------*/
EvMatter* EvSys::begin(u16 evid, u32 paramBegin, const EvCause* cause)
{
	return begin(evid, paramBegin, 0, cause);
}

/*---------------------------------------------------------------------*//**
	イベントの範囲強制終了

	@param	Exclusive	除外イベント ID
**//*---------------------------------------------------------------------*/
void EvSys::terminateEventRange(u16 evidRangeStart, u16 evidRangeEnd)
{
	for(int i = 0; i < _evdataRef->getMatterNum(); i++)
	{
		EvMatter* matter = _evdataRef->getMatter(i);
		if(matter->isBegin())
		{
			if((evidRangeStart <= matter->getEvId()) && (matter->getEvId() <= evidRangeEnd))
			{
				matter->end();
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	全てのイベントの強制終了

	@param	Exclusive	除外イベント ID
**//*---------------------------------------------------------------------*/
void EvSys::terminateAllEvent(u16 evidExclusive)
{
	for(int i = 0; i < _evdataRef->getMatterNum(); i++)
	{
		EvMatter* matter = _evdataRef->getMatter(i);
		if(matter->isBegin())
		{
			if(matter->getEvId() != evidExclusive)
			{
				matter->end();
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	指定マター以外のイベントが起動しているかどうか調べる
**//*---------------------------------------------------------------------*/
bool EvSys::checkBegunOtherMatter(EvMatter* matterExclusion, bool isParentExclusion) const
{
	for(int i = 0; i < _evdataRef->getMatterNum(); i++)
	{
		EvMatter* matter = _evdataRef->getMatter(i);
		if(matter->isBegin())
		{
			// 指定マターを除外
			if(matter->getEvId() == matterExclusion->getEvId())
			{
				continue;
			}
			// 指定マターの親も除外
			if((matter->getEvId() == matterExclusion->getParentEvId()) && isParentExclusion)
			{
				continue;
			}

			return true;	// 起動している
		}
	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EvSys::EvSys()
	: _evdataRef(0L)
	, _isRunMatter(false)
	, _isOpLock(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EvSys::~EvSys()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EvSys::create(EvData* evdatRef)
{
	_evdataRef = evdatRef;
	_evdataRef->bindEvSys(this);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EvSys::destroy()
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void EvSys::exec(ExecRes* res, const ExecCtx* ec)
{
	if(_evdataRef == 0L)	{	return;	}

	// 制御
	for(int i = 0; i < _evdataRef->getMatterNum(); i++)
	{
		EvMatter* matter = _evdataRef->getMatter(i);

		// 開始しているイベントの制御処理
		if(matter->isBegin())
		{
			matter->exec(res, ec);		// 実行
		}
	}

	// 連続実行処理とフラグの更新
	bool isRunMatter = false;
	bool isOpLock = false;
	for(int i = 0; i < _evdataRef->getMatterNum(); i++)
	{
		EvMatter* matter = _evdataRef->getMatter(i);

		// 終了したイベントの連続実行処理
		if(matter->isEndTrigger())		// 終了した
		{
			// 連続実行イベントがある場合は起動する
			u16 evidNext = matter->getNextEvId();
			if(evidNext != 0)
			{
				EvCause evcause(matter);
				begin(evidNext, matter->getBeginParam(), 0, &evcause);
			}
		}

		// フラグの更新
		if(matter->isBegin())
		{
			isRunMatter = true;

			// ロックフラグの更新
			if(matter->isLock())
			{
				isOpLock = true;
			}
		}
	}
	if(_isRunMatter != isRunMatter)
	{
		_isRunMatter = isRunMatter;	// 実行フラグを更新する
	}
	if(_isOpLock != isOpLock)
	{
		_isOpLock = isOpLock;		// ロックフラグを更新する
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void EvSys::render(const RenderCtx* rc)
{
	if(_evdataRef == 0L)	{	return;	}

	for(int i = 0; i < _evdataRef->getMatterNum(); i++)
	{
		EvMatter* matter = _evdataRef->getMatter(i);
		if(matter->isBegin())
		{
			matter->render(rc);
		}
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void EvSys::draw(const RenderCtx* rc)
{
	if(_evdataRef == 0L)	{	return;	}

	for(int i = 0; i < _evdataRef->getMatterNum(); i++)
	{
		EvMatter* matter = _evdataRef->getMatter(i);
		if(matter->isBegin())
		{
			matter->draw(rc);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
