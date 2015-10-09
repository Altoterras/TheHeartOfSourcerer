/***********************************************************************//**
 *	SysMsgWindow.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SysMsgWindow.h"

// Friends
#include "../../../body/EtkExecCtx.h"

// External

// Library
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/g2d/TiledRectDraw.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/ui/TouchPanel.h"

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 表示位置
#define X_PADDING					(32)
#define Y_PADDING_TOP				(26)
#define Y_PADDING_BTM				(16)

// テクスチャ座標値
#define W_TEX						(1024.0f)
#define H_TEX						(1024.0f)
#define UV_BG_L						(0.0f / W_TEX)
#define UV_BG_T						(0.0f / H_TEX)
#define UV_BG_W						(443.0f / W_TEX)
#define UV_BG_H						(106.0f / H_TEX)

// 子パネル
#define X_BTN_YES					(((_rectScreen->w() - 480) / 2) + 30)
#define X_BTN_NO					(((_rectScreen->w() - 480) / 2) + 270)
#define RECT_BTN_YES				X_BTN_YES, 60, 180, 120
#define RECT_BTN_NO					X_BTN_NO, 60, 180, 120
#define COLOR_BTN_N					131, 83, 77, 255
#define COLOR_BTN_H					146, 95, 87, 255
#define W_BTN_CHIP					 32.0f
#define H_BTN_CHIP					 32.0f
#define UV_BTN_LOOP_N				  0.0f, 896.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_EDGE_N				 32.0f, 896.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_CORNER_N				 64.0f, 896.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_LOOP_H				  0.0f, 928.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_EDGE_H				 32.0f, 928.0f, 32.0f, 32.0f, W_TEX, H_TEX
#define UV_BTN_CORNER_H				 64.0f, 928.0f, 32.0f, 32.0f, W_TEX, H_TEX

// フェードカウンタ
#define FRAME_FADE					(3.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SysMsgWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	矩形の設定
**//*---------------------------------------------------------------------*/
void SysMsgWindow::setRectangle(const RectF32* rectWnd, const RectF32* rectScreen)
{
	_rectScreen->copy(rectScreen);

	RectF32 rectChild;
	rectChild.copy(_pnlBtn[BTN_YES]->getRectangle());
	rectChild.left() = X_BTN_YES;
	_pnlBtn[BTN_YES]->setRectangle(&rectChild);
	rectChild.copy(_pnlBtn[BTN_NO]->getRectangle());
	rectChild.left() = X_BTN_NO;
	_pnlBtn[BTN_NO]->setRectangle(&rectChild);

	MessageWindow::setRectangle(rectWnd);
}

/*---------------------------------------------------------------------*//**
	メッセージ表示（外部制御用）
**//*---------------------------------------------------------------------*/
void SysMsgWindow::showMessage(const VcString* msg, s32 hintCtrl)
{
	MessageWindow::showMessage(msg, hintCtrl);

	// 子パネルの有効・無効
	for(int i = 0; i < NUM_BTN; i++)	{	_pnlBtn[i]->setEnable(false);	}
	if(_kind == KIND_YESNO)
	{
		_pnlBtn[BTN_YES]->setEnable(true);
		_pnlBtn[BTN_NO]->setEnable(true);
	}

	// リセット
	_res.reset();
	_confSelfClose = false;
}

