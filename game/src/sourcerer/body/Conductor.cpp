/***********************************************************************//**
 *	Conduct.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/11.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Conduct

// Self
#include "Conductor.h"

// Friends
#include "Game.h"
#include "GameExecCtx.h"
#include "GameFixedStrTbl.h"
#include "GameNotifier.h"
#include "../char/CharLogicData.h"
#include "../char/FixedCharParamTbl.h"
#include "../effect/PopNumEffect.h"
#include "../episode/data/FixedData.h"
#include "../episode/party/MyParty.h"
#include "../episode/save/Score.h"
#include "../gui/GameGui.h"
#include "../gui/window/implement/GameActMsgWindow.h"
#include "../particle/GameParticleDef.h"
#include "../spirit/Spirit.h"
#include "../status/CharParam.h"
#include "../status/CharStat.h"
#include "../status/StatusChangeResult.h"
#include "../status/TransStat.h"
#include "../unit/char/pc/PcUnit.h"
#include "../unit/UnitManager.h"

// External
#include "../../etk/particle/ParticleSys.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/lib/Color.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// チェーンボーナスフレーム数
#define FRAMECNT_CHAIN_BONUS		(60 * FRAMERATE)		// 60 秒

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Conductor メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	メッセージをキューに貯める
**//*---------------------------------------------------------------------*/
void Conductor::on(const VcString* strMsg, const Unit* unitActor, s32 frameDelay)
{
	ASSERT((strMsg != 0L) && (strMsg->getLength() > 0));

	// メッセージ情報を作成
	Msg* msg = new Msg();
	msg->_strMsg = new VcString(strMsg);
	msg->_color = new ColorU8(0xff, 0xff, 0xff, 0xff);
	GameActMsgWindow::getUnitColor(msg->_color, unitActor);
	msg->_frameRest = (f32)frameDelay;

	// リストに追加
	_listMsg->addTail(msg);
}

/*---------------------------------------------------------------------*//**
	メッセージをキューに貯める
**//*---------------------------------------------------------------------*/
void Conductor::on(const VcString* strMsg, const ColorU8* color, s32 frameDelay)
{
	// メッセージ情報を作成
	Msg* msg = new Msg();
	msg->_strMsg = new VcString(strMsg);
	if(color != 0L)	{	msg->_color = new ColorU8(color);					}
	else			{	msg->_color = new ColorU8(0xff, 0xff, 0xff, 0xff);	}
	msg->_frameRest = (f32)frameDelay;

	// リストに追加
	_listMsg->addTail(msg);
}

/*---------------------------------------------------------------------*//**
	キャラクタステータス変更結果をキューに貯める
**//*---------------------------------------------------------------------*/
void Conductor::on(const StatusChangeResult* scres, s32 frameDelay)
{
	if(!scres->isValid())	{	return;	}

	onCharStatResultExp(scres, frameDelay);
}

/*---------------------------------------------------------------------*//**
	全員の経験値獲得処理（即座実行）
**//*---------------------------------------------------------------------*/
void Conductor::onExperienceNow(u16 experience)
{
	onExperience(experience, 0);
}

/*---------------------------------------------------------------------*//**
	数値ポップをキューに貯める
**//*---------------------------------------------------------------------*/
void Conductor::on(s32 num, s32 kind, const Unit* unitTrg)
{
	_popnef->on(num, (PopNumEffect::Kind)kind, unitTrg);
}

/*---------------------------------------------------------------------*//**
	報酬時システムカラーを得る
**//*---------------------------------------------------------------------*/
void Conductor::getSysRewardColor(ColorU8* color)
{
	color->set(0xff, 0xff, 0, 0xff);
}

