/***********************************************************************//**
 *	QuestArticle.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/14.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_QUEST_QUEST_ARTICLE_H_
#define _SRCR_QUEST_QUEST_ARTICLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/serialize/SerializeBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class StatusChangeResult;
class EventMatterPresvParam;
class Game;
class QuestDef;
class QsMod;
class TransStat;
class Unit;

/*---------------------------------------------------------------------*//**
 *	クエスト オブジェクト
 *
**//*---------------------------------------------------------------------*/
class QuestArticle : public SerializeBase
{
	//======================================================================
	// 定数
public:
	// 状態
	const static u8 STAT_NULL		= 0;
	const static u8 STAT_SLEEP		= 1;	// 未起動
	const static u8 STAT_BOOT		= 2;	// 起動している（が、受託していない）
	const static u8 STAT_ACCEPT		= 3;	// クエストを受託している
	const static u8 STAT_ACHVED		= 4;	// クエストを受託し条件を達成している（報酬はもらっていない）

	// フラグ
	const static u8 F_LISTED		= 0x01;	// メニューに表示されている
	const static u8 F_FINISHED		= 0x10;	// クリア済み

	// 汎用値
	const static s32 NUM_GVAL		= 4;

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	クエストデータのセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class SaveStructure
	{
		//==================================================================
		// 変数
	public:
		// [  0]
		u8 _stat;				// [  1] 状態（QuestArticle::STAT_*）
		u8 _flags;				// [  1] フラグ
		u8 _qstslot;			// [  1] クエストスロット
		u8 _cntCleared;			// [  1] クリア回数
		u8 _orderMenu;			// [  1] メニュー表示順
		u8 _cntAutoCtrlEvid;	// [  1] 配置オブジェクト用自動割当制御イベント ID カウンタ
		u8 __pad_0010[10];		// [ 10]
		f32 _gval[NUM_GVAL];	// [ 16] 汎用値
		// [ 32]
		static const s32 SIZE_THIS = 32;
		SaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 有効化どうかを得る
	inline bool isValid() const { return _def != 0L; }
	// 定義を得る
	inline const QuestDef* getDef() const { return _def; }
	// イベント実行時の保持パラメータを得る
	inline EventMatterPresvParam* getEventMatterPresvParam() const { return _evprsvp; }
	// 状態を得る
	inline u8 getStat() const { return _stat; }
	// 一時保存状態を得る
	inline u8 getTempStat() const { return _statTemp; }
	// フラグを得る
	inline u8 getFlags() const { return _flags; }
	// クエストスロットを得る
	inline u8 getQuestSlot() const { return _qstslot; }
	// クリア回数を得る
	inline u8 getClearedCount() const { return _cntCleared; }
	// メニュー表示順を得る
	inline u8 getMenuOrder() const { return _orderMenu; }

	// 一時保存状態を設定する
	inline void setTempStat(u8 stat) { _statTemp = stat; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void boot(u8 qstslot);
	void accept();
	void achieve();
	void unachieve();
	void reward();
	void finish();
	void cancel();

	bool checkAchievementImmediately() const;
	bool handoverRequisites();
	void setMenuOrder(u8 order);

	f32 getGeneralValue(s32 igval) const;
	void setGeneralValue(s32 igval, f32 val);

	u16 nextCtrlEvid();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QuestArticle();
	~QuestArticle();
	bool create(const QuestDef* def, u8 stat);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	void notifyChangedMap(bool isDeserializeCase);
	void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	bool storeToStructure(SaveStructure* svst) const;
	bool restoreFromStructure(const SaveStructure* svst);

private:
	bool checkAchievement_handOverItem() const;
	bool handoverRequisites_handOverItem();
	void reward_experience();
	void reward_obtainItem();

	//======================================================================
	// 変数
private:
	const QuestDef* _def;				// 定義
	QsMod* _qsmod;						// 専用モジュール
	EventMatterPresvParam* _evprsvp;	// イベント実行時の保持パラメータ
	u8 _stat;							// 状態（QuestArticle::STAT_*）
	u8 _statTemp;						// 一時保存状態（QuestArticle::STAT_*）
	u8 _flags;							// フラグ
	u8 _qstslot;						// クエストスロット
	u8 _cntCleared;						// クリア回数
	u8 _orderMenu;						// メニュー表示順（0 は無効値）
	u8 _cntAutoCtrlEvid;				// 配置オブジェクト用自動割当制御イベント ID カウンタ

	// 汎用ワーク変数
	f32 _gval[NUM_GVAL];				// [ 16] 汎用値
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_QUEST_QUEST_ARTICLE_H_