/*---------------------------------------------------------------------*//**
	メッセージ表示（自立制御用）
**//*---------------------------------------------------------------------*/
void SysMsgWindow::showStdMsg(const VcString* msg)
{
	_kind = KIND_TALK;

	showMessage(msg, -1);

	// 自閉フラグを立てる
	_confSelfClose = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SysMsgWindow::SysMsgWindow()
	: _rectScreen(0L)
	, _confSelfClose(false)
	, _confDarkCurtain(false)
	, _confModal(false)
	, _isDoneReserved(false)
	, _fcntFade(0.0f)
{
	for(int i = 0; i < NUM_BTN; i++)
	{
		_pnlBtn[i] = 0;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SysMsgWindow::~SysMsgWindow()
{
	ASSERT(_rectScreen == 0L);
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_BTN; i++)	{	ASSERT(_pnlBtn[i] == 0L);	}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SysMsgWindow::create(const RectF32* rectWnd, const RectF32* rectScreen, Font* fontRef, Texture* texRef, const VcString* strYesButton, const VcString* strNoButton)
{
	RectF32 uv;
	ColorU8 col(255, 255, 255, 255);

	// スクリーン矩形の保存
	_rectScreen = new RectF32(*rectScreen);

	// ウインドウの作成
	if(!MessageWindow::create(fontRef, texRef, rectWnd, &col, X_PADDING, X_PADDING, Y_PADDING_TOP, Y_PADDING_BTM, false))
	{
		return false;
	}

	// 自身のパネル状態の設定
	setStateInfo(STATE_NORMAL, texRef, RectF32::setout(&uv, UV_BG_L, UV_BG_T, UV_BG_W, UV_BG_H));

	// 子パネル作成
	{	
		RectF32 rect(RECT_BTN_YES);
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
		_pnlBtn[BTN_YES] = new Panel();
		_pnlBtn[BTN_YES]->create(&rect);
		_pnlBtn[BTN_YES]->setStateInfo(STATE_NORMAL, tileN);
		_pnlBtn[BTN_YES]->setStateInfo(STATE_HOVER, tileH);
		_pnlBtn[BTN_YES]->createText(fontRef, 16, strYesButton, &col, 0, 0, 3, 0, false, false, false);
		_pnlBtn[BTN_YES]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
		_pnlBtn[BTN_YES]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_pnlBtn[BTN_YES]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	}
	{
		RectF32 rect(RECT_BTN_NO);
		TiledRectDraw* tileN = new TiledRectDraw();
		if(tileN->create(texRef))
		{
			tileN->setTile(TiledRectDraw::TP_LT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_0);
			tileN->setTile(TiledRectDraw::TP_CT, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   rect.w() - (W_BTN_CHIP * 2), H_BTN_CHIP,                  TFW_DISPF_RANG_0);
			tileN->setTile(TiledRectDraw::TP_RT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_90);
			tileN->setTile(TiledRectDraw::TP_LM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   W_BTN_CHIP,                  rect.h() - (H_BTN_CHIP * 2), TFW_DISPF_RANG_270);
			tileN->setTile(TiledRectDraw::TP_CM, Gcalc::calcTexUv(&uv, UV_BTN_LOOP_N, 1.0f),   rect.w() - (W_BTN_CHIP * 2), rect.h() - (H_BTN_CHIP * 2), TFW_DISPF_RANG_0);
			tileN->setTile(TiledRectDraw::TP_RM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   W_BTN_CHIP,                  rect.h() - (H_BTN_CHIP * 2), TFW_DISPF_RANG_90);
			tileN->setTile(TiledRectDraw::TP_LB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_270);
			tileN->setTile(TiledRectDraw::TP_CB, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_N, 1.0f),   rect.w() - (W_BTN_CHIP * 2), H_BTN_CHIP,                  TFW_DISPF_RANG_180);
			tileN->setTile(TiledRectDraw::TP_RB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_N, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_180);
		}
		TiledRectDraw* tileH = new TiledRectDraw();
		if(tileH->create(texRef))
		{
			tileH->setTile(TiledRectDraw::TP_LT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_0);
			tileH->setTile(TiledRectDraw::TP_CT, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   rect.w() - (W_BTN_CHIP * 2), H_BTN_CHIP,                  TFW_DISPF_RANG_0);
			tileH->setTile(TiledRectDraw::TP_RT, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_90);
			tileH->setTile(TiledRectDraw::TP_LM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   W_BTN_CHIP,                  rect.h() - (H_BTN_CHIP * 2), TFW_DISPF_RANG_270);
			tileH->setTile(TiledRectDraw::TP_CM, Gcalc::calcTexUv(&uv, UV_BTN_LOOP_H, 1.0f),   rect.w() - (W_BTN_CHIP * 2), rect.h() - (H_BTN_CHIP * 2), TFW_DISPF_RANG_0);
			tileH->setTile(TiledRectDraw::TP_RM, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   W_BTN_CHIP,                  rect.h() - (H_BTN_CHIP * 2), TFW_DISPF_RANG_90);
			tileH->setTile(TiledRectDraw::TP_LB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_270);
			tileH->setTile(TiledRectDraw::TP_CB, Gcalc::calcTexUv(&uv, UV_BTN_EDGE_H, 1.0f),   rect.w() - (W_BTN_CHIP * 2), H_BTN_CHIP,                  TFW_DISPF_RANG_180);
			tileH->setTile(TiledRectDraw::TP_RB, Gcalc::calcTexUv(&uv, UV_BTN_CORNER_H, 1.0f), W_BTN_CHIP,                  H_BTN_CHIP,                  TFW_DISPF_RANG_180);
		}
		_pnlBtn[BTN_NO] = new Panel();
		_pnlBtn[BTN_NO]->create(&rect);
		_pnlBtn[BTN_NO]->setStateInfo(STATE_NORMAL, tileN);
		_pnlBtn[BTN_NO]->setStateInfo(STATE_HOVER, tileH);
		_pnlBtn[BTN_NO]->createText(fontRef, 16, strNoButton, &col, 0, 0, 3, 0, false, false, false);
		_pnlBtn[BTN_NO]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
		_pnlBtn[BTN_NO]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_pnlBtn[BTN_NO]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
	}
	
	// その他の変数の初期化
	_confDarkCurtain = true;	// デフォルトで暗転させる

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SysMsgWindow::destroy()
{
	// 子パネル削除
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0)
		{
			_pnlBtn[i]->destroy();
			delete _pnlBtn[i];
			_pnlBtn[i] = 0L;
		}
	}
	// スクリーン矩形の削除
	delete _rectScreen;
	_rectScreen = 0L;

	MessageWindow::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SysMsgWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	MessageWindow::execContents(res, ec);

	EtkExecCtx* eec = (EtkExecCtx*)ec;
	TouchPanel* ui = eec->getTouchPanel(); 

	// 子パネル制御
	PointF32 ptN(S32_MIN, S32_MIN);
	PointF32 ptS(S32_MIN, S32_MIN);
	bool isTouch = false;
	bool isClick = false;
	if(!ui->isReacted())
	{
		ui->getTouchStartPosition(&ptS);
		isTouch = ui->getTouchPosition(&ptN);
		isClick = ui->isTapRelease(&ptN, &ptS);
	}
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] == 0)			{	continue;	}
		if(!_pnlBtn[i]->isEnable())	{	continue;	}
		if(isClick && (_fcntFade = F32_PMAX))	// クリック中かつ非フェード中
		{
			if(_pnlBtn[i]->hit(&ptN, &ptS, true))
			{
				switch(i)
				{
				case BTN_YES:
					_res.setClickYesButton();
					ui->setReacted(true);
					_isDoneReserved = true;
					_fcntFade = 0.0f;
					break;
				case BTN_NO:
					_res.setClickNoButton();
					ui->setReacted(true);
					_isDoneReserved = true;
					_fcntFade = 0.0f;
					break;
				}
			}
		}
		else
		{
			_pnlBtn[i]->hit(&ptN, &ptS, isTouch);
		}
		_pnlBtn[i]->exec(res, ec);
	}

	// UI 応答を独占する
	if(_confModal)
	{
		ui->setReacted(true);
	}

	// 自ら閉じる
	if(_confSelfClose)
	{
		if(isShowWindow() && _isDone)
		{
			showWindow(false);

			_confSelfClose = false;	// 自閉フラグクリア
		}
	}

	// フェード処理
	if(_fcntFade < 0.0f)
	{
		_fcntFade += ec->getDeltaFrame();
		if(_fcntFade > 0.0f)
		{
			_fcntFade = F32_PMAX;
		}
	}
	else if((_fcntFade >= 0.0f) && (_fcntFade != F32_PMAX))
	{
		_fcntFade += ec->getDeltaFrame();
		if(_fcntFade > FRAME_FADE)
		{
			_fcntFade = FRAME_FADE;
			_isDone = _isDoneReserved;
		}
	}

}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void SysMsgWindow::draw(const RenderCtx* rc)
{
	// 非表示時は抜ける
	if(!isShowWindow() || isTemporaryHiding())	{	return;	}

	// 暗転
	if(_confDarkCurtain)
	{
		u8 alpha = 127;
		if(_fcntFade < 0.0f)
		{
			alpha = (u8)(127.0f * (_fcntFade + FRAME_FADE) / FRAME_FADE);
		}
		else if((_fcntFade >= 0.0f) && (_fcntFade != F32_PMAX))
		{
			alpha = (u8)(127.0f * (FRAME_FADE - _fcntFade) / FRAME_FADE);
		}
		ColorU8 col(0, 0, 0, alpha);
		RendererUtils::draw2dColorRect(rc->getRenderer(), _rectScreen, &col);
	}

	Window::draw(rc);
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void SysMsgWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	Renderer* rdr = rc->getRenderer();
	PanelText* ptxt = getPanelText();	ASSERT(ptxt != 0L);

	// メッセージウインドウの描画
	MessageWindow::drawContents(rc, rectBase);

	// 次へカーソル
	if(_isForceNextCursor || _isEnableNextCursor)		// カーソル表示有効時
	{
		if((ptxt != 0L) && ptxt->isScrollDone() /*( (_lineScrollCur < (_numLine - 1)) && _txtdraw->isReadingAnimationEnd() )*/)
		{
			if((getExecCount() & 8) == 0)
			{
				rdr->setSolidColor(255, 255, 255, 255);
				f32 x = (f32)(rectBase->x() + rectBase->w() - 16 - 24);
				f32 y = (f32)(rectBase->y() + rectBase->h() - 16 - 10);
				EasyStringDrawer::draw(ptxt->getFont(), "\x81\x9c", x, y, 8, rc);	// "●" の点滅（"\x81\x9c" = "●"; 漢字コードは Shift-JIS）
			}
		}
		else if(!ptxt->isReadingAnimation())
		{
			if((getExecCount() & 8) == 0)
			{
				rdr->setSolidColor(255, 255, 255, 255);
				f32 x = (f32)(rectBase->x() + rectBase->w() - 16 - 24);
				f32 y = (f32)(rectBase->y() + rectBase->h() - 16 - 10);
				EasyStringDrawer::draw(ptxt->getFont(), "\x81\xa5", x, y, 8, rc);	// "▼" の点滅（"\x81\xa5" = "▼"; 漢字コードは Shift-JIS）
			}
		}
	}

	// 子パネル描画
	for(int i = 0; i < NUM_BTN; i++)
	{
		if((_pnlBtn[i] != 0) && _pnlBtn[i]->isEnable())
		{
			_pnlBtn[i]->draw(rc);
		}
	}
}