/*---------------------------------------------------------------------*//**
	ソウマスクリプトのユーザーデバッグカラーを得る
**//*---------------------------------------------------------------------*/
void Conductor::getUserSsDebugColor(ColorU8* color)
{
//	color->set(127, 63, 127, 255);
	color->set(255, 127, 255, 255);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Conductor::Conductor()
	: _listMsg(0L)
	, _popnef(0L)
	, _wndActMsgRef(0L)
	, _fcntChainBonus(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Conductor::~Conductor()
{
	ASSERT(_listMsg == 0L);
	ASSERT(_popnef == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Conductor::create()
{
	_wndActMsgRef = (GameActMsgWindow*)Game::getGame()->getGui()->getActionMsgWindow(); ASSERT(_wndActMsgRef != 0L);

	// リストの作成
	ASSERT(_listMsg == 0L);
	_listMsg = new List<Msg*>(true);

	// 数値表示の作成
	_popnef = new PopNumEffect();
	if(!_popnef->create())
	{
		delete _popnef;
		_popnef = 0L;
		return false;
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Conductor::destroy()
{
	// 数値表示の削除
	if(_popnef != 0L)
	{
		_popnef->destroy();
		delete _popnef;
		_popnef = 0L;
	}

	// リストの削除
	delete _listMsg;
	_listMsg = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void Conductor::exec(const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	// 数値表示の制御
	if(_popnef != 0L)	{	_popnef->exec(ec);	}

	// 操作ロック（イベント等）中は抜ける
	if(gec->isLockOperating() || gec->isOpenedLockGui())
	{
		return;
	}

	// チェインボーナスカウンタの更新
	///STRACE("CB:%f\n", _fcntChainBonus);
	if(_fcntChainBonus > 0.0f)
	{
		_fcntChainBonus -= ec->getDeltaFrame();
		if(_fcntChainBonus < 0.0f)
		{
			_fcntChainBonus = 0.0f;
		}
	}

	// メッセージリストの処理
	if(_listMsg != 0L)
	{
		ListIterator<Msg*> itCur, itNext;
		itCur = _listMsg->iterator();
		while(itCur.has())
		{
			// 削除に対応するため、次を得ておく
			itNext = itCur;
			itNext.next();

			Msg* msg = itCur.object();
			msg->_frameRest -= gec->getDeltaFrame();	// 残りフレーム更新
			if(msg->_frameRest <= 0.0f)	// 実行可
			{
				// メッセージ表示
				ASSERT(_wndActMsgRef != 0L);
				_wndActMsgRef->showMessageCol(msg->_color, msg->_strMsg);

				// リストから削除する
				_listMsg->removeNode(itCur.node());
			}

			// 次のリストノードへ
			itCur = itNext;
		}
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void Conductor::draw(const RenderCtx* rc)
{
	// 数値表示の 2D 描画
	if(_popnef != 0L)	{	_popnef->draw(rc);	}
}

/*---------------------------------------------------------------------*//**
	キャラクタステータス変更と経験値獲得の結果メッセージ表示
**//*---------------------------------------------------------------------*/
void Conductor::onCharStatResultExp(const StatusChangeResult* scres, s32 frameDelay)
{
	// ステータス変更メッセージを表示する
	onCharStatResult(scres, frameDelay);

	// 経験値
	u16 experience = 0;

	// 敵を倒したか
	const Unit* unitRecp = scres->getRecipientUnit();
	if((unitRecp != 0L) && (unitRecp->getUnitType() == UNITTYPE_ENEMY_CHAR))
	{
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_DOWN))	// 倒した
		{
			// 敵は汎用パラメータ１が経験値
			const CharLogicData* cldat = (unitRecp->getSpirit() != 0L) ? unitRecp->getSpirit()->getCharLogicData() : 0L;
			experience = (cldat != 0L) ? cldat->getParam(CharLogicData::PARAM_EXP) : 0;
		}
	}

	// 経験値加算
	if(experience > 0)
	{
		if((scres->getActorUnit() != 0L) && TFW_IS_FLAG(scres->getActorUnit()->getUnitFlags(), Unit::UNITFLAG_PC))
		{
			onExperience(experience, frameDelay);
		}
	}
}

/*---------------------------------------------------------------------*//**
	キャラクタステータス変更の結果メッセージ表示
**//*---------------------------------------------------------------------*/
void Conductor::onCharStatResult(const StatusChangeResult* scres, s32 frameDelay)
{
	const Unit* unitTrg = scres->getNewCharStat()->getOwnerUnit();
	const VcString nameUnknown("???");
	const VcString* nameTrg = ((unitTrg != 0L) && (unitTrg->getMainName() != 0L)) ? unitTrg->getMainName() : &nameUnknown;
	ScoreBase* score = Game::getGame()->getScore();

	ASSERT(unitTrg != 0L);
	///ASSERT(scres->getActorUnit() != 0L);
	ASSERT(score != 0L);
	
	bool isShowDamage = false;
	bool isMakeMsg = false;
	bool isIgnoreState = TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_DOWN);

	if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_ACT_E_RECV))		// エネルギー回復
	{
		s32 val = ((- scres->getEneValuedDamage()) + CharStat::LFENE_VALID_FIGURE - 1) / CharStat::LFENE_VALID_FIGURE;

		#if ENABLE_POPNUM
			on(val, PopNumEffect::K_ENE_RECV, unitTrg);
		#else
			VcString msg(VcString::format(
					GameFixedStrTbl::getString(
						(val == 1) ? GameFixedStrTbl::SID_ENE_RECOVER : GameFixedStrTbl::SID_ENE_RECOVER_POINTS
					)->getRaw(),
					nameTrg->getRaw(),
					val,
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
		#endif
		isMakeMsg = true;
	}

	if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_ACT_E_DMG))		// エネルギーダメージ
	{
		s32 val = (scres->getEneValuedDamage() + CharStat::LFENE_VALID_FIGURE - 1) / CharStat::LFENE_VALID_FIGURE;

		#if ENABLE_POPNUM
			on(val, PopNumEffect::K_ENE_DMG, unitTrg);
		#else
			VcString msg(VcString::format(
					GameFixedStrTbl::getString(
						(val == 1) ? GameFixedStrTbl::SID_ENE_DAMAGE : GameFixedStrTbl::SID_ENE_DAMAGE_POINTS
					)->getRaw(),
					nameTrg->getRaw(),
					val,
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
		#endif
		isShowDamage = true;
		isMakeMsg = true;
	}

	if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_ACT_P_RECV))		// 物理回復
	{
		s32 val = ((- scres->getPhysValuedDamage()) + CharStat::LFENE_VALID_FIGURE - 1) / CharStat::LFENE_VALID_FIGURE;

		#if ENABLE_POPNUM
			on(val, PopNumEffect::K_PHYS_RECV, unitTrg);
		#else
			VcString msg(VcString::format(
					GameFixedStrTbl::getString(
						(val == 1) ? GameFixedStrTbl::SID_PHYS_RECOVER : GameFixedStrTbl::SID_PHYS_RECOVER_POINTS
					)->getRaw(),
					nameTrg->getRaw(),
					val,
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
		#endif
		isMakeMsg = true;
	}

	if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_ACT_P_DMG))		// 物理ダメージ
	{
		s32 val = (scres->getPhysValuedDamage() + CharStat::LFENE_VALID_FIGURE - 1) / CharStat::LFENE_VALID_FIGURE;

		#if ENABLE_POPNUM
			on(val, PopNumEffect::K_PHYS_DMG, unitTrg);
		#else
			VcString msg(VcString::format(
					GameFixedStrTbl::getString(
						(val == 1) ? GameFixedStrTbl::SID_PHYS_INJURY : GameFixedStrTbl::SID_PHYS_INJURY_POINTS
					)->getRaw(),
					nameTrg->getRaw(),
					val,
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
		#endif
		isShowDamage = true;
		isMakeMsg = true;
	}

	// 攻撃ミス
	if(!isShowDamage && ((scres->getTransStat()->getFuncFlags() & EleneStat::FF_MASK_OP) == EleneStat::FF_OP_ATTACK))	// ダメージを与えられなった
	{
		#if ENABLE_POPNUM
			on(0, PopNumEffect::K_NULL, unitTrg);
		#else
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_NO_DAMAGE)->getRaw(),
					nameTrg->getRaw(),
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
		#endif
		isMakeMsg = true;
	}
	else if((scres->getTransStat()->getFuncFlags() & EleneStat::FF_MASK_OP) == EleneStat::FF_OP_MISS_ATTACK)			// もともと攻撃ミス
	{
		#if ENABLE_POPNUM
			on(0, PopNumEffect::K_MISS, unitTrg);
		#else
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_MISS_ATTACK)->getRaw(),
					nameTrg->getRaw(),
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
		#endif
		isMakeMsg = true;
	}

	if(!isIgnoreState)
	{
		// 持続結界
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_STREN_WP_VEIL))		// 持続結界を強化した
		{
			isMakeMsg = onVeilResult(scres, nameTrg, EleneStat::VEIL_WP, true, frameDelay);
		}
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_WEAKN_WP_VEIL))		// 持続結界を弱体した
		{
			isMakeMsg = onVeilResult(scres, nameTrg, EleneStat::VEIL_WP, false, frameDelay);
		}

		// 瞬間結界
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_STREN_BR_VEIL))		// 瞬間結界を強化した
		{
			isMakeMsg = onVeilResult(scres, nameTrg, EleneStat::VEIL_BR, true, frameDelay);
		}
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_WEAKN_BR_VEIL))		// 瞬間結界を弱体した
		{
			isMakeMsg = onVeilResult(scres, nameTrg, EleneStat::VEIL_BR, false, frameDelay);
		}

		// 倦怠
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_HEAVY))			// 倦怠になった
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_BECOME_HEAVY)->getRaw(),
					nameTrg->getRaw(),
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
			isMakeMsg = true;

			// スコアカウント
			if(scres->isPcToEnemyAction())
			{
				score->upScore(Score::SC_EFF_ENEMY_HEAVY);		// ［スコア］敵を倦怠化した
			}
		}
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_OUT_HEAVY))		// 倦怠から回復した
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_WELL_HEAVY)->getRaw(),
					nameTrg->getRaw(),
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
			isMakeMsg = true;

			// スコアカウント
			if(scres->isPcAction())
			{
				score->upScore(Score::SC_EFF_PC_CURE_HEAVY);	// ［スコア］PC が倦怠から回復させた
			}
		}

		// 睡眠
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_SLEEP))			// 睡眠になった
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_BECOME_SLEEP)->getRaw(),
					nameTrg->getRaw(),
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
			isMakeMsg = true;

			// スコアカウント
			if(scres->isPcToEnemyAction())
			{
				score->upScore(Score::SC_EFF_ENEMY_SLEEP);	// ［スコア］敵を眠らせた
			}
		}
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_OUT_SLEEP))		// 睡眠から回復した
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_WELL_SLEEP)->getRaw(),
					nameTrg->getRaw(),
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
			isMakeMsg = true;

			// スコアカウント
			if(scres->isPcAction())
			{
				score->upScore(Score::SC_EFF_PC_CURE_SLEEP);	// ［スコア］PC が睡眠から回復させた
			}
		}

		// 錯乱
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_CONFU))			// 錯乱になった
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_BECOME_CONFU)->getRaw(),
					nameTrg->getRaw(),
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
			isMakeMsg = true;

			// スコアカウント
			if(scres->isPcToEnemyAction())
			{
				score->upScore(Score::SC_EFF_ENEMY_CONFU);	// ［スコア］敵を錯乱させた
			}
		}
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_OUT_CONFU))		// 錯乱から回復した
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_WELL_CONFU)->getRaw(),
					nameTrg->getRaw(),
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
			isMakeMsg = true;

			// スコアカウント
			if(scres->isPcAction())
			{
				score->upScore(Score::SC_EFF_PC_CURE_CONFU);	// ［スコア］PC が錯乱から回復させた
			}
		}

		// 半覚醒
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_TRANS))			// 半覚醒になった
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_BECOME_TRANS)->getRaw(),
					nameTrg->getRaw(),
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
			isMakeMsg = true;

			// スコアカウント
			if(scres->isPcAction())
			{
				score->upScore(Score::SC_EFF_PC_HENLIGHT);	// ［スコア］PC が半覚醒になった
			}
		}
		if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_OUT_TRANS))			// 半覚醒を終えた
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_OUT_TRANS)->getRaw(),
					nameTrg->getRaw(),
					"\n"));
			on(&msg, scres->getActorUnit(), frameDelay);
			isMakeMsg = true;
		}
	}

	// ダウン
	if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_DOWN))			// 活動停止した
	{
		VcString msg(VcString::format(
				GameFixedStrTbl::getString(
						(unitTrg->getUnitType() == UNITTYPE_ENEMY_CHAR) ? GameFixedStrTbl::SID_BECOME_ENEMY_CLEARED : GameFixedStrTbl::SID_BECOME_DOWN
					)->getRaw(),
				nameTrg->getRaw(),
				"\n"));
		on(&msg, scres->getActorUnit(), frameDelay);
		isMakeMsg = true;

		// スコアカウント
		if(scres->isPcToEnemyAction())
		{
			if(scres->isActionBySouma())		// ソウマによる
			{
				score->upScore(Score::SC_DEFEAT_ENEMY_BY_SOUMA);	// ［スコア］ソウマによって敵を倒した
			}
			else if(scres->isActionByWeapon())	// 武器による
			{
				score->upScore(Score::SC_DEFEAT_ENEMY_BY_WEAPON);	// ［スコア］武器によって敵を倒した
			}
		}
	}
	if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_ALIVE))			// 活動再開した
	{
		VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_BECOME_ALIVE)->getRaw(),
				nameTrg->getRaw(),
				"\n"));
		on(&msg, scres->getActorUnit(), frameDelay);
		isMakeMsg = true;

		// スコアカウント
		if(scres->isPcAction())
		{
			score->upScore(Score::SC_EFF_PC_CURE_DOWN);	// ［スコア］PC がダウンから回復させた
		}
	}

