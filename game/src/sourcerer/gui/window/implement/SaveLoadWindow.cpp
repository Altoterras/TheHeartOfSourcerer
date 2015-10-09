/***********************************************************************//**
 *	SaveLoadWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SaveLoadWindow.h"

// Friends
#include "../../../episode/save/SaveStructure.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/panel/HeaderPanel.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../mode/GameMode.h"
#include "../../../save/SaveData.h"
#include "../../../save/SaveSys.h"
#include "../../../save/ScoreBase.h"

// External
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/g2d/TiledRectDraw.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define X_BASE						((Game::getGame()->getLogicalWidth() - 400) / 2)
#define Y_BASE						48
#define RECT_THIS					X_BASE,       Y_BASE, 400,  272
#define RECT_BTN_FILE1				X_BASE + 40,  Y_BASE + 22,  320,  70
#define RECT_BTN_FILE2				X_BASE + 40,  Y_BASE + 112, 320,  70
#define RECT_SCORE					X_BASE + 40,  Y_BASE + 192, 320,  24
#define RECT_FILE1_PSNS_ON			X_BASE + 240, Y_BASE + 43,  30,   14
#define RECT_FILE2_PSNS_ON			X_BASE + 240, Y_BASE + 133, 30,   14
#define RECT_CAP_GOTITLE			Game::getGame()->getLogicalWidth() - 260, Game::getGame()->getLogicalHeight() - 40, 200,  32
#define RECT_BTN_GOTITLE			Game::getGame()->getLogicalWidth() - 56, Game::getGame()->getLogicalHeight() - 40,  32,  32

#define FONTSIZE_CAP_GOTITLE		10
#define COLOR_BTN_N					131, 83, 77, 255
#define COLOR_BTN_H					146, 95, 87, 255

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ
#define UV_THIS						128.0f / W_TEX, 448.0f / H_TEX, 400.0f / W_TEX, 272.0f / H_TEX
#define W_BTN_CHIP					 32.0f
#define H_BTN_CHIP					 32.0f
#define UV_BTN_LOOP_N				  0.0f, 896.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_EDGE_N				 32.0f, 896.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_CORNER_N				 64.0f, 896.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_LOOP_H				  0.0f, 928.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_EDGE_H				 32.0f, 928.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_CORNER_H				 64.0f, 928.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_GOTITLE_N			  0.0f, 960.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_GOTITLE_H			 32.0f, 960.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_PSNS_ON					  1.0f, 113.0f, 30.0f, 14.0f, W_TEX, H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SaveLoadWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SaveLoadWindow::SaveLoadWindow()
	: _saveflags(0)
	, _confirmkind(CFK_NULL)
	, _page(PAGE_SAVE)
	, _isDone(false)
	, _fontRef(0L)
	, _texRef(0L)
	, _hpnlHbarSa(0L)
{
	for(int i = 0; i < NUM_BTN; i++)		{	_btn[i] = 0L;	}
	for(int i = 0; i < NUM_PNL; i++)		{	_pnl[i] = 0L;		}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SaveLoadWindow::~SaveLoadWindow()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_BTN; i++)	{	ASSERT(_btn[i] == 0L);	}
		for(int i = 0; i < NUM_PNL; i++)	{	ASSERT(_pnl[i] == 0L);		}
	#endif
	ASSERT(_hpnlHbarSa == 0L);
}

/*---------------------------------------------------------------------*//**
	作成通知
**//*---------------------------------------------------------------------*/
bool SaveLoadWindow::create(Font* fontRef, Texture* texRef, Page page, bool isStandAlone, u32 saveflags)
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
	_page = page;
	_saveflags = saveflags;

	// 自身の設定
	setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_THIS));

	// 子パネル作成
	{
		TiledRectDraw* tileN = new TiledRectDraw();
		if(tileN->create(texRef))
		{
			tileN->setTile(TiledRectDraw::TP_LT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_0);
			tileN->setTile(TiledRectDraw::TP_CT, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_0);
			tileN->setTile(TiledRectDraw::TP_RT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_90);
			tileN->setTile(TiledRectDraw::TP_LM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_270);
			tileN->setTile(TiledRectDraw::TP_CM, Gcalc::calcTexUv(&uv, UV_BTN_LOOP_N, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_0);
			tileN->setTile(TiledRectDraw::TP_RM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_90);
			tileN->setTile(TiledRectDraw::TP_LB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_270);
			tileN->setTile(TiledRectDraw::TP_CB, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_180);
			tileN->setTile(TiledRectDraw::TP_RB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_180);
		}
		TiledRectDraw* tileH = new TiledRectDraw();
		if(tileH->create(texRef))
		{
			tileH->setTile(TiledRectDraw::TP_LT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_0);
			tileH->setTile(TiledRectDraw::TP_CT, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_0);
			tileH->setTile(TiledRectDraw::TP_RT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_90);
			tileH->setTile(TiledRectDraw::TP_LM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_270);
			tileH->setTile(TiledRectDraw::TP_CM, Gcalc::calcTexUv(&uv, UV_BTN_LOOP_H, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_0);
			tileH->setTile(TiledRectDraw::TP_RM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_90);
			tileH->setTile(TiledRectDraw::TP_LB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_270);
			tileH->setTile(TiledRectDraw::TP_CB, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_180);
			tileH->setTile(TiledRectDraw::TP_RB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_180);
		}
		_btn[BTN_FILE1] = new Panel();
		_btn[BTN_FILE1]->create(RectF32::setout(&rect, RECT_BTN_FILE1));
		_btn[BTN_FILE1]->setStateInfo(STATE_NORMAL, tileN);
		_btn[BTN_FILE1]->setStateInfo(STATE_HOVER, tileH);
		_btn[BTN_FILE1]->createText(fontRef, 16, VcString::setout(&str, ""), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
		_btn[BTN_FILE1]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
		_btn[BTN_FILE1]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_btn[BTN_FILE1]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	}
	{
		TiledRectDraw* tileN = new TiledRectDraw();
		if(tileN->create(texRef))
		{
			tileN->setTile(TiledRectDraw::TP_LT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_0);
			tileN->setTile(TiledRectDraw::TP_CT, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   W_BTN_CHIP, H_BTN_CHIP,                  TFW_DISPF_RANG_0);
			tileN->setTile(TiledRectDraw::TP_RT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_90);
			tileN->setTile(TiledRectDraw::TP_LM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   W_BTN_CHIP,                  H_BTN_CHIP, TFW_DISPF_RANG_270);
			tileN->setTile(TiledRectDraw::TP_CM, Gcalc::calcTexUv(&uv, UV_BTN_LOOP_N, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_0);
			tileN->setTile(TiledRectDraw::TP_RM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   W_BTN_CHIP,                  H_BTN_CHIP, TFW_DISPF_RANG_90);
			tileN->setTile(TiledRectDraw::TP_LB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_270);
			tileN->setTile(TiledRectDraw::TP_CB, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   W_BTN_CHIP, H_BTN_CHIP,                  TFW_DISPF_RANG_180);
			tileN->setTile(TiledRectDraw::TP_RB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_180);
		}
		TiledRectDraw* tileH = new TiledRectDraw();
		if(tileH->create(texRef))
		{
			tileH->setTile(TiledRectDraw::TP_LT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_0);
			tileH->setTile(TiledRectDraw::TP_CT, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   W_BTN_CHIP, H_BTN_CHIP,                  TFW_DISPF_RANG_0);
			tileH->setTile(TiledRectDraw::TP_RT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_90);
			tileH->setTile(TiledRectDraw::TP_LM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   W_BTN_CHIP,                  H_BTN_CHIP, TFW_DISPF_RANG_270);
			tileH->setTile(TiledRectDraw::TP_CM, Gcalc::calcTexUv(&uv, UV_BTN_LOOP_H, 1.0f),   W_BTN_CHIP, H_BTN_CHIP, TFW_DISPF_RANG_0);
			tileH->setTile(TiledRectDraw::TP_RM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   W_BTN_CHIP,                  H_BTN_CHIP, TFW_DISPF_RANG_90);
			tileH->setTile(TiledRectDraw::TP_LB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_270);
			tileH->setTile(TiledRectDraw::TP_CB, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   W_BTN_CHIP, H_BTN_CHIP,                  TFW_DISPF_RANG_180);
			tileH->setTile(TiledRectDraw::TP_RB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_180);
		}
		_btn[BTN_FILE2] = new Panel();
		_btn[BTN_FILE2]->create(RectF32::setout(&rect, RECT_BTN_FILE2));
		_btn[BTN_FILE2]->setStateInfo(STATE_NORMAL, tileN);
		_btn[BTN_FILE2]->setStateInfo(STATE_HOVER, tileH);
		_btn[BTN_FILE2]->createText(fontRef, 16, VcString::setout(&str, ""), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
		_btn[BTN_FILE2]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
		_btn[BTN_FILE2]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_btn[BTN_FILE2]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	}
	{
		_pnl[PNL_SCORE] = new Panel();
		_pnl[PNL_SCORE]->create(RectF32::setout(&rect, RECT_SCORE));
		_pnl[PNL_SCORE]->createText(fontRef, 16, VcString::setout(&str, ""), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
		_pnl[PNL_SCORE]->getPanelText()->setAlign(PanelText::F_ALIGN_RIGHT);
		_pnl[PNL_SCORE]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_pnl[PNL_SCORE]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	}
	{
		_pnl[PNL_FILE1_PSNS_ON] = new Panel();
		_pnl[PNL_FILE1_PSNS_ON]->create(RectF32::setout(&rect, RECT_FILE1_PSNS_ON));
		_pnl[PNL_FILE1_PSNS_ON]->setStateInfo(Panel::STATE_NORMAL, texRef, Gcalc::calcTexUv(&uv, UV_PSNS_ON, 1.0f));
		_pnl[PNL_FILE1_PSNS_ON]->setEnable(false);
		_pnl[PNL_FILE1_PSNS_ON]->setFlags(Panel::F_DRAW_TOP_SHADOW, true);
		_pnl[PNL_FILE2_PSNS_ON] = new Panel();
		_pnl[PNL_FILE2_PSNS_ON]->create(RectF32::setout(&rect, RECT_FILE2_PSNS_ON));
		_pnl[PNL_FILE2_PSNS_ON]->setStateInfo(Panel::STATE_NORMAL, texRef, Gcalc::calcTexUv(&uv, UV_PSNS_ON, 1.0f));
		_pnl[PNL_FILE2_PSNS_ON]->setEnable(false);
		_pnl[PNL_FILE2_PSNS_ON]->setFlags(Panel::F_DRAW_TOP_SHADOW, true);
	}
	{
		_pnl[PNL_CAP_GOTITLE] = new Panel();
		_pnl[PNL_CAP_GOTITLE]->create(RectF32::setout(&rect, RECT_CAP_GOTITLE));
		_pnl[PNL_CAP_GOTITLE]->createText(fontRef, FONTSIZE_CAP_GOTITLE, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CAP_GO_BACK_TO_TITLE), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
		_pnl[PNL_CAP_GOTITLE]->getPanelText()->setAlign(PanelText::F_ALIGN_RIGHT);
		_pnl[PNL_CAP_GOTITLE]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_pnl[PNL_CAP_GOTITLE]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	}
	{
		_btn[BTN_GOTITLE] = new Panel();
		_btn[BTN_GOTITLE]->create(RectF32::setout(&rect, RECT_BTN_GOTITLE));
		_btn[BTN_GOTITLE]->setStateInfo(STATE_NORMAL, texRef, Gcalc::calcTexUv(&uv, UV_BTN_GOTITLE_N, 1.0f));
		_btn[BTN_GOTITLE]->setStateInfo(STATE_HOVER, texRef, Gcalc::calcTexUv(&uv, UV_BTN_GOTITLE_H, 1.0f));
	}

	// スタンドアローン時は自らヘッダーパネルを持つ
	if(isStandAlone)
	{
		HeaderPanel* hpnlHbar = new HeaderPanel();
		if(!hpnlHbar->create(
			fontRef,
			texRef,
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SAVE),
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CLOSE),
			0L,
			0L,
			0	))
		{
			delete hpnlHbar;
			ASSERT(false);
			return false;
		}
		_hpnlHbarSa = hpnlHbar;
	}

	// タイトルに戻るボタンはスタンドアローン時は無効に
	_pnl[PNL_CAP_GOTITLE]->setEnable(!isStandAlone);
	_btn[BTN_GOTITLE]->setEnable(!isStandAlone);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄通知
**//*---------------------------------------------------------------------*/
void SaveLoadWindow::destroy()
{
	// ヘッダーパネル削除
	if(_hpnlHbarSa != 0L)
	{
		_hpnlHbarSa->destroy();
		delete _hpnlHbarSa;
		_hpnlHbarSa = 0L;
	}

	// 子パネル削除
	for(int i = 0; i < NUM_PNL; i++)
	{
		if(_pnl[i] != 0L)
		{
			_pnl[i]->destroy();
			delete _pnl[i];
			_pnl[i] = 0L;
		}
	}
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_btn[i] != 0L)
		{
			_btn[i]->destroy();
			delete _btn[i];
			_btn[i] = 0L;
		}
	}

	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SaveLoadWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	// 親の制御
	Window::execContents(res, ec);

	// YES / NO メッセージウインドウの制御
	if(_confirmkind != CFK_NULL)
	{
		GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
		if((wndSysMsg != 0L) && wndSysMsg->isShowWindow())
		{
			wndSysMsg->exec(res, ec);
			const GameSysMsgWindow::Result* res = wndSysMsg->getResult();
			if(res->isClickYesButton() || res->isClickNoButton())
			{
				if((_confirmkind == CFK_GOTITLE) && res->isClickYesButton())
				{
					// タイトルに戻る処理
					GameMode* gmode = Game::getGame()->getGameMode();
					gmode->changeEndMode(GameMode::MODE_GAME_TITLE);
				}
				// ウインドウを消す
				wndSysMsg->showWindow(false);
				_confirmkind = CFK_NULL;
			}
			return;
		}
	}

	// 子パネル制御
	TouchPanel* ui = gec->getTouchPanel();
	PointF32 ptTouch(S32_MIN, S32_MIN);
	PointF32 ptClick(S32_MIN, S32_MIN);
	bool isTouch = !ui->isReacted() ? ui->getTouchPosition(&ptTouch) : false;
	bool isClick = !ui->isReacted() ? ui->isClickRelease(&ptClick) : false;
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_btn[i] == 0)	{	continue;	}
		if(isClick)
		{
			if(_btn[i]->hit(&ptClick, true))
			{
				switch(i)
				{
				case BTN_FILE1:
					doSave(1);
					break;
				case BTN_FILE2:
					doSave(2);
					break;
				case BTN_GOTITLE:
					{
						GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
						if((wndSysMsg != 0L) && !wndSysMsg->isShowWindow())
						{
							wndSysMsg->setKind(MessageWindow::KIND_YESNO);
							wndSysMsg->setModal(true);
							wndSysMsg->showMessage(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CONFIRM_GO_BACK_TO_TITLE), 0);
							_confirmkind = CFK_GOTITLE;
						}
					}
					break;
				}
				ui->setReacted(true);
			}
		}
		else
		{
			_btn[i]->hit(&ptTouch, isTouch);
		}
		_btn[i]->exec(res, ec);
	}

	// スコア設定
	if((_pnl[PNL_SCORE] != 0L) && (_pnl[PNL_SCORE]->getPanelText() != 0L))
	{
		// 現在のスコアとプレイ時間
		SaveSys* svsys = Game::getGame()->getSaveSys();
		SaveData* svdat = (svsys != 0L) ? svsys->getSaveData() : 0L;
		ScoreBase* score = Game::getGame()->getScore();
		if((svdat != 0L) && (score != 0L))
		{
			u32 secondPlay = svdat->getPlayFrame() / 30;
			VcString wk = VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CURRENT_PLAY_TIME)->getRaw(), secondPlay / 60, secondPlay % 60, score->calcScore());
			_pnl[PNL_SCORE]->getPanelText()->setText(&wk, false);
		}
	}

	// ヘッダーパネル制御
	if(_hpnlHbarSa != 0L)
	{
		_hpnlHbarSa->exec(res, ec);
		const HeaderPanel::Res* cres = _hpnlHbarSa->getResult();
		if(cres->isClickLeftButton())
		{
			// キャンセルで終了
			if(res != 0L)	{	res->setDone();	}
			_isDone = true;
		}
	}
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void SaveLoadWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	// 子パネル描画
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_btn[i] != 0)	{	_btn[i]->draw(rc);	}
	}
	for(int i = 0; i < NUM_PNL; i++)
	{
		if(_pnl[i] != 0)	{	_pnl[i]->draw(rc);		}
	}

	// ヘッダーパネル描画
	if(_hpnlHbarSa != 0L)	{	_hpnlHbarSa->draw(rc);	}
}

