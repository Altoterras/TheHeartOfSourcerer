/***********************************************************************//**
 *	OptionsWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "OptionsWindow.h"

// Friends
#include "../../../episode/save/SaveStructure.h"
#include "../../../episode/save/Score.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/GameRenderCtx.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../body/Party.h"
#include "../../../source/Source.h"
#include "../../../source/SourceManager.h"

// External
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#if 1
#define X_BASE						((Game::getGame()->getLogicalWidth() - 400) / 2)
#define RECT_THIS					X_BASE,  48, 400, 272
#define X_SOUMA_LANGUAGE			(f32)X_BASE + 44
#define Y_SOUMA_LANGUAGE			76
#define RECT_BTN_SS					X_BASE + 150, 96, 100, 32
#define RECT_BTN_JS					X_BASE + 260, 96, 100, 32
#define X_CHANT_WINDOW_MODE			(f32)X_BASE + 44
#define Y_CHANT_WINDOW_MODE			140
#define RECT_BTN_SPELL				X_BASE + 150, 160, 100, 32
#define RECT_BTN_RAW				X_BASE + 260, 160, 100, 32
#if ENABLE_OPT_MOVMSG_FLOW
#define X_MOVMSG_WINDOW_MODE		(f32)X_BASE + 44
#define Y_MOVMSG_WINDOW_MODE		204
#define RECT_BTN_WAIT				X_BASE + 150, 224, 100, 32
#define RECT_BTN_AUTOFLOW			X_BASE + 260, 224, 100, 32
#endif	// #if ENABLE_OPT_MOVMSG_FLOW
#else
#define RECT_THIS					 42,  48, 400, 272
#define X_SOUMA_LANGUAGE			84
#define Y_SOUMA_LANGUAGE			76
#define RECT_BTN_SS					190, 96, 100, 32
#define RECT_BTN_JS					300, 96, 100, 32
#define X_CHANT_WINDOW_MODE			84
#define Y_CHANT_WINDOW_MODE			140
#define RECT_BTN_SPELL				190, 160, 100, 32
#define RECT_BTN_RAW				300, 160, 100, 32
#if ENABLE_OPT_MOVMSG_FLOW
#define X_MOVMSG_WINDOW_MODE		84
#define Y_MOVMSG_WINDOW_MODE		204
#define RECT_BTN_WAIT				190, 224, 100, 32
#define RECT_BTN_AUTOFLOW			300, 224, 100, 32
#endif	// #if ENABLE_OPT_MOVMSG_FLOW
#endif

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ
#define UV_THIS						128.0f / W_TEX, 448.0f / H_TEX, 400.0f / W_TEX, 272.0f / H_TEX
#define UV_BTN_NRM					928.0f / W_TEX, 264.0f / H_TEX, 60.0f / W_TEX, 24.0f / H_TEX
#define UV_BTN_HVR					928.0f / W_TEX, 288.0f / H_TEX, 60.0f / W_TEX, 24.0f / H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// OptionsWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OptionsWindow::OptionsWindow()
	: _isDone(false)
	, _fontRef(0L)
	, _texRef(0L)
	, _confirmkind(CFK_NULL)
{
	for(int i = 0; i < NUM_BTN; i++)
	{
		_pnlBtn[i] = 0;
	}
	for(int i = 0; i < NUM_PRT; i++)
	{
		_pnlPrt[i] = 0;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
OptionsWindow::~OptionsWindow()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_BTN; i++)	{	ASSERT(_pnlBtn[i] == 0L);	}
		for(int i = 0; i < NUM_PRT; i++)	{	ASSERT(_pnlPrt[i] == 0L);	}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成通知
**//*---------------------------------------------------------------------*/
bool OptionsWindow::create(Font* fontRef, Texture* texRef)
{
	RectF32 rect;
	RectF32 uv;
	ColorU8 col;
	VcString str;

	// ウインドウの作成
	RectF32 rectThis(RECT_THIS);
	if(!Window::create(&rectThis))
	{
		return false;
	}

	// 値保存
	_fontRef = fontRef;
	_texRef = texRef;

	// 自身の設定
	setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_THIS));

	// 子パネル作成
	_pnlBtn[BTN_SS] = new Panel();
	_pnlBtn[BTN_SS]->create(RectF32::setout(&rect, RECT_BTN_SS));
	_pnlBtn[BTN_SS]->setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_BTN_NRM));
	_pnlBtn[BTN_SS]->setStateInfo(STATE_ACTIVE, _texRef, RectF32::setout(&uv, UV_BTN_HVR));
	_pnlBtn[BTN_SS]->setFlags(Panel::F_MANUAL_STATE, true);
	_pnlBtn[BTN_SS]->createText(fontRef, 16, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPTFLAGS_SS), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
	_pnlBtn[BTN_SS]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
	_pnlBtn[BTN_SS]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
	_pnlBtn[BTN_SS]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	_pnlBtn[BTN_JS] = new Panel();
	_pnlBtn[BTN_JS]->create(RectF32::setout(&rect, RECT_BTN_JS));
	_pnlBtn[BTN_JS]->setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_BTN_NRM));
	_pnlBtn[BTN_JS]->setStateInfo(STATE_ACTIVE, _texRef, RectF32::setout(&uv, UV_BTN_HVR));
	_pnlBtn[BTN_JS]->setFlags(Panel::F_MANUAL_STATE, true);
	_pnlBtn[BTN_JS]->createText(fontRef, 16, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPTFLAGS_JS), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
	_pnlBtn[BTN_JS]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
	_pnlBtn[BTN_JS]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
	_pnlBtn[BTN_JS]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	_pnlBtn[BTN_SPELL] = new Panel();
	_pnlBtn[BTN_SPELL]->create(RectF32::setout(&rect, RECT_BTN_SPELL));
	_pnlBtn[BTN_SPELL]->setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_BTN_NRM));
	_pnlBtn[BTN_SPELL]->setStateInfo(STATE_ACTIVE, _texRef, RectF32::setout(&uv, UV_BTN_HVR));
	_pnlBtn[BTN_SPELL]->setFlags(Panel::F_MANUAL_STATE, true);
	_pnlBtn[BTN_SPELL]->createText(fontRef, 16, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPTFLAGS_SPELL), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
	_pnlBtn[BTN_SPELL]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
	_pnlBtn[BTN_SPELL]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
	_pnlBtn[BTN_SPELL]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	_pnlBtn[BTN_RAW] = new Panel();
	_pnlBtn[BTN_RAW]->create(RectF32::setout(&rect, RECT_BTN_RAW));
	_pnlBtn[BTN_RAW]->setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_BTN_NRM));
	_pnlBtn[BTN_RAW]->setStateInfo(STATE_ACTIVE, _texRef, RectF32::setout(&uv, UV_BTN_HVR));
	_pnlBtn[BTN_RAW]->setFlags(Panel::F_MANUAL_STATE, true);
	_pnlBtn[BTN_RAW]->createText(fontRef, 16, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPTFLAGS_RAW), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
	_pnlBtn[BTN_RAW]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
	_pnlBtn[BTN_RAW]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
	_pnlBtn[BTN_RAW]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
