/***********************************************************************//**
 *	EventSys.cpp
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
#include "EventSys.h"

// Friends
#include "EventData.h"
#include "EventIdDef.h"
#include "MsgDataKind.h"
#include "matter/EventMatter.h"
#include "matter/EventMatterFactory.h"
#include "../body/Game.h"
#include "../mode/GameMode.h"
#include "../quest/QuestManager.h"
#include "../episode/StoryManager.h"

// External
#include "../../tfw/collection/List.h"
#include "../../tfw/file/IdFile.h"
#include "../../tfw/lib/XmlParser.h"
#include "../../tfw/string/VcString.h"

// Library

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EventSys メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

#if 0
/*---------------------------------------------------------------------*//**
	イベントモード後のゲームモードを設定
**//*---------------------------------------------------------------------*/
void EventSys::setEventAfterGameMode(s32 gmk)
{
	_gmkAfterEvent = gmk;
	TRACELN(" - 3 -> _gmkAfterEvent=%d", _gmkAfterEvent);
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	マップの読み込み
**//*---------------------------------------------------------------------*/
bool EventSys::loadMap(s32 mapid)
{
	// 今までのマップイベントを削除する
	_evdata->unloadMapEvent();

	// 新しいマップイベントを読み込む
	if(!_evdata->loadMapEvent(mapid))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ストーリーの読み込み
**//*---------------------------------------------------------------------*/
bool EventSys::loadStory(s32 storyid)
{
	// 今までのストーリーイベントを削除する
	_evdata->unloadStoryEvent();

	// 新しいストーリーイベントを読み込む
	if(!_evdata->loadStoryEvent(storyid))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	クエストの読み込み
**//*---------------------------------------------------------------------*/
bool EventSys::loadQuest(const QuestArticle* qstatc)
{
	// 新しいクエストイベントを読み込む
	if(!_evdata->loadQuestEvent(qstatc))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	クエストの解放
**//*---------------------------------------------------------------------*/
void EventSys::unloadQuest(const QuestArticle* qstatc)
{
	_evdata->unloadQuestEvent(qstatc);
}

#if 0
/*---------------------------------------------------------------------*//**
	マップ開始時のイベントの発動
**//*---------------------------------------------------------------------*/
EventMatter* EventSys::beginMapStart(u32 param)
{
	// このマップイベント開始は旧方式．Map::beginMapStartEvent を推奨とする
	EvCause evcause(EventCause::CID_MAPSTART);
	return (EventMatter*)begin(0, param, EventMatter::CondFlags::FLG_AUTOBGN_MAPSTART, 0, &evcause);
}
#endif

/*---------------------------------------------------------------------*//**
	テンポラリイベントの発動
	 
	@param	tmpevid		テンポラリイベント ID（イベント ID とは異なる）
**//*---------------------------------------------------------------------*/
bool EventSys::beginTmpEvent(s32 tmpevid, u16 evid, u32 param, const EvCause* cause)
{
	// テンポラリイベントは即座に読み込む（将来的に変えるかも）
	if(!_evdata->loadTmpEvent(tmpevid))	{	return false;	}

	// イベント発動
	return (EventMatter*)begin(evid, param, 0, cause) != 0L;
}

/*---------------------------------------------------------------------*//**
	テンポラリイベントの発動
	 
	@param	tmpevid		テンポラリイベント ID（イベント ID とは異なる）
**//*---------------------------------------------------------------------*/
void EventSys::removeUnitReference(const Unit* unit)
{
	for(int i = 0; i < _evdata->getMatterNum(); i++)
	{
		EventMatter* matter = (EventMatter*)_evdata->getMatter(i);
		if(matter->getCause()->getParameter() == unit)
		{
			matter->cause()->setParameter(0L);
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EventSys::EventSys()
	: _evmfact(0L)
	, _evdata(0L)
///	, _gmkAfterEvent(0)
	, _flagsSp(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EventSys::~EventSys()
{
	ASSERT(_evmfact == 0L);
	ASSERT(_evdata == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EventSys::create()
{
	// イベントマターファクトリの作成
	_evmfact = new EventMatterFactory();

	// イベントデータの作成
	_evdata = new EventData();
	ASSERT(QuestManager::NUM_QUEST_SLOT_MAX == (MsgDataKind::MSGDKIND_QUEST__END_ - MsgDataKind::MSGDKIND_QUEST__START_));
	if(!_evdata->create(_evmfact, MsgDataKind::NUM_MSGDKIND))
	{
		return false;
	}

	// 親クラスによる作成
	if(!EvSys::create(_evdata))
	{
		return false;
	}

	#if 0
		// ムービー時に台詞ポーズを行うかどうかをシステムコンフィグから得る
		if((Game::getGame()->getSaveSys() != 0L) && (Game::getGame()->getSaveSys()->getSysConf() != 0L))
		{
			SysConf* sysconf = Game::getGame()->getSaveSys()->getSysConf();
			setEnableWaitMovieMessage(sysconf->isEnableWaitMovieMessage());
		}
	#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EventSys::destroy()
{
	// 親クラスによる破棄
	EvSys::destroy();

	// イベントデータの削除
	if(_evdata != 0L)
	{
		_evdata->destroy();
		delete _evdata;
		_evdata = 0L;
	}

	// イベントマターファクトリの削除
	delete _evmfact;
	_evmfact = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void EventSys::exec(ExecRes* res, const ExecCtx* ec)
{
	if(_evdata == 0L)	{	return;	}
	if(isEventPause())	{	return;	}	// イベントポーズ中は抜ける

	EvSys::exec(res, ec);

	// テンポラリマターフラグ
	bool isExistTmpMatter = false;
	bool isRunTmpMatter = false;

	// テンポラリイベントの情報収集
	for(int i = 0; i < _evdata->getMatterNum(); i++)
	{
		EventMatter* matter = (EventMatter*)_evdata->getMatter(i);

		bool isTmpMatter = false;
		if((EventIdDef::EVID_TMP__START_ <= matter->getEvId()) && (matter->getEvId() <= EventIdDef::EVID_TMP__END_))
		{
			isTmpMatter = true;
			isExistTmpMatter = true;	// テンポラリマターが存在している
		}

		if(isTmpMatter && matter->isBegin())
		{
			isRunTmpMatter = true;		// テンポラリマターが実行されている
		}
	}

	// テンポラリマターは存在するが、実行されていない場合は削除する
	if(isExistTmpMatter && !isRunTmpMatter)
	{
		_evdata->unloadTmpEvent();
	}

	// ロックフラグによるゲームモードの変更
	if(Game::getGame()->getGameMode()->getChildEndModeKind() == GameMode::MODE_GAME_INGAME_EVENT)
	{
		if(!isOperationLock())	// ロックしない
		{
#if 1
			// イベントモードを戻す
			Game::getGame()->getGameMode()->changePrevEndMode(GameMode::MODE_GAME_INGAME_EVENT);
#else
			// ゲームモードを戻す
			if(_gmkAfterEvent != 0)
			{
				Game::getGame()->getGameMode()->changeEndMode((GameMode::ModeKind)_gmkAfterEvent);
				_gmkAfterEvent = 0;
TRACELN(" - 1 -> _gmkAfterEvent=%d", _gmkAfterEvent);
			}
#endif
		}
	}
	else	// イベントモードではない
	{
		if(isOperationLock())	// ロック中である
		{
			// イベントモードに変更
#if 0
			if(_gmkAfterEvent == 0)	// setEventAfterGameMode などで未設定の場合
			{
				_gmkAfterEvent = Game::getGame()->getGameMode()->getChildEndModeKindConsiderReserved();	// 変更前のモードを取得
TRACELN(" - 2 -> _gmkAfterEvent=%d", _gmkAfterEvent);
			}
#endif
			Game::getGame()->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_EVENT);
		}
	}
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void EventSys::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	// 実行中のマターに伝達
	for(int i = 0; i < _evdata->getMatterNum(); i++)
	{
		EventMatter* matter = (EventMatter*)_evdata->getMatter(i);
		if(matter->isBegin())
		{
			matter->notifyUnitInfluenceStat(unitRecv, scres, tstat);
		}
	}
}

/*---------------------------------------------------------------------*//**
	ソウマ発射通知
**//*---------------------------------------------------------------------*/
void EventSys::notifyDischargeSouma(Souma* souma, u16 mcid)
{
	// 実行中のマターに伝達
	for(int i = 0; i < _evdata->getMatterNum(); i++)
	{
		EventMatter* matter = (EventMatter*)_evdata->getMatter(i);
		if(matter->isBegin())
		{
			matter->notifyDischargeSouma(souma, mcid);
		}
	}
}

/*---------------------------------------------------------------------*//**
	マジッククラスタの消滅通知
**//*---------------------------------------------------------------------*/
void EventSys::notifyDisappearMagicCluster(MagicCluster* mc, u32 dcflags)
{
	// 実行中のマターに伝達
	for(int i = 0; i < _evdata->getMatterNum(); i++)
	{
		EventMatter* matter = (EventMatter*)_evdata->getMatter(i);
		if(matter->isBegin())
		{
			matter->notifyDisappearMagicCluster(mc, dcflags);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
