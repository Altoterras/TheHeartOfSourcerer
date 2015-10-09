/***********************************************************************//**
 *	StoryManager.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/06.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StoryManager.h"

// Friends
#include "FixedSourceIdDef.h"
#include "MapIdDef.h"
#include "Situation.h"
#include "char/pc/PartnerUnit.h"
#include "item/ItemIdDef.h"
#include "mapmod/MapMod.h"
#include "mapmod/MapModFactory.h"
#include "save/SaveIdDef.h"
#include "save/SaveStructure.h"
#include "stmod/StMod.h"
#include "stmod/StModFactory.h"
#include "worldmap/WorldMap.h"
#include "../episode/party/MyParty.h"
#include "../event/AutoBeginEventInfo.h"
#include "../event/EventData.h"
#include "../event/EventIdDef.h"
#include "../event/EventIdInfoArray.h"
#include "../event/EventSys.h"
#include "../event/matter/EventMatter.h"
#include "../event/matter/EventMatterType.h"
#include "../event/matter/OverrideEventMatter.h"
#include "../body/Game.h"
#include "../body/MoveMap.h"
#include "../item/ItemManager.h"
#include "../map/Map.h"
#include "../map/PlacementObj.h"
#include "../mode/GameMode.h"
#include "../quest/QuestManager.h"
#include "../save/SaveData.h"
#include "../save/SaveSys.h"
#include "../source/SourceManager.h"

// External
#include "../../etk/effect/ScreenEffect.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StoryManager メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ニューゲーム
**//*---------------------------------------------------------------------*/
bool StoryManager::newGame(bool isRestart, u32 cntClear)
{
	// セーブフラグの再初期化
	SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
	svd->setSaveF(SaveIdDef::SFID_ST10101_MEET_CORPO, false);					// [St10101] オープニング後にコルポと合流していない
	svd->setSaveF(SaveIdDef::SFID_ST10301_CORPO_SAY_OUTHOUSE, false);			// [St10301] [St10301] コルポが納屋に寄ってくれと言っていない
	svd->setSaveF(SaveIdDef::SFID_ST10301_READ_CERJA_LETTER, false);			// [St10301] セルジャの手紙を読んでいない
	svd->setSaveF(SaveIdDef::SFID_ST10304_TALK_KABAL_BEFORE_REBECCA, false);	// [St10304] レベッカと話す前にカバルと話していない
	svd->setSaveF(SaveIdDef::SFID_ST10306_TOOK_CERJA_LETTER, false);			// [St10306] セルジャの手紙を取得していない
	svd->setSaveF(SaveIdDef::SFID_ST10401_DONE_YASAN_THANK, false);				// [St10401] コルポのアサ出迎えイベントをデフォルトのものと置き換えて、ヤサンからソウマを受け取るイベントを再生していない
	///svd->setSaveF(SaveIdDef::SFID_ST10401_CLEAR_VOL01, true);				// [St10401] 上巻（Vol01）をクリアしていない
	svd->setSaveF(SaveIdDef::SFID_ST10204_GET_LUMIN_SOUMA_FROM_HERMIT, false);	// [St10204] ハーミットから発光のソウマを得ていない
	svd->setSaveF(SaveIdDef::SFID_ST10204_MEET_KOTSUME, false);					// [St10401] コツメと合流していない
	svd->setSaveF(SaveIdDef::SFID_ST10401_CLEAR_ASA_EVENT, false);				// [St10204] アサイベントをクリアしていない
	svd->setSaveF(SaveIdDef::SFID_ST10304_GET_FIRE_SOUMA_FROM_REBECCA, false);	// [St10304] レベッカと話して火の強化ソウマを得ていない
	svd->setSaveF(SaveIdDef::SFID_ST10306_CLEAR_CORPO_EVENT, false);			// [St10306] コルポイベントをクリアしていない
	svd->setSaveF(SaveIdDef::SFID_ST10305_ONCE_DONE_REIKA_APPEAR, false);		// [St10305] 一度レイカを出現させていない（レイカの勘違いでレイカが消えていない）
	svd->setSaveF(SaveIdDef::SFID_ST10203_CORPO_SAY_TOGETHER, false);			// [St10203] アサダウン後コルポ合流時のイベントを見ていない
	///svd->setSaveF(SaveIdDef::SFID_ST10101_PICKUP_TUTORIAL_STONE, false);		// [St10101] アイテム拾いチュートリアルの石を見ている
	svd->setSaveF(SaveIdDef::SFID_ST10203_DONE_KABAL_TALK, false);				// [St10203] カバルと会話してアイテムを取得していない
	svd->setSaveF(SaveIdDef::SFID_ST10204_DONE_KABAL_TALK, false);				// [St10204] カバルと会話してアイテムを取得していない
	svd->setSaveF(SaveIdDef::SFID_ST10401_DONE_KABAL_TALK, false);				// [St10401] カバルと会話してアイテムを取得していない
	///svd->setSaveF(SaveIdDef::SFID_ST10000_GET_ITEM_SYNTHE_SOUMA, false);		// [St10000] アイテム合成ソウマを得ている
	svd->setSaveF(SaveIdDef::SFID_ST10000_TALK_STONE_COLLECTOR, false);			// [St10000] 石コレクター（タケリ）と話している

	// ストーリー読み込み
	loadStory(10101);

	// デフォルトアイテムの追加
	setupDefaultItems();
	// デフォルトのソースリストに設定する
	setupDefaultSourceList(false);

	// 隠しダンジョンへはクリア後リスタートして大猿を倒したら行けるが、２回目クリアでも行けるようにする
	if(isRestart)	// リスタート済みの場合
	{
		if(cntClear >= 2)	// ２回目クリア済みの場合
		{
			// ワールドマップに隠しダンジョンを追加
			_wmap->enableNewMap(MapIdDef::MAPID_EP01_FOREST4_SECRET);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	マップに直接入る（デバッグ用）
**//*---------------------------------------------------------------------*/
void StoryManager::debug_enterMap(s32 storyid)
{
#if defined(_DEBUG)
	// ストーリー読み込み
	loadStory(storyid);

	// デフォルトアイテムの追加
	setupDefaultItems();
	// デフォルトのソースリストに設定する
	setupDefaultSourceList(false);

	// 全てのマップを表示
	_wmap->debug_onDebugLevel(1);

	// 新しいクエストの起動
	QuestManager* qstmng = Game::getGame()->getQuestManager(); ASSERT(qstmng != 0L);
	qstmng->bootQuestToMeet();

	// 強制的に半覚醒
	#if 0
		CharUnit* unitMy = (CharUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
		if(unitMy != 0L)
		{
			TFW_SET_FLAG(*unitMy->charStat()->debug_statFlags(), CharStat::SPSTAT_TRANS, true);
		}
	#endif
#endif
}

/*---------------------------------------------------------------------*//**
	途中再開用の開始イベントを設定する
**//*---------------------------------------------------------------------*/
void StoryManager::reserveEventForResume(s32 evidStartup, bool isLoadIntvlEv)
{
	_listAutoBeginEvi->removeAll();	// 現在ある自動起動イベントをクリアする
	if(evidStartup > 0)
	{
		AutoBeginEventInfo* abevi = new AutoBeginEventInfo();
		if(isLoadIntvlEv)
		{
			_isLoadIntervalEventSave = true;
			abevi->setupForLoadIntervalEventStart(evidStartup);
		}
		else
		{
			abevi->setupForStoryStart(evidStartup);
		}
		addAutoBeginEvent(abevi);
	}
}

/*---------------------------------------------------------------------*//**
	次のストーリーを予約
**//*---------------------------------------------------------------------*/
void StoryManager::reserveNextStory(s32 storyid)
{
	// ストーリーを予約
	ASSERT(_storyidReserved == 0);
	_storyidReserved = storyid;

	// 次のストーリー開始まで、たいていの場合、
	// フェードアウトとフェードインの間の画面が表示されてしまうので、
	// １フレームの塗りつぶしを入れる
	ScreenEffect* screff = Game::getGame()->getSceneScreenEffect();
	// 色設定
	ColorU8 col1(0, 0, 0, 255);
	screff->setColor1(&col1);	// 現仕様では黒のみ対応
	// フレーム数設定
	screff->setMaxFrame(1);
	// 実行
	screff->start(ScreenEffect::MODE_FILL);
}

/*---------------------------------------------------------------------*//**
	ストーリー準備設定
**//*---------------------------------------------------------------------*/
void StoryManager::setup(s32 evidStartup)
{
	if(evidStartup > 0)
	{
		AutoBeginEventInfo* abevi = new AutoBeginEventInfo();
		abevi->setupForStoryStart(evidStartup);
		addAutoBeginEvent(abevi);
	}
}

/*---------------------------------------------------------------------*//**
	配置オブジェクト設定の追加
**//*---------------------------------------------------------------------*/
void StoryManager::addPlacementObjSetting(PlacementObj* pobjEntr)
{
	// 自動割当制御イベント ID
	if(pobjEntr->getCtrlEvid() == 0)
	{
		pobjEntr->setCtrlEvid(_ctrlevidAutoCnt);
		_ctrlevidAutoCnt++;
		ASSERT(_ctrlevidAutoCnt <= EventIdDef::CTRLEVID_STORY__END_);
	}

	// リストに追加
	ASSERT(_listPlaceObj != 0L);
	_listPlaceObj->addTail(pobjEntr);
}

/*---------------------------------------------------------------------*//**
	置換イベント設定の追加
**//*---------------------------------------------------------------------*/
void StoryManager::addOverrideEvent(OverrideEventMatter* oemEntr)
{
	ASSERT(_listOvrdEvm != 0L);
	_listOvrdEvm->addTail(oemEntr);
}

/*---------------------------------------------------------------------*//**
	自動起動イベントの追加
**//*---------------------------------------------------------------------*/
void StoryManager::addAutoBeginEvent(AutoBeginEventInfo* abeviEntr)
{
	ASSERT(_listAutoBeginEvi != 0L);
	_listAutoBeginEvi->addTail(abeviEntr);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StoryManager::StoryManager()
	: _evsysRef(0L)
	, _wmap(0L)
	, _situ(0L)
	, _stmod(0L)
	, _mapmod(0L)
	, _storyid(0)
	, _listPlaceObj(0L)
	, _listOvrdEvm(0L)
	, _listAutoBeginEvi(0L)
	, _isPlayedStoryStartupEvent(false)
	, _isLoadIntervalEventSave(false)
	, _ctrlevidAutoCnt(0)
	, _storyidReserved(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StoryManager::~StoryManager()
{
	ASSERT(_wmap == 0L);
	ASSERT(_situ == 0L);
	ASSERT(_stmod == 0L);
	ASSERT(_mapmod == 0L);
	ASSERT(_listPlaceObj == 0L);
	ASSERT(_listOvrdEvm == 0L);
	ASSERT(_listAutoBeginEvi == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool StoryManager::create(EventSys* evsysRef)
{
	// 参照を保存
	_evsysRef = evsysRef;

	// 世界地図を作成
	_wmap = new WorldMap();
	if(!_wmap->create())
	{
		delete _wmap;
		_wmap = 0L;
		ASSERT(false);
	}

	// シチュエーションを作成
	_situ = new Situation();
	if(!_situ->create())
	{
		delete _situ;
		_situ = 0L;
		ASSERT(false);
	}

	// 配置リストを作成
	_listPlaceObj = new List<PlacementObj*>(true);
	if(_listPlaceObj == 0L)		{	goto FAILED_END;	}

	// 置換イベントリストを作成
	_listOvrdEvm = new List<OverrideEventMatter*>(true);
	if(_listOvrdEvm == 0L)		{	goto FAILED_END;	}

	// 自動起動イベントリストを作成
	_listAutoBeginEvi = new List<AutoBeginEventInfo*>(true);
	if(_listAutoBeginEvi == 0L)	{	goto FAILED_END;	}

	// ダミーのストーリーモジュールを作成
	_stmod = StModFactory::makeStMod(0, this);
	if(_stmod == 0L)			{	goto FAILED_END;	}

	// 変数初期化

	return true;

FAILED_END:
	delete _stmod;
	_stmod = 0L;

	delete _listAutoBeginEvi;
	_listAutoBeginEvi = 0L;

	delete _listOvrdEvm;
	_listOvrdEvm = 0L;

	delete _listPlaceObj;
	_listPlaceObj = 0L;

	return false;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void StoryManager::destroy()
{
	// マップモジュールを削除
	if(_mapmod != 0L)
	{
		_mapmod->destroy();
		delete _mapmod;
		_mapmod = 0L;
	}

	// ストーリーモジュールを削除
	if(_stmod != 0L)
	{
		_stmod->destroy();
		delete _stmod;
		_stmod = 0L;
	}
	
	// 自動起動イベントリストを削除
	if(_listAutoBeginEvi != 0L)
	{
		delete _listAutoBeginEvi;
		_listAutoBeginEvi = 0L;
	}

	// 置換イベントリストを削除
	if(_listOvrdEvm != 0L)
	{
		delete _listOvrdEvm;
		_listOvrdEvm = 0L;
	}
	
	// 配置リストを削除
	if(_listPlaceObj != 0L)
	{
		delete _listPlaceObj;
		_listPlaceObj = 0L;
	}

	// シチュエーションの削除
	if(_situ != 0L)
	{
		_situ->destroy();
		delete _situ;
		_situ = 0L;
	}

	// 世界地図を削除
	if(_wmap != 0L)
	{
		_wmap->destroy();
		delete _wmap;
		_wmap = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StoryManager::exec(ExecRes* res, const ExecCtx* ec)
{
	// ストーリー予約を解決する
	if(_storyidReserved != 0)
	{
		// ストーリー読み込み
		loadStory(_storyidReserved);
		_storyidReserved = 0;

		// 新しいクエストの起動
		QuestManager* qstmng = Game::getGame()->getQuestManager(); ASSERT(qstmng != 0L);
		qstmng->bootQuestToMeet();

		// スタートアップイベントを起動
		beginStoryStartEvent();
	}

	// ストーリーモジュールのフレーム制御
	if(_stmod != 0L)
	{
		_stmod->exec(res, ec);
	}

	// マップモジュールのフレーム制御
	if(_mapmod != 0L)
	{
		_mapmod->exec(res, ec);
	}
}

/*---------------------------------------------------------------------*//**
	ゲームスタートモード開始通知
**//*---------------------------------------------------------------------*/
void StoryManager::notifyBeginStartGameMode()
{
	// スタートアップイベントを起動
	beginStoryStartEvent();

	// ストーリーの判断でモードをインゲームに変更
	Game::getGame()->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME);
}

/*---------------------------------------------------------------------*//**
	インゲームモード開始通知
**//*---------------------------------------------------------------------*/
void StoryManager::notifyBeginIngameMode()
{
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知

	@retval	true	そのままマップ変更を許可する
	@retval false	マップ変更を許可しない
**//*---------------------------------------------------------------------*/
bool StoryManager::notifyChangeMapBefore(s32* mapidNew, s32 mapidOld)
{
	if(_mapmod != 0L)
	{
		_mapmod->end();
		delete _mapmod;
		_mapmod = 0L;
	}

	ASSERT(_stmod != 0L);
	return _stmod->notifyChangeMapBefore(mapidNew, mapidOld);
}

/*---------------------------------------------------------------------*//**
	マップ変更後通知
**//*---------------------------------------------------------------------*/
void StoryManager::notifyChangedMap(bool isDeserializeCase)
{
	Map* map = Game::getGame()->getMap(); ASSERT(map != 0L);
	MoveMap* mvmap = Game::getGame()->getMoveMap(); ASSERT(mvmap != 0L);

	// マップに脇役を配置
	for(ListIterator<PlacementObj*> it = _listPlaceObj->iterator(); it.has(); it.next())
	{
		PlacementObj* pobj = it.object();

		if(isDeserializeCase)	// セーブデータ読み込み時
		{
			if(pobj->isPlaced())		// 配置済み
			{
				map->placePlacementObj(pobj, true);		// マップ配置を復元
			}
			else if(pobj->isNewPlacedCondition())
			{
				map->placePlacementObj(pobj, false);	// マップに通常条件配置
			}
		}
		else
		{
			if(pobj->isNewPlacedCondition())
			{
				map->placePlacementObj(pobj, false);	// マップに配置
			}
			else if(pobj->isPlaced())	// 配置済み
			{
				map->unplacePlacementObj(pobj);			// 配置解除
			}
		}
	}

	// マップ置換イベントを処理
	EventData* evdat = _evsysRef->getEventData();
	for(ListIterator<OverrideEventMatter*> it = _listOvrdEvm->iterator(); it.has(); it.next())
	{
		const OverrideEventMatter* oem = it.object();
		if(oem->getKind() == OverrideEventMatter::KIND_MAP)	// タイプがマップのもののみ
		{
			if(oem->isCondition())
			{
				evdat->overrideMatter(oem);
			}
		}
	}

	// マップモジュールを作成
	_mapmod = MapModFactory::makeMapMod(mvmap->getCurMapId()); ASSERT(_mapmod != 0L);

	// マップ開始時イベントを起動
	beginAutoBeginEvent(AutoBeginEventInfo::KIND_MAP_START);

	// 各通知処理
	ASSERT(_stmod != 0L);
	_stmod->onDecisionParty(mvmap->getCurMapId(), mvmap->getPrevMapId());
	_stmod->notifyChangedMap(mvmap->getCurMapId(), mvmap->getPrevMapId());
	_mapmod->begin();
	_wmap->notifyChangedMap();
}

/*---------------------------------------------------------------------*//**
	イベントモード変更通知
**//*---------------------------------------------------------------------*/
void StoryManager::notifyChangedEventMode(bool isEventMode)
{
	ASSERT(_stmod != 0L);
	_stmod->notifyChangedEventMode(isEventMode);
}

/*---------------------------------------------------------------------*//**
	リアルタイムムービーの再生状態変更通知
**//*---------------------------------------------------------------------*/
void StoryManager::notifyChangedRtmvPlaying(bool isPlay, u16 evidRtmvMatter)
{
	ASSERT(_stmod != 0L);
	_stmod->notifyChangedRtmvPlaying(isPlay, evidRtmvMatter);
}

/*---------------------------------------------------------------------*//**
	家での休息通知
**//*---------------------------------------------------------------------*/
void StoryManager::notifyRestAtHome()
{
	ASSERT(_stmod != 0L);
	_stmod->onRestAtHome();
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void StoryManager::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	ASSERT(_stmod != 0L);
	return _stmod->notifyUnitInfluenceStat(unitRecv, scres, tstat);
}

/*---------------------------------------------------------------------*//**
	無意識フォーカス検索通知
**//*---------------------------------------------------------------------*/
bool StoryManager::notifyFindUnconsciousFocus(Unit** unitFind)
{
	ASSERT(_stmod != 0L);
	return _stmod->notifyFindUnconsciousFocus(unitFind);
}

/*---------------------------------------------------------------------*//**
	ソウマ発射通知
**//*---------------------------------------------------------------------*/
void StoryManager::notifyDischargeSouma(Souma* souma, u16 mcid)
{
	ASSERT(_stmod != 0L);
	return _stmod->notifyDischargeSouma(souma, mcid);
}

/*---------------------------------------------------------------------*//**
	アイテム拾い通知
**//*---------------------------------------------------------------------*/
void StoryManager::notifyPickupItem(const Unit* unitActor, const Unit* unitItem)
{
	ASSERT(_stmod != 0L);
	_stmod->notifyPickupItem(unitActor, unitItem);
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool StoryManager::serialize(Serializer* ser) const
{
	// 実行中の Act イベント ID を得る
	u16 evidActing = 0;
	EventData* evdat = Game::getGame()->getEventSys()->getEventData();	ASSERT(evdat != 0L);
	for(int imat = 0; imat < evdat->getEventMatterNum(); imat++)
	{
		EventMatter* matterWk = evdat->getEventMatter(imat);
		if((matterWk->getMatterType() == EMTYPE_ACT) && matterWk->isBegin())	// 開始済み ActMatter 判定
		{
			evidActing = matterWk->getEvId();
		}
	}

	// ストーリーの保存
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		svst->_gamesvst._storyid = _storyid;										// ストーリー ID の書き込み
		svst->_gamesvst._isPlayedStoryStartupEvent = _isPlayedStoryStartupEvent;	// ストーリー開始イベント起動フラグの書き込み
		svst->_gamesvst._evidActing = evidActing;									// 実行中の Act イベント ID の書き込み
	}
	else						// ダイレクトモード
	{
		ser->store(&_storyid);						// ストーリー ID の書き込み
		ser->store(&_isPlayedStoryStartupEvent);	// ストーリー開始イベント起動フラグの書き込み
		ser->store(&evidActing);					// 実行中の Act イベント ID の書き込み
	}

	// 配置オブジェクトの状態を保存する
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		if(!svst->storePlacementObjData(_listPlaceObj))
		{
			return false;
		}
	}
	else						// ダイレクトモード
	{
		s32 numPobj = _listPlaceObj->getCount();
		ser->store(&numPobj);
		for(ListIterator<PlacementObj*> it = _listPlaceObj->iterator(); it.has(); it.next())
		{
			const PlacementObj* pobj = it.object();
			pobj->serialize(ser);
		}
	}

	// ワールドマップのシリアライズ
	_wmap->serialize(ser);

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool StoryManager::deserialize(Serializer* ser)
{
	// ストーリーデータの読み込み
	s32 storyid = 0;
	u16 evidActing = 0;
	bool isPlayedStoryStartupEvent = false;
	bool isEvIntvl = TFW_IS_FLAG(Game::getGame()->getSaveSys()->getSaveData()->getLoadTimeSaveFlags(), SaveData::SVF_EVENT_INTVL);
	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		storyid = svst->_gamesvst._storyid;										// ストーリー ID 値の読み込み
		isPlayedStoryStartupEvent = svst->_gamesvst._isPlayedStoryStartupEvent;	// ストーリー開始イベント起動フラグの読み込み
		evidActing = svst->_gamesvst._evidActing;								// 実行中の Act イベント ID の読み込み
	}
	else						// ダイレクトモード
	{
		ser->restore(&storyid);						// ストーリー ID 値の読み込み
		ser->restore(&isPlayedStoryStartupEvent);	// ストーリー開始イベント起動フラグの読み込み
		ser->restore(&evidActing);					// 実行中の Act イベント ID の読み込み
	}

	// Volume 1 Lite 版はストーリーが 10203 まで（この実装はあまり美しくない…．オーバーライドによる実装を要検討【2010/10/29 r-kishi】）
	#if defined(_ENLIGHT_V01_LITE)
		if(storyid > 10203)
		{
			storyid	= 10203;
			isPlayedStoryStartupEvent = false;
			Game::getGame()->getSaveSys()->getSaveData()->setSaveF(SaveIdDef::SFID_ST10203_CORPO_SAY_TOGETHER, false);	// [St10203] アサダウン後コルポ合流時のイベントを見ていない
		}
	#endif

	// ストーリーのロード
	loadStory(storyid);	// ストーリーを読み込む
	_isPlayedStoryStartupEvent = isPlayedStoryStartupEvent;

	// 配置オブジェクトの状態を読み込む
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		if(!svst->restorePlacementObjData(_listPlaceObj))
		{
			return false;
		}
	}
	else						// ダイレクトモード
	{
		s32 numPobj = 0;
		ser->restore(&numPobj);
		ASSERT(_listPlaceObj->getCount() == numPobj);
		ListIterator<PlacementObj*> it = _listPlaceObj->iterator();
		for(int i = 0; i < numPobj; i++)
		{
			if(!it.has())
			{
				ASSERT(false);
				return false;
			}
			PlacementObj* pobj = it.object();
			pobj->deserialize(ser);
			it.next();
		}
	}

	// ワールドマップのデシリアライズ
	_wmap->deserialize(ser);

	// イベント途中セーブデータは再開時イベントをセットする
	if(isEvIntvl && (evidActing != 0))
	{
		reserveEventForResume(evidActing, true);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デバッグモード設定
**//*---------------------------------------------------------------------*/
void StoryManager::debug_onDebugLevel(s32 lv)
{
	_wmap->debug_onDebugLevel(lv);
}

/*---------------------------------------------------------------------*//**
	ストーリー読み込み
**//*---------------------------------------------------------------------*/
bool StoryManager::loadStory(s32 storyid)
{
	// 新しいストーリー ID
	releaseStory();
	_storyid = storyid;

	// 新しいストーリーオブジェクトの設定
	_stmod = StModFactory::makeStMod(_storyid, this);	ASSERT(_stmod != 0L);

	// 配置オブジェクト情報をクリアする
	_listPlaceObj->removeAll();
	_ctrlevidAutoCnt = EventIdDef::CTRLEVID_STORY__START_;	// 自動割当カウンタリセット

	if(_storyid == 0)
	{
		// NULL ストーリーでセットアップ
		setup(0);
	}
	else
	{
		// ストーリーイベント読み込み
		_evsysRef->loadStory(_storyid);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ストーリーの解放
**//*---------------------------------------------------------------------*/
void StoryManager::releaseStory()
{
	// ストーリーを削除
	if(_stmod != 0L)
	{
		_stmod->destroy();
		delete _stmod;
		_stmod = 0L;
	}

	// マップ置換イベントを戻す
	EventData* evdat = _evsysRef->getEventData();
	for(ListIterator<OverrideEventMatter*> it = _listOvrdEvm->iterator(); it.has(); it.next())
	{
		const OverrideEventMatter* oem = it.object();
		if(oem->getKind() == OverrideEventMatter::KIND_MAP)	// タイプがマップのもののみ
		{
			evdat->unoverrideMatter(oem);
		}
	}
	_listOvrdEvm->removeAll();	// 置換イベント情報クリア

	// 配置オブジェクト情報クリア
	for(ListIterator<PlacementObj*> it = _listPlaceObj->iterator(); it.has(); it.next())
	{
		PlacementObj* pobj = it.object();
		if(pobj->getMapId() == Game::getGame()->getMoveMap()->getCurMapId())
		{
			Game::getGame()->getMap()->unplacePlacementObj(pobj);	// 配置解除
		}
	}
	_listPlaceObj->removeAll();

	// 自動起動イベントリストをクリア
	_listAutoBeginEvi->removeAll();

	// その他の値をクリア
	_storyid = 0L;
	_isPlayedStoryStartupEvent = false;
}

/*---------------------------------------------------------------------*//**
	ストーリー開始イベント実行
**//*---------------------------------------------------------------------*/
void StoryManager::beginStoryStartEvent()
{
	if(!_isPlayedStoryStartupEvent)
	{
		beginAutoBeginEvent(AutoBeginEventInfo::KIND_STORY_START);
		_isPlayedStoryStartupEvent = true;
	}
	if(_isLoadIntervalEventSave)
	{
		beginAutoBeginEvent(AutoBeginEventInfo::KIND_LOAD_INTVEV_START);
		_isLoadIntervalEventSave = false;
	}
}

/*---------------------------------------------------------------------*//**
	自動開始イベントの開始
**//*---------------------------------------------------------------------*/
bool StoryManager::beginAutoBeginEvent(s32 kind)
{
	bool isBegin = false;

	for(ListIterator<AutoBeginEventInfo*> it = _listAutoBeginEvi->iterator(); it.has(); it.next())
	{
		AutoBeginEventInfo* abevi = it.object();
		if(abevi->getKind() == kind)	// 同じ種別
		{
			// イベント起動原因と起動判定
			short cid = EventCause::CID_NULL;
			bool isCall = true;
			switch(kind)
			{
			case AutoBeginEventInfo::KIND_STORY_START:
				cid = EventCause::CID_STORYSTART;
				break;
			case AutoBeginEventInfo::KIND_LOAD_INTVEV_START:
				cid = EventCause::CID_INTVLEVSTART;
				break;
			case AutoBeginEventInfo::KIND_MAP_START:
				cid = EventCause::CID_MAPSTART;
				if(Game::getGame()->getMoveMap()->getCurMapId() != abevi->getMapId())
				{
					isCall = false;
				}
				break;
			}

			// イベント起動
			if(isCall)
			{
				u16 evid = abevi->getCallEvi()->getInfo(abevi->getActiveCallEviIndex())->getEvid();
				if(evid != 0)	// わざわざ起動しない設定で上書きすることもありうる
				{
					EvCause evcause(cid);
					EventMatter* matter = (EventMatter*)_evsysRef->begin(evid, 0, &evcause);
					if(matter != 0L)
					{
						isBegin = true;

						// イベントが複数定義されている場合は、次に送る
						if(abevi->getCallEvi()->getInfoNum() >= 2)
						{
							abevi->nextCallEvi(false);
						}
					}
				}
			}
		}
	}

	return isBegin;
}

/*---------------------------------------------------------------------*//**
	デフォルトアイテムの追加
**//*---------------------------------------------------------------------*/
void StoryManager::setupDefaultItems()
{
	ItemManager* itmmng = ((MyParty*)Game::getGame()->getMyParty())->getItemManager();

	// 武器が無い場合
	{
		ItemArray itmarr = itmmng->findByCategory(ItemDef::CAT_WEAPON, true);
		if(itmarr.getCount() <= 0)	// 武器がない
		{
			// デフォルトのこん棒を追加
			itmmng->newItem(ItemIdDef::ITEMDID_WEP_FIRST, 0L, Item::F_NOEDIT, true);	// 編集不可で末尾に追加
		}
	}
	
	// 盾が無い場合
	{
		ItemArray itmarr = itmmng->findByCategory(ItemDef::CAT_SHIELD, true);
		if(itmarr.getCount() <= 0)	// 盾がない
		{
			itmarr = itmmng->findByItemDefId(ItemIdDef::ITEMDID_DUMMY, true);
			if(itmarr.getCount() <= 0)	// 盾入手場所確保用のダミーアイテムもない
			{
				// 盾入手場所確保用のダミーアイテムを追加
				itmmng->newItem(ItemIdDef::ITEMDID_DUMMY, 0L, Item::F_NOEDIT, true);		// 編集不可で末尾に追加
			}

			// ダミーを盾に変更
			if(_storyid >= StoryIdDef::STID_EP01_ASA__START_)
			{
				itmarr = itmmng->findByItemDefId(ItemIdDef::ITEMDID_DUMMY, true);	// 予約用ダミーの検索
				if(itmarr.getCount() > 0)
				{
					// 盾入手場所確保用のダミーアイテムを削除
					Item* item = itmarr.item(0);
					itmmng->removeItem(item->getItemGenId());
					// 盾の追加
					itmmng->newItem(ItemIdDef::ITEMDID_SHLD_FIRST, 0L, Item::F_NOEDIT, true);	// 編集不可を末尾に
				}
			}
		}
	}

	// 最強装備に
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	PartnerUnit* unitPc2 = (PartnerUnit*)party->pcUnit(MyParty::PC_PTN);
	unitPc2->equipBest();
}

/*---------------------------------------------------------------------*//**
	デフォルトのソースリストに設定する
**//*---------------------------------------------------------------------*/
void StoryManager::setupDefaultSourceList(bool isReserveOnly)
{
	MyParty* party = ((MyParty*)Game::getGame()->getMyParty());
	SourceManager* srcmng = party->getSourceManager();

	// ソース予約処理
	party->reserveDefaultSourceList();

	// 基本固定ソースの追加
	if(!isReserveOnly)
	{
		srcmng->addFixedSource(FixedSourceIdDef::FSRCID_EP01_AIER);			// トランス
		srcmng->addFixedSource(FixedSourceIdDef::FSRCID_EP01_NERUROS);		// 水飛沫
		srcmng->addFixedSource(FixedSourceIdDef::FSRCID_EP01_NERUHAMAD);	// 水のベール
	}

	// 空きソースを並び替える
//	srcmng->sortForEmptySource();
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
