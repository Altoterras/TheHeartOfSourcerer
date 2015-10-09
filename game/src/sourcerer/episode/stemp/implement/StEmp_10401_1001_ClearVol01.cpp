/***********************************************************************//**
 *	StEmp_10401_1001_ClearVol01.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StEmp_10401_1001_ClearVol01.h"

// Friends
#include "../StEmpType.h"
#include "../../StDepEvDef.h"
#include "../../../episode/char/pc/MyUnit.h"
#include "../../../episode/char/pc/PartnerUnit.h"
#include "../../../episode/char/pc/HelperUnit.h"
#include "../../../episode/party/MyParty.h"
#include "../../../episode/save/Score.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameTaskModDef.h"
#include "../../../gui/GameFontSet.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/SaveLoadWindow.h"
#include "../../../map/PlacementObj.h"
#include "../../../mode/GameMode.h"
#include "../../../save/SaveData.h"
#include "../../../unit/Unit.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/npc/NpcCharUnit.h"
#include "../../../status/CharStat.h"

// External
#include "../../../../etk/effect/ScreenEffect.h"
#include "../../../../tfw/framemod/FedTaskSys.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StEmp_10401_1001_ClearVol01 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void StEmp_10401_1001_ClearVol01::begin(u32 param)
{
	GameGui* gui = Game::getGame()->getGui();

	// 大猿の体力を全快に
	UnitManager* unitmng = Game::getGame()->getUnitManager();
	Unit* unit = unitmng->findUnitFromCtrlEvid(StDepEvDef::CTRLEVID_EP01_SARUNAHA_ENEMY);
	if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_NPC))
	{
		NpcCharUnit* cunit = (NpcCharUnit*)unit;
		// 回復
		CharStat* cstat = cunit->charStat();
		cstat->recoverFull();
		// 所定の位置に戻す
		const PlacementObj* pobj = cunit->getPlacementObj();
		if(pobj != 0L)
		{
			cunit->setPosition(pobj->getPosition());
		}
	}

	// プレイヤーキャラクタ強制移動
	Party* party = Game::getGame()->getMyParty();
	CharUnit* cunitPc1 = party->pcUnit(MyParty::PC_MY);
	CharUnit* cunitPc2 = party->pcUnit(MyParty::PC_PTN);
	CharUnit* cunitPc3 = party->pcUnit(MyParty::PC_HLP);
	Vector3F pos;
	cunitPc1->setPosition(Vector3F::setout(&pos, 5700.0f, 0.0f, -2650.0f));
	cunitPc2->setPosition(Vector3F::setout(&pos, 5700.0f, 0.0f, -2500.0f));
	cunitPc3->setPosition(Vector3F::setout(&pos, 5700.0f, 0.0f, -2800.0f));

	// セーブウインドウの作成
	_wndSave = new SaveLoadWindow();
	if((_wndSave == 0L) || !_wndSave->create(gui->getFontSet()->getFont(GameFontSet::JP), gui->getGuiTexture(), SaveLoadWindow::PAGE_SAVE, true, SaveData::SVF_CLEARING))
	{
		ASSERT(false);
		delete _wndSave;
		_wndSave = 0L;
	}

	// GUI タスクにセーブウインドウを登録
	FedTaskSys* tasksys = Game::getGame()->getTaskSys(); ASSERT(tasksys != 0L);
	ASSERT(!tasksys->isExistModule(GameTaskModDef::MOD_INTVL_SAVE_WND));
	if (!tasksys->registerExecModule(_wndSave, GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_INTVL_SAVE_WND), GameTaskModDef::MOD_INTVL_SAVE_WND, EtkTaskModDef::MGF_WINDOW)) { ASSERT(false); }
	if (!tasksys->registerDrawModule(_wndSave, GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_INTVL_SAVE_WND), false, GameTaskModDef::MOD_INTVL_SAVE_WND, EtkTaskModDef::MGF_WINDOW)) { ASSERT(false); }

	// ［スコア］Vol01 クリア
	ScoreBase* score = Game::getGame()->getScore(); ASSERT(score != 0L);
	if(score->getScoreCount(Score::SC_CLEAR_VOL01) < score->getScoreCount(Score::SC_NEW_GAME_VOL01))	// ニューゲームの数以上はカウントしない
	{
		score->upScore(Score::SC_CLEAR_VOL01);
	}

	// PSNS にスコア送信
	score->updatePsnsScore();

	StEmp::begin(param);
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void StEmp_10401_1001_ClearVol01::end()
{
	// GUI タスクからセーブウインドウを削除
	FedTaskSys* tasksys = Game::getGame()->getTaskSys(); ASSERT(tasksys != 0L);
	if(!tasksys->removeModule(GameTaskModDef::MOD_INTVL_SAVE_WND))	{	ASSERT(false);	}

	// セーブウインドウの削除
	if(_wndSave != 0L)
	{
		_wndSave->destroy();
		delete _wndSave;
		_wndSave = 0L;
	}

	StEmp::end();

	// タイトルへ
	Game::getGame()->getGameMode()->changeEndMode(GameMode::MODE_GAME_TITLE);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10401_1001_ClearVol01::StEmp_10401_1001_ClearVol01()
	: StEmp(STEMPTYPE_10401_1001)
	, _wndSave(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10401_1001_ClearVol01::~StEmp_10401_1001_ClearVol01()
{
	ASSERT(_wndSave == 0L);
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StEmp_10401_1001_ClearVol01::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel*	touchp = gec->getTouchPanel();

	s16 step = getStep();
	switch(step)
	{
	case STEP_SAVE_FIRST:
		{
			// セーブウインドウを表示
			if(_wndSave != 0L)
			{
				_wndSave->showWindow(true);
			}

			// 背景塗りつぶし
			ScreenEffect* screffBack = Game::getGame()->getSceneScreenEffect();
			ColorU8 col1(0, 0, 0, 255);
			screffBack->setColor1(&col1);
			screffBack->setMaxFrame(-1);
			screffBack->start(ScreenEffect::MODE_FILL);

			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_DO_SAVE:
		if((_wndSave == 0L) || _wndSave->isDone())
		{
			// セーブウインドウは描画のみに設定
			ASSERT(_wndSave != 0L);
			if(_wndSave != 0L)	{	_wndSave->setActFlags(Window::AWNDF_DRAW_ONLY, true);	}

			// 次のステップへ
			advanceStep();

			// フェードアウト
			ScreenEffect* screffFore = Game::getGame()->getWholeScreenEffect();
			ColorU8 col1(0, 0, 0, 255);
			screffFore->setColor1(&col1);
			screffFore->setMaxFrame(120);
			screffFore->start(ScreenEffect::MODE_FADE_OUT);
		}
		break;
	case STEP_END:
		{
			// フェードアウトが終了したら終わり
			ScreenEffect* screffFore = Game::getGame()->getWholeScreenEffect();
			if(!screffFore->isWorking())
			{
				screffFore->end(0);
				// 背景塗りつぶしも終了
				ScreenEffect* screffBack = Game::getGame()->getSceneScreenEffect();
				screffBack->end(0);

				// 終了
				this->end();
			}
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS