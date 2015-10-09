/***********************************************************************//**
 *	EnemySpirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_CHAR_NPC_ENEMY_SPIRIT_H_
#define _SRCR_SPIRIT_CHAR_NPC_ENEMY_SPIRIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Spirit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class CharStat;
class EnemyUnit;
class EnemyMode;
class EnemyRole;

/*---------------------------------------------------------------------*//**
 *	敵スピリット
 *
**//*---------------------------------------------------------------------*/
class EnemySpirit : public Spirit
{
	//======================================================================
	// 定数
public:
	// 制御フラグ
	static const u32 CTRLF_LOCKON_TRG				= 0x00000001;	// ターゲットを固定する
	static const u32 CTRLF_UNLOCKON_IN_DETECT_DIST	= 0x00000002;	// 発見距離に入ったらロックオンを解除する

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユニットを得る
	inline EnemyUnit* getThisUnit() const { return (EnemyUnit*)getUnit(); }
	// 制御フラグを取得する
	inline u32 getControlFlags() const { return _ctrlflags; }
	// 対象に行動を起こせるリーチ距離の二乗値を得る
	virtual f32 getReachDistance() const { return _distReach; }

	// 制御フラグを設定する
	inline void setControlFlags(u32 flags, bool on) { TFW_SET_FLAG(_ctrlflags, flags, on); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void breakBehavior(Behavior behv, bool isByGui, bool duetoConfu);
	void searchConfrontTarget();
	void searchConfrontTargetWhenNoConfront();
	virtual s16 decideNextConfrontAction();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	explicit EnemySpirit();
	virtual bool create(SpiritMode* modeEntr);
	virtual bool create();
	virtual bool bind(CharUnit* cunitSelfRef, const CharDef* chrdef, const CharLogicData* chrlgd);
	virtual void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	virtual void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);
	virtual void notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat);

protected:
	virtual void onChangeBehavior(Spirit::Behavior behvNew, Spirit::Behavior behvOld, bool isBreak, bool duetoConfu);

	//======================================================================
	// 変数
protected:
	u32 _ctrlflags;
	bool _isBaganPobjEvent;
	f32 _distReach;
	f32 _lenBattleThrsldParamSq;
	f32 _lenDetectThrsldParamSq;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_CHAR_NPC_ENEMY_SPIRIT_H_
