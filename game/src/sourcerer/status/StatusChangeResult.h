/***********************************************************************//**
 *	StatusChangeResult.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_STATUS_CHANGE_RESULT_H_
#define _SRCR_STATUS_STATUS_CHANGE_RESULT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class CharStat;
class MagicCluster;
class TransStat;
class Unit;

/*---------------------------------------------------------------------*//**
 *	キャラクターステータス変更
 *
**//*---------------------------------------------------------------------*/
class StatusChangeResult
{
	//======================================================================
	// 定数
public:
	// 状態変更フラグ
	const static u32 CSFLAG_ACT_E_RECV		= 0x00000001;	// エネルギー回復
	const static u32 CSFLAG_ACT_E_DMG		= 0x00000002;	// エネルギーダメージ
	const static u32 CSFLAG_ACT_P_RECV		= 0x00000004;	// 物理回復
	const static u32 CSFLAG_ACT_P_DMG		= 0x00000008;	// 物理ダメージ
	const static u32 CSFLAG_IN_DOWN			= 0x00000010;	// 活動停止した
	const static u32 CSFLAG_IN_ALIVE		= 0x00000020;	// 活動再開した
	const static u32 CSFLAG_IN_PURE			= 0x00000040;	// 純性になった
	const static u32 CSFLAG_IN_EXCT			= 0x00000080;	// 激性になった
	const static u32 CSFLAG_IN_DULL			= 0x00000100;	// 鈍性になった
	const static u32 CSFLAG_OUT_PURE		= 0x00000200;	// 純性でなくなった
	const static u32 CSFLAG_OUT_EXCT		= 0x00000400;	// 激性でなくなった
	const static u32 CSFLAG_OUT_DULL		= 0x00000800;	// 鈍性でなくなった
	const static u32 CSFLAG_STREN_WP_VEIL	= 0x00001000;	// 持続結界を強化した
	const static u32 CSFLAG_WEAKN_WP_VEIL	= 0x00002000;	// 持続結界を弱体した
	const static u32 CSFLAG_STREN_BR_VEIL	= 0x00004000;	// 瞬間結界を強化した
	const static u32 CSFLAG_WEAKN_BR_VEIL	= 0x00008000;	// 瞬間結界を弱体した
	const static u32 CSFLAG_IN_HEAVY		= 0x00010000;	// 倦怠になった
	const static u32 CSFLAG_OUT_HEAVY		= 0x00020000;	// 倦怠から回復した
	const static u32 CSFLAG_IN_SLEEP		= 0x00040000;	// 睡眠になった
	const static u32 CSFLAG_OUT_SLEEP		= 0x00080000;	// 睡眠から回復した
	const static u32 CSFLAG_IN_CONFU		= 0x00100000;	// 錯乱になった
	const static u32 CSFLAG_OUT_CONFU		= 0x00200000;	// 錯乱から回復した
	const static u32 CSFLAG_IN_TRANS		= 0x00400000;	// 半覚醒になった
	const static u32 CSFLAG_OUT_TRANS		= 0x00800000;	// 半覚醒を終えた

	const static u32 CSFLAG_UNDEFINED		= 0x80000000;	// 未設定である

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 設定済みかを得る
	inline bool isValid() const { return !TFW_IS_FLAG(_csflags, CSFLAG_UNDEFINED); }
	// フラグを得る
	inline u32 getChangeFlags() const { return _csflags; }
	// エネルギーダメージ値を得る（マイナス値は回復）
	inline s32 getEneValuedDamage() const { return _damageEneValued; }
	// 実際のエネルギーダメージ値を得る（マイナス値は回復）
	inline s32 getEneActualDamage() const { return _damageEneActual; }
	// 物理ダメージ値を得る（マイナス値は回復）
	inline s32 getPhysValuedDamage() const { return _damagePhysValued; }
	// 実際の物理ダメージ値を得る（マイナス値は回復）
	inline s32 getPhysActualDamage() const { return _damagePhysActual; }
	// 変更後の新しいキャラクタステータスを得る
	inline const CharStat* getNewCharStat() const { return _cstatNew; }
	// 変更前の古いキャラクタステータスを得る
	inline const CharStat* getOldCharStat() const { return _cstatOld; }
	// 行動者を得る
	inline const Unit* getActorUnit() const { return _unitActRef; }
	// 対象者を得る
	inline const Unit* getTargetUnit() const { return _unitTrgRef; }
	// 行動者によるマジッククラスターを得る（NULL のことがある）
	inline const MagicCluster* getActorMagicCluster() const { return _mgcActRef; }
	// 伝達ステータスを得る
	inline const TransStat* getTransStat() const { return _tstat; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void addEneDamage(s32 dmgValued, s32 dmgActual);
	void addPhysDamage(s32 dmgValued, s32 dmgActual);

	const Unit* getRecipientUnit() const;

	bool isPcToEnemyAction() const;
	bool isPcAction() const;
	bool isActionBySouma() const;
	bool isActionByWeapon() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	explicit StatusChangeResult(const Unit* unitTrgRef, const MagicCluster* mgcActRef);
	explicit StatusChangeResult(const Unit* unitTrgRef);
	explicit StatusChangeResult();
	~StatusChangeResult();
	void setOldStat(const CharStat* cstatOld);
	void setResult(u32 csflags, const TransStat* tstat);
	void setResult(const CharStat* cstatNew, const TransStat* tstat);
	void setParties(const Unit* unitTrgRef, const MagicCluster* mgcActRef);

private:
	void construct();

	//======================================================================
	// 変数
private:
	u32					_csflags;
	s32					_damageEneValued;
	s32					_damageEneActual;
	s32					_damagePhysValued;
	s32					_damagePhysActual;
	CharStat*			_cstatOld;
	CharStat*			_cstatNew;
	TransStat*			_tstat;
	const Unit*			_unitActRef;
	const Unit*			_unitTrgRef;
	const MagicCluster*	_mgcActRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_STATUS_CHANGE_RESULT_H_
