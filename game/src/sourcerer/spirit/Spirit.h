/***********************************************************************//**
 *	Spirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_SPIRIT_H_
#define _SRCR_SPIRIT_SPIRIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"

#include "SpiritType.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class ActStat;
class CharDef;
class StatusChangeResult;
class CharLogicData;
class CharParam;
class CharStat;
class CharUnit;
class Confront;
class Game;
class SpiritMode;
class TransStat;
class Souma;
class Unit;

/*---------------------------------------------------------------------*//**
 *	動作ロジック
 *		※	Unit がゲーム中の論理単位．
 *			Spirit がユニットを動かすアルゴリズム、という別け方である．
 *
**//*---------------------------------------------------------------------*/
class Spirit : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	// 行動
	enum Behavior
	{
		BEHV_NULL,
		BEHV_DEFAULT,
		BEHV_SOUMA,
		BEHV_MEDITATION,
		BEHV_ATTACK,
		BEHV_GUARD_CHARGE,
		BEHV_ATTRACT,
		BEHV_CURE,
		BEHV_USE_ITEM,
		BEHV_PICKUP_ITEM,
	};
	
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	行動パラメータ
	 *
	**//*-----------------------------------------------------------------*/
	class BehaviorParam
	{
	};

	/*-----------------------------------------------------------------*//**
	 *	行動パラメータ - ソウマ
	 *
	**//*-----------------------------------------------------------------*/
	class SoumaBehaviorParam : public BehaviorParam
	{
	public:
		const VcString* _sourceCall;
		u8 _srckindCall;
		bool _isUserCustomizedSource;
		SoumaBehaviorParam(const VcString* sourceCall, u8 srckindCall, bool isUserCustomizedSource) : _sourceCall(sourceCall), _srckindCall(srckindCall), _isUserCustomizedSource(isUserCustomizedSource) {}
	};

	/*-----------------------------------------------------------------*//**
	 *	行動パラメータ - アイテム使用
	 *
	**//*-----------------------------------------------------------------*/
	class UseItemBehaviorParam : public BehaviorParam
	{
	public:
		u16 _itmgidRsvUsing;
		UseItemBehaviorParam(u16 itmgidRsvUsing) : _itmgidRsvUsing(itmgidRsvUsing) {}
	};

	/*-----------------------------------------------------------------*//**
	 *	行動パラメータ - アイテム拾い
	 *
	**//*-----------------------------------------------------------------*/
	class PickupItemBehaviorParam : public BehaviorParam
	{
	public:
		u16 _unitidRsvPickup;
		PickupItemBehaviorParam(u16 unitidRsvPickup) : _unitidRsvPickup(unitidRsvPickup) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// スピリットタイプを得る
	inline SpiritType			getType() const				{	return _sprttype;			}
	// 自身のユニットを得る（NULL のこともある）
	inline const CharUnit*		getUnit() const				{	return _unitSelfRef;		}
	inline CharUnit*			unit() const				{	return _unitSelfRef;		}
	// 対決状態かどうかを得る
	inline bool					isConfront() const			{	return _unitCfrntRef != 0L;	}
	// 対決相手を得る
	inline const Unit*			getConfrontUnit() const		{	return _unitCfrntRef;		}
	inline Unit*				confrontUnit() const		{	return _unitCfrntRef;		}
	// フォーカス相手を得る
	inline const Unit*			getFocusUnit() const		{	return focusUnit();			}
	virtual Unit*				focusUnit() const			{	return _unitFocusRef;		}
	// 話し相手を得る
	inline const Unit*			getTalkUnit() const			{	return _unitTalkRef;		}
	// キャラクタステータスを得る
	inline const CharStat*		getCharStat() const			{	return _cstatRef;			}
	inline CharStat*			charStat() const			{	return _cstatRef;			}
	// アクションステータスを得る
	inline const ActStat*		getActStat() const			{	return _astat;				}
	inline ActStat*				actStat() const				{	return _astat;				}
	// キャラクタ定義を設定する（無い場合は NULL が返る）
	inline const CharDef* getCharDef() const				{	return _chrdefRef;			}
	// キャラクタロジックデータを設定する（無い場合は NULL が返る）
	inline const CharLogicData* getCharLogicData() const	{	return _chrlgdRef;			}
	// 現在の行動を得る
	inline Behavior getBehavior() const { return _behvCur; }
	// 現在の行動フレーム数を得る
	virtual f32 getBefaviorCurrentFrame() const { return _frameBehvCur; }
	// 最大の行動フレーム数を得る
	virtual f32 getBefaviorMaxFrame() const { return _frameBehvMax; }
	// ソウマを得る
	virtual Souma* getSouma() const { return 0L; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// 対決相手を設定する
	virtual void setConfrontUnit(Unit* unit) { _unitCfrntRef = unit; }
	// フォーカス相手を設定する
	virtual void setFocusUnit(Unit* unit, bool isByGui) { _unitFocusRef = unit; }
	// 話し相手を設定する
	virtual void setTalkUnit(const Unit* unit) { _unitTalkRef = unit; }

	virtual void startBehavior(Behavior behv, const BehaviorParam* bparam);
	virtual void breakBehavior(Behavior behv, bool isByGui, bool duetoConfu);
	virtual bool isBehaviorActing() const;
	virtual void setBefaviorCurrentFrame(f32 value) { _frameBehvCur = value; }
	virtual void setBefaviorMaxFrame(f32 value) { _frameBehvMax = value; }
	virtual void resetBefaviorFrame();

	virtual void searchConfrontTarget() {}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Spirit(SpiritType sprttype);
	virtual ~Spirit();
	virtual bool create(SpiritMode* modeEntr);
	virtual void destroy();
	virtual bool bind(CharUnit* cunitSelfRef, const CharDef* chrdef, const CharLogicData* chrlgd);
	virtual void exec(ExecRes* res, const ExecCtx* ec);		// フレーム制御
	virtual void render(const RenderCtx* rc) {}				// 描画
	virtual void execSearchConfrontTarget(const ExecCtx* ec);
	virtual void notifyChangeMapBefore();
	virtual void notifyChangedMap();
	virtual void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);
	virtual void notifyChangedEventMode(bool isEventMode);
	virtual void notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat);
	virtual void notifySoumaAction(s32 snotifid, const StatusChangeResult* scres, const VcString* msg);
	virtual void notifyTerminateGame() {}

protected:
	virtual void onChangeBehavior(Spirit::Behavior behvNew, Spirit::Behavior behvOld, bool isBreak, bool duetoConfu) {}

	//======================================================================
	// メンバ変数
protected:
	SpiritType				_sprttype;				// スピリットタイプ（SPRTTYPE_*）
	ActStat*				_astat;					// アクションステータス
	SpiritMode*				_mode;					// スピリット モード
	CharStat*				_cstatRef;				// キャラクタステータスへの参照
	const CharDef*			_chrdefRef;				// キャラクタ定義
	const CharLogicData*	_chrlgdRef;				// キャラクタロジックデータ

	Behavior				_behvCur;				// 現在の行動
	f32						_frameBehvCur;			// 現在の行動フレーム数
	f32						_frameBehvMax;			// 最大の行動フレーム数

private:
	Unit*					_unitCfrntRef;			// 対決相手ユニットへの参照
	Unit*					_unitFocusRef;			// フォーカス相手ユニットへの参照
	const Unit*				_unitTalkRef;			// 話し相手ユニットへの参照
	CharUnit*				_unitSelfRef;			// 自身のユニットへの参照
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_SPIRIT_H_
