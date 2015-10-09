/***********************************************************************//**
 *	PcRole.cpp
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
#include "PcRole.h"

// Friends
#include "PcSpirit.h"
#include "../../../confront/Confront.h"
#include "../../../effect/StatusChangeEffect.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/GameNotifier.h"
#include "../../../body/Party.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../item/Item.h"
#include "../../../item/ItemManager.h"
#include "../../../quest/QuestManager.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/StatusIfEx.h"
#include "../../../status/TransStat.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/pc/PcUnit.h"
#include "../../../unit/item/FieldItemUnit.h"

// External
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/modemng/Role.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PcRole メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PcRole::PcRole(PcSpirit* sprtRef)
	: _sprtRef(sprtRef)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PcRole::~PcRole()
{
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 何もしない
**//*---------------------------------------------------------------------*/
int PcRole::execCommand_nothing(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 通常歩行
**//*---------------------------------------------------------------------*/
int PcRole::execCommand_walkNormal(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	// 目標位置の計算
	_sprtRef->pcUnit()->calcTargetPos();

	// 歩行フレーム処理
	_sprtRef->pcUnit()->execWalking(ec, false);

	return 0;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - アイテムを使う
**//*---------------------------------------------------------------------*/
int PcRole::execCommand_useItem(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_ANIM_START		= 1;
	const int	STEP_ANIM_WAIT		= 2;
	const int	END					= 999;
	const f32	FRAME_WAIT_STEP		= 0.5f * FRAMERATE;
	const f32	FRAME_END_STEP		= 2.0f * FRAMERATE;

	int step = param->_step;
	switch(step)
	{
	case STEP_ANIM_START:

		{
			// 対象アイテム取得
			const ItemDef* itmdf = 0L;
			do
			{
				u16 itmgidRsvUsing = _sprtRef->getReservedBefaviorParameter();
				if(itmgidRsvUsing == 0)	{	break;	}
				ItemManager* itmmng = _sprtRef->getPcUnit()->getPartyRef()->getItemManager(); ASSERT(itmmng != 0L);
				Item* item = itmmng->itemFromGenId(itmgidRsvUsing);
				if(item == 0L)			{	break;	}
				itmdf = item->getItemDef();
			}
			while(false);
			if(itmdf == 0L)	// 取得失敗
			{
				res->setDone();
				return END;
			}

			// メッセージを表示する
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_USE_FIRST)->getRaw(),
					_sprtRef->getPcUnit()->getMainName()->getRaw(),
					itmdf->getName(Env_getLangId())->getRaw(),
					"\n"));
			Game::getGame()->getConductor()->on(&msg, _sprtRef->getPcUnit(), 0);
			// SE
			///Game::getGame()->getSoundManager()->playSe(???, false, 1.0f, _sprtRef->getThisUnit()->getCenterPos(), 0L);

			// アニメーション変更
			_sprtRef->pcUnit()->changeAnim(CharUnit::ANIMID_USE_ITEM, false);

			_sprtRef->setBefaviorMaxFrame(FRAME_WAIT_STEP);	// 最大の行動フレーム数設定
		}

		step = STEP_ANIM_WAIT;
		break;

	case STEP_ANIM_WAIT:
		if(param->_frmcntStep >= FRAME_WAIT_STEP)	// 実行フレーム
		{
			// 対象アイテム取得
			ItemManager* itmmng = _sprtRef->getPcUnit()->getPartyRef()->getItemManager(); ASSERT(itmmng != 0L);
			Item* item = 0L;
			do
			{
				u16 itmgidRsvUsing = _sprtRef->getReservedBefaviorParameter();
				if(itmgidRsvUsing == 0)	{	break;	}
				item = itmmng->itemFromGenId(itmgidRsvUsing);
			}
			while(false);
			if(item == 0L)
			{
				res->setDone();
				return END;
			}

			// 効果
			CharStat* cstatSelf = _sprtRef->pcUnit()->charStat(); ASSERT(cstatSelf != 0L);
			TransStat tstat;
			tstat.setupForUsingItem(_sprtRef->pcUnit(), item->getEleneStat());
			switch(tstat.getFuncFlags() & EleneStat::FF_MASK_OP)
			{
			case EleneStat::FF_NULL:
			case EleneStat::FF_OP_IMMATURE:
			case EleneStat::FF_OP_OVERBREAK:
				{
					// メッセージを表示する 「何も起きなかった。」
				   VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_NOT_HAPPENED)->getRaw(), "\n"));
					Game::getGame()->getConductor()->on(&msg, _sprtRef->getPcUnit(), 0);
				}
				break;
			default:
				StatusIfEx::influence(cstatSelf, &tstat, true, true);	// ステータス影響
				break;
			}
			///_sprtRef->reserveUsingItem(0);						// 予約クリア
			_sprtRef->startBehavior(Spirit::BEHV_DEFAULT, 0L);		// 行動を戻す
			itmmng->used(item);	

			// ★★★トロフィー処理挿入場所候補★★★

			step = END;
		}
		_sprtRef->setBefaviorCurrentFrame(param->_frmcntStep);	// 現在の行動フレーム数設定
		break;
		
	case END:
		if(param->_frmcntStep >= FRAME_END_STEP)
		{
			_sprtRef->resetBefaviorFrame();		// 行動フレームリセット
			res->setDone();
			break;
		}
		break;
	}

	//STRACE("... use_item ...\n");
	
	return step;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - アイテムを拾う
