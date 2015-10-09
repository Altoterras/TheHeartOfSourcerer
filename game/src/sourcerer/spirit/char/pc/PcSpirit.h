/***********************************************************************//**
 *	PcSpirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_CHAR_PC_PC_SPIRIT_H_
#define _SRCR_SPIRIT_CHAR_PC_PC_SPIRIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Spirit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class PcMode;
class PcUnit;

/*---------------------------------------------------------------------*//**
 *	プレイヤー キャララクター スピリット
 *
**//*---------------------------------------------------------------------*/
class PcSpirit : public Spirit
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// フォーカス相手を得る
	virtual Unit* focusUnit() const;
	// ユニットを得る
	inline const PcUnit* getPcUnit() const { return (const PcUnit*)getUnit(); }
	inline PcUnit* pcUnit() { return (PcUnit*)getUnit(); }
	// モードを得る
	inline const PcMode* pcMode() const { return (const PcMode*)_mode; }
	inline PcMode* pcMode() { return (PcMode*)_mode; }
	// 予約された汎用行動パラメータを得る
	inline u16 getReservedBefaviorParameter() { return _paramReservedBehavior; }
	// リーダーかどうかを得る
	bool isLeader() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setFocusUnit(Unit* unit, bool isByGui);
	virtual void startBehavior(Behavior behv, const BehaviorParam* bparam);
	virtual void breakBehavior(Behavior behv, bool isByGui, bool duetoConfu);
	virtual void beginEventMode();
	virtual void endEventMode();

	virtual void searchEnemy();
	virtual u16 searchFieldItem(f32 rangeSq);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PcSpirit(SpiritType sprttype);
	virtual void exec(ExecRes* res, const ExecCtx* ec);
	virtual void notifyChangeMapBefore();
	virtual void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);
	virtual void notifyChangedEventMode(bool isEventMode);
	virtual void notifyTerminateGame();

protected:
	virtual void onChangeBehavior(Spirit::Behavior behvNew, Spirit::Behavior behvOld, bool isBreak, bool duetoConfu);

	//======================================================================
	// 変数
protected:
	s32 _paramReservedBehavior;	// 予約された汎用行動パラメータ
	Unit* _unitFocusRefGui;		// GUI によるフォーカス相手ユニットへの参照
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_CHAR_PC_PC_SPIRIT_H_
