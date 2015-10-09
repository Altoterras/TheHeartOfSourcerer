/***********************************************************************//**
 *	MessageWindow.cpp
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
#include "MessageWindow.h"

// Friends
#include "../../body/EtkExecCtx.h"

// External

// Library
#include "../../../tfw/lib/Rect.h"
#include "../../../tfw/lib/Size.h"
#include "../../../tfw/lib/Color.h"
#include "../../../tfw/g2d/StringDrawer.h"
#include "../../../tfw/g2d/font/Font.h"
#include "../../../tfw/gcmn/Texture.h"
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/gcmn/RendererUtils.h"
#include "../../../tfw/gui/PanelText.h"
#include "../../../tfw/ui/TouchPanel.h"

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MessageWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	オートスクロールするかどうかを設定する
**//*---------------------------------------------------------------------*/
void MessageWindow::setAutoScroll(bool isEnable)
{
	PanelText* ptxt = getPanelText(); ASSERT(ptxt != 0L);
	if(ptxt != 0L)	{	ptxt->setAutoScroll(isEnable);			}
}

/*---------------------------------------------------------------------*//**
	読み上げアニメーション中かを得る
**//*---------------------------------------------------------------------*/
bool MessageWindow::isReadingAnimation() const
{
	PanelText* ptxt = getPanelText();
	if(ptxt == 0L)	{	return false;	}
	return ptxt->isReadingAnimation();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	矩形を設定する
**//*---------------------------------------------------------------------*/
void MessageWindow::setRectangle(const RectF32* rect)
{
	Window::setRectangle(rect);
}

/*---------------------------------------------------------------------*//**
	メッセージ設定
**//*---------------------------------------------------------------------*/
void MessageWindow::setWindowText(const VcString* text)
{
	PanelText* ptxt = getPanelText();	ASSERT(ptxt != 0L);
	if(ptxt != 0L)
	{
		ptxt->setText(text, false);	
		ASSERTM((_kind != KIND_SAY) || !ptxt->isMultiplePagesText(), "Text Over!");
	}
}

/*---------------------------------------------------------------------*//**
	メッセージ追加
**//*---------------------------------------------------------------------*/
void MessageWindow::addWindowText(const VcString* text)
{
	PanelText* ptxt = getPanelText();	ASSERT(ptxt != 0L);
	if(ptxt != 0L)
	{
		ptxt->setText(text, true);
		ASSERTM((_kind != KIND_SAY) || !ptxt->isMultiplePagesText(), "Text Over!");
	}
}

/*---------------------------------------------------------------------*//**
	メッセージ表示
**//*---------------------------------------------------------------------*/
void MessageWindow::showMessage(const VcString* msg, s32 hintCtrl)
{
	// ウインドウ設定
	setWindowText(msg);
	showWindow(true);

	// ヒントを保存
	_hintCtrl = hintCtrl;

	// 終了フラグを戻す
	_isDone = false;
}

/*---------------------------------------------------------------------*//**
	次のページへ送る
**//*---------------------------------------------------------------------*/
void MessageWindow::nextPage(s32 hintCtrl)
{
	if(hintCtrl != _hintCtrl)	{	return;	}	// 違うメッセージの制御である

	PanelText* ptxt = getPanelText();	ASSERT(ptxt != 0L);
	if(ptxt != 0L)
	{
		// 次のページに送る
		ptxt->nextPage();
	}
}

/*---------------------------------------------------------------------*//**
	読み上げアニメーションをするかどうかを設定する
**//*---------------------------------------------------------------------*/
void MessageWindow::setReadingAnimation(bool isEnable)
{
	PanelText* ptxt = getPanelText(); ASSERT(ptxt != 0L);
	if(ptxt != 0L) { ptxt->setReadingAnimation(isEnable, 1.2f, 0.0f); }
}

/*---------------------------------------------------------------------*//**
	縦スクロールアニメーションをするかどうかを設定する
**//*---------------------------------------------------------------------*/
void MessageWindow::setVerticalScrollAnimation(bool isEnable)
{
	PanelText* ptxt = getPanelText(); ASSERT(ptxt != 0L);
	if(ptxt != 0L) { ptxt->setVerticalScrollAnimation(isEnable, 1.0f); }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MessageWindow::MessageWindow()
	: _isEnableNextCursor(false)
	, _isForceNextCursor(false) 
	, _isDone(false)
	, _isWaitSay(false)
	, _kind(KIND_NULL)
	, _hintCtrl(0)
	, _gparam(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MessageWindow::~MessageWindow()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MessageWindow::create(Font* fontRef, Texture* texRef, const RectF32* rectWnd, const ColorU8* colorTxt, s16 xPaddingLeft, s16 xPaddingRight, s16 yPaddingTop, s16 yPaddingBottom, bool isAutoScroll)
{
	// ウインドウの作成
	if(!Window::create(rectWnd))
	{
		return false;
	}

	// パネルのテキスト機能を有効化する
	if(colorTxt != 0L)
	{
		createText(fontRef, 16, colorTxt, xPaddingLeft, yPaddingTop, yPaddingTop, yPaddingBottom, false, false, isAutoScroll);
	}

	// デフォルトのモード設定
	if(_kind == KIND_NULL)	{	_kind = KIND_TALK;	}	// ユーザーからの応答を受け付けるモード
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MessageWindow::destroy()
{
	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void MessageWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	EtkExecCtx* eec = (EtkExecCtx*)ec;
	TouchPanel* ui = eec->getTouchPanel();

	if(_kind == KIND_TALK)	// ユーザーの入力を受け付けるモードの場合
	{
		///if(!ui->isReacted() && ui->isTapRelease(0L, 0L) /* et.GetPad().IsTrigger(Pad.KeyId.OK) || et.GetPad().IsTrigger(Pad.KeyId.CHECK) */)
		if(!ui->isReacted() && ui->isTapRelease(0L, 0L))
		{
			PanelText* ptxt = getPanelText(); ASSERT(ptxt != 0L);
			if(ptxt != 0L)
			{
				// スクロール終了後のタップ
				if(ptxt->isScrollDone())
				{
					// 終了フラグ設定
					_isDone = true;
					ui->setReacted(true);	// 他が反応しないように UI に反応済みフラグを立てる
					///if(res != 0L)	{	res->setDone();	}
				}
				// 次の行に送る
				ptxt->nextPage();
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void MessageWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
}

/*---------------------------------------------------------------------*//**
	ウィンドウ表示通知
**//*---------------------------------------------------------------------*/
void MessageWindow::onShowWindow(bool isShow)
{
	if(isShow)	// 表示になるタイミング
	{
		// リセット
		_isDone = false;
	}
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
