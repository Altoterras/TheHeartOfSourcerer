/***********************************************************************//**
 *	EventMovieFront.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/24.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EventMovieFront.h"

// Friends
#include "GameGui.h"
#include "window/implement/GameSysMsgWindow.h"
#include "../event/EventData.h"
#include "../event/EventSys.h"
#include "../event/matter/EventMatterType.h"
#include "../event/matter/implement/ActMatter.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/GameFixedStrTbl.h"
#include "../body/GameRenderCtx.h"
#include "../mode/GameMode.h"

// External

// Library
#include "../../etk/sound/SoundMngr.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gui/Panel.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/ui/TouchPanel.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 配置場所
#define X_SKIP				Game::getGame()->getLogicalWidth() - 42
#define RECT_SKIP			X_SKIP, 10, 32, 16

// テクスチャ UV 値
#define W_TEX				(1024.0f)
#define H_TEX				(1024.0f)
#define UV_SKIP				864.0f, 928.0f, 32.0f, 16.0f

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EventMovieFront メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	外部有効かどうかを設定する
**//*---------------------------------------------------------------------*/
void EventMovieFront::setEnable(bool isEnable)
{
	// 状態更新
	if(_isEnableOut == isEnable)
	{
		return;
	}
	_isEnableOut = isEnable;

	// フラグリセット
	_isAskYesNo = false;
}

