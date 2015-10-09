/***********************************************************************//**
 *	HelperSpirit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/23.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HelperSpirit.h"

// Friends
#include "HelperUnit.h"
#include "../../../effect/StatusChangeEffect.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/Party.h"
#include "../../../particle/GameParticleDef.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/StatusIfEx.h"
#include "../../../status/TransStat.h"

// External
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HelperSpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	行動を開始する
**//*---------------------------------------------------------------------*/
void HelperSpirit::startBehavior(Behavior behv, const BehaviorParam* bparam)
{
	PcSpirit::startBehavior(behv, bparam);

	// アクション待ちではなく、即座に実行するものの開始
	switch(_behvCur)
	{
	case BEHV_CURE:
		((HelperMode*)_mode)->toCure();
		break;
	}

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HelperSpirit::HelperSpirit()
	: PcSpirit(SPRTTYPE_PARTNER)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HelperSpirit::create()
{
	// モードの作成
	HelperMode* mode = new HelperMode();
	if(!mode->create(this))
	{
		delete mode;
		return false;
	}

	return PcSpirit::create(mode);
}

/*---------------------------------------------------------------------*//**
	行動変更通知
**//*---------------------------------------------------------------------*/
void HelperSpirit::onChangeBehavior(Spirit::Behavior behvNew, Spirit::Behavior behvOld, bool isBreak, bool duetoConfu)
{
	switch(behvNew)
	{
	case Spirit::BEHV_CURE:
		#if !ENABLE_POPNUM
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_START_DANCING)->getRaw(),
				getPcUnit()->getMainName()->getRaw(),
				"\n"));
			Game::getGame()->getConductor()->on(&msg, getUnit(), 0);
		}
		#endif
		break;
	default:
		PcSpirit::onChangeBehavior(behvNew, behvOld, isBreak, duetoConfu);
		break;
	}
}

