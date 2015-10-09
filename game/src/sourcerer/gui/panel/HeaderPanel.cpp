/***********************************************************************//**
 *	HeaderPanel.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HeaderPanel.h"

// Friends
#include "../../body/GameExecCtx.h"
#include "../../sound/GameSoundDef.h"

// External
#include "../../../etk/sound/SoundMngr.h"
#include "../../../tfw/gui/PanelText.h"
#include "../../../tfw/lib/Rect.h"
#include "../../../tfw/lib/Color.h"
#include "../../../tfw/lib/TypeUtils.h"
#include "../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define H_HEADER_BAR				48

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ

#define RECT_THIS					0, 0, Game::getGame()->getLogicalWidth(), H_HEADER_BAR
#define UV_THIS						448.0f / W_TEX, 0.0f / H_TEX, 480.0f / W_TEX, 48.0f / H_TEX

#define RECT_LBTN					8, 4, 80, 32
#define UV_LBTN_NRM					808.0f / W_TEX, 696.0f / H_TEX, 80.0f / W_TEX, 32.0f / H_TEX
#define UV_LBTN_HVR					808.0f / W_TEX, 728.0f / H_TEX, 80.0f / W_TEX, 32.0f / H_TEX
#define RECT_RBTN					100, 4, 80, 32
#define UV_RBTN_NRM					672.0f / W_TEX, 320.0f / H_TEX, 80.0f / W_TEX, 32.0f / H_TEX
#define UV_RBTN_HVR					752.0f / W_TEX, 320.0f / H_TEX, 80.0f / W_TEX, 32.0f / H_TEX
//#define RECT_FBTN					Game::getGame()->getLogicalWidth() - 88, 4, 80, 32
#define RECT_FBTN					Game::getGame()->getLogicalWidth() - 96, 4, 80, 32
#define UV_FBTN_NRM					672.0f / W_TEX, 320.0f / H_TEX, 80.0f / W_TEX, 32.0f / H_TEX
#define UV_FBTN_HVR					752.0f / W_TEX, 320.0f / H_TEX, 80.0f / W_TEX, 32.0f / H_TEX

#define COLOR_BAR					55, 118, 132, 255
#define COLOR_BTN_N					72, 148, 163, 255
#define COLOR_BTN_H					80, 163, 191, 255

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	結果値のリセット
**//*---------------------------------------------------------------------*/
void HeaderPanel::resetResult()
{
	_res.reset();
}

/*---------------------------------------------------------------------*//**
	インターフェイス文字列類の変更
**//*---------------------------------------------------------------------*/
void HeaderPanel::changeGuiString(const VcString* title, const VcString* labelLeft, const VcString* labelRight, const VcString* labelFunc)
{
	if(title != 0L)			{	getPanelText()->setText(title, false);							}
	if(labelLeft != 0L)		{	_pnlBtn[BTN_LEFT]->getPanelText()->setText(labelLeft, false);	}
	if(labelRight != 0L)	{	_pnlBtn[BTN_RIGHT]->getPanelText()->setText(labelRight, false);	}
	if(labelFunc != 0L)		{	_pnlBtn[BTN_FUNC]->getPanelText()->setText(labelFunc, false);	}
}

