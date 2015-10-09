/***********************************************************************//**
 *	SubtitlesWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SubtitlesWindow.h"

// Friends
#include "../../../body/GameExecCtx.h"

// External

// Library
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/ui/TouchPanel.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 表示位置
#define W_THIS				(Game::getGame()->getLogicalWidth())
#define H_THIS				(112)
#define X_THIS				(0)
#define Y_THIS				(Game::getGame()->getLogicalHeight() - H_THIS)
#define X_PADDING			(16)
#define Y_PADDING			(8)
#define W_LOCK				(24)
#define H_LOCK				(24)
#define X_LOCK				X_THIS
#define Y_LOCK				(Y_THIS - H_LOCK)

// テクスチャ座標値
#define W_TEX				(1024.0f)
#define H_TEX				(1024.0f)
#define UV_THIS_L			(0.0f / W_TEX)
#define UV_THIS_T			(0.0f / H_TEX)
#define UV_THIS_W			(443.0f / W_TEX)
#define UV_THIS_H			(106.0f / H_TEX)
#define UV_LOCK_L			(928.0f / W_TEX)
#define UV_LOCK_T			(0.0f / H_TEX)
#define UV_LOCK_W			(24.0f / W_TEX)
#define UV_LOCK_H			(24.0f / H_TEX)

#define ENABLE_CHG_PMSG		(0)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SubtitlesWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SubtitlesWindow::SubtitlesWindow()
{
	for(int i = 0; i < NUM_PANEL; i++)	{	_pnl[i] = 0L;	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SubtitlesWindow::~SubtitlesWindow()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_PANEL; i++)	{	ASSERT(_pnl[i] == 0L);	}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SubtitlesWindow::create(Font* fontRef, Texture* texRef)
{
	// ウインドウの作成
	RectF32 rect(X_THIS, Y_THIS, W_THIS, H_THIS);
	ColorU8 col(255, 255, 255, 255);
	if(!MessageWindow::create(fontRef, texRef, &rect, &col, X_PADDING, X_PADDING, Y_PADDING, Y_PADDING, false))
	{
		return false;
	}
	getPanelText()->setReadingAnimation(true, 1.2f, 0.0f);
	
	// 子パネルの作成
	#if ENABLE_CHG_PMSG
		_pnl[PANEL_LOCK] = new Panel();
		if(!_pnl[PANEL_LOCK]->create(RectF32::setout(&rect, X_LOCK, Y_LOCK, W_LOCK, H_LOCK))) { ASSERT(false); return false; }
		_pnl[PANEL_LOCK]->setStateInfo(Panel::STATE_NORMAL, texRef, RectF32::setout(&rect, UV_LOCK_L, UV_LOCK_T, UV_LOCK_W, UV_LOCK_H), ColorU8::setout(&col, 191, 191, 191, 63));
		_pnl[PANEL_LOCK]->setStateInfo(Panel::STATE_HOVER, texRef, RectF32::setout(&rect, UV_LOCK_L, UV_LOCK_T, UV_LOCK_W, UV_LOCK_H), ColorU8::setout(&col, 255, 255, 255, 127));
	#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SubtitlesWindow::destroy()
{
	// 子パネルの削除
	for(int i = 0; i < NUM_PANEL; i++)
	{
		if(_pnl[i] != 0L)
		{
			_pnl[i]->destroy();
			delete _pnl[i];
			_pnl[i] = 0L;
		}
	}

	MessageWindow::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SubtitlesWindow::exec(ExecRes* res, const ExecCtx* ec)
{
	if(((GameExecCtx*)ec)->isEventPause())	{	return;	}	// イベントポーズ中は抜ける

	MessageWindow::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	コンテンツのフレーム制御
**//*---------------------------------------------------------------------*/
void SubtitlesWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	MessageWindow::execContents(res, ec);

	// 子パネル制御
	#if ENABLE_CHG_PMSG
	{
		// パネルのアクティブ化
		EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
		_pnl[PANEL_LOCK]->hit(_isWaitSay, false);

		TouchPanel* ui = gec->getTouchPanel(); 
		if(!ui->isReacted())
		{
			// 操作
			PointF32 pt;
			if(ui->isClickRelease(&pt))
			{
				for(int i = 0; i < NUM_PANEL; i++)
				{
					if(_pnl[i]->hitTest(&pt))
					{
						switch(i)
						{
						case PANEL_LOCK:
							// ロックボタン
							_isWaitSay = !_isWaitSay;	// SAY ウェイトモードのスイッチング
							ui->setReacted(true);	// UI 反応済み
							break;
						}
						break;
					}
				}
			}
		}

		// 子パネルのフレーム処理
		for(int i = 0; i < NUM_PANEL; i++)	{	_pnl[i]->exec(res, ec);	}
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void SubtitlesWindow::draw(const RenderCtx* rc)
{
	if(!isShowWindow() && (getHideCount() >= 2))	{	return;	}	// ちかちか点滅しないように、非表示になっても次のフレームまで描画

	Renderer* rdr = rc->getRenderer();
	PanelText* ptxt = getPanelText();	ASSERT(ptxt != 0L);
	const RectF32* rectBase = getRectangle();

	// 背景描画
	if(_kind == KIND_TALK)
	{
		rdr->setSolidColor(0, 0, 0, 63);
		rdr->bindTexture(0L);
		RendererUtils::draw2dRect(rdr, (f32)rectBase->x(), (f32)rectBase->y(), (f32)rectBase->w(), (f32)rectBase->h());
	}

	// パネル機能による文字描画
	Panel::draw(rc);

	// 次へカーソル
	if(_isForceNextCursor || _isEnableNextCursor)		// カーソル表示有効時
	{
		if((ptxt != 0L) && ptxt->isScrollDone())
		{
			if((getExecCount() & 16) == 0)
			{
				rdr->setSolidColor(255, 255, 255, 255);
				f32 x = (f32)(rectBase->x() + rectBase->w() - 9);
				f32 y = (f32)(rectBase->y() + rectBase->h() - 9);
				EasyStringDrawer::draw(ptxt->getFont(), "\x81\x9c", x, y, 8, rc);	// "●" の点滅（"\x81\x9c" = "●"; 漢字コードは Shift-JIS）
			}
		}
		else if(!ptxt->isReadingAnimation())
		{
			if((getExecCount() & 8) == 0)
			{
				rdr->setSolidColor(255, 255, 255, 255);
				f32 x = (f32)(rectBase->x() + rectBase->w() - 9);
				f32 y = (f32)(rectBase->y() + rectBase->h() - 9);
				EasyStringDrawer::draw(ptxt->getFont(), "\x81\xa5", x, y, 8, rc);	// "▼" の点滅（"\x81\xa5" = "▼"; 漢字コードは Shift-JIS）
			}
		}
	}

	// 子パネルの描画
	#if ENABLE_CHG_PMSG
		for(int i = 0; i < NUM_PANEL; i++)	{	_pnl[i]->draw(rc);	}
	#endif
}

/*---------------------------------------------------------------------*//**
	ウインドウ表示通知
**//*---------------------------------------------------------------------*/
void SubtitlesWindow::onShowWindow(bool isShow)
{
	if(isShow)
	{
		// 現在の画面解像度における位置調整
		RectF32 rectWnd(X_THIS, Y_THIS, W_THIS, H_THIS);
		setRectangle(&rectWnd);

		// ロックウインドウの有効・無効化
		if(_pnl[PANEL_LOCK] != 0L)
		{
			_pnl[PANEL_LOCK]->setEnable(_kind == KIND_SAY);

			// 現在の画面解像度における位置調整
			RectF32 rectChild(X_LOCK, Y_LOCK, W_LOCK, H_LOCK);
			_pnl[PANEL_LOCK]->setRectangle(&rectChild);
		}
	}

	MessageWindow::onShowWindow(isShow);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
