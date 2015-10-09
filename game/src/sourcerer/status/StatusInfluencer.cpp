/***********************************************************************//**
 *	StatusInfluencer.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/16.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StatusInfluencer.h"

// Friends
#include "StatusChangeResult.h"
#include "CharStat.h"
#include "Elene.h"
#include "EquipStat.h"
#include "TransStat.h"
#include "../common/CalcUtils.h"
#include "../item/Item.h"
#include "../spirit/Spirit.h"
#include "../unit/Unit.h"
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
void StatusInfluencer::influence(StatusChangeResult* scres, CharStat* cstat, const TransStat* tstat)
{
	u32 func = tstat->getFuncFlags() & EleneStat::FF_MASK_OP;

	// 変更前の状態を設定
	if(scres != 0L)	{	scres->setOldStat(cstat);	}

	// 機能フラグより処理
	switch(func)
	{
		// ▽ 純化 ---------------------------------------------------------
	case EleneStat::FF_OP_PURIFY:
		{
			Elene eleneWk(tstat->getEleneStat()->calcCurrentAndWrapperElene());
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			purification(0L, cstat, &eleneWk);										// ○ 純化
		}
		break;
		// ▽ 回復 ---------------------------------------------------------
	case EleneStat::FF_OP_CURE:
		{
			Elene eleneWk(tstat->getEleneStat()->calcCurrentAndWrapperElene());
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			recoverEnergy(scres, cstat, &eleneWk);									// ○ 回復
		}
		break;
		// ▽ 浄化 ---------------------------------------------------------
	case EleneStat::FF_OP_CATHARSIS:
		{
			Elene eleneWk(tstat->getEleneStat()->calcCurrentAndWrapperElene());
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			// まずは純化を試みる
			purification(&eleneWk, cstat, &eleneWk);								// ○ 純化
			// 残った元素で直接回復
			if(eleneWk.sumExceptAethel() <= 0)	{	break;	}
			recoverEnergy(scres, cstat, &eleneWk);									// ○ 回復
		}
		break;
		// ▽ 増悪 ---------------------------------------------------------
	case EleneStat::FF_OP_EXCEED:
		{
			Elene eleneWk(tstat->getEleneStat()->calcCurrentAndWrapperElene());
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			impurification(cstat, &eleneWk);											// ○ 増悪
		}
		break;
		// ▽ 持続結界強化 -------------------------------------------------
	case EleneStat::FF_OP_WRAP:
		if(strengthenVeil(0L, cstat, tstat->getEleneStat()->getCql(), EleneStat::VEIL_WP, tstat->getFuncFlags(), 0L))	// ○ 結界強化
		{
			scres->setResult(StatusChangeResult::CSFLAG_STREN_WP_VEIL, tstat);
		}
		break;
		// ▽ 瞬間結界強化 -------------------------------------------------
	case EleneStat::FF_OP_BARRIER:
		if(strengthenVeil(0L, cstat, tstat->getEleneStat()->getCql(), EleneStat::VEIL_BR, tstat->getFuncFlags(), tstat->getOwnerUnit()))	// ○ 結界強化
		{
			scres->setResult(StatusChangeResult::CSFLAG_STREN_BR_VEIL, tstat);
		}
		break;
		// ▽ 両結界強化 ---------------------------------------------------
	case EleneStat::FF_OP_SUFFUSE:
		{
			Elene eleneWk(*tstat->getEleneStat()->getCql());
			if(strengthenVeil(&eleneWk, cstat, &eleneWk, EleneStat::VEIL_BR, tstat->getFuncFlags(), tstat->getOwnerUnit()))	// ○ 結界強化
			{
				scres->setResult(StatusChangeResult::CSFLAG_STREN_BR_VEIL, tstat);
			}
			if(eleneWk.sumExceptAethel() <= 0)	{	break;	}
			if(strengthenVeil(0L, cstat, &eleneWk, EleneStat::VEIL_WP, tstat->getFuncFlags(), 0L))	// ○ 結界強化
			{
				scres->setResult(StatusChangeResult::CSFLAG_STREN_WP_VEIL, tstat);
			}
		}
		break;
		// ▽ 持続結界弱体 -------------------------------------------------
	case EleneStat::FF_OP_UNWRAP:
		if(weakenVeil(0L, cstat, tstat->getEleneStat()->getCql(), EleneStat::VEIL_WP))	// ○ 結界弱体
		{
			scres->setResult(StatusChangeResult::CSFLAG_WEAKN_WP_VEIL, tstat);
		}
		break;
		// ▽ 瞬間結界弱体 -------------------------------------------------
	case EleneStat::FF_OP_UNBARRIER:
		if(weakenVeil(0L, cstat, tstat->getEleneStat()->getCql(), EleneStat::VEIL_BR))	// ○ 結界弱体
		{
			scres->setResult(StatusChangeResult::CSFLAG_WEAKN_BR_VEIL, tstat);
		}
		break;
		// ▽ 両結界弱体 ---------------------------------------------------
	case EleneStat::FF_OP_UNSUFFUSE:
		{
			Elene eleneWk(*tstat->getEleneStat()->getCql());
			if(weakenVeil(&eleneWk, cstat, &eleneWk, EleneStat::VEIL_BR))				// ○ 結界弱体
			{
				scres->setResult(StatusChangeResult::CSFLAG_WEAKN_BR_VEIL, tstat);
			}
			if(eleneWk.sumExceptAethel() <= 0)	{	break;	}
			if(weakenVeil(0L, cstat, &eleneWk, EleneStat::VEIL_WP))						// ○ 結界弱体
			{
				scres->setResult(StatusChangeResult::CSFLAG_WEAKN_WP_VEIL, tstat);
			}
		}
		break;
		// ▽ 全体アセンション ---------------------------------------------
	case EleneStat::FF_OP_ASCEN_A:
		{
			Elene eleneWk(tstat->getEleneStat()->calcCurrentAndWrapperElene());
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			shift(cstat, &eleneWk, tstat->getQualityFlags(), tstat->getFuncFlags());		// ○ シフト
		}
		break;
		// ▽ 全体ディセンション -------------------------------------------
	case EleneStat::FF_OP_DESCEN_A:
		{
			Elene eleneWk(tstat->getEleneStat()->calcCurrentAndWrapperElene());
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			shift(cstat, &eleneWk, tstat->getQualityFlags(), tstat->getFuncFlags());		// ○ シフト
		}
		break;
		// ▽ 単元素アセンション -------------------------------------------
	case EleneStat::FF_OP_ASCEN_S:
		{
			Elene eleneWk(tstat->getEleneStat()->calcCurrentAndWrapperElene());
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			shiftSingle(cstat, &eleneWk, tstat->getQualityFlags(), tstat->getFuncFlags());	// ○ シフト
		}
		break;
		// ▽ 単元素ディセンション -----------------------------------------
	case EleneStat::FF_OP_DESCEN_S:
		{
			Elene eleneWk(tstat->getEleneStat()->calcCurrentAndWrapperElene());
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			shiftSingle(cstat, &eleneWk, tstat->getQualityFlags(), tstat->getFuncFlags());	// ○ シフト
		}
		break;
		// ▽ 攻撃 ---------------------------------------------------------
	case EleneStat::FF_OP_ATTACK:
		{
			Elene eleneWk(tstat->getEleneStat()->calcCurrentAndWrapperElene());
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			// まずは消化を試みる
			digest(scres, &eleneWk, cstat, &eleneWk, tstat->getQualityFlags());		// ○ 消化
			// 残った元素でエネルギー打撃と物理打撃
			if(eleneWk.sumExceptAethel() <= 0)	{	break;	}
			shockEnergy(scres, &eleneWk, cstat, &eleneWk);							// ○ エネルギー打撃
			shockPhysics(scres, cstat, &eleneWk);									// ○ 物理打撃
		}
		break;
		// ▽ 増悪攻撃 -----------------------------------------------------
	case EleneStat::FF_OP_EXC_ATTACK:
		{
			Elene eleneWk(tstat->getEleneStat()->calcCurrentAndWrapperElene());
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			// まずは消化を試みる
			digest(scres, &eleneWk, cstat, &eleneWk, tstat->getQualityFlags());		// ○ 消化
			// 残った半分元素でエネルギー打撃と物理打撃
			Elene eleneAttack;
			for(int i = 0; i < Elene::NUM_EL; i++)	{	eleneAttack.set(i, eleneWk.get(i) / 2);	}
			if(eleneWk.sumExceptAethel() <= 0)	{	break;	}
			shockEnergy(scres, &eleneWk, cstat, &eleneAttack);						// ○ エネルギー打撃
			shockPhysics(scres, cstat, &eleneWk);									// ○ 物理打撃
			// 更に残った元素で増悪
			eleneWk.add(&eleneAttack);	// 引いた半分を戻す
			if(eleneWk.sumExceptAethel() <= 0)	{	break; }
			impurification(cstat, &eleneWk);										// ○ 増悪
		}
		break;
		// ▽ 持続結界弱体攻撃 ---------------------------------------------
	case EleneStat::FF_OP_UNW_ATTACK:
		{
			Elene eleneWk(*tstat->getEleneStat()->getCql());
			// まず結界弱体
			if(weakenVeil(&eleneWk, cstat, &eleneWk, EleneStat::VEIL_WP))			// ○ 結界弱体
			{
				scres->setResult(StatusChangeResult::CSFLAG_WEAKN_WP_VEIL, tstat);
			}
			// 結界によるガード
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			// まずは消化を試みる
			digest(scres, &eleneWk, cstat, &eleneWk, tstat->getQualityFlags());		// ○ 消化
			// 残った元素でエネルギー打撃と物理打撃
			if(eleneWk.sumExceptAethel() <= 0)	{	break;	}
			shockEnergy(scres, &eleneWk, cstat, &eleneWk);							// ○ エネルギー打撃
			shockPhysics(scres, cstat, &eleneWk);									// ○ 物理打撃
		}
		break;
		// ▽ 瞬間結界弱体攻撃 ---------------------------------------------
	case EleneStat::FF_OP_UNB_ATTACK:
		{
			Elene eleneWk(*tstat->getEleneStat()->getCql());
			// まず結界弱体
			if(weakenVeil(&eleneWk, cstat, &eleneWk, EleneStat::VEIL_BR))			// ○ 結界弱体
			{
				scres->setResult(StatusChangeResult::CSFLAG_WEAKN_BR_VEIL, tstat);
			}
			// 結界によるガード
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			// まずは消化を試みる
			digest(scres, &eleneWk, cstat, &eleneWk, tstat->getQualityFlags());		// ○ 消化
			// 残った半分元素でエネルギー打撃と物理打撃
			if(eleneWk.sumExceptAethel() <= 0)	{	break;	}
			shockEnergy(scres, &eleneWk, cstat, &eleneWk);							// ○ エネルギー打撃
			shockPhysics(scres, cstat, &eleneWk);									// ○ 物理打撃
		}
		break;
	case EleneStat::FF_OP_UNS_ATTACK:
		{
			Elene eleneWk(*tstat->getEleneStat()->getCql());
			// まず結界弱体
			if(weakenVeil(&eleneWk, cstat, &eleneWk, EleneStat::VEIL_BR))			// ○ 結界弱体
			{
				scres->setResult(StatusChangeResult::CSFLAG_WEAKN_BR_VEIL, tstat);
			}
			if(weakenVeil(&eleneWk, cstat, &eleneWk, EleneStat::VEIL_WP))			// ○ 結界弱体
			{
				scres->setResult(StatusChangeResult::CSFLAG_WEAKN_WP_VEIL, tstat);
			}
			// 結界によるガード
			if(!guardByVeil(&eleneWk, cstat, &eleneWk))	{	break;	}
			// まずは消化を試みる
			digest(scres, &eleneWk, cstat, &eleneWk, tstat->getQualityFlags());		// ○ 消化
			// 残った半分元素でエネルギー打撃と物理打撃
			if(eleneWk.sumExceptAethel() <= 0)	{	break;	}
			shockEnergy(scres, &eleneWk, cstat, &eleneWk);							// ○ エネルギー打撃
			shockPhysics(scres, cstat, &eleneWk);									// ○ 物理打撃
		}
		break;
#if defined(_DEBUG)
		// ▽ デバッグ -----------------------------------------------------
	case EleneStat::FF_OP_IMMATURE:
	case EleneStat::FF_OP_OVERBREAK:
		TRACE("{StatusInfluencer::influence} check eneleStat : %d\n", EleneStat(*tstat->getEleneStat()).calcFuncFlags(tstat->getKind() == TransStat::KIND_SOUMA));
		break;
#endif
	}

	// 返答を設定
	if(scres != 0L)	{	scres->setResult(cstat, tstat);	}

	// ユニットとスピリットに通知
	Unit* unitSelf = cstat->getOwnerUnit();
	if(unitSelf != 0L)
	{
		unitSelf->notifyInfluenceStat(scres, tstat);
		Spirit* sprt = unitSelf->spirit();
		if(sprt != 0L)
		{
			sprt->notifyInfluenceStat(scres, tstat);
		}
	}
}

/*---------------------------------------------------------------------*//**
	ソウマ実体化時のエネルギー消費
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
bool StatusInfluencer::wasteSoumaMaterializeEnergy(StatusChangeResult* scres, CharStat* cstat, const Elene* input, bool isForce)
{
	return wasteEnergyBySouma(scres, cstat, input->getSoumaMaterializeLifeEnergy(), isForce);
}

/*---------------------------------------------------------------------*//**
	ソウマ関数のエネルギー消費
**//*---------------------------------------------------------------------*/
bool StatusInfluencer::wasteSoumaFunctionEnergy(StatusChangeResult* scres, CharStat* cstat, s32 lfeneCost, bool isForce)
{
#if 1	// Ver3.x～ 意識深度消費型に戻したため、関数によるエネルギー消費は無しに	【2015/02/27 r-kishi】
	return true;
#else
	return wasteEnergyBySouma(scres, cstat, lfeneCost, isForce);
#endif
}