/*---------------------------------------------------------------------*//**
	画面解像度変更に対応する
**//*---------------------------------------------------------------------*/
void HeaderPanel::complyChangeScreenSize()
{
	// 現在の画面解像度における位置調整
	RectF32 rect(RECT_THIS);
	setRectangle(&rect);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HeaderPanel::HeaderPanel()
	: _hpflags(0)
{
	for(int i = 0; i < NUM_BTN; i++)
	{
		_pnlBtn[i] = 0;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HeaderPanel::~HeaderPanel()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_BTN; i++)	{	ASSERT(_pnlBtn[i] == 0L);	}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HeaderPanel::create(Font* fontRef, Texture* texRef, const VcString* title, const VcString* labelLeft, const VcString* labelRight, const VcString* labelFunc, u32 hpflags)
{
	RectF32 rect;
	RectF32 uv;
	ColorU8 col;

	// 値取得
	_hpflags = hpflags;

	// 自身の作成
	RectF32 rectThis(RECT_THIS);
	if(!Panel::create(&rectThis))	{	return false;	}
	setStateInfo(STATE_NORMAL, texRef, RectF32::setout(&uv, UV_THIS));
	createText(fontRef, 16, (title != 0L) ? title : TypeUtils::getEmptyString(), ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
	getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
	getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
	getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);

	// 子パネル作成
	if(labelLeft != 0L)
	{
		_pnlBtn[BTN_LEFT] = new Panel();
		_pnlBtn[BTN_LEFT]->create(RectF32::setout(&rect, RECT_LBTN));
		_pnlBtn[BTN_LEFT]->setStateInfo(STATE_NORMAL, texRef, RectF32::setout(&uv, UV_LBTN_NRM));
		_pnlBtn[BTN_LEFT]->setStateInfo(STATE_HOVER, texRef, RectF32::setout(&uv, UV_LBTN_HVR));
		_pnlBtn[BTN_LEFT]->createText(fontRef, 12, labelLeft, ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
		_pnlBtn[BTN_LEFT]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
		_pnlBtn[BTN_LEFT]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_pnlBtn[BTN_LEFT]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
		_pnlBtn[BTN_LEFT]->setHorizontalHitMargin(8);		// ヒット範囲幅を広めに
		_pnlBtn[BTN_LEFT]->setVerticalHitMargin(8);			// ヒット範囲高さを広めに
	}
	if(labelRight != 0L)
	{
		_pnlBtn[BTN_RIGHT] = new Panel();
		_pnlBtn[BTN_RIGHT]->create(RectF32::setout(&rect, RECT_RBTN));
		_pnlBtn[BTN_RIGHT]->setStateInfo(STATE_NORMAL, texRef, RectF32::setout(&uv, UV_RBTN_NRM));
		_pnlBtn[BTN_RIGHT]->setStateInfo(STATE_HOVER, texRef, RectF32::setout(&uv, UV_RBTN_HVR));
		_pnlBtn[BTN_RIGHT]->createText(fontRef, 12, labelRight, ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
		_pnlBtn[BTN_RIGHT]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
		_pnlBtn[BTN_RIGHT]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_pnlBtn[BTN_RIGHT]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
		_pnlBtn[BTN_RIGHT]->setHorizontalHitMargin(8);		// ヒット範囲幅を広めに
		_pnlBtn[BTN_RIGHT]->setVerticalHitMargin(8);		// ヒット範囲高さを広めに
	}
	if(labelFunc != 0L)
	{
		_pnlBtn[BTN_FUNC] = new Panel();
		_pnlBtn[BTN_FUNC]->create(RectF32::setout(&rect, RECT_FBTN));
		_pnlBtn[BTN_FUNC]->setStateInfo(STATE_NORMAL, texRef, RectF32::setout(&uv, UV_FBTN_NRM));
		_pnlBtn[BTN_FUNC]->setStateInfo(STATE_HOVER, texRef, RectF32::setout(&uv, UV_FBTN_HVR));
		_pnlBtn[BTN_FUNC]->createText(fontRef, 12, labelFunc, ColorU8::setout(&col, 255, 255, 255, 255), 0, 0, 3, 0, false, false, false);
		_pnlBtn[BTN_FUNC]->getPanelText()->setAlign(PanelText::F_ALIGN_CENTER);
		_pnlBtn[BTN_FUNC]->getPanelText()->setValign(PanelText::F_VALIGN_MIDDLE);
		_pnlBtn[BTN_FUNC]->getPanelText()->setFlags(PanelText::F_DRAW_TOP_SHADOW, true);
		_pnlBtn[BTN_FUNC]->setHorizontalHitMargin(8);		// ヒット範囲幅を広めに
		_pnlBtn[BTN_FUNC]->setVerticalHitMargin(8);			// ヒット範囲高さを広めに
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void HeaderPanel::destroy()
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

	Panel::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void HeaderPanel::exec(ExecRes* res, const ExecCtx* ec)
{
	Panel::exec(res, ec);

	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel();

	// 子パネル制御
	PointF32 ptTouch(S32_MIN, S32_MIN);
	PointF32 ptClick(S32_MIN, S32_MIN);
	bool isTouch = !ui->isReacted() ? ui->getTouchPosition(&ptTouch) : false;
	bool isClick = !ui->isReacted() ? ui->isTapRelease(&ptClick, 0L) : false;	///ui->isClickRelease(&ptClick)
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] == 0)	{	continue;	}
		if(isClick)
		{
			if(_pnlBtn[i]->hit(&ptClick, true))
			{
				switch(i)
				{
				case BTN_LEFT:
					_res.setFlags(Res::F_CLICK_L, true);
					Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_OK, false);
					break;
				case BTN_RIGHT:
					_res.setFlags(Res::F_CLICK_R, true);
					Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_OK, false);
					break;
				case BTN_FUNC:
					_res.setFlags(Res::F_CLICK_F, true);
					Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_OK, false);
					break;
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

	// ボディのタップ判定
	if(!ui->isReacted() && isClick)
	{
		if(getRectangle()->isPointIn(&ptClick))
		{
			_res.setFlags(Res::F_CLICK_BODY, true);
		}
	}
	if(!ui->isReacted() && isTouch)
	{
		if(getRectangle()->isPointIn(&ptTouch))
		{
			_res.setFlags(Res::F_TOUCH_BODY, true);
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void HeaderPanel::draw(const RenderCtx* rc)
{
	Panel::draw(rc);

	// 子パネル描画
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0)	{	_pnlBtn[i]->draw(rc);	}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
