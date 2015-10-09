/***********************************************************************//**
 *	IngameFront.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "IngameFront.h"

// Friends
#include "../episode/party/MyParty.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../mode/GameMode.h"
#include "../spirit/Spirit.h"
#include "../spirit/char/pc/PcSpirit.h"

// External

// Library
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gui/Panel.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/ui/TouchPanel.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 配置場所
#define RECT_MENU			Game::getGame()->getLogicalWidth() - 80, 248, 64, 64
#define RECT_SOUMA			Game::getGame()->getLogicalWidth() - 80, 8, 64, 64
#define RECT_MEDITATION		Game::getGame()->getLogicalWidth() - 80, 88, 64, 64
#define RECT_GUARDCHARGE	Game::getGame()->getLogicalWidth() - 80, 8, 64, 64
#define RECT_ATTRACT		Game::getGame()->getLogicalWidth() - 80, 88, 64, 64
#define RECT_CURE			Game::getGame()->getLogicalWidth() - 80, 8, 64, 64
#define W_BBTN				32
#define H_BBTN				32

// テクスチャ UV 値
#define W_TEX				(1024.0f)
#define H_TEX				(1024.0f)
#define UV_MENU				960.0f, 960.0f, 64.0f, 64.0f
#define UV_SOUMA			896.0f, 960.0f, 64.0f, 64.0f
#define UV_MEDITATION		832.0f, 896.0f, 64.0f, 64.0f
#define UV_GUARDCHARGE		896.0f, 896.0f, 64.0f, 64.0f
#define UV_ATTRACT			960.0f, 832.0f, 64.0f, 64.0f
#define UV_CURE				960.0f, 896.0f, 64.0f, 64.0f
#define UV_BBTN_N			128.0f, 992.0f, 32.0f, 32.0f
#define UV_BBTN_H			160.0f, 992.0f, 32.0f, 32.0f

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// IngameFront メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	パネルを個別に有効・無効化
**//*---------------------------------------------------------------------*/
void IngameFront::setEneblePanel(ButtonKind btnk, bool isEnable)
{
	_btn[btnk]._isEnable = isEnable;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
IngameFront::IngameFront()
	: _isEnableOut(true)
	, _isEnableSelf(false)
	, _pnlBreak(0L)
	, _ipcLeaderCur(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
IngameFront::~IngameFront()
{
	ASSERT(_pnlBreak == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool IngameFront::create(Texture* texGuiRef)
{
	RectF32 rect;
	RectF32 uv;
	ColorU8 col(255, 255, 255, 255);

	// ボタンの作成
	if(!_btn[BTN_MENU].create(texGuiRef, RectF32::setout(&rect, RECT_MENU), Gcalc::calcTexUv(&uv, UV_MENU, W_TEX, H_TEX, 1.0f)))						{	ASSERT(false);	return false;	}
	if(!_btn[BTN_SOUMA].create(texGuiRef, RectF32::setout(&rect, RECT_SOUMA), Gcalc::calcTexUv(&uv, UV_SOUMA, W_TEX, H_TEX, 1.0f)))						{	ASSERT(false);	return false;	}
	if(!_btn[BTN_MEDITATION].create(texGuiRef, RectF32::setout(&rect, RECT_MEDITATION), Gcalc::calcTexUv(&uv, UV_MEDITATION, W_TEX, H_TEX, 1.0f)))		{	ASSERT(false);	return false;	}
	if(!_btn[BTN_GUARDCHARGE].create(texGuiRef, RectF32::setout(&rect, RECT_GUARDCHARGE), Gcalc::calcTexUv(&uv, UV_GUARDCHARGE, W_TEX, H_TEX, 1.0f)))	{	ASSERT(false);	return false;	}
	if(!_btn[BTN_ATTRACT].create(texGuiRef, RectF32::setout(&rect, RECT_ATTRACT), Gcalc::calcTexUv(&uv, UV_ATTRACT, W_TEX, H_TEX, 1.0f)))				{	ASSERT(false);	return false;	}
	if(!_btn[BTN_CURE].create(texGuiRef, RectF32::setout(&rect, RECT_CURE), Gcalc::calcTexUv(&uv, UV_CURE, W_TEX, H_TEX, 1.0f)))						{	ASSERT(false);	return false;	}

	// パネルの作成
	_pnlBreak = new Panel();
	if(!_pnlBreak->create())	{	ASSERT(false);	return false;	}
	_pnlBreak->setStateInfo(Panel::STATE_NORMAL, texGuiRef, Gcalc::calcTexUv(&uv, UV_BBTN_N, W_TEX, H_TEX, 1.0f), &col);
	_pnlBreak->setStateInfo(Panel::STATE_HOVER, texGuiRef, Gcalc::calcTexUv(&uv, UV_BBTN_H, W_TEX, H_TEX, 1.0f), &col);

	// 初期化
	_btn[BTN_MENU]._bstat = BSTAT_ON;
	_isEnableSelf = true;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void IngameFront::destroy()
{
	_isEnableSelf = false;

	// パネルの削除
	if(_pnlBreak != 0L)
	{
		_pnlBreak->destroy();
		delete _pnlBreak;
		_pnlBreak = 0L;
	}

	// ボタンの削除
	for(int i = 0; i < NUM_BTN; i++)
	{
		_btn[i].destroy();
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void IngameFront::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!_isEnableOut)	{	return;	}	// 外部からの無効化

	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel(); 

	// 有効性判定
	_isEnableSelf = !gec->isOpenedLockGui() && !gec->isLockOperating();

	// パネルの状態更新
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	PcSpirit* pcsprtLeader = party->pcSpirit(_ipcLeaderCur);
	bool isBehvActingLeader = (pcsprtLeader != 0L) ? pcsprtLeader->isBehaviorActing() : false;
	if(pcsprtLeader != 0L)
	{
		switch(_ipcLeaderCur)
		{
		case MyParty::PC_MY:
			_btn[BTN_SOUMA]._bstat = ((pcsprtLeader->getBehavior() == Spirit::BEHV_SOUMA) && isBehvActingLeader) ? BSTAT_BREAKABLE : BSTAT_ON;
			////_btn[BTN_MEDITATION]._bstat = ((pcsprtLeader->getBehavior() == Spirit::BEHV_MEDITATION) && isBehvActingLeader) ? BSTAT_BREAKABLE : BSTAT_ON;
			break;
		case MyParty::PC_PTN:
			_btn[BTN_GUARDCHARGE]._bstat = ((pcsprtLeader->getBehavior() == Spirit::BEHV_GUARD_CHARGE) && isBehvActingLeader) ? BSTAT_BREAKABLE : BSTAT_ON;
			////_btn[BTN_ATTRACT]._bstat = ((pcsprtLeader->getBehavior() == Spirit::BEHV_ATTRACT) && isBehvActingLeader) ? BSTAT_BREAKABLE : BSTAT_ON;
			break;
		case MyParty::PC_HLP:
			_btn[BTN_CURE]._bstat = ((pcsprtLeader->getBehavior() == Spirit::BEHV_CURE) && isBehvActingLeader) ? BSTAT_BREAKABLE : BSTAT_ON;
			////_btn[BTN_MEDITATION]._bstat = ((pcsprtLeader->getBehavior() == Spirit::BEHV_MEDITATION) && isBehvActingLeader) ? BSTAT_BREAKABLE : BSTAT_ON;
			break;
		}
	}

	// UI 反応
	if(_isEnableSelf)
	{
		PointF32 ptN(S32_MIN, S32_MIN), ptS(S32_MIN, S32_MIN), ptR(S32_MIN, S32_MIN);
		bool isActive = (ui->getTouchingNum() == 1) && !ui->isReacted();	// シングル
		ui->getTouchPosition(&ptN);
		ui->getTouchStartPosition(&ptS);
		bool isAction = ui->isTapRelease(&ptR, &ptS) && (!ui->isDragOperating());
		for(int i = 0; i < NUM_BTN; i++)
		{
			if((_btn[i]._bstat != BSTAT_OFF) && _btn[i]._isEnable)
			{
				Panel::HitExecRes heres = _btn[i]._pnl->hitExec(&ptN, &ptS, &ptR, isActive, isAction && !ui->isReacted());
				if (heres == Panel::HERES_HIT_AND_ACTION)
				{
					Spirit::Behavior behv = Spirit::BEHV_NULL;

					switch(i)
					{
					case BTN_MENU:
						// メニューへモード変更
						gec->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_MENU);
						break;
					case BTN_SOUMA:
						if(_btn[i]._bstat == BSTAT_BREAKABLE)
						{
							behv = Spirit::BEHV_SOUMA;
						}
						else
						{
							// ソウマ入力へモード変更
							if(Game::getGame()->isConfront()){	gec->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_CONFRONT_MAGICTYPING);	}
							else							{	gec->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_WALKING_MAGICTYPING);	}
						}
						break;
					case BTN_MEDITATION:
						behv = Spirit::BEHV_MEDITATION;
						break;
					case BTN_GUARDCHARGE:
						behv = Spirit::BEHV_GUARD_CHARGE;
						break;
					case BTN_ATTRACT:
						behv = Spirit::BEHV_ATTRACT;
						break;
					case BTN_CURE:
						behv = Spirit::BEHV_CURE;
						break;
					}

					// 行動の設定／キャンセル
					if((pcsprtLeader != 0L) && (behv != Spirit::BEHV_NULL))	// 行動がない場合はモード移行などの動作をした
					{
						if(isBehvActingLeader)	{	pcsprtLeader->breakBehavior(behv, true, false);	}
						else					{	pcsprtLeader->startBehavior(behv, 0L);			}
					}

					ui->setReacted(true);	// UI 反応済み
				}
				else if (heres == Panel::HERES_HIT)
				{
					// クリック開始も操作反応とみなす
					ui->setReacted(true);	// UI 反応済み
				}

				// ボタンの定型フレーム処理
				_btn[i]._pnl->exec(res, ec);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void IngameFront::draw(const RenderCtx* rc)
{
	if(!_isEnableOut || !_isEnableSelf)	{	return;	}	// 無効化中

	// 通常アルファ合成
	rc->getRenderer()->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);

	// ボタンの描画
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_btn[i]._isEnable)
		{
			switch(_btn[i]._bstat)
			{
			case BSTAT_ON:
				_btn[i]._pnl->draw(rc);
				break;
			case BSTAT_BREAKABLE:
				_btn[i]._pnl->draw(rc);
				{
					RectF32 rect(_btn[i]._pnl->getRectangle()->x() + (_btn[i]._pnl->getRectangle()->w() - W_BBTN) / 2, _btn[i]._pnl->getRectangle()->y() + (_btn[i]._pnl->getRectangle()->h() - H_BBTN) / 2, W_BBTN, H_BBTN);
					_pnlBreak->setRectangle(&rect);
					_pnlBreak->draw(rc);
				}
				break;
			}
		}
	}
}


/*---------------------------------------------------------------------*//**
	スクリーンリサイズ通知
**//*---------------------------------------------------------------------*/
void IngameFront::notifyResizeView()
{
	RectF32 rect;
	_btn[BTN_MENU]._pnl->setRectangle(RectF32::setout(&rect, RECT_MENU));
	_btn[BTN_SOUMA]._pnl->setRectangle(RectF32::setout(&rect, RECT_SOUMA));
	_btn[BTN_MEDITATION]._pnl->setRectangle(RectF32::setout(&rect, RECT_MEDITATION));
	_btn[BTN_GUARDCHARGE]._pnl->setRectangle(RectF32::setout(&rect, RECT_GUARDCHARGE));
	_btn[BTN_ATTRACT]._pnl->setRectangle(RectF32::setout(&rect, RECT_ATTRACT));
	_btn[BTN_CURE]._pnl->setRectangle(RectF32::setout(&rect, RECT_CURE));
}

/*---------------------------------------------------------------------*//**
	モード変更通知
**//*---------------------------------------------------------------------*/
void IngameFront::notifyChangeMode(s32 gmode)
{
	// 対決時はメニューを無効化
	if(Game::getGame()->isConfront() && _btn[BTN_MENU]._pnl->isEnable())
	{
		_btn[BTN_MENU]._pnl->setEnable(false);
	}
	else if(!Game::getGame()->isConfront() && !_btn[BTN_MENU]._pnl->isEnable())
	{
		_btn[BTN_MENU]._pnl->setEnable(true);
	}
}

/*---------------------------------------------------------------------*//**
	リーダー変更通知
**//*---------------------------------------------------------------------*/
void IngameFront::notifyChangeLeader()
{
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	_ipcLeaderCur = party->getLeaderPcIndex();
	_btn[BTN_SOUMA]._bstat			= (_ipcLeaderCur == MyParty::PC_MY) ? BSTAT_ON : BSTAT_OFF;
	////_btn[BTN_MEDITATION]._bstat		= ((_ipcLeaderCur == MyParty::PC_MY) || (_ipcLeaderCur == MyParty::PC_HLP)) ? BSTAT_ON : BSTAT_OFF;
	_btn[BTN_GUARDCHARGE]._bstat	= (_ipcLeaderCur == MyParty::PC_PTN) ? BSTAT_ON : BSTAT_OFF;
	////_btn[BTN_ATTRACT]._bstat		= (_ipcLeaderCur == MyParty::PC_PTN) ? BSTAT_ON : BSTAT_OFF;
	_btn[BTN_CURE]._bstat			= (_ipcLeaderCur == MyParty::PC_HLP) ? BSTAT_ON : BSTAT_OFF;
}

//==========================================================================
// IngameFront::Button メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
IngameFront::Button::Button()
	: _pnl(0L)
	, _bstat(BSTAT_OFF)
	, _isEnable(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
IngameFront::Button::~Button()
{
	ASSERT(_pnl == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool IngameFront::Button::create(Texture* texGuiRef, const RectF32* rectPanel, const RectF32* uvPanel)
{
	ColorU8 col;

	_pnl = new Panel();
	if(!_pnl->create(rectPanel))
	{
		ASSERT(false);
		return false;
	}
	_pnl->setStateInfo(Panel::STATE_NORMAL, texGuiRef, uvPanel, ColorU8::setout(&col, 255, 255, 255, 211));
	_pnl->setStateInfo(Panel::STATE_HOVER, texGuiRef, uvPanel, ColorU8::setout(&col, 255, 255, 255, 255));

	_isEnable = true;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void IngameFront::Button::destroy()
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
