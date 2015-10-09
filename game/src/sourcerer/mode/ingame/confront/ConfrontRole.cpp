/***********************************************************************//**
 *	ConfrontRole.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/25.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ConfrontRole.h"

// Friends
#include "ConfrontMode.h"
#include "../../../camera/ConfrontCam.h"
#include "../../../confront/Confront.h"
#include "../../../event/EventIdDef.h"
#include "../../../event/EventSys.h"
#include "../../../body/Conductor.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/Party.h"
#include "../../../gui/window/implement/GameActMsgWindow.h"
#include "../../../unit/char/pc/PcUnit.h"

// External
#include "../../../../etk/effect/ScreenEffect.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ConfrontRole メソッド

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理
**//*---------------------------------------------------------------------*/
int ConfrontRole::execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	switch(param->_cid)
	{
	case START:			return execCommand_start(res, ec, param);
	case STABLE:		return execCommand_stable(res, ec, param);
	case CONCLUSION:	return execCommand_conclusion(res, ec, param);
	case END:			return execCommand_end(res, ec, param);
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	開始演出
**//*---------------------------------------------------------------------*/
int ConfrontRole::execCommand_start(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_WAIT			= 1;
	const int	STEP_OUT			= 2;
	
	int step = param->_step;
	
	switch(step)
	{
	case STEP_WAIT:
		if(param->_ecntStep == 1)
		{
			_camCfRef->setMode(ConfrontCam::MODE_WHOLE);	//ConfrontCam::MODE_CHARS);	// カメラモード設定

			// メッセージ表示
			ASSERT(_confrontRef != 0L);
			if(_confrontRef->getFrameCount() == 0.0f)	// フレームゼロのはずである（あまり良い判定方法ではないか？）
			{
				// 敵の数を数える
				const Unit* unitEnemy = 0L;
				s32 cntEnemy = countEnemy(&unitEnemy);
				// 敵によってメッセージ変更
				const PcUnit* pcunitLeader = Game::getGame()->getMyParty()->getLeaderPcUnit();
				if(cntEnemy == 1)		// 敵が単体
				{
					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_ENTER_BATTLE)->getRaw(),
							((pcunitLeader != 0L) && (pcunitLeader->getMainName() != 0L)) ? pcunitLeader->getMainName()->getRaw() : "???",
							((unitEnemy != 0L) && (unitEnemy->getMainName() != 0L)) ? unitEnemy->getMainName()->getRaw() : "???",
							"\n"));
					Game::getGame()->getConductor()->on(&msg, (const Unit*)0L, 0);
				}
				else if(cntEnemy >= 2)	// 敵が複数
				{
					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_ENTER_BATTLE_PLURAL)->getRaw(),
							((pcunitLeader != 0L) && (pcunitLeader->getMainName() != 0L)) ? pcunitLeader->getMainName()->getRaw() : "???",
							"\n"));
					Game::getGame()->getConductor()->on(&msg, (const Unit*)0L, 0);
				}
			}
		}
		else if(param->_frmcntStep >= 120.0f)
		{
			step = STEP_OUT;
		}
		break;

	case STEP_OUT:
		res->setDone();
		break;
	}

	return step;
}

/*---------------------------------------------------------------------*//**
	定常時演出
**//*---------------------------------------------------------------------*/
int ConfrontRole::execCommand_stable(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	if(param->_ecntCmd == 1)
	{
		_camCfRef->setMode(ConfrontCam::MODE_WHOLE);	// カメラモード設定
	}

	// 対象が全員ダウンしたら戦闘終了
	if(_confrontRef->isAllTargetDown() || _confrontRef->isMyUnitDown())
	{
		// 現在の子モードが通常以外だったら通常に戻す
		if(this->getOwnerMode()->getCurrentModeId() != GameMode::MODE_GAME_INGAME_CONFRONT_NORMAL)
		{
			///this->getOwnerMode()->reserveMode(GameMode::MODE_GAME_INGAME_CONFRONT_NORMAL);
			Game::getGame()->getGameMode()->changePrevEndMode(GameMode::MODE_NULL);
		}

		// ロールコマンドを次へ
		setForceNextCommand(true);
	}

	return -1;
}

