/***********************************************************************//**
 *	StatusIfEx.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2015/02/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StatusIfEx.h"

// Friends
#include "CharStat.h"
#include "StatusChangeResult.h"
#include "StatusInfluencer.h"
#include "../effect/GaugeAnimEffect.h"
#include "../effect/StatusChangeEffect.h"
#include "../body/Conductor.h"
#include "../body/Game.h"
#include "../souma/Souma.h"
#include "../spirit/Spirit.h"
#include "../unit/char/CharUnit.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StatusInfluencer メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ステータス影響
**//*---------------------------------------------------------------------*/
bool StatusIfEx::influence(CharStat* cstat, const TransStat* tstat, bool showEffect, bool showMsg)
{
	const Unit* unit = cstat->getOwnerUnit();
	StatusChangeResult scres(unit);

	// ステータス影響
	StatusInfluencer::influence(&scres, cstat, tstat);
	if(!scres.isValid())
	{
		return false;
	}

	// エフェクト表示
	if(showEffect)
	{
		StatusChangeEffect::on(&scres);
	}

	// メッセージ表示
	if(showMsg)
	{
		Game::getGame()->getConductor()->on(&scres, 0);
	}

	// ゲージアニメ表示
	if((scres.getEneActualDamage() > 0) && unit->isCategory(Unit::UNITCAT_CHAR))
	{
		const CharUnit* cunit = (const CharUnit*)unit;
		Game::getGame()->getGaugeAnimEffect()->onDamage(cunit, scres.getEneActualDamage(), cstat->getMaxEnergy());
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	ソウマ実体化時のエネルギー消費
**//*---------------------------------------------------------------------*/
bool StatusIfEx::wasteSoumaMaterializeEnergy(CharStat* cstat, const Elene* input, bool isForce)
{
	Unit* unit = cstat->getOwnerUnit();
	StatusChangeResult scres(unit);

	// ステータス影響
	bool res = StatusInfluencer::wasteSoumaMaterializeEnergy(&scres, cstat, input, isForce);
	if(!scres.isValid())
	{
		return res;
	}

	// ゲージアニメ表示
	if((scres.getEneActualDamage() > 0) && unit->isCategory(Unit::UNITCAT_CHAR))
	{
		const CharUnit* cunit = (const CharUnit*)unit;
		Game::getGame()->getGaugeAnimEffect()->onDamage(cunit, scres.getEneActualDamage(), cstat->getMaxEnergy());
	}

	// スピリットへ通知
	if(isForce)
	{
		Spirit* sprt = unit->spirit();
		if(sprt != 0L)
		{
			sprt->notifySoumaAction(Souma::SNOTIFID_FORCE_WASTE_ENE, &scres, 0L);
		}
	}

	return res;
}

/*---------------------------------------------------------------------*//**
	ソウマ関数のエネルギー消費
**//*---------------------------------------------------------------------*/
bool StatusIfEx::wasteSoumaFunctionEnergy(CharStat* cstat, s32 lfeneCost, bool isForce)
{
	const Unit* unit = cstat->getOwnerUnit();
	StatusChangeResult scres(unit);

	// ステータス影響
	bool res = StatusInfluencer::wasteSoumaFunctionEnergy(&scres, cstat, lfeneCost, isForce);
	if(!scres.isValid())
	{
		return res;
	}

	// ゲージアニメ表示
	if((scres.getEneActualDamage() > 0) && unit->isCategory(Unit::UNITCAT_CHAR))
	{
		const CharUnit* cunit = (const CharUnit*)unit;
		Game::getGame()->getGaugeAnimEffect()->onDamage(cunit, scres.getEneActualDamage(), cstat->getMaxEnergy());
	}

	// スピリットへ通知
	if(isForce)
	{
		Spirit* sprt = unit->spirit();
		if(sprt != 0L)
		{
			sprt->notifySoumaAction(Souma::SNOTIFID_FORCE_WASTE_ENE, &scres, 0L);
		}
	}

	return res;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
