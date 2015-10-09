/***********************************************************************//**
 *	StEmp_10101_1002_MagicTutorial02.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/22.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StEmp_10101_1002_MagicTutorial02.h"

// Friends
#include "../StEmpType.h"
#include "../../char/pc/MyUnit.h"
#include "../../party/MyParty.h"
#include "../../../common/SimpleSprite.h"
#include "../../../event/EventSys.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameRenderCtx.h"
#include "../../../gui/CharFronts.h"
#include "../../../gui/FocusCursor.h"
#include "../../../gui/FocusMenu.h"
#include "../../../gui/GameFontSet.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/IngameFront.h"
#include "../../../gui/MoveCursor.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../magic/MagicCluster.h"
#include "../../../mode/GameMode.h"
#include "../../../unit/Unit.h"
#include "../../../unit/UnitManager.h"
#include "../../../status/CharStat.h"
#include "../../../status/EleneStat.h"
#include "../../../status/TransStat.h"

// External
#include "../../../../tfw/ui/TouchPanel.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/smenu/Menu.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define CTRLEVID_TARGET					(51002)		// ソウマチュートリアルの石

#define EVID_SUCCESS_SOUMA_MSG			(10004)
#define EVID_FAIL_SOUMA_MSG				(10005)

#define FILEID_PLEASE_TAP_FOCUS			FILEID_CONV_LANG( OTHEREVENT_STEV10101_TUTORIAL21_EN_PNG )
#define FILEID_SHOW_FOCUSICON			FILEID_CONV_LANG( OTHEREVENT_STEV10101_TUTORIAL22_EN_PNG )
#define FILEID_PLEASE_DO_SOUMA_2		FILEID_CONV_LANG( OTHEREVENT_STEV10101_TUTORIAL23_EN_PNG )

#define FRAMECNT_DELAY_CLOSE_SPRITE		(15.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StEmp_10101_1002_MagicTutorial02 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void StEmp_10101_1002_MagicTutorial02::begin(u32 param)
{
	// スプライト作成
	_sprite = new SimpleSprite(); ASSERT(_sprite != 0L);

	// 最初のテクスチャを読み込む
	readTexture(FILEID_PLEASE_TAP_FOCUS);

	// 不要なフォーカスを無効化し、もともとフォーカス可能なユニットを配列に保存する
	_uarrEnableFocusBk = new UnitArray(); ASSERT(_uarrEnableFocusBk != 0L);
	UnitManager* unitmng = Game::getGame()->getUnitManager();
	for(int i = 0; i < unitmng->getUnitNum(); i++)
	{
		Unit* unitWk = unitmng->getUnitFromIndex(i);
		if((unitWk->getCtrlEvid() != CTRLEVID_TARGET) && unitWk->isEnableFocus())
		{
			_uarrEnableFocusBk->addUnit(unitWk);
			unitWk->setEnableFocus(false);
		}
	}

	// フレームカウンタリセット
	_frameElapsedStep = 0.0f;

	// ダウン防止
	getOwner()->setPreventDown(true);
	// 移動カーソルを無効に
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor();
	if(mvcsr->isEnable(MoveCursor::EDCH_EVENT))		{	mvcsr->setEnable(MoveCursor::EDCH_EVENT, false);	}

	return StEmp::begin(param);
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void StEmp_10101_1002_MagicTutorial02::end()
{
	// 移動カーソルを有効に
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor();
	if(!mvcsr->isEnable(MoveCursor::EDCH_EVENT))	{	mvcsr->setEnable(MoveCursor::EDCH_EVENT, true);		}
	// インゲームフロントのソウマアイコンを無効化
	IngameFront* igfr = Game::getGame()->getGui()->getIngameFront();
	#if 0
		igfr->setEneblePanel(IngameFront::BTN_MENU, true);
	#endif
	igfr->setEneblePanel(IngameFront::BTN_SOUMA, false);
	// ダウン防止解除
	getOwner()->setPreventDown(false);
	// GUI のサブモジュールの自動有効・無効化機能を有効に戻す
	Game::getGame()->getGui()->setExecFlags(GameGui::EF_EXT_ENABLE_CTRL, false);

	// フォーカスフラグを戻す
	if(_uarrEnableFocusBk != 0L)
	{
		for(int i = 0; i < _uarrEnableFocusBk->getCount(); i++)
		{
			Unit* unitWk = _uarrEnableFocusBk->unit(i);
			unitWk->setEnableFocus(true);
		}
	}
	delete _uarrEnableFocusBk;
	_uarrEnableFocusBk = 0L;

	// スプライト削除
	if(_sprite != 0L)
	{
		_sprite->destroy();
		delete _sprite;
		_sprite = 0L;
	}

	StEmp::end();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10101_1002_MagicTutorial02::StEmp_10101_1002_MagicTutorial02()
	: StEmp(STEMPTYPE_10101_1001)
	, _sprite(0L)
	, _uarrEnableFocusBk(0L)
	, _frameElapsedStep(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10101_1002_MagicTutorial02::~StEmp_10101_1002_MagicTutorial02()
{
	ASSERT(_sprite == 0L);
	ASSERT(_uarrEnableFocusBk == 0L);
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void StEmp_10101_1002_MagicTutorial02::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel*	touchp = gec->getTouchPanel();

	s16 step = getStep();
	switch(step)
	{
	case STEP_PLEASE_TAP_FOCUS:
		// “「neruhamad」のソウマはかける対象（ターゲット）を定める必要があります。
		// 　ターゲットを定める行為を「フォーカス」と言います。
		// 　ターゲットアイコンをタップし、開いたメニューから「フォーカス」をタップしてください。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// 操作ロック解除
			setLockWithParent(false);
			// GUI のサブモジュールの自動有効・無効化機能を切る
			Game::getGame()->getGui()->setExecFlags(GameGui::EF_EXT_ENABLE_CTRL, true);
			// GUI サブモジュールの有効・無効化
			IngameFront* igfr = Game::getGame()->getGui()->getIngameFront();
			if(igfr->isEnable()) { igfr->setEnable(false); }
			CharFronts* chrfrnt = Game::getGame()->getGui()->getCharFronts();
			if(chrfrnt->isEnable())	{ chrfrnt->setEnable(false); }
			/*/
			MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor();
			if(mvcsr->isEnable(MoveCursor::EDCH_EVENT))		{	mvcsr->setEnable(MoveCursor::EDCH_EVENT, false);	}
			/*/
			FocusCursor* fccsr = Game::getGame()->getGui()->getFocusCursor();
			if(!fccsr->isEnable()) { fccsr->setEnable(true); }
			FocusMenu* fcmenu = Game::getGame()->getGui()->getFocusMenu();
			if(!fcmenu->isEnable())	{ fcmenu->setEnable(true); }
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_TAP_FOCUS:
		{
			// ターゲットユニットを得る
			UnitManager* unitmng = Game::getGame()->getUnitManager();
			Unit* unitTrg = 0L;
			for(int i = 0; i < unitmng->getUnitNum(); i++)
			{
				Unit* unitWk = unitmng->getUnitFromIndex(i);
				if(unitWk->getCtrlEvid() == CTRLEVID_TARGET)
				{
					unitTrg = unitWk;
					break;
				}
			}
			// フォーカスされたら次へ
			FocusCursor* fccsr = Game::getGame()->getGui()->getFocusCursor();
			if(fccsr->getFocus(0) == unitTrg)
			{
				// テクスチャを読み込む
				readTexture(FILEID_SHOW_FOCUSICON);
				// 次のステップへ
				advanceStep();
			}
		}
		break;
	case STEP_SHOW_FOCUSICON:
		// “フォーカスが設定されるとフォーカスアイコンが表示されます。
		// 　この状態のままソウマを発動させると、
		// 　ソウマをフォーカスされた対象に飛ばすことができます。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// テクスチャを読み込む
			readTexture(FILEID_PLEASE_DO_SOUMA_2);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_PLEASE_DO_SOUMA_2:
		// “さきほどと同じ手順で、
		// 　ソウマ「neruhamad」のソースを入力してみましょう。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// インゲームフロントのソウマアイコンのみを有効化
			IngameFront* igfr = Game::getGame()->getGui()->getIngameFront();
			igfr->setEnable(true);
			igfr->setEneblePanel(IngameFront::BTN_SOUMA, true);
			igfr->setEneblePanel(IngameFront::BTN_MENU, false);
			// エネルギーを満タンに
			((MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY))->charStat()->recoverFull();
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_DO_SOUMA_2:
		// notifyUnitInfluenceStat か notifyDisappearMagicCluster で判定して次のステップへ進める
		break;
	case STEP_SHOW_SUCCESS_SOUMA_2:
		{
			#if 1	// “もう一度練習しますか？”を表示しないで続行する
				if(_frameElapsedStep >= (FRAMERATE * 2))
				{
					// 終了へ
					jumpStep(STEP_END);
				}
			#else
				// 成功メッセージイベント起動
				EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
				EvCause evcause(EventCause::CID_EVENT);
				evsys->begin(EVID_SUCCESS_SOUMA_MSG, 0, &evcause);
				// 次のステップへ
				advanceStep();
			#endif
		}
		break;
	case STEP_WAIT_SUCCESS_SOUMA_2:
		if(Game::getGame()->getGameMode()->getChildEndModeKind() == GameMode::MODE_GAME_INGAME_WALKING_NORMAL)
		{
			/*/
			// 移動カーソルを再び無効化する（MyRole::endChanting で有効化してしまうので）
			MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
			if(mvcsr->isEnable(MoveCursor::EDCH_EVENT))		{	mvcsr->setEnable(MoveCursor::EDCH_EVENT, false);	}
			/*/

			// 成功メッセージイベントイベント終了
			GameSysMsgWindow* wndMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndMsg != 0L);
			const GameSysMsgWindow::Result* res = wndMsg->getResult();
			// YES or NO 判定
			if(res->isClickYesButton())
			{
				// エネルギーを満タンに
				((MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY))->charStat()->recoverFull();
				// YES : リトライ
				jumpStep(STEP_DO_SOUMA_2);
			}
			else
			{
				// NO : 終了
				jumpStep(STEP_END);
			}
		}
		break;
	case STEP_SHOW_FAIL_SOUMA_2:
		{
			// 失敗メッセージイベント起動
			EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
			EvCause evcause(EventCause::CID_EVENT);
			evsys->begin(EVID_FAIL_SOUMA_MSG, 0, &evcause);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_FAIL_SOUMA_2:
		if(Game::getGame()->getGameMode()->getChildEndModeKind() == GameMode::MODE_GAME_INGAME_WALKING_NORMAL)
		{
			/*/
			// 移動カーソルを再び無効化する（MyRole::endChanting で有効化してしまうので）
			MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
			if(mvcsr->isEnable(MoveCursor::EDCH_EVENT))		{	mvcsr->setEnable(MoveCursor::EDCH_EVENT, false);	}
			/*/

			// 失敗メッセージイベントイベント終了
			jumpStep(STEP_DO_SOUMA_2);
		}
		break;
	case STEP_END:
		if(Game::getGame()->getGameMode()->getChildEndModeKind() == GameMode::MODE_GAME_INGAME_WALKING_NORMAL)
		{
			// 終了
			this->end();
		}
	}

	// ステップ内フレームカウンタ更新
	if(step != getStep())	{	_frameElapsedStep = 0.0f;					}
	else					{	_frameElapsedStep += gec->getDeltaFrame();	}

	// スプライトのフレーム制御
	switch(getStep())
	{
	case STEP_PLEASE_TAP_FOCUS:
	case STEP_SHOW_FOCUSICON:
	case STEP_PLEASE_DO_SOUMA_2:
		if(_sprite != 0L) { _sprite->exec(ec); }
		break;
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void StEmp_10101_1002_MagicTutorial02::draw(const RenderCtx* rc)
{
	GameRenderCtx* grc = (GameRenderCtx*)rc;
	Renderer* rdr = rc->getRenderer();

	switch(getStep())
	{
	case STEP_PLEASE_TAP_FOCUS:
	case STEP_SHOW_FOCUSICON:
	case STEP_PLEASE_DO_SOUMA_2:
		if((_sprite != 0L) && _sprite->isValid())
		{
			_sprite->draw(rc);
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void StEmp_10101_1002_MagicTutorial02::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	switch(getStep())
	{
	case STEP_DO_SOUMA_2:
		if(unitRecv->getCtrlEvid() == CTRLEVID_TARGET)
		{
			u32 ffop = tstat->getFuncFlags() & EleneStat::FF_MASK_OP;
			if(ffop == EleneStat::FF_OP_WRAP)
			{
				jumpStep(STEP_SHOW_SUCCESS_SOUMA_2);	// 成功へ
			}
			else
			{
				jumpStep(STEP_SHOW_FAIL_SOUMA_2);	// 失敗へ
			}
		}
		else
		{
			jumpStep(STEP_SHOW_FAIL_SOUMA_2);	// 失敗へ
		}
		_frameElapsedStep = 0.0f;
		break;
	}
}

/*---------------------------------------------------------------------*//**
	マジッククラスタの消滅通知
**//*---------------------------------------------------------------------*/
void StEmp_10101_1002_MagicTutorial02::notifyDisappearMagicCluster(MagicCluster* mc, u32 dcflags)
{
	if(TFW_IS_FLAG(dcflags, MagicCluster::DCF_IS_CREATION))	{	return;	}	// 創作クラスターは除外

	// エネルギーを満タンに
	((MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY))->charStat()->recoverFull();

	switch(getStep())
	{
	case STEP_DO_SOUMA_2:
		// 消滅時に STEP_DO_SOUMA_2 ということは、notifyUnitInfluenceStat が呼ばれなかったということである
		jumpStep(STEP_SHOW_FAIL_SOUMA_2);	// 失敗へ
		break;
	}
}

/*---------------------------------------------------------------------*//**
	テクスチャを読み込む
**//*---------------------------------------------------------------------*/
void StEmp_10101_1002_MagicTutorial02::readTexture(u32 fileid)
{
	ASSERT(_sprite != 0L);

	// 以前のスプライトを削除する
	if(_sprite->isValid())
	{
		_sprite->destroy();
	}

	// 新たにスプライトを読み込む
	_sprite->create(fileid, FRAMECNT_DELAY_CLOSE_SPRITE, true, 0x0000007f, true);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
