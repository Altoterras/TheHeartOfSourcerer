/***********************************************************************//**
 *	StoryManager.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/06.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STORY_MANAGER_H_
#define _SRCR_EPISODE_STORY_MANAGER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"
#include "../../tfw/serialize/SerializeBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

SRCR_BEGIN_NS

class AutoBeginEventInfo;
class StatusChangeResult;
class CharUnit;
class EventMatter;
class EventSys;
class Game;
class MapMod;
class OverrideEventMatter;
class PlacementObj;
class Situation;
class Souma;
class StMod;
class TransStat;
class Unit;
class WorldMap;

/*---------------------------------------------------------------------*//**
 *	ストーリー管理
 *		※タイトル依存のモジュール郡管理
 *		　この story フォルダにはタイトル依存のものを入れる
 *
**//*---------------------------------------------------------------------*/
class StoryManager : public FrameExec, SerializeBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 世界地図を得る
	inline WorldMap* worldMap() const { return _wmap; }
	// シチュエーションを得る
	inline Situation* situation() const { return _situ; }

	// 現在のストーリー ID を得る
	inline s32 getCurrentStoryId() const { return _storyid; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool newGame(bool isRestart, u32 cntClear);
	void debug_enterMap(s32 storyid);
	void reserveEventForResume(s32 evidStartup, bool isLoadIntvlEv);
	void reserveNextStory(s32 storyid);

	void setup(s32 evidStartup);
	void addPlacementObjSetting(PlacementObj* pobjEntr);
	void addOverrideEvent(OverrideEventMatter* oemEntr);
	void addAutoBeginEvent(AutoBeginEventInfo* abeviEntr);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StoryManager();
	~StoryManager();
	bool create(EventSys* evsysRef);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	void notifyBeginStartGameMode();
	void notifyBeginIngameMode();
	bool notifyChangeMapBefore(s32* mapidNew, s32 mapidOld);
	void notifyChangedMap(bool isDeserializeCase);
	void notifyChangedEventMode(bool isEventMode);
	void notifyChangedRtmvPlaying(bool isPlay, u16 evidRtmvMatter);
	void notifyRestAtHome();
	void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);
	bool notifyFindUnconsciousFocus(Unit** unitFind);
	void notifyDischargeSouma(Souma* souma, u16 mcid);
	void notifyPickupItem(const Unit* unitActor, const Unit* unitItem);

	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	void debug_onDebugLevel(s32 lv);

private:
	bool loadStory(s32 storyid);
	void releaseStory();
	void beginStoryStartEvent();
	bool beginAutoBeginEvent(s32 kind);
	void setupDefaultItems();
	void setupDefaultSourceList(bool isReserveOnly);

	//======================================================================
	// 変数
private:
	// 参照
	EventSys* _evsysRef;

	// サブモジュール
	WorldMap* _wmap;
	Situation* _situ;
	StMod* _stmod;
	MapMod*	_mapmod;

	// 設定等
	s32 _storyid;
	List<PlacementObj*>* _listPlaceObj;
	List<OverrideEventMatter*>* _listOvrdEvm;
	List<AutoBeginEventInfo*>* _listAutoBeginEvi;
	bool _isPlayedStoryStartupEvent;
	bool _isLoadIntervalEventSave;
	u16 _ctrlevidAutoCnt;				// 配置オブジェクト用自動割当制御イベント ID カウンタ
	s32 _storyidReserved;

	// 作業変数
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_STORY_MANAGER_H_