/*---------------------------------------------------------------------*//**
	ウインドウ表示・非表示通知
**//*---------------------------------------------------------------------*/
void SaveLoadWindow::onShowWindow(bool isShow)
{
	if(isShow)
	{
		updateSaveFileInfo();	// セーブファイルの情報を更新する

		// 値のクリア
		_isDone = false;

		complyChangeScreenSize(); // 位置調整
	}
}

/*---------------------------------------------------------------------*//**
	論理画面解像度変更通知
**//*---------------------------------------------------------------------*/
void SaveLoadWindow::notifyChangeLogicalScreenSize()
{
	complyChangeScreenSize(); // 位置調整
}

/*---------------------------------------------------------------------*//**
	セーブする

	@param svid セーブデータID (1 or 2)
**//*---------------------------------------------------------------------*/
bool SaveLoadWindow::doSave(s32 svid)
{
	SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
	if(!svsys->save(svid, _saveflags))	{	return false;	}

	GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
	wndSysMsg->showStdMsg(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_SAVE_DONE));

	updateSaveFileInfo();	// セーブファイルの情報を更新する

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブファイルの情報を更新する
**//*---------------------------------------------------------------------*/
void SaveLoadWindow::updateSaveFileInfo()
{
	SaveSys* svsys = Game::getGame()->getSaveSys();
	for(int i = 0; i < NUM_SAVEFILE; i++)
	{
		// セーブファイルの情報を得る
		SaveStructure::SummarySaveStructure sumsvst;
		bool isPsnsUserData = false;
		if(svsys->readSaveData(i + 1, &sumsvst, sizeof(SaveStructure::SummarySaveStructure), &isPsnsUserData))
		{
			_sfi[i]._framePlay = sumsvst._framePlay;
			_sfi[i]._isCleared = TFW_IS_FLAG(sumsvst._flags, SaveStructure::SummarySaveStructure::F_CLEARED);
			_sfi[i]._score = sumsvst._score;
		}

		// ボタンのテキストを書き換える
		PanelText* ptxt = _btn[i]->getPanelText();
		VcString strBtn = VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_SAVE_FILE_1_TIME_AND_SCORE + i)->getRaw(),
				_sfi[i]._framePlay / 30 / 60,
				(_sfi[i]._framePlay / 30) % 60,
				_sfi[i]._isCleared ? "*" : "",
				_sfi[i]._score);
		ptxt->setText(&strBtn, false);
		_pnl[PNL_FILE1_PSNS_ON + i]->setEnable(isPsnsUserData);
	}
}