/*---------------------------------------------------------------------*//**
	ベールによるガード

	@param	rest		処理後の余りの元素エネルギー
	@param	input		入力された元素エネルギー
**//*---------------------------------------------------------------------*/
bool StatusInfluencer::guardByVeil(Elene* rest, CharStat* cstat, const Elene* input)
{
	Elene eeGuard;
	f32 grdrtBrr  = 0.1f;				// 持続結界防御率
	f32 grdrtEef  = 1.0f;				// 一時結界防御率

	// 元素毎の判定
	s32 eegrd;
	for(int i = Elene::EL_SO; i <= Elene::EL_WI; i++)	// 水，火，風のみ
	{
		// 結界防御
		eegrd = (s32)(cstat->getEleneStat()->getVeil(EleneStat::VEIL_WP)->getElene()->get(i) * grdrtBrr);
		eegrd += (s32)(cstat->getEleneStat()->getVeil(EleneStat::VEIL_BR)->getElene()->get(i) * grdrtEef);
		// 防御値加算
		eeGuard.add(i, eegrd);
	}

	// 攻撃元素減算
	for(int i = Elene::EL_SO; i <= Elene::EL_WI; i++)	// 水，火，風のみ
	{
		s32 ee = input->get(i);
		s32 sub = eeGuard.get(i);
		if(sub > ee)
		{
			rest->set(i, 0);
		}
		else
		{
			rest->set(i, ee - sub);
		}
	}

	return (rest->sumExceptAethel() >= 0);
}

