/***********************************************************************//**
 *	PartnerSpirit.cpp
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
#include "PartnerSpirit.h"

// Friends
#include "PartnerUnit.h"
#include "../../party/MyParty.h"
#include "../../../confront/Confront.h"
#include "../../../effect/StatusChangeEffect.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../item/Item.h"
#include "../../../item/ItemEquip.h"
#include "../../../particle/GameParticleDef.h"
#include "../../../sound/GameSoundDef.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/EquipStat.h"
#include "../../../status/StatusIfEx.h"
#include "../../../status/TransStat.h"
#include "../../../unit/Unit.h"
#include "../../../unit/UnitArray.h"

// External
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// +++ Adjustable Parameter >>>
// 防御時の自キャラとの距離
#define DIST_DEF_TO_MYCHAR			(100.0f)
#define DIST_DEF_TO_MYCHAR_SQ		(DIST_DEF_TO_MYCHAR * DIST_DEF_TO_MYCHAR)
#define DIST_DEF_REACH				(300.0f)
#define DIST_DEF_REACH_SQ			(DIST_DEF_REACH * DIST_DEF_REACH)
// 攻撃時の敵キャラとの距離
#define DIST_ATK_TO_ENE				(150.0f)
#define DIST_ATK_TO_ENE_SQ			(DIST_ATK_TO_ENE * DIST_ATK_TO_ENE)
#define DIST_ATK_REACH				(275.0f)
#define DIST_ATK_REACH_SQ			(DIST_ATK_REACH * DIST_ATK_REACH)
// フィールドアイテムを拾える距離
#define DIST_PICKUP_ITEM			(120.0f)
#define DIST_PICKUP_ITEM_SQ			(DIST_PICKUP_ITEM * DIST_PICKUP_ITEM)
// +++ Adjustable Parameter <<<

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PartnerSpirit 静的メンバ変数

//==========================================================================
// PartnerSpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	行動を開始する
**//*---------------------------------------------------------------------*/
void PartnerSpirit::startBehavior(Behavior behv, const BehaviorParam* bparam)
{
	PcSpirit::startBehavior(behv, bparam);

	// アクション待ちではなく、即座に実行するものの開始
	switch(_behvCur)
	{
	case BEHV_GUARD_CHARGE:
		((PartnerMode*)_mode)->toGuardCharge();
		break;
	}

}

/*---------------------------------------------------------------------*//**
	行動の中断

	@param behv 中断する行動（BEHV_NULL は全てを対象とした中断）
**//*---------------------------------------------------------------------*/
void PartnerSpirit::breakBehavior(Behavior behv, bool isByGui, bool duetoConfu)
{
	if((_behvCur == behv) || (behv == BEHV_NULL))
	{
		if(((_behvCur == BEHV_ATTACK) || (_behvCur == BEHV_DEFAULT)) && !isByGui)
		{
			// GUI からでない攻撃中止は無視する
			return;
		}
	}

	PcSpirit::breakBehavior(behv, isByGui, duetoConfu);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PartnerSpirit::PartnerSpirit()
	: PcSpirit(SPRTTYPE_PARTNER)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PartnerSpirit::create()
{
	// モードの作成
	PartnerMode* mode = new PartnerMode();
	if(!mode->create(this))
	{
		delete mode;
		return false;
	}

	return PcSpirit::create(mode);
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void PartnerSpirit::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	if(gec->isLockOperating())	// 操作ロック時
	{
		PcSpirit::exec(res, ec);
		return;
	}

	if(_mode->getCurrentModeId() != PartnerMode::ACT_EVENT)	// 非イベント時
	{
		if(isConfront())	// 対決時
		{
			// 策敵
			searchEnemy();
		}
	}

	PcSpirit::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	行動変更通知
**//*---------------------------------------------------------------------*/
void PartnerSpirit::onChangeBehavior(Spirit::Behavior behvNew, Spirit::Behavior behvOld, bool isBreak, bool duetoConfu)
{
	switch(behvNew)
	{
	case Spirit::BEHV_GUARD_CHARGE:
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_START_GUARD)->getRaw(),
				getPcUnit()->getMainName()->getRaw(),
				"\n"));
			Game::getGame()->getConductor()->on(&msg, getUnit(), 0);
		}
		break;
	default:
		PcSpirit::onChangeBehavior(behvNew, behvOld, isBreak, duetoConfu);
		break;
	}
}