#if ENABLE_OPT_MOVMSG_FLOW
	_pnlBtn[BTN_WAIT] = new Panel();
	_pnlBtn[BTN_WAIT]->create(RectF32::setout(&rect, RECT_BTN_WAIT));
	_pnlBtn[BTN_WAIT]->setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_BTN_NRM));
	_pnlBtn[BTN_WAIT]->setStateInfo(STATE_ACTIVE, _texRef, RectF32::setout(&uv, UV_BTN_HVR));
	_pnlBtn[BTN_WAIT]->setFlags(Panel::F_MANUAL_STATE, true);
	_pnlBtn[BTN_WAIT]->createText(fontRef, 16, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPTFLAGS_WAIT), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
	_pnlBtn[BTN_WAIT]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
	_pnlBtn[BTN_WAIT]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
	_pnlBtn[BTN_WAIT]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	_pnlBtn[BTN_AUTOFLOW] = new Panel();
	_pnlBtn[BTN_AUTOFLOW]->create(RectF32::setout(&rect, RECT_BTN_AUTOFLOW));
	_pnlBtn[BTN_AUTOFLOW]->setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_BTN_NRM));
	_pnlBtn[BTN_AUTOFLOW]->setStateInfo(STATE_ACTIVE, _texRef, RectF32::setout(&uv, UV_BTN_HVR));
	_pnlBtn[BTN_AUTOFLOW]->setFlags(Panel::F_MANUAL_STATE, true);
	_pnlBtn[BTN_AUTOFLOW]->createText(fontRef, 16, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPTFLAGS_AUTOFLOW), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
	_pnlBtn[BTN_AUTOFLOW]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
	_pnlBtn[BTN_AUTOFLOW]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
	_pnlBtn[BTN_AUTOFLOW]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