/*---------------------------------------------------------------------*//**
	パネルを個別に有効・無効化
**//*---------------------------------------------------------------------*/
void EventMovieFront::setEneblePanel(ButtonKind btnk, bool isEnable)
{
	_btn[btnk]._isEnable = isEnable;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EventMovieFront::EventMovieFront()
	: _isEnableOut(false)
	, _isEnableSelf(false)
	, _isAskYesNo(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EventMovieFront::~EventMovieFront()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EventMovieFront::create(Texture* texGuiRef)
{
	// ボタンの作成
	RectF32 rect(RECT_SKIP);
	RectF32 uv;
	if(!_btn[BTN_SKIP].create(texGuiRef, &rect, Gcalc::calcTexUv(&uv, UV_SKIP, W_TEX, H_TEX, 1.0f)))
	{
		ASSERT(false);
		return false;
	}
	_btn[BTN_SKIP]._pnl->setHorizontalHitMargin(16);	// ヒット範囲幅を広めに
	_btn[BTN_SKIP]._pnl->setVerticalHitMargin(16);		// ヒット範囲高さを広めに

	// 初期化
	_isEnableSelf = true;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EventMovieFront::destroy()
{
	_isEnableSelf = false;

	// ボタンの削除
	for(int i = 0; i < NUM_BTN; i++)
	{
		_btn[i].destroy();
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void EventMovieFront::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!_isEnableOut)	{	return;	}	// 外部からの無効化

	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel(); 
	GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);

	if(wndSysMsg->isShowWindow() && _isAskYesNo)
	{
		// Yest / No ウインドウの制御
		const GameSysMsgWindow::Result* res = wndSysMsg->getResult();
		if(res->isClickYesButton() || res->isClickNoButton())
		{
			if(res->isClickYesButton())
			{
				// イベントスキップ
				operationToEvent(EOP_SKIP);
			}
			// イベントポーズの解除
			operationToEvent(EOP_PLAY);
			Game::getGame()->getSoundManager()->resumeBgm();	// BGM 再開
			// ウインドウを閉じる
			wndSysMsg->showWindow(false);
			_isAskYesNo = false;
		}
	}
	else
	{
		// パネルの制御
		{
			// ボタンの位置補正
			if(_btn[BTN_SKIP]._pnl->getRectangle()->left() != X_SKIP)
			{
				RectF32 rect(RECT_SKIP);
				_btn[BTN_SKIP]._pnl->setRectangle(&rect);
			}

			// スキップ可能か
			_btn[BTN_SKIP]._isEnable = operationToEvent(EOP_CHECKSKIP);

			// タッチをパネルに伝達
			PointF32 ptTouch(S32_MIN, S32_MIN);
			PointF32 ptClick(S32_MIN, S32_MIN);
			bool isTouch = !ui->isReacted() ? ui->getTouchPosition(&ptTouch) : false;
			bool isClick = !ui->isReacted() ? ui->isTapRelease(&ptClick, 0L) : false;
			for(int i = 0; i < NUM_BTN; i++)
			{
				if(!_btn[i]._isEnable)	{	continue;	}
				if(isClick)
				{
					if(_btn[i]._pnl->hit(&ptClick, true))
					{
						switch(i)
						{
						case BTN_SKIP:
							{
								// イベントスキップ確認のための一時停止
								if(operationToEvent(EOP_PAUSE))
								{
									ui->setReacted(true);	// UI 反応済み
								}
								Game::getGame()->getSoundManager()->pauseBgm();	// BGM も一時停止
								// イベントスキップの確認
								wndSysMsg->setKind(GameSysMsgWindow::KIND_YESNO);
								wndSysMsg->setReadingAnimation(false);
								wndSysMsg->showMessage(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_CONFIRM_SKIP), 0);
								_isAskYesNo = true;
								break;
							}
							break;
						}
						ui->setReacted(true);
					}
				}
				else
				{
					_btn[i]._pnl->hit(&ptTouch, isTouch);
				}
				_btn[i]._pnl->exec(res, ec);
			}
		}

		// 台詞待ちの終了操作
		if(!ui->isReacted())
		{
			if(ui->isClickRelease(0L))
			{
				// 台詞待ち終了
				if(operationToEvent(EOP_ENDWAIT))
				{
					ui->setReacted(true);	// UI 反応済み
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void EventMovieFront::draw(const RenderCtx* rc)
{
	if(!_isEnableOut || !_isEnableSelf)	{	return;	}	// 無効化中

	// 通常アルファ合成
	rc->getRenderer()->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);

	GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
	if(wndSysMsg->isShowWindow() && _isAskYesNo)
	{
		// 背景描画
		rc->getRenderer()->setSolidColor(0, 0, 0, 127);
		rc->getRenderer()->bindTexture(0L);
		RendererUtils::draw2dRect(rc->getRenderer(), 0.0f, 0.0f, (f32)Game::getGame()->getLogicalWidth(), (f32)Game::getGame()->getLogicalHeight());
	}
	else
	{
		// パネルの描画
		for(int i = 0; i < NUM_BTN; i++)
		{
			if(_btn[i]._isEnable)
			{
				_btn[i]._pnl->draw(rc);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	ユーザー操作
**//*---------------------------------------------------------------------*/
bool EventMovieFront::operationToEvent(EventOp eop)
{
	EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
	if((eop == EOP_PAUSE) || (eop == EOP_PLAY))
	{
		evsys->setEventPause(eop == EOP_PAUSE);
		return true;
	}

	bool is = false;

	// 現在駆動している全ての Act に送信する
	ActMatter* actm = 0L;
	EventData* evdat = evsys->getEventData(); ASSERT(evdat != 0L);
	for(s32 i = 0; i < evdat->getEventMatterNum(); i++)
	{
		EventMatter* matter = evdat->getEventMatter(i);
		if(matter->getMatterType() == EMTYPE_ACT)	// ActMatter 判定
		{
			if(((ActMatter*)matter)->isBegin())	// 実行中かどうか判定
			{
				actm = (ActMatter*)matter;

				switch(eop)
				{
				case EOP_SKIP:
					actm->skip();					// スキップを行う
					is = true;
					break;
				case EOP_ENDWAIT:
					if(actm->isWaitMovieMsg())			// ムービー台詞待ち中
					{
						actm->doneWaitMovieMsg();	// 台詞待ちを終了する
						is = true;
					}
					break;
				case EOP_CHECKSKIP:
					if(actm->checkValidSkip())
					{
						is = true;					// 一つでもスキップ可能なら可とする
					}
					break;
				}
			}
		}
	}

	return is;
}

//==========================================================================
// EventMovieFront::Button メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EventMovieFront::Button::Button()
	: _pnl(0L)
	, _isEnable(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EventMovieFront::Button::~Button()
{
	ASSERT(_pnl == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EventMovieFront::Button::create(Texture* texGuiRef, const RectF32* rectPanel, const RectF32* uvPanel)
{
	_pnl = new Panel();
	if(!_pnl->create(rectPanel))
	{
		ASSERT(false);
		return false;
	}
	ColorU8 col;
	_pnl->setStateInfo(Panel::STATE_NORMAL, texGuiRef, uvPanel, ColorU8::setout(&col, 255, 255, 255, 63));
	_pnl->setStateInfo(Panel::STATE_HOVER, texGuiRef, uvPanel, ColorU8::setout(&col, 255, 255, 255, 255));

	_isEnable = true;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EventMovieFront::Button::destroy()
{
	if(_pnl != 0L)
	{
		_pnl->destroy();
		delete _pnl;
		_pnl = 0L;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