//==========================================================================
// HelperSpirit::HelperRole メソッド

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理
**//*---------------------------------------------------------------------*/
int HelperSpirit::HelperRole::execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	switch(param->_cid)
	{
	case NOTHING:			return execCommand_nothing(res, ec, param);			// 何もしない
	case WALK_NORMAL:		return execCommand_walkNormal(res, ec, param);		// 通常歩行
	case CURE_PARTY:		return execCommand_cureParty(res, ec, param);		// パーティ回復
	case MEDITATION:		return execCommand_meditation(res, ec, param);		// 瞑想
	case CHANGE_ATRB:		return execCommand_changeAtrb(res, ec, param);		// 形態変更
	case ENTER_CF:			return execCommand_enterCf(res, ec, param);			// 対決開始
	case LEAVE_CF:			return execCommand_leaveCf(res, ec, param);			// 対決終了
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - パーティ回復
**//*---------------------------------------------------------------------*/
int HelperSpirit::HelperRole::execCommand_cureParty(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const f32 FRAME_STEP = 2.0f * FRAMERATE;	// 2 秒でひと溜まり
	const s32 NUM_STEP = 5;

	CharStat* cstatMe = _sprtRef->charStat();
	cstatMe->calmingFrame();		// 静止中

	// 回転アニメーション
	CharUnit* cunitMe = _sprtRef->pcUnit();
	Vector3F dir(*cunitMe->getDirection());
	dir.y() += (2.0f / TFW_PI) * ec->getDeltaFrame();
	dir.y() = Gcalc::adjustAngle(dir.y());
	cunitMe->setDirection(&dir);

	if(param->_ecntStep == 1)
	{
		// エフェクト表示
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
		ptclsys->appearParticle(GameParticleDef::PTCLK_CHARGE, _sprtRef->getPcUnit()->getCenterPos(), 0);

		_sprtRef->setBefaviorMaxFrame(FRAME_STEP * NUM_STEP);	// 最大の行動フレーム数設定
	}

	// チャージ処理
	s32 addcc = (cstatMe->getCharParam()->getUnity() >> 1);
	cstatMe->charge((u16)addcc);

	_sprtRef->setBefaviorCurrentFrame((FRAME_STEP * (param->_step - 1)) + param->_frmcntStep);	// 現在の行動フレーム数設定

	if(param->_frmcntStep >= FRAME_STEP)
	{
		cstatMe->dianaFrame();			// 集中度向上

		if(param->_step >= NUM_STEP)	// Max
		{
			// パーティ回復処理
			s32 charge = cstatMe->getChargeCounter() * 8;
			cstatMe->resetCharge();		// ためをリセット
			// 対象を集める
			List<CharUnit*> listTrgUnit;
			{
				Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
				s32 ipcMe = party->findPcUnit(cunitMe);
				for(int i = 0; i < party->getPcNum(); i++)
				{
					CharUnit* cunitTrg = party->pcUnit(i);
					if(i == ipcMe)
					{
						listTrgUnit.addTail(cunitTrg);
					}
					else	// 自分以外
					{
						Vector3F diffMeToTrg = *cunitTrg->getPosition() - *cunitMe->getPosition();
						const f32 LEN_RANGE_SQ = 500.0f * 500.0f;	// 5m
						if(diffMeToTrg.lengthSq() > LEN_RANGE_SQ)
						{
							continue;	// 離れすぎているため、範囲外
						}
						listTrgUnit.addTail(cunitTrg);
					}
				}
			}
			// それぞれを回復
			charge /= listTrgUnit.getCount();
			for(ListIterator<CharUnit*> it = listTrgUnit.iterator(); it.has(); it.next())
			{
				// 回復処理
				CharUnit* cunitTrg = it.object();
				CharStat* cstatTrg = cunitTrg->charStat();
				s32 dmgv = cstatTrg->getEnergyDamage();			// ダメージ量
				s32 recv = dmgv < charge ? dmgv : charge;		// 基本回復量
				s32 recvMax = cstatTrg->getMaxEnergy() / 4;		// 最大回復量
				recv = recvMax < recv ? recvMax : recv;			// 最大回復量にクリッピング
				if(recv > 0)
				{
					Elene elene(0, recv << TransStat::BITSHIFT_EL_TO_RECV_DEFAULT, 0, 0, 0);
					EleneStat eestat(&elene);
					TransStat tstat;
					tstat.setupForDirectFunc(_sprtRef->unit(), &eestat, EleneStat::FF_OP_CURE);
#if 1
					StatusIfEx::influence(cstatTrg, &tstat, true, true);
#else
					StatusChangeResult scres(cunitTrg);
					StatusInfluencer::influence(&scres, cstatTrg, &tstat);	// 回復処理
					StatusChangeEffect::on(&scres);							// エフェクト表示
					Game::getGame()->getConductor()->on(&scres, 0);			// 結果を処理
#endif
				}
			}

			_sprtRef->resetBefaviorFrame();		// 行動フレームリセット
			res->setDone();		// コマンド終了
		}

		return param->_step + 1;	// 次のステップへ
	}

	return param->_step;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 瞑想
**//*---------------------------------------------------------------------*/
int HelperSpirit::HelperRole::execCommand_meditation(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	if((param->_ecntStep & 0x3f) == 0)
	{
		// （テスト）エフェクト表示
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
		ptclsys->appearParticle(GameParticleDef::PTCLK_CHARGE, _sprtRef->getPcUnit()->getCenterPos(), 0);
	}
	if(param->_frmcntStep >= 300.0f)
	{
		_sprtRef->startBehavior(Spirit::BEHV_DEFAULT, 0L);	// 行動を戻す
		res->setDone();										// コマンド終了
	}

	return 1;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 形態変更
**//*---------------------------------------------------------------------*/
int HelperSpirit::HelperRole::execCommand_changeAtrb(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	return 1;
}

//==========================================================================
// HelperSpirit::HelperMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	回復モードへ移行
**//*---------------------------------------------------------------------*/
void HelperSpirit::HelperMode::toCure()
{
	changeMode((s16)ACT_CURE, false);
}

/*---------------------------------------------------------------------*//**
	アクション
**//*---------------------------------------------------------------------*/
bool HelperSpirit::HelperMode::action()
{
	for(int itry = 0; itry < 2; itry++)	// judgeConfrontAutoBehavior 内での行動決定のため２回実行する
	{
		switch(_sprtRef->getBehavior())
		{
		case Spirit::BEHV_MEDITATION:
			changeMode((s16)ACT_MEDITATION, false);
			return true;
		case Spirit::BEHV_DEFAULT:
			if(!judgeConfrontAutoBehavior())
			{
				if(getCurrentModeId() != ACT_WALKING)
				{
					changeMode((s16)ACT_WALKING, false);
				}
			}
			return true;
		}
	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HelperSpirit::HelperMode::create(HelperSpirit* sprtRef)
{
	// ロールの作成
	if(!PcMode::create(sprtRef, new HelperRole(sprtRef)))
	{
		return false;
	}

	// モード登録と初期モード設定
	registerMode((s16)ACT_WALKING, new Mode());
	registerMode((s16)ACT_CURE, new Mode());
	registerMode((s16)ACT_MEDITATION, new Mode());
	registerMode((s16)ACT_ENTER_CONFRONT, new Mode());
	registerMode((s16)ACT_LEAVE_CONFRONT, new Mode());
	changeMode((s16)ACT_WALKING, false);

	return true;
}

/*---------------------------------------------------------------------*//**
	子モードの開始通知
**//*---------------------------------------------------------------------*/
void HelperSpirit::HelperMode::notifyChildModeBegin(short modeid, void* objParamRef)
{
	getSelfRole()->reset();
	switch(modeid)
	{
	case ACT_WALKING:
		getSelfRole()->stackCommand(HelperSpirit::HelperRole::WALK_NORMAL, 0L);
		getSelfRole()->begin();
		break;
	case ACT_EVENT:
		getSelfRole()->stackCommand(HelperSpirit::HelperRole::NOTHING, 0L);
		getSelfRole()->begin();
		break;
	case ACT_CURE:
		getSelfRole()->stackCommand(HelperSpirit::HelperRole::CURE_PARTY, 0L);
		getSelfRole()->begin();
		break;
	case ACT_MEDITATION:
		getSelfRole()->stackCommand(HelperSpirit::HelperRole::MEDITATION, 0L);
		getSelfRole()->begin();
		break;
	case ACT_ENTER_CONFRONT:
		getSelfRole()->stackCommand(HelperSpirit::HelperRole::ENTER_CF, 0L);
		getSelfRole()->begin();
		break;
	case ACT_LEAVE_CONFRONT:
		getSelfRole()->stackCommand(HelperSpirit::HelperRole::LEAVE_CF, 0L);
		getSelfRole()->begin();
		break;
	}
}

/*---------------------------------------------------------------------*//**
	対決時の自動行動判定
**//*---------------------------------------------------------------------*/
bool HelperSpirit::HelperMode::judgeConfrontAutoBehavior()
{
	if(!_sprtRef->isConfront())	{	return false;	}	// 非対決時
	if(_sprtRef->isLeader())	{	return false;	}	// リーダー時

	// 誰かの体力が半分以下ならば CURE へ
	Party* party = _sprtRef->getPcUnit()->getPartyRef();
	for(int i = 0; i < party->getPcNum(); i++)
	{
		const CharStat* cstat = party->getPcUnit(i)->getCharStat();
		if((cstat->getEnergy() * 2) < cstat->getMaxEnergy())
		{
			_sprtRef->startBehavior(Spirit::BEHV_CURE, 0L);
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