/*---------------------------------------------------------------------*//**
	決着演出
**//*---------------------------------------------------------------------*/
int ConfrontRole::execCommand_conclusion(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_WAIT			= 1;
	const int	STEP_OUT			= 2;

	const f32	FRAME_WAIT			= 60.0f;
	
	// 対決対象追加などで終了でなくなった場合は復帰する
	if(!_confrontRef->isAllTargetDown() && !_confrontRef->isMyUnitDown())
	{
		setForceJumpCommand(STABLE);
		return 0;
	}
	
	int step = param->_step;

	switch(step)
	{
	case STEP_WAIT:
		if(_camCfRef->getCurrentMode() == ConfrontCam::MODE_DEFAULT)	// デフォルトカメラ時
		{
			// カメラモード設定
			_camCfRef->setMode(ConfrontCam::MODE_PARTY);
			// カメラ演出設定
			switch(::rand() & 3)
			{
			case 0:	break;
			case 1:	_camCfRef->startPerformance(ConfrontCam::PERFF_ROTATE, TFW_DEG_TO_RAD(-30.0f), 10.0f);	break;
			case 2:	_camCfRef->startPerformance(ConfrontCam::PERFF_LEAVE, TFW_DEG_TO_RAD(-30.0f), 10.0f);	break;
			case 3:	_camCfRef->startPerformance(ConfrontCam::PERFF_ROTATE | ConfrontCam::PERFF_LEAVE, TFW_DEG_TO_RAD(-30.0f), 10.0f);	break;
			}
		}

		if(param->_frmcntStep >= FRAME_WAIT)
		{
			step = STEP_OUT;
		}
		else
		{
			// WAIT スキップ
			GameExecCtx* gec = (GameExecCtx*)ec;
			TouchPanel* ui = gec->getTouchPanel(); 
			if(!ui->isReacted())
			{
				if(ui->isClickRelease(0L))
				{
					step = STEP_OUT;
					ui->setReacted(true);
				}
			}
		}
		break;

	case STEP_OUT:
		res->setDone();	// END へ
		break;
	}

	return step;
}

/*---------------------------------------------------------------------*//**
	終了演出
**//*---------------------------------------------------------------------*/
int ConfrontRole::execCommand_end(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_WAIT			= 1;
	const int	STEP_OUT			= 2;
	
	const f32	FRAME_WAIT			= 120.0f;
	
	// 対決対象追加などで終了でなくなった場合は復帰する
	if(!_confrontRef->isAllTargetDown() && !_confrontRef->isMyUnitDown())
	{
		setForceJumpCommand(STABLE);
		return 0;
	}
	
	int step = param->_step;
	
	switch(step)
	{
	case STEP_WAIT:
		{
			if(param->_ecntStep == 1)
			{
				// 自キャラダウン時
				if(_confrontRef->isMyUnitDown())	// 負け
				{
					// ロール終了
					res->setDone();
					// 対決終了
					bool isBegunAfterEvent = false;
					_confrontRef->end(&isBegunAfterEvent);

					// 主人公ダウンイベント起動
					if(!isBegunAfterEvent)	// ただし、敗北時イベントが設定されていないときのみ
					{
						EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
						EvCause evcause(EventCause::CID_NULL);
						evsys->begin(EventIdDef::EVID_FIXED_DOWN_ME, 0, &evcause);
					}
				}
				else								// 勝ち
				{
					#if 0
						// メッセージ表示
						ASSERT(_confrontRef != 0L);
						// 敵の数を数える
						const Unit* unitEnemy = 0L;
						s32 cntEnemy = countEnemy(&unitEnemy);
						if(cntEnemy == 1)			// 敵が単体
						{
							VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_WIN_BATTLE)->getRaw(),
									((unitEnemy != 0L) && (unitEnemy->getMainName() != 0L)) ? unitEnemy->getMainName()->getRaw() : "???",
									"\n"));
							Game::getGame()->getConductor()->on(
								&msg,
								(const Unit*)0L,
								0	);
						}
						else if(cntEnemy >= 2)		// 敵が複数
						{
							VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_WIN_BATTLE_PLURAL)->getRaw(),
									"\n"));
							Game::getGame()->getConductor()->on(
								&msg,
								(const Unit*)0L,
								0	);
						}
					#endif
				}
			}
			else if(param->_frmcntStep >= FRAME_WAIT)
			{
				step = STEP_OUT;
			}
			else
			{
				// WAIT スキップ
				GameExecCtx* gec = (GameExecCtx*)ec;
				TouchPanel* ui = gec->getTouchPanel(); 
				if(!ui->isReacted())
				{
					if(ui->isTapTrigger(0L))	//(ui->isClickRelease(0L))
					{
						step = STEP_OUT;
						ui->setReacted(true);
					}
				}
			}
		}
		break;

	case STEP_OUT:
		// ロール終了
		res->setDone();
		// 対決終了
		_confrontRef->end(0L);
		break;
	}

	return step;
}

/*---------------------------------------------------------------------*//**
	敵の数を数える
**//*---------------------------------------------------------------------*/
s32 ConfrontRole::countEnemy(const Unit** unitEnemyFirst) const
{
	s32 cntEnemy = 0;

	const UnitArray* uaTrg = _confrontRef->getTargets();
	for(int i = 0; i < uaTrg->getCount(); i++)
	{
		if(uaTrg->getUnit(i)->getUnitType() == UNITTYPE_ENEMY_CHAR)
		{
			cntEnemy++;

			if((unitEnemyFirst != 0L) && (*unitEnemyFirst == 0L))
			{
				*unitEnemyFirst = uaTrg->getUnit(i);
			}
		}
	}

	return cntEnemy;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