//==========================================================================
// PartnerSpirit::PartnerRole メソッド

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理
**//*---------------------------------------------------------------------*/
int PartnerSpirit::PartnerRole::execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	switch(param->_cid)
	{
	case NOTHING:			return execCommand_nothing(res, ec, param);			// 何もしない
	case WALK_NORMAL:		return execCommand_walkNormal(res, ec, param);		// 通常歩行
	case WALK_CF_AUTO:		return execCommand_walkCfAuto(res, ec, param);		// 対決時自動歩行
	case WALK_CF_ATTACK:	return execCommand_walkCfAttack(res, ec, param);	// 対決時攻撃歩行
	case MOVE_TO_MYU:		return execCommand_moveToMyu(res, ec, param);		// 自キャラの位置へ移動
	case ATTACK:			return execCommand_attack(res, ec, param);			// 対象を攻撃
	case GUARD_CHARGE:		return execCommand_guardChage(res, ec, param);		// 防御とチャージ
	case ATTRACT:			return execCommand_attract(res, ec, param);			// 挑発
	case USE_ITEM:			return execCommand_useItem(res, ec, param);			// アイテム使用
	case PICKUP_ITEM:		return execCommand_pickupItem(res, ec, param);		// アイテム拾い
	case ENTER_CF:			return execCommand_enterCf(res, ec, param);			// 対決開始
	case LEAVE_CF:			return execCommand_leaveCf(res, ec, param);			// 対決終了
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 通常歩行
**//*---------------------------------------------------------------------*/
int PartnerSpirit::PartnerRole::execCommand_walkNormal(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	// アイテム拾い処理
	u16 unitidFiUnit = _sprtRef->searchFieldItem(DIST_PICKUP_ITEM_SQ);
	if(unitidFiUnit != 0)
	{
		Spirit::PickupItemBehaviorParam ibp(unitidFiUnit);
		_sprtRef->startBehavior(Spirit::BEHV_PICKUP_ITEM, &ibp);
	}

	return PcRole::execCommand_walkNormal(res, ec, param);
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 対決時自動歩行
**//*---------------------------------------------------------------------*/
int PartnerSpirit::PartnerRole::execCommand_walkCfAuto(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const PcUnit* unitPc1 = _sprtRef->getPcUnit()->getPartyRef()->getPcUnit(MyParty::PC_MY);
	const Unit* unitTrg = _sprtRef->getFocusUnit();
	if(unitTrg == 0L) { return 0; }

	// 自キャラから DIST_DEF_TO_MYCHAR の位置を目標位置に
	Vector3F diffMyToEne = *unitTrg->getPosition() - *unitPc1->getPosition();
	f32 lenEneToSelfSq = diffMyToEne.lengthSq();
	diffMyToEne.normalize();
	if(lenEneToSelfSq < DIST_DEF_TO_MYCHAR_SQ)	{	diffMyToEne *= ::sqrtf(lenEneToSelfSq) * 0.5f;	}	// 規定距離未満の場合の差分ベクトル
	else										{	diffMyToEne *= DIST_DEF_TO_MYCHAR;				}	// 規定距離分の差分ベクトル
	Vector3F posTrg = *unitPc1->getPosition() + diffMyToEne;
	_sprtRef->pcUnit()->setTargetPosition(&posTrg);

	// 歩行フレーム処理
	_sprtRef->pcUnit()->execWalking(ec, false);

	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 対決時攻撃歩行
**//*---------------------------------------------------------------------*/
int PartnerSpirit::PartnerRole::execCommand_walkCfAttack(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const PcUnit* unitPc1 = _sprtRef->getPcUnit()->getPartyRef()->getPcUnit(MyParty::PC_MY);
	const Unit* unitTrg = _sprtRef->getFocusUnit();
	if(unitTrg == 0L)
	{
		res->setDone();
		return 0;
	}

	// 敵キャラから DIST_ATK_TO_ENE の位置を目標位置に
	Vector3F diffEneToMy = *unitPc1->getPosition() - *unitTrg->getPosition();
	f32 lenEneToSelfSq = diffEneToMy.lengthSq();
	diffEneToMy.normalize();
	///if(lenEneToSelfSq < DIST_ATK_TO_ENE_SQ)	{	diffEneToMy *= ::sqrtf(lenEneToSelfSq) * 0.5f;	}	// 規定距離未満の場合の差分ベクトル
	///else										{	diffEneToMy *= DIST_ATK_TO_ENE;					}	// 規定距離分の差分ベクトル
	diffEneToMy *= DIST_ATK_TO_ENE;
	Vector3F posTrg = *unitTrg->getPosition() + diffEneToMy;
	_sprtRef->pcUnit()->setTargetPosition(&posTrg);

	// 歩行フレーム処理
	_sprtRef->pcUnit()->execWalking(ec, true);

	// 十分近づいたら終了
	Vector3F diffSelfToEne = posTrg - *_sprtRef->getPcUnit()->getPosition();
	if(diffSelfToEne.lengthSq() <= DIST_ATK_REACH_SQ)
	{
		res->setDone();
	}

	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 自キャラの位置へ移動
**//*---------------------------------------------------------------------*/
int PartnerSpirit::PartnerRole::execCommand_moveToMyu(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	// 目標位置設定
	const PcUnit* unitPc1 = _sprtRef->getPcUnit()->getPartyRef()->getPcUnit(MyParty::PC_MY);
	Vector3F posTrg = *unitPc1->getPosition();
	_sprtRef->pcUnit()->setTargetPosition(&posTrg);

	// 歩行フレーム処理
	_sprtRef->pcUnit()->execWalking(ec, false);

	// 十分近づいたら終了
	Vector3F diffSelfToMyu = posTrg - *_sprtRef->getPcUnit()->getPosition();
	if(diffSelfToMyu.lengthSq() <= DIST_DEF_REACH_SQ)
	{
		res->setDone();
	}

	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 対象を攻撃
**//*---------------------------------------------------------------------*/
int PartnerSpirit::PartnerRole::execCommand_attack(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_ANIM_START		= 1;
	const int	STEP_ANIM_WAIT		= 2;
	const int	END					= 999;
	
	int step = param->_step;

	CharUnit* cunitTrg = (CharUnit*)_sprtRef->getConfrontUnit();
	if(cunitTrg == 0L)	// 対象がいなくなった（倒された）
	{
		res->setDone();
		return END;
	}
	CharStat* cstatTrg = cunitTrg->charStat(); ASSERT(cstatTrg != 0L);

	// 敵の状態を確認
	if(cstatTrg->getStat() == CharStat::STAT_DOWN)	// 活動を停止した
	{
		res->setDone();
		return END;
	}
	// 敵との距離を確認
	{
		const Unit* unitTrg = _sprtRef->getFocusUnit();
		if(unitTrg == 0L)
		{
			res->setDone();
			return END;
		}

		Vector3F diffSelfToEne = *unitTrg->getPosition() - *_sprtRef->getPcUnit()->getPosition();
		if(diffSelfToEne.lengthSq() > DIST_ATK_REACH_SQ)
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

	switch(step)
	{
	case STEP_ANIM_START:

		step = STEP_ANIM_WAIT;

		{
			#if !ENABLE_POPNUM
				// メッセージを表示する
				ASSERT(_confrontRef != 0L);
				VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_ATTACK_FIRST)->getRaw(),
					_sprtRef->getPcUnit()->getMainName()->getRaw(),
					"\n"));
				Game::getGame()->getConductor()->on(&msg, _sprtRef->getPcUnit(), 0);
			#endif
			// SE
			Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_ENE_SHAKE_STICK, false, 1.0f, _sprtRef->getPcUnit()->getCenterPos(), 0L);
		}

		break;

	case STEP_ANIM_WAIT:
		if(param->_ecntStep == 1)
		{
			// アニメーション変更
			_sprtRef->pcUnit()->changeAnim(CharUnit::ANIMID_ATTACK_1, false);
		}
		else if(param->_frmcntStep >= 16.0f)	// 攻撃フレーム
		{
			// 効果
			const Item* itemWep = _sprtRef->getPcUnit()->getItemEquip()->getEquipStat()->getEquipItem(EquipPlace::EQP_RHAND);
			if(itemWep != 0L)
			{
				CharStat* cstatSelf = _sprtRef->pcUnit()->charStat(); ASSERT(cstatSelf != 0L);
				TransStat tstat;
				tstat.setupForWeaponAttack(_sprtRef->pcUnit(), itemWep->getEleneStat(), cstatSelf);
				cstatSelf->resetCharge();								// 溜めリセット
#if 1
				StatusIfEx::influence(cstatTrg, &tstat, true, true);
#else
				StatusChangeResult scres(cunitTrg);
				StatusInfluencer::influence(&scres, cstatTrg, &tstat);	// ステータス影響
				StatusChangeEffect::on(&scres);							// エフェクト表示
				Game::getGame()->getConductor()->on(&scres, 0);			// 結果を処理
#endif
			}

			step = END;
		}
		break;
		
	case END:
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
	コマンドフレーム処理 - 防御とチャージ
**//*---------------------------------------------------------------------*/
int PartnerSpirit::PartnerRole::execCommand_guardChage(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const f32 FRAME_STEP = 2.0f * FRAMERATE;	// 2 秒でひとたまり
	const s32 NUM_STEP = 15;

	CharStat* cstatSelf = _sprtRef->charStat();
	cstatSelf->guardingFrame();	// 防御
	cstatSelf->calmingFrame();	// 静止中

	if(param->_ecntStep == 1)
	{
		// エフェクト表示
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
		ptclsys->appearParticle(GameParticleDef::PTCLK_CHARGE, _sprtRef->getPcUnit()->getCenterPos(), 0);

		// 構えアニメーション
		///_sprtRef->pcUnit()->changeAnim(CharUnit::ANIMID_CF_STOP, false);
		_sprtRef->pcUnit()->changeAnim(CharUnit::ANIMID_GUARD_1, false);

		_sprtRef->setBefaviorMaxFrame(FRAME_STEP * NUM_STEP);	// 最大の行動フレーム数設定
	}

	// チャージ処理	※『ため（チャージ）計算.ods』参照
	s32 addcc = 16 + ((cstatSelf->getCharParam()->getUnity() + cstatSelf->getCharParam()->getActivity() + cstatSelf->getCharParam()->getStability()) >> 11);
	cstatSelf->charge((u16)addcc);

	_sprtRef->setBefaviorCurrentFrame((FRAME_STEP * (param->_step - 1)) + param->_frmcntStep);	// 現在の行動フレーム数設定

	if(param->_frmcntStep >= FRAME_STEP)
	{
		cstatSelf->dianaFrame();		// 集中度向上

		if(param->_step >= NUM_STEP)		// Max FRAME_STEP * NUM_STEP 秒
		{
			_sprtRef->resetBefaviorFrame();		// 行動フレームリセット
			res->setDone();				// コマンド終了
		}
		return param->_step + 1;	// 次のステップへ
	}

	return param->_step;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 挑発
**//*---------------------------------------------------------------------*/
int PartnerSpirit::PartnerRole::execCommand_attract(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	if(param->_frmcntStep >= 1.0f)
	{
		// エフェクト表示
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
		ptclsys->appearParticle(GameParticleDef::PTCLK_DETECT, _sprtRef->getPcUnit()->getCenterPos(), 0);

		_sprtRef->startBehavior(Spirit::BEHV_DEFAULT, 0L);	// 行動を戻す
		res->setDone();										// コマンド終了
	}

	return 1;
}

//==========================================================================
// PartnerSpirit::PartnerMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	防御＆チャージへ移行
**//*---------------------------------------------------------------------*/
void PartnerSpirit::PartnerMode::toGuardCharge()
{
	changeMode((s16)ACT_GUARD_CHARGE, false);
}

/*---------------------------------------------------------------------*//**
	アクション実行
**//*---------------------------------------------------------------------*/
bool PartnerSpirit::PartnerMode::action()
{
	for(int itry = 0; itry < 2; itry++)	// judgeConfrontAutoBehavior 内での行動決定のため２回実行する
	{
		switch(_sprtRef->getBehavior())
		{
		case Spirit::BEHV_ATTRACT:
			changeMode((s16)ACT_ATTRACT, false);
			return true;
		case Spirit::BEHV_USE_ITEM:
			if(_sprtRef->getReservedBefaviorParameter() != 0)
			{
				changeMode((s16)ACT_USE_ITEM, false);
				return true;
			}
			break;
		case Spirit::BEHV_PICKUP_ITEM:
			if(_sprtRef->getReservedBefaviorParameter() != 0)
			{
				changeMode((s16)ACT_PICKUP_ITEM, false);
				return true;
			}
			break;
		case Spirit::BEHV_DEFAULT:
			if(!judgeConfrontAutoBehavior())
			{
				if(_sprtRef->isConfront())
				{
					changeMode((s16)ACT_ATTACK, false);
					return true;
				}
				else if(getCurrentModeId() != ACT_WALKING)
				{
					changeMode((s16)ACT_WALKING, false);
				}
			}
			break;
		}
	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PartnerSpirit::PartnerMode::create(PartnerSpirit* sprtRef)
{
	// ロールの作成
	if(!PcMode::create(sprtRef, new PartnerRole(sprtRef)))
	{
		return false;
	}

	// モード登録と初期モード設定
	registerMode((s16)ACT_WALKING, new Mode());
	registerMode((s16)ACT_EVENT, new Mode());
	registerMode((s16)ACT_ATTACK, new Mode());
	registerMode((s16)ACT_GUARD_CHARGE, new Mode());
	registerMode((s16)ACT_ATTRACT, new Mode());
	registerMode((s16)ACT_USE_ITEM, new Mode());
	registerMode((s16)ACT_PICKUP_ITEM, new Mode());
	registerMode((s16)ACT_ENTER_CONFRONT, new Mode());
	registerMode((s16)ACT_LEAVE_CONFRONT, new Mode());
	changeMode((s16)ACT_WALKING, false);

	return true;
}

/*---------------------------------------------------------------------*//**
	子モードの開始通知
**//*---------------------------------------------------------------------*/
void PartnerSpirit::PartnerMode::notifyChildModeBegin(short modeid, void* objParamRef)
{
	getSelfRole()->reset();
	switch(modeid)
	{
	case ACT_WALKING:
		if(_sprtRef->isConfront() && !_sprtRef->isLeader())
		{
			getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::WALK_CF_AUTO, 0L);
		}
		else
		{
			getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::WALK_NORMAL, 0L);
		}
		getSelfRole()->begin();
		break;
	case ACT_EVENT:
		getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::NOTHING, 0L);
		getSelfRole()->begin();
		break;
	case ACT_ATTACK:
		if(_sprtRef->isConfront() && !_sprtRef->isLeader())
		{
			getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::MOVE_TO_MYU, 0L);
			getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::WALK_CF_ATTACK, 0L);
		}
		getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::ATTACK, 0L);
		getSelfRole()->begin();
		break;
	case ACT_GUARD_CHARGE:
		getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::GUARD_CHARGE, 0L);
		getSelfRole()->begin();
		break;
	case ACT_ATTRACT:
		getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::ATTRACT, 0L);
		getSelfRole()->begin();
		break;
	case ACT_USE_ITEM:
		getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::USE_ITEM, 0L);
		getSelfRole()->begin();
		break;
	case ACT_PICKUP_ITEM:
		getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::PICKUP_ITEM, 0L);
		getSelfRole()->begin();
		break;
	case ACT_ENTER_CONFRONT:
		getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::ENTER_CF, 0L);
		if(!_sprtRef->isLeader())
		{
			getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::MOVE_TO_MYU, 0L);
		}
		getSelfRole()->begin();
		break;
	case ACT_LEAVE_CONFRONT:
		getSelfRole()->stackCommand(PartnerSpirit::PartnerRole::LEAVE_CF, 0L);
		getSelfRole()->begin();
		break;
	}
}

/*---------------------------------------------------------------------*//**
	対決時の自動行動判定
**//*---------------------------------------------------------------------*/
bool PartnerSpirit::PartnerMode::judgeConfrontAutoBehavior()
{
	if(!_sprtRef->isConfront())	{	return false;	}	// 非対決時
	if(_sprtRef->isLeader())	{	return false;	}	// リーダー時

	// 体力が 1/5 以下ならば GUARDCHARGE へ
	const CharStat* cstat = _sprtRef->getCharStat();
	if((cstat->getEnergy() * 5) < cstat->getMaxEnergy())
	{
		_sprtRef->startBehavior(Spirit::BEHV_GUARD_CHARGE, 0L);
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