**//*---------------------------------------------------------------------*/
int PcRole::execCommand_pickupItem(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_ANIM_START		= 1;
	const int	STEP_ANIM_WAIT		= 2;
	const int	END					= 999;
	
	int step = param->_step;

	switch(step)
	{
	case STEP_ANIM_START:

		step = STEP_ANIM_WAIT;

		{
			// アイテムの空き確認
			ItemManager* itmmng = Game::getGame()->getMyParty()->getItemManager(); ASSERT(itmmng != 0L);
			if(itmmng->getEmptyItemIndex(false) == -1)
			{
				res->setDone();	// この時点で空きがなかったら失敗
				return END;
			}

			// アニメーション変更
			_sprtRef->pcUnit()->changeAnim(CharUnit::ANIMID_PICKUP_ITEM, false);
		}

		break;

	case STEP_ANIM_WAIT:
		if(param->_frmcntStep >= 20.0f)	// 実行フレーム
		{
			// 対象アイテム取得
			FieldItemUnit* fiunit = 0L;
			const ItemDef* itmdf = 0L;
			do
			{
				u16 unitidRsvPickup = _sprtRef->getReservedBefaviorParameter();
				if(unitidRsvPickup == 0)	{	break;	}
				UnitManager* unitmng = Game::getGame()->getUnitManager();	ASSERT(unitmng != 0L);
				Unit* unit = unitmng->getUnitFromId(unitidRsvPickup);
				if((unit == 0L) || (unit->getUnitType() != UNITTYPE_FIELD_ITEM))	{	break;	}
				fiunit = (FieldItemUnit*)unit;
				itmdf = fiunit->getItemDef();
			}
			while(false);
			if(itmdf == 0L)
			{
				res->setDone();
				return END;
			}

			// アイテムを拾う
			ItemManager* itmmng = Game::getGame()->getMyParty()->getItemManager(); ASSERT(itmmng != 0L);
			if(itmmng->pickupItem(Game::getGame()->getMap(), fiunit->getPlacementObj(), fiunit->getEleneStat()))
			{
				// 成功時
				fiunit->setEnable(false);	// ユニット無効化

				// メッセージを表示する
				VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GET_SOMETHING)->getRaw(), itmdf->getName(Env_getLangId())->getRaw(), "\n"));
				Game::getGame()->getConductor()->on(&msg, _sprtRef->getPcUnit(), 0);
				// SE
				///Game::getGame()->getSoundManager()->playSe(???, false, 1.0f, _sprtRef->getThisUnit()->getCenterPos(), 0L);
				// 通知発行
				Game::getGame()->getGameNotifier()->notifyPickupItem(_sprtRef->getPcUnit(), fiunit);
				// クエストの可変達成状態の更新
				QuestManager* qsmng = Game::getGame()->getQuestManager();
				qsmng->updateVariableAchievementState();
			}

			step = END;
		}
		break;
		
	case END:
		if(param->_frmcntStep >= 10.0f)
		{
			res->setDone();
		}
		break;
	}

	//STRACE("... use_item ...\n");
	
	return step;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 対決開始
**//*---------------------------------------------------------------------*/
int PcRole::execCommand_enterCf(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_ANIM_START		= 1;
	const int	STEP_ANIM_WAIT		= 2;
	const int	END					= 999;

	int step = param->_step;
	switch(step)
	{
	case STEP_ANIM_START:
		// アニメーション変更
		_sprtRef->pcUnit()->changeAnim(CharUnit::ANIMID_CF_ENTER, false);
		step = STEP_ANIM_WAIT;
		break;

	case STEP_ANIM_WAIT:
		if(param->_frmcntStep >= 15.0f)
		{
			step = END;
		}
		break;
		
	case END:
		res->setDone();
		break;
	}

	return step;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 対決終了
**//*---------------------------------------------------------------------*/
int PcRole::execCommand_leaveCf(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_ANIM_START		= 1;
	const int	STEP_ANIM_WAIT		= 2;
	const int	END					= 999;

	int step = param->_step;
	switch(step)
	{
	case STEP_ANIM_START:
		// アニメーション変更
		_sprtRef->pcUnit()->changeAnim(CharUnit::ANIMID_CF_LEAVE, false);
		step = STEP_ANIM_WAIT;
		break;

	case STEP_ANIM_WAIT:
		if(param->_frmcntStep >= 15.0f)
		{
			step = END;
		}
		break;
		
	case END:
		res->setDone();
		break;
	}

	return step;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