/*---------------------------------------------------------------------*//**
	純化
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと

	@param	rest		処理後の余りの元素エネルギー
	@param	cstat		対象キャラクタステータス
	@param	input		入力された元素エネルギー
**//*---------------------------------------------------------------------*/
void StatusInfluencer::purification(Elene* rest, CharStat* cstat, const Elene* input)
{
	cstat->eleneStat()->purification(rest, input);

	// 状態チェック
	cstat->checkStat();
}

/*---------------------------------------------------------------------*//**
	消化
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと

	@param	scres		キャラステータス返答値
	@param	rest		処理後の余りの元素エネルギー
	@param	cstat		対象キャラクタステータス
	@param	input		入力された元素エネルギー
**//*---------------------------------------------------------------------*/
void StatusInfluencer::digest(StatusChangeResult* scres, Elene* rest, CharStat* cstat, const Elene* input, u32 eeqf)
{
	// 消化力
	s32 dgpw = cstat->eleneStat()->getCql()->getFire();	// アグニ
	ASSERT(dgpw >= 0);	// 保持値は必ず正の値

	// 消化力を冷やす
	s32 cdpw = (input->getWater() + cstat->eleneStat()->getCql()->getWater()) >> 2;	// ÷ 4;
	dgpw -= cdpw;

	// 0 以下は消化できなかったとして返す
	if(dgpw <= 0)
	{
		// 余り
		for(int i = 0; i < Elene::NUM_EL; i++)
		{
			rest->set(i, input->get(i));
		}
		return;
	}

	// それぞれの元素を消化する
	Elene eleneDgst, eleneRest;
	s32 tgee, dgee;
	// 地
	tgee = TFW_MIN(input->getSoil(), cstat->eleneStat()->getCql()->getSoil());		// 消化対象値
	tgee >>= 10;	// ÷ 1024
	dgee = TFW_MIN(tgee, dgpw);	// 元素消化量
	eleneDgst.setSoil(dgee);
	eleneRest.setSoil(input->getSoil() - dgee);	// 余り
	// 水
	tgee = TFW_MIN(input->getWater(), cstat->eleneStat()->getCql()->getWater());	// 消化対象値
	tgee >>= 4;		// ÷ 16
	dgee = TFW_MIN(tgee, dgpw);	// 元素消化量
	eleneDgst.setWater(dgee);
	eleneRest.setWater(input->getWater() - dgee);	// 余り
	// 火
	tgee = TFW_MIN(input->getFire(), cstat->eleneStat()->getCql()->getFire());		// 消化対象値
	tgee >>= 1;		// ÷ 2
	dgee = TFW_MIN(tgee, dgpw);	// 元素消化量
	eleneDgst.setFire(dgee);
	eleneRest.setFire(input->getFire() - dgee);	// 余り
	// 風
	tgee = TFW_MIN(input->getWind(), cstat->eleneStat()->getCql()->getWind());		// 消化対象値
	tgee >>= 2;		// ÷ 4
	dgee = TFW_MIN(tgee, dgpw);	// 元素消化量
	eleneDgst.setWind(dgee);
	eleneRest.setWind(input->getWind() - dgee);	// 余り
	// 空
	eleneRest.setAethel(input->getAethel());		// 余り

	// それぞれの質で処理する
	u32 qfInput = eeqf;
	if(TFW_IS_FLAG(qfInput, EleneStat::QF_PURE_1))		// 純性
	{
		// 純性化
		Elene elenePurifRest;
		cstat->eleneStat()->purification(&elenePurifRest, &eleneDgst);
		// 更にエネルギー回復
		if(elenePurifRest.sumExceptAethel() > 0)
		{
			recoverEnergy(scres, cstat, &elenePurifRest);
		}
	}
	else if(TFW_IS_FLAG(qfInput, EleneStat::QF_EXCT_1))	// 激性
	{
		#if 0	// 非純性時の吸収は現在保留
			// 現保持元素量が、消化元素量の２倍以上あった場合はエネルギー吸収される
			for(int i = 0; i < Elene::NUM_EL; i++)
			{
				if(cstat->getEleneStat()->getCql()->get(i) < (eleneDgst.get(i) * 2))
				{
					eleneDgst.set(i, 0);	// この元素は２倍ないので吸収されない
				}
			}
			if(eleneDgst.sumExceptAethel() > 0)
			{
				recoverEnergy(scres, &eleneDgst);	// エネルギー回復
			}
		#endif
	}
	else if(TFW_IS_FLAG(qfInput, EleneStat::QF_DULL_1))	// 鈍性
	{
		#if 0	// 非純性時の吸収は現在保留
			// 現保持元素量が、消化元素量の２倍以上あった場合は純性化に使われる
			for(int i = 0; i < Elene::NUM_EL; i++)
			{
				if(cstat->getEleneStat()->getCql()->get(i) < (eleneDgst.get(i) * 2))
				{
					eleneDgst.set(i, 0);	// この元素は２倍ないので吸収されない
				}
			}
			if(eleneDgst.sumExceptAethel() > 0)
			{
				cstat->getEleneStat()->purification(0L, &eleneDgst);
			}
		#endif
	}

	if(rest != 0L)
	{
		rest->copy(&eleneRest);
	}

	// 状態チェック
	cstat->checkStat();
}

