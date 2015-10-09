/***********************************************************************//**
 *	StEmp_10203_1001_UngetsuSecondBattle.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/11.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StEmp_10203_1001_UngetsuSecondBattle.h"

// Friends
#include "../StEmpType.h"
#include "../../char/pc/MyUnit.h"
#include "../../char/pc/PartnerUnit.h"
#include "../../party/MyParty.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../confront/Confront.h"
#include "../../../event/EventSys.h"
#include "../../../status/CharStat.h"
#include "../../../status/EleneStat.h"
#include "../../../status/StatusIfEx.h"
#include "../../../status/TransStat.h"
#include "../../../unit/UnitManager.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define CTRLEVID_CF_UNGETSU		(51001)			// 雲月岩の制御用イベント ID

#define EVID_CORPO_CONFU		10003			// コルポ錯乱イベント ID
#define EVID_CORPO_COOL			10004			// コルポ沈静イベント ID

#define FRAMECNT_CONFU			(16.0f * FRAMERATE)	// 錯乱イベントまでのフレーム
#define FRAMECNT_COOL			(3.0f * FRAMERATE)	// 冷静イベントまでのフレーム

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StEmp_10203_1001_UngetsuSecondBattle メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void StEmp_10203_1001_UngetsuSecondBattle::begin(u32 param)
{
	// 値のリセット（２回目の起動のことを考えると、全ての値をリセットする必要がある）
	_fcntBattle = 0.0f;
	_fcntCool = -1.0f;
	_sstepCorpoConfuEvent = SSTEP_NULL;
	_sstepCorpoCoolEvent = SSTEP_NULL;
	_isLockOpPrevFrame = false;

	// ロックを解除し、ユーザー操作可能に
	setLockWithParent(false);

	// コルポの状態を錯乱に（雲月の台詞で興奮した）
	PartnerUnit* unitPtn = (PartnerUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_PTN); ASSERT(unitPtn != 0L);
	if((unitPtn != 0L) && unitPtn->isEnable())
	{
		// 増悪
		CharStat* cstatPtn = unitPtn->charStat();
		const EleneStat* eestatPtn = cstatPtn->getEleneStat();
		const Elene* eePtnNql = eestatPtn->getNql();
		Elene eeImp(
			eePtnNql->getSoil() / 2048,
			eePtnNql->getWater() / 1024,
			eePtnNql->getFire() * 5 / 2,
			eePtnNql->getWind() * 3 / 2,
			eePtnNql->getAethel()	);
		TransStat tstat;
		EleneStat eestat(&eeImp);
		tstat.setupForDirectFunc(0L, &eestat, EleneStat::FF_OP_EXCEED);
#if 1
		StatusIfEx::influence(cstatPtn, &tstat, false, true);	// ここはエフェクトを出さない
#else
		StatusChangeResult scres;
		StatusInfluencer::influence(&scres, cstatPtn, &tstat);
		Game::getGame()->getConductor()->on(&scres, 0);	// 結果を表示
#endif
		cstatPtn->resetConcentrate();	// 意識集中度のリセット
	}

	StEmp::begin(param);
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void StEmp_10203_1001_UngetsuSecondBattle::end()
{
	StEmp::end();

	// 雲月岩を消す
	UnitManager* unitmng = Game::getGame()->getUnitManager();
	CharUnit* cunit = (CharUnit*)unitmng->findUnitFromCtrlEvid(CTRLEVID_CF_UNGETSU);
	if(cunit != 0L)	// 敗北時などはマップ移動しているため、ユニットは見つからない
	{
		cunit->setEnable(false);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10203_1001_UngetsuSecondBattle::StEmp_10203_1001_UngetsuSecondBattle()
	: StEmp(STEMPTYPE_10203_1001)
	, _fcntBattle(0.0f)
	, _fcntCool(0.0f)
	, _sstepCorpoConfuEvent(SSTEP_NULL)
	, _sstepCorpoCoolEvent(SSTEP_NULL)
	, _isLockOpPrevFrame(false)
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御実装
**//*---------------------------------------------------------------------*/
void StEmp_10203_1001_UngetsuSecondBattle::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	if(Game::getGame()->isConfront())	// 戦闘中
	{
		// フレームをカウント
		_fcntBattle += ec->getDeltaFrame();

		// コルポ混乱イベントを時間で起動要求
		if((_sstepCorpoConfuEvent == SSTEP_NULL) && (_fcntBattle >= FRAMECNT_CONFU))	// 一定フレーム以上
		{
			// コルポの状態確認
			const PartnerUnit* unitPtn = (PartnerUnit*)Game::getGame()->getMyParty()->getPcUnit(MyParty::PC_PTN); ASSERT(unitPtn != 0L);
			const CharStat* cstatPtn = unitPtn->getCharStat(); ASSERT(cstatPtn != 0L);
			if(	TFW_IS_FLAG(cstatPtn->getSpStatFlags(), CharStat::SPSTAT_CONFU) &&	// 錯乱中
				unitPtn->isEnable() &&												// 有効
				(cstatPtn->getStat() == CharStat::STAT_ALIVE)	)					// 生きている
			{
				// イベント起動要求
				_sstepCorpoConfuEvent = SSTEP_REQ;
			}
		}

		// 冷静化イベントへのカウント
		if((_sstepCorpoCoolEvent == SSTEP_DELAY) && (_fcntCool >= 0.0f))
		{
			_fcntCool += ec->getDeltaFrame();
			if(_fcntCool >= FRAMECNT_COOL)
			{
				// イベント起動要求
				_sstepCorpoCoolEvent = SSTEP_REQ;
			}
		}
	}
	else	// 非戦闘時
	{
		if(_fcntBattle > 0.0f)	// 戦闘開始後
		{
			end();
		}
	}

	if(!gec->isLockOperating())	// イベント非実行時
	{
		if(_sstepCorpoConfuEvent == SSTEP_REQ)
		{
			_sstepCorpoConfuEvent = SSTEP_DONE;
			EvCause evcause;
			Game::getGame()->getEventSys()->begin(EVID_CORPO_CONFU, 0, &evcause);	// コルポ混乱イベント
		}
		else if(_sstepCorpoCoolEvent == SSTEP_REQ)
		{
			_sstepCorpoCoolEvent = SSTEP_DONE;
			EvCause evcause;
			Game::getGame()->getEventSys()->begin(EVID_CORPO_COOL, 0, &evcause);	// コルポ沈静イベント
		}
	}

	// 操作ロックが切り替わるタイミング、つまりムービーイベントが終了したタイミングでアニメーション補間を有効にする
	if(_isLockOpPrevFrame != gec->isLockOperating())
	{
		_isLockOpPrevFrame = gec->isLockOperating();
		if(!_isLockOpPrevFrame)	// 操作が有効になった＝イベントが終わった
		{
			// アニメーション補間を有効に
			Game::getGame()->endNoAnimInterpolation(0);
		}
	}
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void StEmp_10203_1001_UngetsuSecondBattle::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	if((_sstepCorpoCoolEvent == SSTEP_NULL) && (unitRecv->getUnitType() == UNITTYPE_PARTNER_CHAR))
	{
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_OUT_CONFU) || TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_OUT_EXCT))
		{
			// カウンタ開始
			_sstepCorpoCoolEvent = SSTEP_DELAY;
			_fcntCool = 0.0f;
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