#if 0
	// 何も起きなかった（エフェクトは何も起きないものなので表示しない）
	if(!isMakeMsg && ((scres->getTransStat()->getFuncFlags() & EleneStat::FF_MASK_OP) != EleneStat::FF_OP_NOP))
	{
		VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_NOT_HAPPENED)->getRaw(),
				"\n"));
		on(&msg, scres->getActorUnit(), frameDelay);
	}
#endif
}

/*---------------------------------------------------------------------*//**
	ベール（結界）についての結果メッセージ表示
**//*---------------------------------------------------------------------*/
bool Conductor::onVeilResult(const StatusChangeResult* scres, const VcString* nameTrg, s32 iveil, bool isStren, s32 frameDelay)
{
	// 属性文字列を得る
	ASSERT(scres->getTransStat() != 0L);
	u32 ff = scres->getTransStat()->getFuncFlags();
	VcString strAtrb;
	getEleneAtrbString(&strAtrb, ff);

	// ベールを得る
	const EleneStat* eestatOld = scres->getOldCharStat()->getEleneStat(); ASSERT(eestatOld != 0L);
	const EleneVeil* veilOld = eestatOld->getVeil(iveil); ASSERT(veilOld != 0L);
	const EleneStat* eestatNew = scres->getNewCharStat()->getEleneStat(); ASSERT(eestatNew != 0L);
	const EleneVeil* veilNew = eestatNew->getVeil(iveil); ASSERT(veilNew != 0L);

	if(veilNew->isValid() && !veilOld->isValid())
	{
		// 新規作成
		VcString msg(VcString::format(GameFixedStrTbl::getString((iveil == EleneStat::VEIL_WP) ? GameFixedStrTbl::SID_APPEAR_WRAPPER :  GameFixedStrTbl::SID_APPEAR_BARIEER)->getRaw(),
				nameTrg->getRaw(),
				strAtrb.getRaw(),
				"\n"));
		on(&msg, scres->getActorUnit(), frameDelay);
		return true;
	}
	else if(!veilNew->isValid() && veilOld->isValid())
	{
		// 消滅
		VcString msg(VcString::format(GameFixedStrTbl::getString((iveil == EleneStat::VEIL_WP) ? GameFixedStrTbl::SID_DISAPPEAR_WRAPPER : GameFixedStrTbl::SID_DISAPPEAR_BARIEER)->getRaw(),
				nameTrg->getRaw(),
				"\n"));
		on(&msg, scres->getActorUnit(), frameDelay);
		return true;
	}
	else if(veilNew->isValid() && veilOld->isValid())
	{
		if(isStren)
		{
			// 強化
			VcString str;
			// 言語によって単語の定義順が違う
			// → 将来的に定義順を決められる format に移行．現状はマスター直前なので…【2010/09/28 r-kishi】
			if(Env_getLangId() == LANGID_JP)
			{
				str = VcString::format(GameFixedStrTbl::getString((iveil == EleneStat::VEIL_WP) ? GameFixedStrTbl::SID_STREN_WRAPPER : GameFixedStrTbl::SID_STREN_BARIEER)->getRaw(),
					nameTrg->getRaw(),
					strAtrb.getRaw(),
					"\n");
			}
			else
			{
				str = VcString::format(GameFixedStrTbl::getString((iveil == EleneStat::VEIL_WP) ? GameFixedStrTbl::SID_STREN_WRAPPER : GameFixedStrTbl::SID_STREN_BARIEER)->getRaw(),
					strAtrb.getRaw(),
					nameTrg->getRaw(),
					"\n");
			}
			on(&str, scres->getActorUnit(), frameDelay);
		}
		else
		{
			// 弱体
			VcString str;
			// 言語によって単語の定義順が違う
			// → 将来的に定義順を決められる format に移行．現状はマスター直前なので…【2010/09/28 r-kishi】
			if(Env_getLangId() == LANGID_JP)
			{
				str = VcString::format(GameFixedStrTbl::getString((iveil == EleneStat::VEIL_WP) ? GameFixedStrTbl::SID_WEAKN_WRAPPER : GameFixedStrTbl::SID_WEAKN_BARIEER)->getRaw(),
					nameTrg->getRaw(),
					strAtrb.getRaw(),
					"\n");
			}
			else
			{
				str = VcString::format(GameFixedStrTbl::getString((iveil == EleneStat::VEIL_WP) ? GameFixedStrTbl::SID_WEAKN_WRAPPER : GameFixedStrTbl::SID_WEAKN_BARIEER)->getRaw(),
					strAtrb.getRaw(),
					nameTrg->getRaw(),
					"\n");
			}
			on(&str, scres->getActorUnit(), frameDelay);
		}
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	元素属性文字列を得る
	※　将来的には GameFixedStrTbl に移行？
**//*---------------------------------------------------------------------*/
void Conductor::getEleneAtrbString(VcString* str, u32 ff)
{
	if(TFW_IS_FLAG(ff, EleneStat::FF_ATRB_SOIL))
	{
		str->add(GameFixedStrTbl::getString(GameFixedStrTbl::SID_SOIL));
	}
	if(TFW_IS_FLAG(ff, EleneStat::FF_ATRB_WATER))
	{
		if(str->getLength() > 0)	{	str->add(GameFixedStrTbl::getString(GameFixedStrTbl::SID_EL_PLUS));	}
		str->add(GameFixedStrTbl::getString(GameFixedStrTbl::SID_WATER));
	}
	if(TFW_IS_FLAG(ff, EleneStat::FF_ATRB_FIRE))
	{
		if(str->getLength() > 0)	{	str->add(GameFixedStrTbl::getString(GameFixedStrTbl::SID_EL_PLUS));	}
		str->add(GameFixedStrTbl::getString(GameFixedStrTbl::SID_FIRE));
	}
	if(TFW_IS_FLAG(ff, EleneStat::FF_ATRB_WIND))
	{
		if(str->getLength() > 0)	{	str->add(GameFixedStrTbl::getString(GameFixedStrTbl::SID_EL_PLUS));	}
		str->add(GameFixedStrTbl::getString(GameFixedStrTbl::SID_WIND));
	}
	if(TFW_IS_FLAG(ff, EleneStat::FF_ATRB_AETHEL))
	{
		if(str->getLength() > 0)	{	str->add(GameFixedStrTbl::getString(GameFixedStrTbl::SID_EL_PLUS));	}
		str->add(GameFixedStrTbl::getString(GameFixedStrTbl::SID_AETHEL));
	}
}

/*---------------------------------------------------------------------*//**
	経験値獲得
**//*---------------------------------------------------------------------*/
void Conductor::onExperience(u16 experience, s32 frameDelay)
{
	s32 exp = experience;
	s32 expChainBonus = 0;

	// チェーンボーナス
	if(_fcntChainBonus > 0.0f)	// チェーンボーナスフレーム内
	{
		// チェーンボーナス値
		expChainBonus = exp / 10;
		if(expChainBonus < 1)
		{
			expChainBonus = 1;
		}
	}
	_fcntChainBonus = FRAMECNT_CHAIN_BONUS;	// 次のチェーンボーナスフレームを設定

	// 有効な PC 全員が経験値を取得する（ダウンはしていてもＯＫ）
	s32 numEnalbedPc = 0;
	Party* partyPc = Game::getGame()->getMyParty();
	const VcString* namePcFirst = 0L;
	for(int ipc = 0; ipc < partyPc->getPcNum(); ipc++)
	{
		if(!partyPc->isEnablePc(ipc))	{	continue;	}
		numEnalbedPc++;

		// 最初の者の名前を得る
		if(namePcFirst == 0L)
		{
			const CharUnit* unit = partyPc->getPcUnit(ipc);
			ASSERT((unit != 0L) && TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_PC));
			namePcFirst = unit->getMainName();
			ASSERT(namePcFirst != 0L);
		}
	}
	if(numEnalbedPc <= 0)	{	return;	}	// タイミングによってはありうるかも？

	// 経験値を有効な PC の数で割る
	s32 expOnePc = ((exp + numEnalbedPc - 1) / numEnalbedPc) + expChainBonus;
	ASSERT(expOnePc > 0);

	// 経験値取得メッセージ色
	ColorU8 colorMsg;
	getSysRewardColor(&colorMsg);

	// チェーンボーナスメッセージ
	if(expChainBonus > 0)
	{
		VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GET_EXP_CHAIN)->getRaw(),
				expChainBonus,
				"\n"));
		on(&msg, &colorMsg, frameDelay);
	}

	// 経験値取得メッセージ
	if(numEnalbedPc >= 2)
	{
		if(Env_getLangId() == LANGID_JP)
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GET_EXPERIENCE_EACH)->getRaw(),
					namePcFirst->getRaw(),
					expOnePc,
					"\n"));
			on(&msg, &colorMsg, frameDelay);
		}
		else
		{
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GET_EXPERIENCE_EACH)->getRaw(),
					expOnePc,
					"\n"));
			on(&msg, &colorMsg, frameDelay);
		}
	}
	else
	{
		VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GET_EXPERIENCE)->getRaw(),
				namePcFirst->getRaw(),
				expOnePc,
				"\n"));
		on(&msg, &colorMsg, frameDelay);
	}

	// それぞれの PC に経験値を加算する
	for(int ipc = 0; ipc < partyPc->getPcNum(); ipc++)
	{
		if(!partyPc->isEnablePc(ipc))	{	continue;	}

		PcUnit* unitPc = (PcUnit*)partyPc->getPcUnit(ipc);

		// 経験値取得とレベルアップ処理
		CharParam cprmOld, cprmNew;
		if(onExperiencePc(&cprmOld, &cprmNew, unitPc, expOnePc, frameDelay))
		{
			const VcString* namePc = unitPc->getMainName(); ASSERT(namePc != 0L);

			// レベル上昇メッセージ
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_LV_UP)->getRaw(),
					namePc->getRaw(),
					cprmOld.getLevel(),
					cprmNew.getLevel(),
					"\n"));
			on(&msg, &colorMsg, frameDelay);

			#if 0
			{
				// エネルギー値上昇メッセージ
				if(cprmOld.getEnergy() != cprmNew.getEnergy())
				{
					s32 val = (cprmNew.getEnergy() - cprmOld.getEnergy()) / CharParam::VALUE_DEFAULT;

					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_UP_ENERGY)->getRaw(),
							namePc->getRaw(),
							val,
							"\n"));
					on(&msg, &colorMsg, frameDelay);
				}

				// 集中力上昇メッセージ
				if(cprmOld.getUnity() != cprmNew.getUnity())
				{
					s32 val = cprmNew.getUnity() - cprmOld.getUnity();

					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_UP_UNI)->getRaw(),
							namePc->getRaw(),
							val,
							"\n"));
					on(&msg, &colorMsg, frameDelay);
				}

				// 活動力上昇メッセージ
				if(cprmOld.getActivity() != cprmNew.getActivity())
				{
					s32 val = cprmNew.getActivity() - cprmOld.getActivity();

					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_UP_ACT)->getRaw(),
							namePc->getRaw(),
							val,
							"\n"));
					on(&msg, &colorMsg, frameDelay);
				}

				// 安定力上昇メッセージ
				if(cprmOld.getStability() != cprmNew.getStability())
				{
					s32 val = cprmNew.getStability() - cprmOld.getStability();

					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHARPARAM_UP_STB)->getRaw(),
							namePc->getRaw(),
							val,
							"\n"));
					on(&msg, &colorMsg, frameDelay);
				}
			}
			#endif

			// エフェクト表示
			ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
			ptclsys->appearParticle(GameParticleDef::PTCLK_LVUP, unitPc->getCenterPos(), 0);
		}
	}
}

