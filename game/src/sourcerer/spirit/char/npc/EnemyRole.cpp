/***********************************************************************//**
 *	EnemyRole.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EnemyRole.h"

// Friends
#include "EnemySpirit.h"
#include "../../../char/CharLogicData.h"
#include "../../../confront/Confront.h"
#include "../../../effect/StatusChangeEffect.h"
#include "../../../episode/char/pc/MyUnit.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/window/implement/GameActMsgWindow.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../sound/GameSoundDef.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/StatusIfEx.h"
#include "../../../status/TransStat.h"
#include "../../../unit/char/npc/EnemyUnit.h"

// External
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/lib/Matrix.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/modemng/Role.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// +++ Adjustable Parameter >>>
#define DISTSQ_TRG_OK_ZX			(3.0f * 3.0f)	// 3cm
#define DIST_TRG_OK_Y				(30.0f)			// 30cm
// +++ Adjustable Parameter <<<

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EnemyRole メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EnemyRole::EnemyRole(EnemySpirit* sprtRef)
	: _sprtRef(sprtRef)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EnemyRole::~EnemyRole()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EnemyRole::create(Mode* modeOwnerRef)
{
	if(!Role::create(modeOwnerRef))	{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EnemyRole::destroy()
{
	Role::destroy();
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	switch(param->_cid)
	{
	case WALK_NORMAL:		return execCommand_walkNormal(res, ec, param);		// 通常歩行
	case WALK_STANDBY:		return execCommand_walkStandby(res, ec, param);		// 待機時歩行
	case WALK_CF_DEFENSE:	return execCommand_walkCfDefense(res, ec, param);	// 対決時防御歩行
	case WALK_CF_ATTACK:	return execCommand_walkCfAttack(res, ec, param);	// 対決時攻撃歩行
	case MOVE_TO_TRG:		return execCommand_moveToTrg(res, ec, param);		// ターゲットの位置へ移動
	case MOVE_TO_HOME:		return execCommand_moveToHome(res, ec, param);		// 元位置へ移動
	case MOVE_AFTER_ATTACK:	return execCommand_moveAfterAttack(res, ec, param);	// 攻撃後の移動
	case ATTACK:			return execCommand_attack(res, ec, param);			// 対象を攻撃
	case GUARD:				return execCommand_guard(res, ec, param);			// 攻撃をガード
	case SKILL:				return execCommand_skill(res, ec, param);			// 技を発動
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 通常歩行
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand_walkNormal(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	// 歩行フレーム処理
	_sprtRef->getThisUnit()->execWalking(ec);

	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 待機時歩行
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand_walkStandby(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	if(_sprtRef->getFocusUnit() != 0L)
	{
		// 目標へ移動
		execWalkToTarget(ec);
	}
	else
	{
		// ランダム歩行フレーム処理
		_sprtRef->getThisUnit()->execWalkingRandom(ec);
	}

	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 対決時防御歩行
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand_walkCfDefense(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	// 索敵し、目標へ移動
	execWalkToTarget(ec);

	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 対決時攻撃歩行
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand_walkCfAttack(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	// 索敵し、目標へ移動
	if(execWalkToTarget(ec))
	{
		res->setDone();	// 十分近づいたら終了
	}

	// リーチ内に入ったら終了
	EnemyUnit* cunitThis = (EnemyUnit*)_sprtRef->getThisUnit();
	const CharUnit* cunitTrg = (CharUnit*)_sprtRef->getFocusUnit();
	if((cunitThis != 0L) && (cunitTrg != 0L))
	{
		Vector3F diffSelfToTrg = *cunitThis->getPosition() - *cunitTrg->getPosition();
		f32 distToTrgSq = diffSelfToTrg.lengthSq();
		f32 reachSq = _sprtRef->getReachDistance() * _sprtRef->getReachDistance();
		if(distToTrgSq <= reachSq)
		{
			res->setDone();	// 十分近づいたら終了
		}
	}

	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - ターゲットの位置へ移動
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand_moveToTrg(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	// 索敵し、目標へ移動
	if(execWalkToTarget(ec))
	{
		res->setDone();	// 十分近づいたら終了
	}

	return 0;
}

/*---------------------------------------------------------------------*//**
	索敵して目標へ移動するフレーム制御
**//*---------------------------------------------------------------------*/
bool EnemyRole::execWalkToTarget(const ExecCtx* ec)
{
	// 索敵
	if(_sprtRef->isConfront())
	{
		_sprtRef->searchConfrontTarget();
	}
	else
	{
		_sprtRef->searchConfrontTargetWhenNoConfront();
	}

	// 目標位置計算
	EnemyUnit* cunitThis = (EnemyUnit*)_sprtRef->unit();
	cunitThis->calcTargetPosition();

	// 歩行フレーム処理
	cunitThis->execWalking(ec);

	// 十分近づいているか終了
	Vector3F diffSelfToTrg = *cunitThis->getTargetPosition() - *cunitThis->getPosition();
	f32 distTpEneY = TFW_FABS(diffSelfToTrg.y());
	diffSelfToTrg.y() = 0.0f;
	f32 distToTrgSq = diffSelfToTrg.lengthSq();
	if((distToTrgSq < DISTSQ_TRG_OK_ZX) && (distTpEneY < DIST_TRG_OK_Y))
	{
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 元位置へ移動
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand_moveToHome(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	// 歩行フレーム処理
	_sprtRef->getThisUnit()->execWalking(ec);

	res->setDone();
	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 攻撃後の移動
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand_moveAfterAttack(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	// 索敵し、目標へ移動
	execWalkToTarget(ec);

	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 対象を攻撃
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand_attack(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_ANIM_START		= 1;
	const int	STEP_ANIM_WAIT		= 2;
	const int	END					= 999;
	
	// 敵との距離を確認
	{
		const Unit* unitTrg = _sprtRef->getFocusUnit();
		if(unitTrg == 0L)
		{
			res->setDone();
			return END;
		}

		Vector3F diffSelfToTrg = *unitTrg->getPosition() - *_sprtRef->getThisUnit()->getPosition();
		f32 distToTrgSq = diffSelfToTrg.lengthSq();
		f32 distReachSq = _sprtRef->getReachDistance() * _sprtRef->getReachDistance();
		if(distToTrgSq > distReachSq)
		{
			if(param->_frmcntCmd < 300.0f) 	// 10 秒は諦めずに近づく
			{
				ExecRes res;
				execCommand_walkCfAttack(&res, ec, param);
			}
			else
			{
				res->setDone();
				return END;
			}
		}
	}
	
	int step = param->_step;

	switch(step)
	{
	case STEP_ANIM_START:
		//((CharUnit*)getUnit())->changeAnim(CharUnit::ANIMID_ATTACK_1);
		step = STEP_ANIM_WAIT;
		break;

	case STEP_ANIM_WAIT:
		if(param->_ecntStep == 1)
		{
			// アニメーション変更
			_sprtRef->getThisUnit()->changeAnim(CharUnit::ANIMID_ATTACK_1, false);
			#if !ENABLE_POPNUM
				// メッセージを表示する
				ASSERT(_confrontRef != 0L);
				VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_ATTACK_FIRST)->getRaw(),
						_sprtRef->getThisUnit()->getMainName()->getRaw(),
						"\n"));
				Game::getGame()->getConductor()->on(&msg, _sprtRef->getThisUnit(), 0);
			#endif
			// SE
			Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_ENE_FLING_ATTACK, false, 0.9f, _sprtRef->getThisUnit()->getCenterPos(), 0L);
		}
		else if(param->_frmcntStep >= 15.0f)	// 攻撃フレーム
		{
			// 効果
			CharUnit* cunitTrg = (CharUnit*)_sprtRef->getFocusUnit();
			if(cunitTrg == 0L)
			{
				res->setDone();
				return END;
			}
			
			CharStat* cstatTrg = cunitTrg->charStat(); ASSERT(cstatTrg != 0L);
			StatusChangeResult scres(cunitTrg);
			TransStat tstat;
			const CharStat* cstatSelf = _sprtRef->getThisUnit()->getCharStat(); ASSERT(cstatSelf != 0L);
			const CharLogicData* chrlgd = _sprtRef->getCharLogicData();
			tstat.setupForSelfBodyAttack(_sprtRef->getThisUnit(), cstatSelf, (chrlgd != 0L) ? chrlgd->getParam(CharLogicData::PARAM_DEFAULT_ATTACK_BODY_RATE) : 1);
#if 1
			StatusIfEx::influence(cstatTrg, &tstat, true, true);
#else
			StatusInfluencer::influence(&scres, cstatTrg, &tstat);	// ステータス影響
			StatusChangeEffect::on(&scres);							// エフェクト表示
			Game::getGame()->getConductor()->on(&scres, 0);			// 結果を処理
#endif

			step = END;
		}
		break;
		
	case END:
		if(param->_ecntStep == 1)
		{
			// 攻撃アニメーション終了後に停止アニメーションへ変更を指示
			_sprtRef->getThisUnit()->changeAnim(CharUnit::ANIMID_STOP, true, true, false);
		}
		if(param->_frmcntStep >= 30.0f)
		{
			res->setDone();
		}
		break;
	}

	//STRACE("... attack ...\n");
	
	return step;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 攻撃をガード
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand_guard(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	if(param->_frmcntStep >= 30.0f)
	{
		res->setDone();
	}

	//STRACE("... guard ...\n");

	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 技を発動
**//*---------------------------------------------------------------------*/
int EnemyRole::execCommand_skill(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	// ダミー（要オーバーライド）
	return execCommand_attack(res, ec, param);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
