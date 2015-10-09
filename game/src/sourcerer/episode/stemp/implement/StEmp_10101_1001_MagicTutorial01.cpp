/***********************************************************************//**
 *	StEmp_10101_1001_MagicTutorial01.cpp
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
#include "StEmp_10101_1001_MagicTutorial01.h"

// Friends
#include "../StEmpType.h"
#include "../../char/pc/MyUnit.h"
#include "../../../common/SimpleSprite.h"
#include "../../../episode/FixedSourceIdDef.h"
#include "../../../episode/party/MyParty.h"
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
#include "../../../gui/panel/HeaderPanel.h"
#include "../../../gui/window/implement/AriaWindow.h"
#include "../../../gui/window/implement/SourceListWindow.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../gui/window/implement/SoftKeyboardWindow.h"
#include "../../../gui/window/implement/SourceEditWindow.h"
#include "../../../gui/window/menu/MenuWindow.h"
#include "../../../magic/MagicCluster.h"
#include "../../../mode/GameMode.h"
#include "../../../source/FixedSourceDef.h"
#include "../../../source/Source.h"
#include "../../../source/SourceManager.h"
#include "../../../status/CharStat.h"
#include "../../../unit/Unit.h"
#include "../../../unit/UnitManager.h"

// External
#include "../../../../tfw/evsys/MsgData.h"
#include "../../../../tfw/evsys/MsgDataSet.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/smenu/Menu.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define FILEID_PLEASE_TAP_SOUMAICON		FILEID_CONV_LANG( OTHEREVENT_STEV10101_TUTORIAL01_EN_PNG )
#define FILEID_PLEASE_TAP_SOURCEBOOK	FILEID_CONV_LANG( OTHEREVENT_STEV10101_TUTORIAL02_EN_PNG )
#define FILEID_PLEASE_TAP_SOUMA_1		FILEID_CONV_LANG( OTHEREVENT_STEV10101_TUTORIAL03_EN_PNG )
#define FILEID_PLEASE_TAP_RETURN		FILEID_CONV_LANG( OTHEREVENT_STEV10101_TUTORIAL04_EN_PNG )

#define FRAMECNT_DELAY_CLOSE_SPRITE		(15.0f)

#define MENUINDEX_WATER_SPRAY			(2)
#define SOURCEINDEX_WATER_SPRAY			(1)

#define SPECCHANGE_DIRECT_LIST_INPUT	(1)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StEmp_10101_1001_MagicTutorial01 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void StEmp_10101_1001_MagicTutorial01::begin(u32 param)
{
	// スプライト作成
	_sprite = new SimpleSprite();

	// 最初のテクスチャを読み込む
	readTexture(FILEID_PLEASE_TAP_SOUMAICON);

	// 不要なフォーカスを無効化し、もともとフォーカス可能なユニットを配列に保存する
	_uarrEnableFocusBk = new UnitArray(); ASSERT(_uarrEnableFocusBk != 0L);
	UnitManager* unitmng = Game::getGame()->getUnitManager();
	for(int i = 0; i < unitmng->getUnitNum(); i++)
	{
		Unit* unitWk = unitmng->getUnitFromIndex(i);
		if(unitWk->isEnableFocus())
		{
			_uarrEnableFocusBk->addUnit(unitWk);
			unitWk->setEnableFocus(false);
		}
	}
	
	// 水のソースが必ず２番目にくるようにソート
	SourceManager* srcmng = ((MyParty*)Game::getGame()->getMyParty())->getSourceManager();
	if(srcmng != 0L)
	{
		s32 index1 = SOURCEINDEX_WATER_SPRAY;
		s32 index2 = -1;
		for(int i = 0; i < srcmng->getSourceNum(); i++)
		{
			if(i == SOURCEINDEX_WATER_SPRAY)	{	continue;	}
			const Source* sourceWk = srcmng->getSource(i);
			if(sourceWk != 0L) 
			{
				const FixedSourceDef* fsrcdf = sourceWk->getFixedSourceDef();
				if(fsrcdf != 0L)
				{
					if(fsrcdf->getFixedSourceDefId() == FixedSourceIdDef::FSRCID_EP01_NERUROS)
					{
						index2 = i;
						break;
					}
				}
			}
		}
		// ソースの置換
		if((index2 != -1) && (index1 != index2))
		{
			Source* source1 = srcmng->source(index1);	
			Source* source2 = srcmng->source(index2);
			if((source1 != 0L) && (source2 != 0L))
			{
				Source::swap(source1, source2);
			}
		}
	}

	// フレームカウンタリセット
	_frameElapsedSouma = -1.0f;
	_frameElapsedStep = 0.0f;

	// 詠唱ウインドウの中断ボタンを無効化
	Game::getGame()->getGui()->getMyAriaWindow()->setEnableBreakButton(false);
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
void StEmp_10101_1001_MagicTutorial01::end()
{
	// 移動カーソルを有効に
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor();
	if(!mvcsr->isEnable(MoveCursor::EDCH_EVENT))	{	mvcsr->setEnable(MoveCursor::EDCH_EVENT, true);		}
	#if 0
		// インゲームフロントのメニューアイコンを有効化
		IngameFront* igfr = Game::getGame()->getGui()->getIngameFront();
		igfr->setEneblePanel(IngameFront::BTN_MENU, true);
	#endif
	// GUI のサブモジュールの自動有効・無効化機能を有効に戻す
	Game::getGame()->getGui()->setExecFlags(GameGui::EF_EXT_ENABLE_CTRL, false);
	// ソースエディットの各種ボタンを使用可能に戻す
	SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();
	HeaderPanel* hpnl = wndSrcedt->headerPanel();
	Panel* panelLeftBtn = hpnl->leftButtonPanel();
	panelLeftBtn->setUnused(false);
	Panel* panelRightBtn = hpnl->rightButtonPanel();
	panelRightBtn->setUnused(false);
	SoftKeyboardWindow* wndSoftkb = wndSrcedt->softKeyboardWindow();
	wndSoftkb->setUnused(false);
	for(int i  = 0; i < SoftKeyboardWindow::NUM_BTN; i++)
	{
		wndSoftkb->buttonPanel((SoftKeyboardWindow::Button)i)->setUnused(false);
	}
	// ソースブックの戻るボタンを使用可能に戻す
	SourceListWindow* wndSrcList = Game::getGame()->getGui()->getMagicListWindow();
	hpnl = wndSrcList->headerPanel();
	panelLeftBtn = hpnl->leftButtonPanel();
	panelLeftBtn->setUnused(false);
	// インゲームフロント以外の各種 GUI モジュールを無効化を解除
	CharFronts* chrfrnt = Game::getGame()->getGui()->getCharFronts();
	if(!chrfrnt->isEnable()) { chrfrnt->setEnable(true); }
	FocusCursor* fccsr = Game::getGame()->getGui()->getFocusCursor();
	if(!fccsr->isEnable()) { fccsr->setEnable(true); }
	FocusMenu* fcmenu = Game::getGame()->getGui()->getFocusMenu();
	if(!fcmenu->isEnable()) { fcmenu->setEnable(true); }
	Game::getGame()->getGui()->getMyAriaWindow()->setEnableBreakButton(true);	// 詠唱ウインドウの中断ボタンを戻す
	// ダウン防止解除
	getOwner()->setPreventDown(false);

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
StEmp_10101_1001_MagicTutorial01::StEmp_10101_1001_MagicTutorial01()
	: StEmp(STEMPTYPE_10101_1001)
	, _sprite(0L)
	, _uarrEnableFocusBk(0L)
	, _frameElapsedSouma(0.0f)
	, _frameElapsedStep(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10101_1001_MagicTutorial01::~StEmp_10101_1001_MagicTutorial01()
{
	ASSERT(_sprite == 0L);
	ASSERT(_uarrEnableFocusBk == 0L);
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void StEmp_10101_1001_MagicTutorial01::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel*	touchp = gec->getTouchPanel();

	s16 step = getStep();
	switch(step)
	{
	case STEP_PLEASE_TAP_SOUMAICON:
		// “「ソウマ」は対応する「ソース」を
		// 　入力することにより発動します。
		// 　画面右上のソウマアイコンをタップしてください。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// 操作ロック解除
			setLockWithParent(false);
			// GUI のサブモジュールの自動有効・無効化機能を切る
			Game::getGame()->getGui()->setExecFlags(GameGui::EF_EXT_ENABLE_CTRL, true);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_SETUP_GUI_SOUMAICON:
		if(Game::getGame()->getGameMode()->getChildEndModeKind() == GameMode::MODE_GAME_INGAME_WALKING_NORMAL)
		{
			// インゲームフロントのソウマアイコンのみを有効化
			IngameFront* igfr = Game::getGame()->getGui()->getIngameFront();
			igfr->setEnable(true);
			igfr->setEneblePanel(IngameFront::BTN_SOUMA, true);
			igfr->setEneblePanel(IngameFront::BTN_MENU, false);
			// インゲームフロント以外の各種 GUI モジュールを無効化
			CharFronts* chrfrnt = Game::getGame()->getGui()->getCharFronts();
			if(chrfrnt->isEnable())	{ chrfrnt->setEnable(false); }
			/*/
			MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor();
			if(mvcsr->isEnable(MoveCursor::EDCH_EVENT))		{	mvcsr->setEnable(MoveCursor::EDCH_EVENT, false);	}
			/*/
			FocusCursor* fccsr = Game::getGame()->getGui()->getFocusCursor();
			if(fccsr->isEnable()) { fccsr->setEnable(false); }
			FocusMenu* fcmenu = Game::getGame()->getGui()->getFocusMenu();
			if(fcmenu->isEnable()) { fcmenu->setEnable(false); }
			// 次のステップへ
			#if SPECCHANGE_DIRECT_LIST_INPUT
				jumpStep(STEP_WAIT_TAP_SOURCEBOOK);
			#else
				advanceStep();
			#endif
		}
		break;
	case STEP_WAIT_TAP_SOUMAICON:
		{
			SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();
			if(wndSrcedt->isShowWindow())	// ソースエディットを開いた
			{
				// ソースエディットの各種ボタンを使用不能に
				SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();
				HeaderPanel* hpnl = wndSrcedt->headerPanel();
				Panel* panelLeftBtn = hpnl->leftButtonPanel();
				panelLeftBtn->setUnused(true);
				Panel* panelRightBtn = hpnl->rightButtonPanel();
				panelRightBtn->setUnused(true);
				SoftKeyboardWindow* wndSoftkb = wndSrcedt->softKeyboardWindow();
				wndSoftkb->setUnused(true);
				// テクスチャを読み込む
				readTexture(FILEID_PLEASE_TAP_SOURCEBOOK);
				// 操作ロック有効化
				///setLockWithParent(true);	// →操作ロックを行うとメニューが消えてしまうので廃止
				// 次のステップへ
				advanceStep();
			}
		}
		break;
	case STEP_PLEASE_TAP_SOURCEBOOK:
		// “ソースはキーボードから１字づつ入れることもできますが、上級者向けです。
		// 　ここでは、既にあるソースをリストから選択するこによってソースの入力を行います。
		// 　画面左上の「ソースブック」ボタンをタップしてください。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// ソースエディットのソースブックボタンを使用可能に
			SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();
			HeaderPanel* hpnl = wndSrcedt->headerPanel();
			Panel* panelRightBtn = hpnl->rightButtonPanel();
			panelRightBtn->setUnused(false);
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_TAP_SOURCEBOOK:
		{
			SourceListWindow* wndSrcList = Game::getGame()->getGui()->getMagicListWindow();
			if(wndSrcList->isShowWindow())	// ソースブックを開いた
			{
				// ソースブックの戻るボタンを使用不能に
				SourceListWindow* wndSrcList = Game::getGame()->getGui()->getMagicListWindow();
				HeaderPanel* hpnl = wndSrcList->headerPanel();
				Panel* panelLeftBtn = hpnl->leftButtonPanel();
				panelLeftBtn->setUnused(true);
				// メニューの項目を制限
				Menu* menu = wndSrcList->listMenu();
				MenuTreeNode* mtnode = menu->getCurrentMenuNode();
				MenuWindow* wndMenu = (MenuWindow*)mtnode->getPanel();
				for(int i = 0; i < wndMenu->getElementNum(); i++)
				{
					if(i != MENUINDEX_WATER_SPRAY)	// ２番目のソウマのみ使用可能に
					{
						wndMenu->setElementUnused(i, true);
					}
				}
				// テクスチャを読み込む
				readTexture(FILEID_PLEASE_TAP_SOUMA_1);
				// 次のステップへ
				advanceStep();
			}
		}
		break;
	case STEP_PLEASE_TAP_SOUMA_1:
		// “ソースリストの中から、「neruros」を選んでください。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_TAP_SOUMA_1:
		{
			SourceListWindow* wndSrcList = Game::getGame()->getGui()->getMagicListWindow();
			if(!wndSrcList->isShowWindow())	// ソースブックを閉じた
			{
				// ソースエディットのソースブックボタンも使用不能に
				SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();
				HeaderPanel* hpnl = wndSrcedt->headerPanel();
				Panel* panelRightBtn = hpnl->rightButtonPanel();
				panelRightBtn->setUnused(true);
				// テクスチャを読み込む
				readTexture(FILEID_PLEASE_TAP_RETURN);
				// 次のステップへ
				#if SPECCHANGE_DIRECT_LIST_INPUT
					jumpStep(STEP_DO_SOUMA_1);
				#else
					advanceStep();
				#endif
			}
		}
		break;
	case STEP_PLEASE_TAP_RETURN:
		// “return キーを２回入力することによってソースが決定されます。
		// 　画面下の、return キーを２回タップしてください。”
		if((_sprite != 0L) && _sprite->isEnd())	// スプライトが終了した
		{
			// ソースエディットをリターンキーのみを使用可能に
			SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();
			SoftKeyboardWindow* wndSoftkb = wndSrcedt->softKeyboardWindow();
			wndSoftkb->setUnused(false);	// 全体使用不可を解除
			for(int i  = 0; i < SoftKeyboardWindow::NUM_BTN; i++)
			{
				if(i != SoftKeyboardWindow::BTN_RET)
				{
					wndSoftkb->buttonPanel((SoftKeyboardWindow::Button)i)->setUnused(true);
				}
			}
			// 次のステップへ
			advanceStep();
		}
		break;
	case STEP_WAIT_TAP_RETURN:
		{
			SourceEditWindow* wndSrcedt = Game::getGame()->getGui()->getSourceEditWindow();
			if(!wndSrcedt->isShowWindow())	// ソースエディタを閉じた
			{
				// インゲームフロントを無効化
				IngameFront* igfr = Game::getGame()->getGui()->getIngameFront();
				igfr->setEnable(false);
				// エネルギーを満タンに
				((MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY))->charStat()->recoverFull();
				// 次のステップへ
				advanceStep();
			}
		}
		break;
	case STEP_DO_SOUMA_1:
		if(_frameElapsedSouma >= 0.0f)	// フレームカウンタが有効化された（ソウマが放たれた）
		{
			/*/
			// 移動カーソルを再び無効化する（MyRole::endChanting で有効化してしまうので）
			MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
			if(mvcsr->isEnable(MoveCursor::EDCH_EVENT))		{	mvcsr->setEnable(MoveCursor::EDCH_EVENT, false);	}
			/*/

			_frameElapsedSouma += gec->getDeltaFrame();
			if(_frameElapsedSouma >= FRAMERATE)	//70.0f)
			{
				// 次のステップへ
				advanceStep();
			}
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
	case STEP_PLEASE_TAP_SOUMAICON:
	case STEP_PLEASE_TAP_SOURCEBOOK:
	case STEP_PLEASE_TAP_SOUMA_1:
	case STEP_PLEASE_TAP_RETURN:
		if(_sprite != 0L) { _sprite->exec(ec); }
		break;
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void StEmp_10101_1001_MagicTutorial01::draw(const RenderCtx* rc)
{
	GameRenderCtx* grc = (GameRenderCtx*)rc;
	Renderer* rdr = rc->getRenderer();

	switch(getStep())
	{
	case STEP_PLEASE_TAP_SOUMAICON:
	case STEP_PLEASE_TAP_SOURCEBOOK:
	case STEP_PLEASE_TAP_SOUMA_1:
	case STEP_PLEASE_TAP_RETURN:
		if((_sprite != 0L) && _sprite->isValid())
		{
			_sprite->draw(rc);

			#if 0	// フォントによるテキスト描画→止め．スプライトにそれぞれの言語のテキストを含める
				const MsgDataSet* msgds = Game::getGame()->getEventSys()->getEventData()->getMsgDataSet();
				const MsgData::Msg* msg = msgds->getMessageFromMsgid(1101);
				Font* font = Game::getGame()->getGui()->getFontSet()->getFont(GameFontSet::JP);
				font->draw(32, 224, &msgds->getString(msg), 16, rc);
			#endif
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	マジッククラスタの消滅通知
**//*---------------------------------------------------------------------*/
void StEmp_10101_1001_MagicTutorial01::notifyDisappearMagicCluster(MagicCluster* mc, u32 dcflags)
{
	if(TFW_IS_FLAG(dcflags, MagicCluster::DCF_IS_CREATION))	{	return;	}	// 創作クラスターは除外

	// エネルギーを満タンに
	((MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY))->charStat()->recoverFull();

	switch(getStep())
	{
	case STEP_DO_SOUMA_1:
		// フレームカウンタ有効化
		_frameElapsedSouma = 0.0f;
		break;
	}
}

/*---------------------------------------------------------------------*//**
	テクスチャを読み込む
**//*---------------------------------------------------------------------*/
void StEmp_10101_1001_MagicTutorial01::readTexture(u32 fileid)
{
	ASSERT(_sprite != 0L);

	// 以前のテクスチャを削除する
	if(_sprite->isValid())
	{
		_sprite->destroy();
	}

	// 新たにテクスチャを読み込む
	_sprite->create(fileid, FRAMECNT_DELAY_CLOSE_SPRITE, true, 0x0000007f, true);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
