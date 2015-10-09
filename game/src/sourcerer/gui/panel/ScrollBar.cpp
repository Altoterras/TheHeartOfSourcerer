/***********************************************************************//**
 *  ScrollBar.cpp
 *  Enlight Game Application
 *
 *  Created by Ryoutarou Kishi on 2010/07/05.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ScrollBar.h"

// Friends
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/gcmn/RendererUtils.h"
#include "../../../tfw/gcmn/Texture.h"
#include "../../../tfw/lib/Point.h"
#include "../../../tfw/lib/Rect.h"

// External

// Library

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define TEX_W						(1024.0f)					// テクスチャ全体の幅
#define TEX_H						(1024.0f)					// テクスチャ全体の高さ

#define W_BTN						32	// 矢印ボタンの幅
#define H_BTN						32	// 矢印ボタンの高さ
#define UV_BG						968 / TEX_W, 320 / TEX_H, 32 / TEX_W, 256 / TEX_H
#define UV_BG_UBTN					968 / TEX_W, 320 / TEX_H, 32 / TEX_W, 32 / TEX_H
#define UV_BG_L						968 / TEX_W, 352 / TEX_H, 32 / TEX_W, 192 / TEX_H
#define UV_BG_DBTN					968 / TEX_W, 544 / TEX_H, 32 / TEX_W, 32 / TEX_H
#define UV_UBTN_HVR					992 / TEX_W, 256 / TEX_H, 32 / TEX_W, 32 / TEX_H
#define UV_DBTN_HVR					992 / TEX_W, 288 / TEX_H, 32 / TEX_W, 32 / TEX_H
#define H_KNOB_T					8
#define H_KNOB_B					8
#define H_KNOB_M					16
#define UV_KNOB_T_NRM				928 / TEX_W, 224 / TEX_H, 19 / TEX_W, H_KNOB_T / TEX_H
#define UV_KNOB_L_NRM				928 / TEX_W, 232 / TEX_H, 19 / TEX_W, 8 / TEX_H
#define UV_KNOB_M_NRM				928 / TEX_W, 240 / TEX_H, 19 / TEX_W, 16 / TEX_H
#define UV_KNOB_B_NRM				928 / TEX_W, 256 / TEX_H, 19 / TEX_W, 8 / TEX_H
#define UV_KNOB_T_HVR				952 / TEX_W, 224 / TEX_H, 19 / TEX_W, H_KNOB_T / TEX_H
#define UV_KNOB_L_HVR				952 / TEX_W, 232 / TEX_H, 19 / TEX_W, 8 / TEX_H
#define UV_KNOB_M_HVR				952 / TEX_W, 240 / TEX_H, 19 / TEX_W, 16 / TEX_H
#define UV_KNOB_B_HVR				952 / TEX_W, 256 / TEX_H, 19 / TEX_W, 8 / TEX_H

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ScrollBar メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	矩形設定
**//*---------------------------------------------------------------------*/
#if 0
void ScrollBar::setRectangle(const RectF32* rect)
{
	_rectThis->set(rect);

	ASSERT(!isHorizontal());	// 縦型のみサポート
	_rectUpArw->set(_rectThis->x(), _rectThis->y(), _rectThis->w(), H_BTN);
	_rectDownArw->set(_rectThis->x(), _rectThis->y() + _rectThis->h() - H_BTN, _rectThis->w(), H_BTN);

	setKnobRectangle();
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ScrollBar::ScrollBar()
	: _texRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ScrollBar::~ScrollBar()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ScrollBar::create(const RectF32* rect, Texture* texRef)
{
	if(!BasicScrollBar::create(rect))	{	return false;	}

	_texRef = texRef;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ScrollBar::destroy()
{

	BasicScrollBar::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ScrollBar::exec(ExecRes* res, TouchPanel* ui)
{
	BasicScrollBar::exec(res, ui);
}

/*---------------------------------------------------------------------*//**
	背景を描画する
**//*---------------------------------------------------------------------*/
void ScrollBar::drawBackground(const RenderCtx* rc, const RectF32* rect)
{
	if(_texRef != 0L)
	{
		rc->getRenderer()->bindTexture(_texRef);
		rc->getRenderer()->setSolidColor(255, 255, 255, 255);

		RectF32 uv;

		if(rect->h() <= (H_BTN * 2))
		{
			RectF32 rectWk(rect->x(), rect->y(), rect->w(), rect->h() / 2);
			RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_BG_UBTN));
			rectWk.set(rect->x(), rect->y() + (rect->h() / 2), rect->w(), rect->h() / 2);
			RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_BG_DBTN));
		}
		else
		{
			RectF32 rectWk(rect->x(), rect->y(), rect->w(), H_BTN);
			RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_BG_UBTN));
			rectWk.set(rect->x(), rect->y() + H_BTN, rect->w(), rect->h() - (H_BTN * 2));
			RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_BG_L));
			rectWk.set(rect->x(), rect->y() + rect->h() - H_BTN, rect->w(), H_BTN);
			RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_BG_DBTN));
		}
	}
	else
	{
		BasicScrollBar::drawBackground(rc, rect);
	}
}

