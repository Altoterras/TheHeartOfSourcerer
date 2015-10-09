/***********************************************************************//**
 *	StatusChangeResult.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StatusChangeResult.h"

// Friends
#include "CharStat.h"
#include "TransStat.h"
#include "../body/GameFixedStrTbl.h"
#include "../gui/window/implement/GameActMsgWindow.h"
#include "../spirit/Spirit.h"
#include "../unit/Unit.h"

// External

// Library


////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StatusChangeResult メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	エネルギーダメージ値を加算
**//*---------------------------------------------------------------------*/
void StatusChangeResult::addEneDamage(s32 dmgValued, s32 dmgActual)
{
	_damageEneValued += dmgValued;
	_damageEneActual += dmgActual;

	if(_damageEneValued < 0)
	{
		TFW_SET_FLAG(_csflags, CSFLAG_ACT_E_DMG, false);
		TFW_SET_FLAG(_csflags, CSFLAG_ACT_E_RECV, true);
	}
	else if(_damageEneValued > 0)
	{
		TFW_SET_FLAG(_csflags, CSFLAG_ACT_E_RECV, false);
		TFW_SET_FLAG(_csflags, CSFLAG_ACT_E_DMG, true);
	}
}

/*---------------------------------------------------------------------*//**
	物理ダメージ値を加算
**//*---------------------------------------------------------------------*/
void StatusChangeResult::addPhysDamage(s32 dmgValued, s32 dmgActual)
{
	_damagePhysValued += dmgValued;
	_damagePhysActual += dmgActual;

	if(_damagePhysValued < 0)
	{
		TFW_SET_FLAG(_csflags, CSFLAG_ACT_P_DMG, false);
		TFW_SET_FLAG(_csflags, CSFLAG_ACT_P_RECV, true);
	}
	else if(_damagePhysValued > 0)
	{
		TFW_SET_FLAG(_csflags, CSFLAG_ACT_P_RECV, false);
		TFW_SET_FLAG(_csflags, CSFLAG_ACT_P_DMG, true);
	}
}

/*---------------------------------------------------------------------*//**
	ステータスが変更されたユニットを得る
**//*---------------------------------------------------------------------*/
const Unit* StatusChangeResult::getRecipientUnit() const
{
	return _cstatNew->getOwnerUnit();
}

