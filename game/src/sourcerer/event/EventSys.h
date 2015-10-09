/***********************************************************************//**
 *	EventSys.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_EVENT_SYS_H_
#define _SRCR_EVENT_EVENT_SYS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../tfw/evsys/EvSys.h"

#include "EventCause.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

TFW_END_NS

SRCR_BEGIN_NS

class StatusChangeResult;
class Game;
class EventData;
class EventMatter;
class EventMatterFactory;
class MagicCluster;
class QuestArticle;
class Souma;
class Story;
class Unit;
class TransStat;

/*---------------------------------------------------------------------*//**
 *	イベント システム
 *		※　汎用システム化検討中．ゲーム固有ソースを極力減らすこと．
 *
**//*---------------------------------------------------------------------*/
class EventSys : public EvSys
{
	//======================================================================
	// 定数
private:
	// 特殊フラグ
	static const u32	SPF_ENABLE_WAIT_MVMSG	= 0x00000001;	// ムービー台詞待ち有効
	static const u32	SPF_WAIT_MVMSG			= 0x00000002;	// ムービー台詞待ち中
	static const u32	SPF_EVENT_PAUSE			= 0x00000010;	// イベントポーズ
	static const u32	SPF_PREVENT_DOWN		= 0x00010000;	// ダウン防止
	static const u32	SPF_NO_WORLD_MAP		= 0x00020000;	// ワールドマップ禁止

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// イベントデータを得る
	inline EventData* getEventData() { return (EventData*)EvSys::getEvData(); }
///	// イベントモード後のゲームモードを取得
///	inline s32 getEventAfterGameMode() const { return _gmkAfterEvent; }
	// ムービー台詞待ち中が有効かどうかを得る
	inline bool isEnableWaitMovieMessage() const { return TFW_IS_FLAG(_flagsSp, SPF_ENABLE_WAIT_MVMSG); }
	// ムービー台詞待ち中かどうかを得る
	inline bool isWaitMovieMessageNow() const { return TFW_IS_FLAG(_flagsSp, SPF_WAIT_MVMSG); }
	// イベントポーズ中かどうかを得る
	inline bool isEventPause() const { return TFW_IS_FLAG(_flagsSp, SPF_EVENT_PAUSE); }
	// ダウン防止かどうかを得る
	inline bool isPareventDown() const { return TFW_IS_FLAG(_flagsSp, SPF_PREVENT_DOWN); }
	// ワールドマップ禁止をかどうかを得る
	inline bool isNoWorldMap() { return TFW_IS_FLAG(_flagsSp, SPF_NO_WORLD_MAP); }

///	// イベントモード後のゲームモードを設定
///	void setEventAfterGameMode(s32 gmk);
	// ムービー台詞待ちを有効・無効化する
	inline void setEnableWaitMovieMessage(bool is) { TFW_SET_FLAG(_flagsSp, SPF_ENABLE_WAIT_MVMSG, is); }
	// ムービー台詞待ちを設定する
	inline void setWaitMovieMessageNow(bool is) { TFW_SET_FLAG(_flagsSp, SPF_WAIT_MVMSG, is); }
	// イベントポーズ中かどうかを得る
	inline void setEventPause(bool is) { TFW_SET_FLAG(_flagsSp, SPF_EVENT_PAUSE, is); }
	// ダウン防止を設定する
	inline void setPreventDown(bool is) { TFW_SET_FLAG(_flagsSp, SPF_PREVENT_DOWN, is); }
	// ワールドマップ禁止を設定する
	inline void setNoWorldMap(bool is) { TFW_SET_FLAG(_flagsSp, SPF_NO_WORLD_MAP, is); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool loadMap(s32 mapid);
	bool loadStory(s32 storyid);
	bool loadQuest(const QuestArticle* qstatc);
	void unloadQuest(const QuestArticle* qstatc);
	EventMatter* beginMapStart(u32 param);
	bool beginTmpEvent(s32 tmpevid, u16 evid, u32 param, const EvCause* cause);
	void removeUnitReference(const Unit* unit);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EventSys();
	~EventSys();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);
	void notifyDischargeSouma(Souma* souma, u16 mcid);
	void notifyDisappearMagicCluster(MagicCluster* mc, u32 dcflags);

	//======================================================================
	// 変数
private:
	EventMatterFactory* _evmfact;
	EventData* _evdata;
///	s32 _gmkAfterEvent;		// イベントモードに入る前のゲームモード
	u32 _flagsSp;			// 特殊フラグ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_EVENT_SYS_H_