/*---------------------------------------------------------------------*//**
	質の増悪化
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
void StatusInfluencer::impurification(CharStat* cstat, const Elene* input)
{
	// 増悪化
	cstat->eleneStat()->impurification(input);

	// 最終値更新
	cstat->eleneStat()->updateResultValue(false);

	// 状態チェック
	cstat->checkStat();
}

/*---------------------------------------------------------------------*//**
	結界強化
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
bool StatusInfluencer::strengthenVeil(Elene* rest, CharStat* cstat, const Elene* input, s32 iveil, u32 eeff, Unit* unitMaster)
{
	if(cstat->getEleneStat()->getCql()->getAethel() <= 0)				{	return false;	}
	if((iveil < EleneStat::VEIL_WP) || (EleneStat::VEIL_BR < iveil))	{ return false; }
	ASSERT((iveil != EleneStat::VEIL_BR) || (unitMaster != 0L));

	// 強化値の最大値（限界値）を得る
	Elene eleneLimit;
	cstat->calcVeilLimitElene(&eleneLimit, iveil);

	// 強化値
	Elene eeAdd;
	Elene eeRest;
	bool isEv[Elene::NUM_EL];
	TFW_ZERO_MEMORY(isEv, sizeof(bool) * Elene::NUM_EL);
	if(TFW_IS_FLAG(eeff, EleneStat::FF_ATRB_SOIL))	{	isEv[Elene::EL_SO] = true;	}
	if(TFW_IS_FLAG(eeff, EleneStat::FF_ATRB_WATER))	{	isEv[Elene::EL_WA] = true;	}
	if(TFW_IS_FLAG(eeff, EleneStat::FF_ATRB_FIRE))	{	isEv[Elene::EL_FI] = true;	}
	if(TFW_IS_FLAG(eeff, EleneStat::FF_ATRB_WIND))	{	isEv[Elene::EL_WI] = true;	}
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		s32 limit = eleneLimit.get(i);
		s32 add = isEv[i] ? input->get(i) : (iveil == EleneStat::VEIL_BR) ? input->get(i) >> 1 : input->get(i) >> 3;
		s32 cur = cstat->getEleneStat()->getVeil(iveil)->getElene()->get(i);
		s32 res = add + cur;
		if(res > limit)	{	add = limit - cur;	}
		eeAdd.set(i, add);
		eeRest.set(i, input->get(i) - add);
	}

	// 強化
	s32 eeAddSum = eeAdd.sum();
	if(eeAddSum > 0)
	{
		cstat->strengthenVeil(&eeAdd, iveil, unitMaster);
	}

	// 余り値
	if(rest != 0L)
	{
		rest->copy(&eeRest);
	}

	return eeAddSum > 0;
}

/*---------------------------------------------------------------------*//**
	結界弱体
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
bool StatusInfluencer::weakenVeil(Elene* rest, CharStat* cstat, const Elene* input, s32 iveil)
{
	if(cstat->getEleneStat()->getCql()->getAethel() <= 0)				{	return false;	}
	if((iveil < EleneStat::VEIL_WP) || (EleneStat::VEIL_BR < iveil))	{ return false; }

	// 安定率による減衰軽減率
	f32 rateRedu = 0.5f - (cstat->getCharParam()->getStability() * 0.0001f * 0.5f);
	if(rateRedu > 0.5f)	{	rateRedu = 0.5f;	}

	// 減衰値
	Elene eeSub;
	Elene eeRest;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		eeSub.set(i, (s32)(input->get(i) * rateRedu));
		eeRest.set(i, input->get(i) - eeSub.get(i));
	}

	// 弱体
	cstat->weakenVeil(&eeSub, iveil);

	if(rest != 0L)
	{
		rest->copy(&eeRest);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	全体アセンション
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
bool StatusInfluencer::shift(CharStat* cstat, const Elene* input, u32 eeqf, u32 eeff)
{
	// 基本機能確認
	u32 func = eeff & EleneStat::FF_MASK_OP;
	if((func != EleneStat::FF_OP_DESCEN_A) && (func != EleneStat::FF_OP_ASCEN_A))
	{
		return false;
	}

	// シフト値を得る
	s32 shiftval = input->sum();

	// 激性は不可、鈍性はディセンション
	if(TFW_IS_FLAG(eeqf, EleneStat::QF_EXCT_1))
	{
		return false;
	}
	else if(TFW_IS_FLAG(eeqf, EleneStat::QF_DULL_1))
	{
		shiftval = - shiftval;
	}

	// 実際の影響値
	shiftval /= 8;

	// 元素シフト処理
	cstat->eleneStat()->shift(shiftval);

	// 最終値更新
	cstat->eleneStat()->updateResultValue(false);

	// 状態チェック
	cstat->checkStat();

	return true;
}

/*---------------------------------------------------------------------*//**
	単元素アセンション
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
bool StatusInfluencer::shiftSingle(CharStat* cstat, const Elene* input, u32 eeqf, u32 eeff)
{
	// 基本機能確認
	u32 func = eeff & EleneStat::FF_MASK_OP;
	if((func != EleneStat::FF_OP_DESCEN_S) && (func != EleneStat::FF_OP_ASCEN_S))
	{
		return false;
	}

	// 対象元素、シフト値を得る
	s32 el = 0;
	s32 shiftval = 0;
	bool isShift = TFW_IS_FLAG(eeqf, EleneStat::QF_PURE_1);
	u32 atrbf = eeff & EleneStat::FF_MASK_ATRB;
	if(TFW_IS_FLAG(atrbf, EleneStat::FF_ATRB_SOIL))
	{
		if(!isShift)	{	el = Elene::EL_WA;	}
		shiftval = input->getSoil();
	}
	else if(TFW_IS_FLAG(atrbf, EleneStat::FF_ATRB_WATER))
	{
		if(isShift)		{	el = Elene::EL_SO;	}
		else			{	el = Elene::EL_FI;	}
		shiftval = input->getWater();
	}
	else if(TFW_IS_FLAG(atrbf, EleneStat::FF_ATRB_FIRE))
	{
		if(isShift)		{	el = Elene::EL_WA;	}
		else			{	el = Elene::EL_WI;	}
		shiftval = input->getFire();
	}
	else if(TFW_IS_FLAG(atrbf, EleneStat::FF_ATRB_WIND))
	{
		if(isShift)		{	el = Elene::EL_FI;	}
		else			{	el = Elene::EL_AE;	}
		shiftval = input->getWind();
	}
	else if(TFW_IS_FLAG(atrbf, EleneStat::FF_ATRB_AETHEL))
	{
		if(isShift)		{	el = Elene::EL_WI;	}
		shiftval = input->getAethel();
	}

	// 対象元素なし
	if(el == 0)
	{
		return false;
	}

	// 激性は不可、鈍性はディセンション
	if(TFW_IS_FLAG(eeqf, EleneStat::QF_EXCT_1))
	{
		return false;
	}
	else if(TFW_IS_FLAG(eeqf, EleneStat::QF_DULL_1))
	{
		shiftval = - shiftval;
	}

	// 実際の影響値
	shiftval /= 8;

	// 元素シフト処理
	cstat->eleneStat()->shiftSingle(el, shiftval);

	// 最終値更新
	cstat->eleneStat()->updateResultValue(false);

	// 状態チェック
	cstat->checkStat();

	return true;
}

/*---------------------------------------------------------------------*//**
	エネルギー打撃
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
void StatusInfluencer::shockEnergy(StatusChangeResult* scres, Elene* rest, CharStat* cstat, const Elene* input)
{
	Elene eeWork(*input);

	// 自身のエネルギー状態
	Elene eeSelf(*cstat->getEleneStat()->getCql());

	// 防御
	{
		Elene eeGuard;
		f32 grdrtGea  = cstat->getCharParam()->getStability() * 0.0001f;		// 防具防御率
		f32 grdrtBody = cstat->getCharParam()->getStability() * 0.00001f;		// 身体防御率
		static const f32 grdrtEneBody[Elene::NUM_EL] = { 0.25f, 0.5f, 0.8f, 0.9f, 0.0f };	// エネルギー自然干渉性
		// 意識集中度による防御力向上（エネルギーダメージのみ）
		f32 rateCons = cstat->getConcentrateDepth() / (f32)CharStat::CONCD_TOTAL_MAX;
		grdrtBody += rateCons * 0.1f;
		// ガードによる防御力向上
		if(cstat->isGuarding())
		{
			grdrtGea += cstat->getCharParam()->getStability() * 0.001f;
			grdrtBody += cstat->getCharParam()->getStability() * 0.0005f;
		}
		// 元素毎の判定
		s32 eegrd, eegrdR;
		int j;
		for(int i = Elene::EL_SO; i <= Elene::EL_WI; i++)	// 水，火，風のみ
		{
			eegrd = 0;

			// 防具防御
			for(int ie = 0; ie < ((cstat->getEquipStat() != 0L) ? EquipPlace::NUM_EQP : 0); ie++)
			{
				const Item* item = cstat->getEquipStat()->getEquipItem(ie);
				if((item != 0L) && (item->getItemDef()->getCategory() == ItemDef::CAT_SHIELD))
				{
					eegrd += (s32)(item->getEleneStat()->getCql()->get(i) * grdrtGea);
				}
			}
			// 身体エネ防御
			eegrd += (s32)(eeSelf.get(i) * grdrtBody * grdrtEneBody[i]);

			// 防御値加算
			eeGuard.add(i, eegrd);
			// 隣接元素の防御値加算
			j = i - 1;
			if(j >= Elene::EL_SO)
			{
				eegrdR = eegrd >> 2;	// ÷ 4
				eeGuard.add(j, eegrdR);
			}
			j = i + 1;
			if(j <= Elene::EL_FI)
			{
				eegrdR = eegrd >> 2;	// ÷ 4
				eeGuard.add(j, eegrdR);
			}
		}

		// 攻撃元素減算
		for(int i = Elene::EL_SO; i <= Elene::EL_WI; i++)	// 水，火，風のみ
		{
			s32 ee = eeWork.get(i);
			s32 sub = eeGuard.get(i);
			if(sub > ee)	{	sub = ee;	}
			eeWork.sub(i, sub);
		}
	}

	// 元素の乱れエネルギー
	s32 eedmgSoWa = eeWork.getSoil() >> 5;				// 地→水 （×0.0312）
	eedmgSoWa = TFW_MIN(eedmgSoWa, eeSelf.getWater());
	s32 eedmgWaFi = eeWork.getWater() >> 1;				// 水→火 （×0.5）
	eedmgWaFi = TFW_MIN(eedmgWaFi, eeSelf.getFire());
	s32 eedmgFiWi = (eeWork.getFire() * 3) >> 1;		// 火→風 （×1.5）
	eedmgFiWi = TFW_MIN(eedmgFiWi, eeSelf.getWind());

	// 風元素によるダメージボーナス
	f32 eedmgrateWi = eeWork.getWind() * CalcUtils::randF() * 0.5f;

	// 入力元素消費
	s32 eeWasteSo = eedmgSoWa;
	s32 eeWasteWa = eedmgWaFi;
	s32 eeWasteFi = eedmgFiWi;
	s32 eeWasteWi = (s32)eedmgrateWi;

	// 基本ダメージ（元素エネルギー→生命エネルギー変換）
	f32 dmg = (f32)(eedmgSoWa + eedmgWaFi + eedmgFiWi);
	if(dmg > 0.0f)
	{
		dmg *= 1.0f + (eedmgrateWi * 0.000001f);

		// 有効桁未満は切捨て
		s32 dmgi = ((s32)(dmg / (f32)CharStat::LFENE_VALID_FIGURE)) * CharStat::LFENE_VALID_FIGURE;

		// ダメージ処理
		if(dmgi > 0)
		{
			if(scres != 0L)	{ scres->addEneDamage(dmgi, - cstat->damageEnergy(dmgi)); }
			else			{ cstat->damageEnergy(dmgi); }
		}
	}

	// 余り元素
	if(rest != 0L)
	{
		// 残り
		rest->copy(input);
		rest->subSoil(eeWasteSo);
		rest->subWater(eeWasteWa);
		rest->subFire(eeWasteFi);
		rest->subWind(eeWasteWi);
	}
}

/*---------------------------------------------------------------------*//**
	物理打撃
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
void StatusInfluencer::shockPhysics(StatusChangeResult* scres, CharStat* cstat, const Elene* input)
{
	// 重さ
	f32 weightInput = (f32)input->getWeight();	// 武器の重さ
	f32 weightThis = (f32)cstat->getEleneStat()->getCql()->getWeight();	// 自身の重さ

	// 基本乱数
	f32 rand = CalcUtils::randF() + 0.5f;
	if(rand > 1.0f)	{	rand = 1.0f;	}

	// 運動量
	f32 moment = input->getWind() * rand * 0.00001f;

	// 運動量による基本ダメージ値
	f32 fdmg = moment * (weightInput - (weightThis * 0.01f));

	// 物質率反映
	f32 ratioInputPhys = input->getMaterialRatio();
	f32 ratioThisPhys = cstat->getEleneStat()->getCql()->getMaterialRatio();
	fdmg *= ratioInputPhys * ratioThisPhys;

	// 回避と防御
	s32 dmg = (s32)fdmg;
 	if(dmg > 0)
	{
		s32 grd = 0;
		// 結界による回避
		for(int iv = 0; iv < EleneStat::NUM_VEIL; iv++)
		{
			grd += (cstat->getEleneStat()->getVeil(iv)->getElene()->getWind() + cstat->getEleneStat()->getVeil(iv)->getElene()->getWater()) >> 20;	// >> 20 は ÷1048576 と同等
		}
		// 自身の回避
		s32 wk = (cstat->getEleneStat()->getNql()->getWind() + cstat->getEleneStat()->getNql()->getWater()) >> 13;	// >> 13 は ÷8192 と同等
		grd += wk * cstat->getCharParam()->getActivity() / 128;
		// ガードによる回避向上
		if(cstat->isGuarding())
		{
			grd += cstat->getCharParam()->getStability() * 32;
		}
		// 結界による防御
		grd += (cstat->getEleneStat()->getVeil(EleneStat::VEIL_WP)->getElene()->getSoil() + cstat->getEleneStat()->getVeil(EleneStat::VEIL_WP)->getElene()->getWater()) >> 13;	// >> 13 は ÷8192 と同等
		grd += (cstat->getEleneStat()->getVeil(EleneStat::VEIL_BR)->getElene()->getSoil() + cstat->getEleneStat()->getVeil(EleneStat::VEIL_BR)->getElene()->getWater()) >> 11;	// >> 11 は ÷2048 と同等
		// 防具による防御
		for(int ie = 0; ie < ((cstat->getEquipStat() != 0L) ? EquipPlace::NUM_EQP : 0); ie++)
		{
			const Item* item = cstat->getEquipStat()->getEquipItem(ie);
			if((item != 0L) && (item->getItemDef()->getCategory() == ItemDef::CAT_SHIELD))
			{
				wk = item->getEleneStat()->getCql()->getSoil() + item->getEleneStat()->getCql()->getWater();
				grd += wk * cstat->getCharParam()->getStability() / 64;
			}
		}
		dmg -= grd;

		// 微量物理ダメージキャンセル
		if(dmg < CharStat::LFENE_VALID_FIGURE)	{	dmg = 0;	}

		// ダメージ処理
		if(dmg > 0)
		{
			dmg = (dmg / CharStat::LFENE_VALID_FIGURE) * CharStat::LFENE_VALID_FIGURE;	// 有効桁未満切捨て
			if(scres != 0L)	{ scres->addPhysDamage(dmg, - cstat->damagePhysics(dmg)); }
			else			{ cstat->damagePhysics(dmg); }
		}
	}
}

/*---------------------------------------------------------------------*//**
	元素によるエネルギー回復
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
void StatusInfluencer::recoverEnergy(StatusChangeResult* scres, CharStat* cstat, const Elene* elene)
{
	s32 recv = elene->getSoil() + elene->getWater() + elene->getFire() + elene->getWind() + elene->getAethel();
	recv = recv >> TransStat::BITSHIFT_EL_TO_RECV_DEFAULT;

	// 反属性は正負反転
	if(TFW_IS_FLAG(cstat->getCreateFlags(), CharStat::CF_NEGATIVE_ATRB))
	{
		recv = - recv;
	}

	if(scres != 0L)	{ scres->addEneDamage(- recv, - cstat->recoverEnergy(recv)); }
	else			{ cstat->recoverEnergy(recv); }

	// 状態チェック
	cstat->checkStat();
}

/*---------------------------------------------------------------------*//**
	ソウマによるエネルギー消費
**//*---------------------------------------------------------------------*/
bool StatusInfluencer::wasteEnergyBySouma(StatusChangeResult* scres, CharStat* cstat, s32 lfeneCost, bool isForce)
{
	// 意識深度の消費（生命エネルギーよりも優先的に消費する）
#if 1
	f32 ratio = cstat->calcLfDamageToConcdDamageRatio();
	s32 concdCost = (s32)(lfeneCost * ratio);
	if(concdCost > cstat->getConcentrateDepth())
	{
		concdCost = cstat->getConcentrateDepth();
	}
	lfeneCost -= (s32)(concdCost / ratio);
#else
	s32 consdCost;
	s32 rLfToCd = cstat->calcLfDamageToConcentrateDepthDamage(s32 lfeneCost);
	if(lfeneCost >= (S32_MAX / rLfToCd))
	{
		consdCost = S32_MAX;
	}
	else
	{
		consdCost = lfeneCost * rLfToCd;
	}
	if(consdCost > cstat->getConcentrateDepth())
	{
		consdCost = cstat->getConcentrateDepth();
	}
	lfeneCost -= consdCost / rLfToCd;
#endif

	// 非強制時の残し生命エネルギーチェック
	if(!isForce)
	{
		s32 damageEneNew = cstat->getEnergyDamage() + lfeneCost;
		if(damageEneNew >= (cstat->getMaxEnergy() - cstat->getPhysicsDamage()))
		{
			return false;
		}
	}

	bool isAliveOld = (cstat->getStat() == CharStat::STAT_ALIVE);

	// 意識深度消費
	if(concdCost > 0)
	{
		cstat->damageConcentrateDepth(concdCost);
	}

	// 生命エネルギー消費
	if(lfeneCost > 0)
	{
		if(scres != 0L)
		{
			scres->addEneDamage(lfeneCost, -cstat->damageEnergy(lfeneCost, false));
			scres->setResult(cstat, 0L);
		}
		else
		{
			cstat->damageEnergy(lfeneCost, false);
		}
	}

	// ダウン時にダウンアニメーションへ
	bool isAliveNew = (cstat->getStat() == CharStat::STAT_ALIVE);
	if(isAliveOld && !isAliveNew)
	{
		CharUnit* cunit = cstat->getOwnerUnit()->isCategory(Unit::UNITCAT_CHAR) ? (CharUnit*)cstat->getOwnerUnit() : 0L;
		if(cunit != 0L)
		{
			cunit->changeAnim(CharUnit::ANIMID_TO_DOWN, false);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