/*---------------------------------------------------------------------*//**
	プレイヤーキャラクタから敵への行動である
**//*---------------------------------------------------------------------*/
bool StatusChangeResult::isPcToEnemyAction() const
{
	if(_unitActRef == 0L	)			{	return false;	}
	if(_cstatNew == 0L)					{	return false;	}
	if(_cstatNew->getOwnerUnit() == 0L)	{	return false;	}
	if(!TFW_IS_FLAG(_unitActRef->getUnitFlags(), Unit::UNITFLAG_PC))	{	return false;	}
	if(_cstatNew->getOwnerUnit()->getUnitType() != UNITTYPE_ENEMY_CHAR)		{	return false;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	プレイヤーキャラクタの行動である
**//*---------------------------------------------------------------------*/
bool StatusChangeResult::isPcAction() const
{
	if(_unitActRef == 0L)			{	return false;	}
	if(!TFW_IS_FLAG(_unitActRef->getUnitFlags(), Unit::UNITFLAG_PC))	{	return false;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	ソウマによる行動である
**//*---------------------------------------------------------------------*/
bool StatusChangeResult::isActionBySouma() const
{
	ASSERT(_tstat != 0L);
	return _tstat->getKind() == TransStat::KIND_SOUMA;
}

/*---------------------------------------------------------------------*//**
	武器による行動である
**//*---------------------------------------------------------------------*/
bool StatusChangeResult::isActionByWeapon() const
{
	ASSERT(_tstat != 0L);
	return _tstat->getKind() == TransStat::KIND_WEAPON;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StatusChangeResult::StatusChangeResult(const Unit* unitTrgRef, const MagicCluster* mgcActRef)
{
	construct();
	setParties(unitTrgRef, mgcActRef);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StatusChangeResult::StatusChangeResult(const Unit* unitTrgRef)
{
	construct();
	setParties(unitTrgRef, 0L);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StatusChangeResult::StatusChangeResult()
{
	construct();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StatusChangeResult::~StatusChangeResult()
{
	delete _tstat;
	delete _cstatNew;
	delete _cstatOld;
}

/*---------------------------------------------------------------------*//**
	変化前の状態を保存
**//*---------------------------------------------------------------------*/
void StatusChangeResult::setOldStat(const CharStat* cstatOld)
{
	_cstatOld->copy(cstatOld);
}

/*---------------------------------------------------------------------*//**
	結果の設定
**//*---------------------------------------------------------------------*/
void StatusChangeResult::setResult(u32 csflags, const TransStat* tstat)
{
	_csflags = csflags; 
	_tstat->copy(tstat);
	_unitActRef = tstat->getOwnerUnit();

	TFW_SET_FLAG(_csflags, CSFLAG_UNDEFINED, false);	// 未設定フラグを外す
}

/*---------------------------------------------------------------------*//**
	結果の設定
**//*---------------------------------------------------------------------*/
void StatusChangeResult::setResult(const CharStat* cstatNew, const TransStat* tstat)
{
	// 各種値を設定
	_cstatNew->copy(cstatNew);
	if(tstat != 0L)
	{
		_tstat->copy(tstat);
		_unitActRef = tstat->getOwnerUnit();
	}

	// ダウン判定
	if((_cstatOld->getStat() == CharStat::STAT_DOWN) && (_cstatNew->getStat() == CharStat::STAT_ALIVE))
	{
		// 立ち上がった
		TFW_SET_FLAG(_csflags, CSFLAG_IN_ALIVE, true);
	}
	else if((_cstatOld->getStat() == CharStat::STAT_ALIVE) && (_cstatNew->getStat() == CharStat::STAT_DOWN))
	{
		// 倒れた
		TFW_SET_FLAG(_csflags, CSFLAG_IN_DOWN, true);
	}

	// 純性判定
	if(!TFW_IS_FLAG(_cstatOld->getQualityFlags(), EleneStat::QF_PURE_1) && TFW_IS_FLAG(_cstatNew->getQualityFlags(), EleneStat::QF_PURE_1))
	{
		// 純性になった
		TFW_SET_FLAG(_csflags, CSFLAG_IN_PURE, true);
	}
	else if(TFW_IS_FLAG(_cstatOld->getQualityFlags(), EleneStat::QF_PURE_1) && !TFW_IS_FLAG(_cstatNew->getQualityFlags(), EleneStat::QF_PURE_1))
	{
		// 純性でなくなった
		TFW_SET_FLAG(_csflags, CSFLAG_OUT_PURE, true);
	}

	// 激性判定
	if(!TFW_IS_FLAG(_cstatOld->getQualityFlags(), EleneStat::QF_EXCT_1) && TFW_IS_FLAG(_cstatNew->getQualityFlags(), EleneStat::QF_EXCT_1))
	{
		// 激性になった
		TFW_SET_FLAG(_csflags, CSFLAG_IN_EXCT, true);
	}
	else if(TFW_IS_FLAG(_cstatOld->getQualityFlags(), EleneStat::QF_EXCT_1) && !TFW_IS_FLAG(_cstatNew->getQualityFlags(), EleneStat::QF_EXCT_1))
	{
		// 激性でなくなった
		TFW_SET_FLAG(_csflags, CSFLAG_OUT_EXCT, true);
	}

	// 鈍性判定
	if(!TFW_IS_FLAG(_cstatOld->getQualityFlags(), EleneStat::QF_DULL_1) && TFW_IS_FLAG(_cstatNew->getQualityFlags(), EleneStat::QF_DULL_1))
	{
		// 鈍性になった
		TFW_SET_FLAG(_csflags, CSFLAG_IN_DULL, true);
	}
	else if(TFW_IS_FLAG(_cstatOld->getQualityFlags(), EleneStat::QF_DULL_1) && !TFW_IS_FLAG(_cstatNew->getQualityFlags(), EleneStat::QF_DULL_1))
	{
		// 鈍性でなくなった
		TFW_SET_FLAG(_csflags, CSFLAG_OUT_DULL, true);
	}

	// 倦怠判定
	if(TFW_IS_FLAG(_cstatOld->getSpStatFlags(), CharStat::SPSTAT_HEAVY) && !TFW_IS_FLAG(_cstatNew->getSpStatFlags(), CharStat::SPSTAT_HEAVY))
	{
		// 倦怠から回復
		TFW_SET_FLAG(_csflags, CSFLAG_OUT_HEAVY, true);
	}
	else if(!TFW_IS_FLAG(_cstatOld->getSpStatFlags(), CharStat::SPSTAT_HEAVY) && TFW_IS_FLAG(_cstatNew->getSpStatFlags(), CharStat::SPSTAT_HEAVY))
	{
		// 倦怠になった
		TFW_SET_FLAG(_csflags, CSFLAG_IN_HEAVY, true);
	}

	// 睡眠判定
	if(TFW_IS_FLAG(_cstatOld->getSpStatFlags(), CharStat::SPSTAT_SLEEP) && !TFW_IS_FLAG(_cstatNew->getSpStatFlags(), CharStat::SPSTAT_SLEEP))
	{
		// 睡眠から回復
		TFW_SET_FLAG(_csflags, CSFLAG_OUT_SLEEP, true);
	}
	else if(!TFW_IS_FLAG(_cstatOld->getSpStatFlags(), CharStat::SPSTAT_SLEEP) && TFW_IS_FLAG(_cstatNew->getSpStatFlags(), CharStat::SPSTAT_SLEEP))
	{
		// 睡眠になった
		TFW_SET_FLAG(_csflags, CSFLAG_IN_SLEEP, true);
	}

	// 錯乱判定
	if(TFW_IS_FLAG(_cstatOld->getSpStatFlags(), CharStat::SPSTAT_CONFU) && !TFW_IS_FLAG(_cstatNew->getSpStatFlags(), CharStat::SPSTAT_CONFU))
	{
		// 錯乱から回復
		TFW_SET_FLAG(_csflags, CSFLAG_OUT_CONFU, true);
	}
	else if(!TFW_IS_FLAG(_cstatOld->getSpStatFlags(), CharStat::SPSTAT_CONFU) && TFW_IS_FLAG(_cstatNew->getSpStatFlags(), CharStat::SPSTAT_CONFU))
	{
		// 錯乱になった
		TFW_SET_FLAG(_csflags, CSFLAG_IN_CONFU, true);
	}

	// 半覚醒判定
	if(TFW_IS_FLAG(_cstatOld->getSpStatFlags(), CharStat::SPSTAT_TRANS) && !TFW_IS_FLAG(_cstatNew->getSpStatFlags(), CharStat::SPSTAT_TRANS))
	{
		// 半覚醒を終えた
		TFW_SET_FLAG(_csflags, CSFLAG_OUT_TRANS, true);
	}
	else if(!TFW_IS_FLAG(_cstatOld->getSpStatFlags(), CharStat::SPSTAT_TRANS) && TFW_IS_FLAG(_cstatNew->getSpStatFlags(), CharStat::SPSTAT_TRANS))
	{
		// 半覚醒になった
		TFW_SET_FLAG(_csflags, CSFLAG_IN_TRANS, true);
	}

	TFW_SET_FLAG(_csflags, CSFLAG_UNDEFINED, false);	// 未設定フラグを外す
}

/*---------------------------------------------------------------------*//**
	結果の設定
**//*---------------------------------------------------------------------*/
void StatusChangeResult::setParties(const Unit* unitTrgRef, const MagicCluster* mgcActRef)
{
	_unitTrgRef = unitTrgRef;
	_mgcActRef = mgcActRef;
}

/*---------------------------------------------------------------------*//**
	構築
**//*---------------------------------------------------------------------*/
void StatusChangeResult::construct()
{
	_csflags = CSFLAG_UNDEFINED;
	_damageEneValued = 0;
	_damageEneActual = 0;
	_damagePhysValued = 0;
	_damagePhysActual = 0;
	_cstatOld = new CharStat(true);
	_cstatNew = new CharStat(true);
	_tstat = new TransStat();
	_unitActRef = 0L;
	_unitTrgRef = 0L;
	_mgcActRef = 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