/*---------------------------------------------------------------------*//**
	画面解像度変更に対応する
**//*---------------------------------------------------------------------*/
void SaveLoadWindow::complyChangeScreenSize()
{
	// 現在の画面解像度における位置調整
	RectF32 rect;
	setRectangle(RectF32::setout(&rect, RECT_THIS));
	_pnl[PNL_SCORE]->setRectangle(RectF32::setout(&rect, RECT_SCORE));
	_pnl[PNL_FILE1_PSNS_ON]->setRectangle(RectF32::setout(&rect, RECT_FILE1_PSNS_ON));
	_pnl[PNL_FILE2_PSNS_ON]->setRectangle(RectF32::setout(&rect, RECT_FILE2_PSNS_ON));
	_pnl[PNL_CAP_GOTITLE]->setRectangle(RectF32::setout(&rect, RECT_CAP_GOTITLE));
	_btn[BTN_FILE1]->setRectangle(RectF32::setout(&rect, RECT_BTN_FILE1));
	_btn[BTN_FILE2]->setRectangle(RectF32::setout(&rect, RECT_BTN_FILE2));
	_btn[BTN_GOTITLE]->setRectangle(RectF32::setout(&rect, RECT_BTN_GOTITLE));
	if(_hpnlHbarSa != 0L) { _hpnlHbarSa->complyChangeScreenSize(); }
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