/*---------------------------------------------------------------------*//**
	ノブを描画する
**//*---------------------------------------------------------------------*/
void ScrollBar::drawKnob(const RenderCtx* rc, const RectF32* rect)
{
	bool isEnabled = false;	// 仮

	if(_texRef != 0L)
	{
		rc->getRenderer()->bindTexture(_texRef);
		rc->getRenderer()->setSolidColor(255, 255, 255, 255);

		RectF32 uv;

		if(rect->h() <= (H_KNOB_T + H_KNOB_B))
		{
			RectF32 rectWk(rect->x(), rect->y(), rect->w(), rect->h() / 2);
			if(isEnabled)	{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_T_HVR));	}
			else			{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_T_NRM));	}
			rectWk.set(rect->x(), rect->y() + (rect->h() / 2), rect->w(), rect->h() / 2);
			if(isEnabled)	{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_B_HVR));	}
			else			{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_B_NRM));	}
		}
		else
		{
			RectF32 rectWk(rect->x(), rect->y(), rect->w(), H_KNOB_T);
			if(isEnabled)	{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_T_HVR));	}
			else			{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_T_NRM));	}
			rectWk.set(rect->x(), rect->y() + H_KNOB_T, rect->w(), rect->h() - (H_KNOB_T + H_KNOB_B));
			if(isEnabled)	{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_L_HVR));	}
			else			{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_L_NRM));	}
			rectWk.set(rect->x(), rect->y() + ((rect->h() / 2) - (H_KNOB_M / 2)), rect->w(), H_KNOB_M);
			if(isEnabled)	{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_M_HVR));	}
			else			{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_M_NRM));	}
			rectWk.set(rect->x(), rect->y() + (rect->h() - H_KNOB_B), rect->w(), H_KNOB_B);
			if(isEnabled)	{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_B_HVR));	}
			else			{	RendererUtils::draw2dTextureRect(rc->getRenderer(), &rectWk, RectF32::setout(&uv, UV_KNOB_B_NRM));	}
		}
	}
	else
	{
		BasicScrollBar::drawKnob(rc, rect);
	}
}

/*---------------------------------------------------------------------*//**
	上ボタンを描画する
**//*---------------------------------------------------------------------*/
void ScrollBar::drawUpButton(const RenderCtx* rc, const RectF32* rect, bool isEnabled)
{
	if(_texRef != 0L)
	{
		rc->getRenderer()->bindTexture(_texRef);
		rc->getRenderer()->setSolidColor(255, 255, 255, 255);
		if(isEnabled)
		{
			RectF32 uv;
			RendererUtils::draw2dTextureRect(rc->getRenderer(), rect, RectF32::setout(&uv, UV_UBTN_HVR));
		}
		else
		{
		}
	}
	else
	{
		BasicScrollBar::drawUpButton(rc, rect, isEnabled);
	}
}

/*---------------------------------------------------------------------*//**
	下ボタンを描画する
**//*---------------------------------------------------------------------*/
void ScrollBar::drawDownButton(const RenderCtx* rc, const RectF32* rect, bool isEnabled)
{
	if(_texRef != 0L)
	{
		rc->getRenderer()->bindTexture(_texRef);
		rc->getRenderer()->setSolidColor(255, 255, 255, 255);
		if(isEnabled)
		{
			RectF32 uv;
			RendererUtils::draw2dTextureRect(rc->getRenderer(), rect, RectF32::setout(&uv, UV_DBTN_HVR));
		}
		else
		{
		}
	}
	else
	{
		BasicScrollBar::drawDownButton(rc, rect, isEnabled);
	}
}

/*---------------------------------------------------------------------*//**
	ノブの矩形を設定する
**//*---------------------------------------------------------------------*/
void ScrollBar::setKnobRectangle()
{
	ASSERT(!isHorizontal());	// 縦型のみサポート

	f32 pxArw = W_BTN;
	f32 pxMax = _rectThis->h() - (pxArw * 2);
	#define W_PADDING	(6)

	if(_valMax <= _valMin)
	{
		TFW_SET_FLAG(_eflags, EF_HIDE, true);
		_rectKnob->set(_rectThis->x() + W_PADDING, _rectThis->y() + pxArw, _rectThis->w() - (W_PADDING * 2), pxMax);
	}
	else
	{
		TFW_SET_FLAG(_eflags, EF_HIDE, false);
		f32 sPage = (_valPage / (_valPage + _valMax - _valMin)) * pxMax;
		f32 sValidVal = pxMax - sPage;
		f32 sCur = (_valCur / (_valMax - _valMin)) * sValidVal;
		_rectKnob->set(_rectThis->x() + W_PADDING, _rectThis->y() + pxArw + sCur, _rectThis->w() - (W_PADDING * 2), sPage);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
