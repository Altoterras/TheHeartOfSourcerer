/***********************************************************************//**
 *	TransStat.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_TRANS_STAT_H_
#define _SRCR_STATUS_TRANS_STAT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "EleneStat.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class ExecCtx;

TFW_END_NS

SRCR_BEGIN_NS

class CharStat;
class Unit;

/*---------------------------------------------------------------------*//**
 *	伝達ステータス
 *
**//*---------------------------------------------------------------------*/
class TransStat
{
	//======================================================================
	// 定数
public:
	// 種類
	enum Kind
	{
		KIND_NULL,		// 未定義
		KIND_SOUMA,		// ソウマによる
		KIND_WEAPON,	// 武器による
		KIND_BODY,		// 身体による
		KIND_SKILL,		// 技による
		KIND_ITEM,		// アイテムによる
		KIND_DIRECT,	// 直接指定による
	};

	// 補助フラグ
	static const u16 SF_USER_CUSTOMIZED_SOUMA	= 0x0001;	// ユーザー独自のソースのソウマである

	// 標準計算定数
	static const s32 BITSHIFT_EL_TO_RECV_DEFAULT = 1;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 種類を得る
	inline Kind getKind() const { return (Kind)_kind; }
	// 発行元ユニットを得る
	inline Unit* getOwnerUnit() const { return _unitOwnRef; }
	// 元素エネルギー状態を得る
	inline const EleneStat*	getEleneStat() const { return &_eestat; }
	// 変更可能な元素エネルギー状態を得る
	inline EleneStat* eleneStat() { return &_eestat; }
	// 五元素の質フラグ（EleneStat::QF_*）を得る
	inline u32 getQualityFlags() const { return _eeqf; }
	// 五元素の機能フラグ（EleneStat::FF_*）を得る
	inline u32 getFuncFlags() const { return _eeff; }
	// 補助フラグを得る
	inline u32 getSubFlags() const { return _flagsSub; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void calcQualityAndFunc(bool isSouma);
	bool overrideFuncFlags(u32 eeff);
	bool isNegativeOperation() const;

	void scale(s32 ratio);
	void scale(f32 ratio);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TransStat() { clear(); }
	TransStat(const TransStat& src) { copy(&src); }

	inline void copy(const TransStat* src)
	{
		_kind = src->_kind;
		_flagsSub = src->_flagsSub;
		_unitOwnRef = src->_unitOwnRef;
		_eestat.copy(&src->_eestat);
		_eeqf = src->_eeqf;
		_eeff = src->_eeff;
	}
	TransStat& operator =(const TransStat& src) { copy(&src); return *this; }

	inline void clear()
	{
		_kind = KIND_NULL;
		_flagsSub = 0;
		_unitOwnRef = 0L;
		_eestat.clear();
		_eeqf = 0;
		_eeff = 0;
	}

	void exec(const ExecCtx* ec);

	void setupForMagicClusterMaking(Unit* unitOwnRef, const Elene* eleneOql, u16 flagsSub);
	void setupForWeaponAttack(Unit* unitOwnRef, const EleneStat* eestatWeapon, const CharStat* cstat);
	void setupForSelfBodyAttack(Unit* unitOwnRef, const CharStat* cstat, u16 rateBody);
	void setupForSkill(Unit* unitOwnRef, const EleneStat* eestat);
	void setupForUsingItem(Unit* unitOwnRef, const EleneStat* eestatItem);
	void setupForDirectFunc(Unit* unitOwnRef, const EleneStat* eestat, u32 eeff);

private:
	f32 calcBaseAttackElementsRatio(const CharStat* cstat, u16 rateWeaponEffective);
	void calcBaseAttackValue(s32* velwind, f32* critical, f32* stbrandf, const CharStat* cstat);

	//======================================================================
	// 変数
private:
	u16 _kind;			// 種類
	u16 _flagsSub;		// 補助フラグ
	Unit* _unitOwnRef;	// 発行元ユニット
	EleneStat _eestat;	// 元素エネルギー状態
	u32 _eeqf;			// 五元素の質フラグ
	u32 _eeff;			// 五元素の機能フラグ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_TRANS_STAT_H_