#endif	// #if ENABLE_OPT_MOVMSG_FLOW

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄通知
**//*---------------------------------------------------------------------*/
void OptionsWindow::destroy()
{
	// 子パネル削除
	for(int i = 0; i < NUM_PRT; i++)
	{
		if(_pnlPrt[i] != 0)
		{
			_pnlPrt[i]->destroy();
			delete _pnlPrt[i];
			_pnlPrt[i] = 0L;
		}
	}
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0)
		{
			_pnlBtn[i]->destroy();
			delete _pnlBtn[i];
			_pnlBtn[i] = 0L;
		}
	}

	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void OptionsWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	// 親の制御
	Window::execContents(res, ec);

	// YES / NO メッセージウインドウの制御
	GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
	if((wndSysMsg != 0L) && wndSysMsg->isShowWindow())
	{
		wndSysMsg->exec(res, ec);
		const GameSysMsgWindow::Result* res = wndSysMsg->getResult();
		if(res->isClickYesButton() || res->isClickNoButton())
		{
			// ウインドウを消す
			wndSysMsg->showWindow(false);
			if(res->isClickYesButton())
			{
				// 全てのソース種別を変更する
				SourceManager* srcmng = Game::getGame()->getMyParty()->getSourceManager(); ASSERT(srcmng != 0L);
				srcmng->changeAllSourceKind((_confirmkind == CFK_CHANGE_TO_JS) ? SourceDef::SRCKIND_JS : SourceDef::SRCKIND_SS);
			}
		}
		return;
	}

	// 子パネル制御
	TouchPanel* ui = gec->getTouchPanel();
	PointF32 ptTouch(S32_MIN, S32_MIN);
	PointF32 ptClick(S32_MIN, S32_MIN);
	bool isTouch = !ui->isReacted() ? ui->getTouchPosition(&ptTouch) : false;
	bool isClick = !ui->isReacted() ? ui->isClickRelease(&ptClick) : false;
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] == 0)	{	continue;	}
		if(isClick)
		{
			if(_pnlBtn[i]->hit(&ptClick, true))
			{
				switch(i)
				{
				case BTN_SS:		changeDefaultSoumaLang(SourceDef::SRCKIND_SS);		break;
				case BTN_JS:		changeDefaultSoumaLang(SourceDef::SRCKIND_JS);		break;
				case BTN_RAW:		changeChantWindowMode(true);						break;
				case BTN_SPELL:		changeChantWindowMode(false);						break;
#if ENABLE_OPT_MOVMSG_FLOW
				case BTN_WAIT:		changeMovieMsgFlowMode(false);						break;
				case BTN_AUTOFLOW:	changeMovieMsgFlowMode(true);						break;
#endif
				}
				ui->setReacted(true);
			}
		}
		else
		{
			_pnlBtn[i]->hit(&ptTouch, isTouch);
		}
		_pnlBtn[i]->exec(res, ec);
	}
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void OptionsWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	rc->getRenderer()->setSolidColor(0, 0, 0, 255);
	EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPT_DEFAULT_SOUMA_LANG), X_SOUMA_LANGUAGE, Y_SOUMA_LANGUAGE, 16, rc);
	EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPT_CHANT_WINDOW_MODE), X_CHANT_WINDOW_MODE, Y_CHANT_WINDOW_MODE, 16, rc);