/*---------------------------------------------------------------------*//**
	ウインドウ表示通知
**//*---------------------------------------------------------------------*/
void SysMsgWindow::onShowWindow(bool isShow)
{
	if(isShow)
	{
		// フェードカウンタのリセット
		_fcntFade = -FRAME_FADE;

		// 終了予約のリセット
		_isDoneReserved = false;
	}
	else
	{
		// モーダル設定のリセット
		_confModal = false;

		// 子パネルボタンのホバー・アクティブ状態をリセット
		for(int i = 0; i < NUM_BTN; i++)
		{
			if((_pnlBtn[i] != 0) && _pnlBtn[i]->isEnable())
			{
				_pnlBtn[i]->hit(false, false);
			}
		}

		// 終了予約のリセット
		_isDoneReserved = false;
	}

	MessageWindow::onShowWindow(isShow);
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void SysMsgWindow::changeLanguage(const VcString* strYesButton, const VcString* strNoButton)
{
	// Yes, No ボタンの言語を変更する
	if((_pnlBtn[BTN_YES] != 0L) && (_pnlBtn[BTN_YES]->getPanelText() != 0L))
	{
		_pnlBtn[BTN_YES]->getPanelText()->setText(strYesButton, false);
	}
	if((_pnlBtn[BTN_NO] != 0L) && (_pnlBtn[BTN_NO]->getPanelText() != 0L))
	{
		_pnlBtn[BTN_NO]->getPanelText()->setText(strNoButton, false);
	}
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