/*---------------------------------------------------------------------*//**
	経験値取得と PC のレベルアップ処理

	@retval true レベルアップした
	@retval false レベルアップしなかった
**//*---------------------------------------------------------------------*/
bool Conductor::onExperiencePc(CharParam* cprmOldOut, CharParam* cprmNewOut, PcUnit* unitPc, s32 experience, s32 frameDelay)
{
	CharStat* cstatPc = unitPc->charStat(); ASSERT(cstatPc != 0L);
	CharParam* cprmPc = cstatPc->charParam(); ASSERT(cprmPc != 0L);

	// PC の識別
	s32 ipc = -1;
	switch(unitPc->getUnitType())
	{
	case UNITTYPE_MY_CHAR:		ipc = 0;	break;
	case UNITTYPE_PARTNER_CHAR:	ipc = 1;	break;
	case UNITTYPE_HELPER_CHAR:	ipc = 2;	break;
	}
	ASSERT(ipc != -1);
	if(ipc == -1)	{	return false;	}

	// 経験値加算
	u16 expPcNew = cprmPc->getExperience();
	if((expPcNew + experience) >= U16_MAX)	{	expPcNew = U16_MAX;		}
	else									{	expPcNew += experience;	}
	// 各ボリュームは既定経験値まで
	#if defined(_ENLIGHT_V01) && !defined(_DEBUG)
		if(expPcNew >= CharParam::EXP_MAX_VOL01)	{	expPcNew = CharParam::EXP_MAX_VOL01;	}
	#endif
	cprmPc->setExperience(expPcNew);

	// レベルアップ処理
	bool isLevelUp = false;
	cprmOldOut->copy(cprmPc);
	const FixedCharParamTbl* fcprmtbl = FixedData::getFixedCharParamTable();
	for(s32 ilv = 0; ilv < fcprmtbl->getLevelNum(); ilv++)
	{
		// Volume01 は レベル 10 まで．11 以上は上がらない
		#if defined(_ENLIGHT_V01) && !defined(_DEBUG)
			if(ilv >= 10)	{	break;	}
		#endif

		const CharParam* cprmWk = fcprmtbl->getParam(ipc, ilv);
		if(expPcNew >= cprmWk->getExperience())	// 到達しているレベルである
		{
			if(cprmPc->getLevel() < cprmWk->getLevel())
			{
				// レベルアップ
				cstatPc->levelup(cprmWk);
				isLevelUp = true;
			}
		}
		else
		{
			break;
		}
	}

	cprmNewOut->copy(cprmPc);
	return isLevelUp;
}

//==========================================================================
// Conductor::Msg メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Conductor::Msg::Msg()
	: _strMsg(0L)
	, _color(0L)
	, _frameRest(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Conductor::Msg::~Msg()
{
	delete _strMsg;
	delete _color;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