#if ENABLE_OPT_MOVMSG_FLOW
	EasyStringDrawer::draw(_fontRef, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPT_MOVMSG_WINDOW_MODE), X_MOVMSG_WINDOW_MODE, Y_MOVMSG_WINDOW_MODE, 16, rc);
#endif

	// 子パネル描画
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0L)	{	_pnlBtn[i]->draw(rc);	}
	}
	for(int i = 0; i < NUM_PRT; i++)
	{
		if(_pnlPrt[i] != 0L)	{	_pnlPrt[i]->draw(rc);	}
	}
}

/*---------------------------------------------------------------------*//**
	ウインドウ表示・非表示通知
**//*---------------------------------------------------------------------*/
void OptionsWindow::onShowWindow(bool isShow)
{
	if(isShow)
	{
		// 値のクリア
		_isDone = false;

		// ボタン状態の更新
		updateDefaultSoumaLangButton();
		updateChantWindowModeButton();
#if ENABLE_OPT_MOVMSG_FLOW
		updateMovieMsgFlowModeButton();
#endif
	}
	else
	{
		// YES / NO メッセージウインドウが出ている場合は強制的に消す
		GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
		if((wndSysMsg != 0L) && wndSysMsg->isShowWindow())
		{
			wndSysMsg->showWindow(false);	
		}
	}
}

/*---------------------------------------------------------------------*//**
	デフォルトのソウマ言語を変更する
**//*---------------------------------------------------------------------*/
void OptionsWindow::changeDefaultSoumaLang(s32 srckind)
{
	Game* game = Game::getGame(); ASSERT(game != 0L);
	if(srckind == SourceDef::SRCKIND_SS)
	{
		if(!TFW_IS_FLAG(game->getOptionFlags(), Game::OPTF_SOUMA_DEFAULT_LANG_JS))
		{
			return;	// 変更不要
		}
	}
	else if(srckind == SourceDef::SRCKIND_JS)
	{
		if(TFW_IS_FLAG(game->getOptionFlags(), Game::OPTF_SOUMA_DEFAULT_LANG_JS))
		{
			return;	// 変更不要
		}
	}
	else
	{
		return;	// まだない
	}
	// ゲームの設定変更
	game->setOptionFlags(Game::OPTF_SOUMA_DEFAULT_LANG_JS, srckind == SourceDef::SRCKIND_JS);
	updateDefaultSoumaLangButton();	// ボタン状態の更新

	// 既存のソースの変更確認メッセージ表示
	bool existDiffentKind = false;
	SourceManager* srcmng = Game::getGame()->getMyParty()->getSourceManager(); ASSERT(srcmng != 0L);
	for(int i = 0; i < srcmng->getSourceNum(); i++)
	{
		const Source* source = srcmng->getSource(i);
		if((source->getSourceKind() != SourceDef::SRCKIND_NULL) && (source->getSourceKind() != srckind))
		{
			existDiffentKind = true;
			break;
		}
	}
	if(existDiffentKind)
	{
		_confirmkind = (srckind == SourceDef::SRCKIND_JS) ? CFK_CHANGE_TO_JS : CFK_CHANGE_TO_SS;
		// YES / NO システムメッセージを表示する
		GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow();
		wndSysMsg->setKind(MessageWindow::KIND_YESNO);
		VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_CONFIRM_CHANGE_SOUMA_LANG)->getRaw(), (srckind == SourceDef::SRCKIND_JS) ? GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPTFLAGS_JS)->getRaw() : GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPTFLAGS_SS)->getRaw()));
		wndSysMsg->setModal(true);
		wndSysMsg->showMessage(&msg, 0);
	}
}

/*---------------------------------------------------------------------*//**
	デフォルトのソウマ言語変更ボタン状態の更新
**//*---------------------------------------------------------------------*/
void OptionsWindow::updateDefaultSoumaLangButton()
{
	if(TFW_IS_FLAG(Game::getGame()->getOptionFlags(), Game::OPTF_SOUMA_DEFAULT_LANG_JS))
	{
		_pnlBtn[BTN_SS]->setCurrentState(Panel::STATE_NORMAL);
		_pnlBtn[BTN_JS]->setCurrentState(Panel::STATE_ACTIVE);
	}
	else
	{
		_pnlBtn[BTN_SS]->setCurrentState(Panel::STATE_ACTIVE);
		_pnlBtn[BTN_JS]->setCurrentState(Panel::STATE_NORMAL);
	}
}

/*---------------------------------------------------------------------*//**
	詠唱ウインドウモードを変更する
**//*---------------------------------------------------------------------*/
void OptionsWindow::changeChantWindowMode(bool isRawMode)
{
	Game::getGame()->setOptionFlags(Game::OPTF_ARIA_WINDOUW_RAW_SRC, isRawMode);

	updateChantWindowModeButton();	// ボタン状態の更新
}

/*---------------------------------------------------------------------*//**
	詠唱ウインドウモード変更ボタン状態の更新
**//*---------------------------------------------------------------------*/
void OptionsWindow::updateChantWindowModeButton()
{
	if(TFW_IS_FLAG(Game::getGame()->getOptionFlags(), Game::OPTF_ARIA_WINDOUW_RAW_SRC))
	{
		_pnlBtn[BTN_RAW]->setCurrentState(Panel::STATE_ACTIVE);
		_pnlBtn[BTN_SPELL]->setCurrentState(Panel::STATE_NORMAL);
	}
	else
	{
		_pnlBtn[BTN_RAW]->setCurrentState(Panel::STATE_NORMAL);
		_pnlBtn[BTN_SPELL]->setCurrentState(Panel::STATE_ACTIVE);
	}
}

#if ENABLE_OPT_MOVMSG_FLOW
/*---------------------------------------------------------------------*//**
	ムービー メッセージ フローを変更する
**//*---------------------------------------------------------------------*/
void OptionsWindow::changeMovieMsgFlowMode(bool isAutoFlow)
{
	Game::getGame()->setOptionFlags(Game::OPTF_AUTO_FLOW_MOVIE_MSG, isAutoFlow);

	// ムービー時に台詞ポーズを行うかどうかをイベントシステムに反映
	Game::getGame()->getEventSys()->setEnableWaitMovieMessage(!isAutoFlow);

	updateMovieMsgFlowModeButton();	// ボタン状態の更新
}

/*---------------------------------------------------------------------*//**
	ムービー メッセージ フロー変更ボタン状態の更新
**//*---------------------------------------------------------------------*/
void OptionsWindow::updateMovieMsgFlowModeButton()
{
	if(TFW_IS_FLAG(Game::getGame()->getOptionFlags(), Game::OPTF_AUTO_FLOW_MOVIE_MSG))
	{
		_pnlBtn[BTN_WAIT]->setCurrentState(Panel::STATE_NORMAL);
		_pnlBtn[BTN_AUTOFLOW]->setCurrentState(Panel::STATE_ACTIVE);
	}
	else
	{
		_pnlBtn[BTN_WAIT]->setCurrentState(Panel::STATE_ACTIVE);
		_pnlBtn[BTN_AUTOFLOW]->setCurrentState(Panel::STATE_NORMAL);
	}
}
#endif	// #if ENABLE_OPT_MOVMSG_FLOW

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
