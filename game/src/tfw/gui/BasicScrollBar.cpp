/***********************************************************************//**
 *  BasicScrollBar.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2010/03/05.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BasicScrollBar.h"

// Friends
#include "../gcmn/Renderer.h"
#include "../gcmn/RendererUtils.h"
#include "../gcmn/Texture.h"
#include "../lib/Point.h"
#include "../lib/Rect.h"
#include "../lib/Color.h"
#include "../ui/TouchPanel.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BasicScrollBar メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	表示されているかどうかを得る
**//*---------------------------------------------------------------------*/
bool BasicScrollBar::isShow() const
{
	if(_rectThis == 0L)		{	return false;	}	// 未作成
	if(TFW_IS_FLAG(_eflags, EF_DISABLED))	{	return false;	}	// 無効中
	if(TFW_IS_FLAG(_eflags, EF_HIDE) && !TFW_IS_FLAG(_cflags, CF_IGNORE_HIDE))	{	return false;	}	// 隠し中
	return true;
}

/*---------------------------------------------------------------------*//**
	矩形設定
	※独自に矩形を設定したい場合はオーバーライドすること
**//*---------------------------------------------------------------------*/
void BasicScrollBar::setRectangle(const RectF32* rect)
{
	_rectThis->copy(rect);

	if(isHorizontal())
	{
		f32 wArw = _rectThis->h();
		_rectUpArw->set(_rectThis->x(), _rectThis->y(), wArw, _rectThis->h());
		_rectDownArw->set(_rectThis->x() + _rectThis->w() - wArw, _rectThis->y(), wArw, _rectThis->h());
	}
	else
	{
		f32 hArw = _rectThis->w();
		_rectUpArw->set(_rectThis->x(), _rectThis->y(), _rectThis->w(), hArw);
		_rectDownArw->set(_rectThis->x(), _rectThis->y() + _rectThis->h() - hArw, _rectThis->w(), hArw);
	}
	setKnobRectangle();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ヒットテスト
**//*---------------------------------------------------------------------*/
bool BasicScrollBar::hitTest(const PointF32* pt)
{
	if(TFW_IS_FLAG(_eflags, EF_DISABLED))	{	return false;	}
	return _rectThis->isPointIn(pt);
}

/*---------------------------------------------------------------------*//**
	最大値を設定
**//*---------------------------------------------------------------------*/
void BasicScrollBar::setScrollMinMax(f32 valMin, f32 valMax)
{
	_valMin = valMin;
	_valMax = (valMin < valMax) ? valMax : valMin;
	setKnobRectangle();
}

/*---------------------------------------------------------------------*//**
	現在値を設定
**//*---------------------------------------------------------------------*/
void BasicScrollBar::setScrollPos(f32 valCur)
{
	_valCur = valCur;
	setKnobRectangle();
}

/*---------------------------------------------------------------------*//**
	ページサイズを設定
**//*---------------------------------------------------------------------*/
void BasicScrollBar::setScrollPage(f32 valPage)
{
	_valPage = valPage;
	setKnobRectangle();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BasicScrollBar::BasicScrollBar()
	: _cflags(0)
	, _eflags(0)
	, _valMin(0)
	, _valMax(0)
	, _valCur(0)
	, _valPage(0)
	, _valDragStart(0)
	, _rectThis(0L)
	, _rectUpArw(0L)
	, _rectDownArw(0L)
	, _rectKnob(0L)
	, _pxDragStart(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BasicScrollBar::~BasicScrollBar()
{
	ASSERT(_rectThis == 0L);
	ASSERT(_rectUpArw == 0L);
	ASSERT(_rectDownArw == 0L);
	ASSERT(_rectKnob == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool BasicScrollBar::create(const RectF32* rect)
{
	// 矩形を作成
	ASSERT(_rectThis == 0L);	// 未作成のはず
	_rectThis = new RectF32();
	_rectUpArw = new RectF32();
	_rectDownArw = new RectF32();
	_rectKnob = new RectF32();
	setRectangle(rect);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void BasicScrollBar::destroy()
{
	TFW_SET_FLAG(_eflags, EF_DISABLED, true);	// 無効化

	// 矩形を削除
	delete _rectKnob;
	_rectKnob = 0L;
	delete _rectDownArw;
	_rectDownArw = 0L;
	delete _rectUpArw;
	_rectUpArw = 0L;
	delete _rectThis;
	_rectThis = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void BasicScrollBar::exec(ExecRes* res, TouchPanel* ui)
{
	if(_rectThis == 0L)		{	return;	}	// 未作成
	if(TFW_IS_FLAG(_eflags, EF_DISABLED))	{	return;	}	// 無効中
	if(TFW_IS_FLAG(_eflags, EF_HIDE) && !TFW_IS_FLAG(_cflags, CF_IGNORE_HIDE))		{	return;	}	// 隠し中

	ScrollBarExecRes* cres = (ScrollBarExecRes*)res;
	cres->setAction(ScrollBarExecRes::ACT_NULL);

	bool isForceReacted = false;
	if(!TFW_IS_FLAG(_eflags, EF_CAPTURE))	// ドラッグ中で無い場合
	{
		PointF32 ptCur, ptPrev, ptStart;
		if(ui->isTapTrigger(&ptCur) && _rectKnob->isPointIn(&ptCur))
		{
			execStartCapture(&ptCur);	// キャプチャ開始
		}
		else if(ui->isTapRepeat(&ptCur, &ptStart))
		{
			if((_valCur > _valMin) && _rectUpArw->isPointIn(&ptCur))
			{
				cres->setAction(ScrollBarExecRes::ACT_CLICK_UP_ARROW);
			}
			else if((_valCur < _valMax) && _rectDownArw->isPointIn(&ptCur))
			{
				cres->setAction(ScrollBarExecRes::ACT_CLICK_DOWN_ARROW);
			}
			else if(!_rectKnob->isPointIn(&ptCur) && _rectThis->isPointIn(&ptCur))
			{
				if(isHorizontal() ? ptCur.x() < _rectKnob->x() : ptCur.y() < _rectKnob->y())
				{
					cres->setAction(ScrollBarExecRes::ACT_CLICK_PAGE_UP);
				}
				else
				{
					cres->setAction(ScrollBarExecRes::ACT_CLICK_PAGE_DOWN);
				}
			}
		}
	}
	if(cres->getAction() == ScrollBarExecRes::ACT_NULL)	// 上記で未反応の場合
	{
		switch(ui->getLastAction())
		{
		case TouchPanel::ACT_CLICK:
			{
				PointF32 ptCur;
				ui->getClickParam(&ptCur, 0L, 0L);
				if(_rectThis->isPointIn(&ptCur))
				{
					isForceReacted = true;
				}
			}
			break;
		case TouchPanel::ACT_DRAG:
			{
				PointF32 ptCur, ptPrev, ptStart;
				ui->getDragParam(&ptStart, &ptCur, &ptPrev);
				if(TFW_IS_FLAG(_eflags, EF_CAPTURE) || _rectKnob->isPointIn(&ptStart))
				{
					f32 pxDragCur, pxKnobMoveMax;
					if(isHorizontal())
					{
						pxDragCur = ptCur.x();
						pxKnobMoveMax = _rectDownArw->left() - _rectUpArw->right() - _rectKnob->w();
					}
					else
					{
						pxDragCur = ptCur.y();
						pxKnobMoveMax = _rectDownArw->top() - _rectUpArw->bottom() - _rectKnob->h();
					}
					f32 valCur = ((pxDragCur - _pxDragStart) / pxKnobMoveMax * (_valMax - _valMin)) + _valDragStart;
					if(valCur < _valMin)	{	valCur = _valMin;	}
					if(valCur > _valMax)	{	valCur = _valMax;	}
					cres->setAction(ScrollBarExecRes::ACT_DRAG_KNOB);
					cres->setValue(valCur);
				}
			}
			break;
		case TouchPanel::ACT_FLIP:
			{
				PointF32 ptCur, ptStart;
				f32 speed;
				ui->getFlipParam(&ptStart, &speed, &ptCur, 0L);
				if(_rectKnob->isPointIn(&ptStart))
				{
					if(isHorizontal() ? ptCur.x() < ptStart.x() : ptCur.y() < ptStart.y())
					{
						cres->setAction(ScrollBarExecRes::ACT_FLIP_UP);
						cres->setValue(- speed);
					}
					else if(isHorizontal() ? ptCur.x() > ptStart.x() : ptCur.y() > ptStart.y())
					{
						cres->setAction(ScrollBarExecRes::ACT_FLIP_DOWN);
						cres->setValue(speed);
					}
				}
			}
			break;
		case TouchPanel::ACT_ZOOM:
			{
				f32 rCur, rPrev;
				ui->getZoomParam(0L, 0L, &rCur, &rPrev);
				if(rCur > rPrev)
				{
					cres->setAction(ScrollBarExecRes::ACT_ZOOM_IN);
					cres->setValue(rCur - rPrev);
				}
				else if(rCur < rPrev)
				{
					cres->setAction(ScrollBarExecRes::ACT_ZOOM_OUT);
					cres->setValue(rPrev - rCur);
				}
			}
			break;
		default:
			if(!ui->isTouching())	// タッチなし
			{
				if(TFW_IS_FLAG(_eflags, EF_CAPTURE))
				{
					TFW_SET_FLAG(_eflags, EF_CAPTURE, false);
				}
			}
			break;
		}
	}
	///STRACE("## act=%d, cap=%d, val=%d ##", ui->getLastAction(), TFW_IS_FLAG(_eflags, EF_CAPTURE), res->_value);

	if((cres->getAction() != ScrollBarExecRes::ACT_NULL) || isForceReacted)
	{
		ui->setReacted(true);
	}
}

/*---------------------------------------------------------------------*//**
	キャプチャスタート
**//*---------------------------------------------------------------------*/
void BasicScrollBar::execStartCapture(const PointF32* pt)
{
	if(!TFW_IS_FLAG(_eflags, EF_CAPTURE))
	{
		TFW_SET_FLAG(_eflags, EF_CAPTURE, true);
		if(isHorizontal())
		{
			_pxDragStart = pt->x();
			_valDragStart = _valCur;
		}
		else
		{
			_pxDragStart = pt->y();
			_valDragStart = _valCur;
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void BasicScrollBar::draw(const RenderCtx* rc)
{
	if(_rectThis == 0L)		{	return;	}	// 未作成
	if(TFW_IS_FLAG(_eflags, EF_DISABLED))	{	return;	}	// 無効中
	if(TFW_IS_FLAG(_eflags, EF_HIDE) && !TFW_IS_FLAG(_cflags, CF_IGNORE_HIDE))	{	return;	}	// 隠し中

	drawBackground(rc, _rectThis);
	drawKnob(rc, _rectKnob);
	drawUpButton(rc, _rectUpArw, _valCur > _valMin);
	drawDownButton(rc, _rectDownArw, _valCur < _valMax);
}

/*---------------------------------------------------------------------*//**
	背景を描画する
**//*---------------------------------------------------------------------*/
void BasicScrollBar::drawBackground(const RenderCtx* rc, const RectF32* rect)
{
	rc->getRenderer()->setSolidColor(0, 0, 0, 191);
	RendererUtils::draw2dRect(rc->getRenderer(), rect);
}

/*---------------------------------------------------------------------*//**
	ノブを描画する
**//*---------------------------------------------------------------------*/
void BasicScrollBar::drawKnob(const RenderCtx* rc, const RectF32* rect)
{
	rc->getRenderer()->setSolidColor(255, 255, 255, 191);
	RendererUtils::draw2dRect(rc->getRenderer(), _rectKnob);
}

/*---------------------------------------------------------------------*//**
	上ボタンを描画する
**//*---------------------------------------------------------------------*/
void BasicScrollBar::drawUpButton(const RenderCtx* rc, const RectF32* rect, bool isEnabled)
{
	if(isEnabled)
	{
		RendererUtils::draw2dRect(rc->getRenderer(), rect);
	}
	else
	{
		rc->getRenderer()->setSolidColor(255, 255, 255, 63);
		RendererUtils::draw2dRect(rc->getRenderer(), rect);
		rc->getRenderer()->setSolidColor(255, 255, 255, 191);
	}
}

/*---------------------------------------------------------------------*//**
	下ボタンを描画する
**//*---------------------------------------------------------------------*/
void BasicScrollBar::drawDownButton(const RenderCtx* rc, const RectF32* rect, bool isEnabled)
{
	if(isEnabled)
	{
		RendererUtils::draw2dRect(rc->getRenderer(), rect);
	}
	else
	{
		rc->getRenderer()->setSolidColor(255, 255, 255, 63);
		RendererUtils::draw2dRect(rc->getRenderer(), rect);
	}
}

/*---------------------------------------------------------------------*//**
	ノブの矩形を設定する
**//*---------------------------------------------------------------------*/
void BasicScrollBar::setKnobRectangle()
{
	f32 pxArw = isHorizontal() ? _rectThis->h() : _rectThis->w();
	f32 pxMax = isHorizontal() ? _rectThis->w() - (pxArw * 2) : _rectThis->h() - (pxArw * 2);

	if(_valMax <= _valMin)
	{
		TFW_SET_FLAG(_eflags, EF_HIDE, true);
		if(isHorizontal())	{	_rectKnob->set(_rectThis->x() + pxArw, _rectThis->y(), pxMax, _rectThis->w());			}
		else				{	_rectKnob->set(_rectThis->x(), _rectThis->y() + pxArw, _rectThis->w(), pxMax);			}
	}
	else
	{
		TFW_SET_FLAG(_eflags, EF_HIDE, false);
		f32 sPage = (_valPage / (f32)(_valPage + _valMax - _valMin)) * pxMax;
		f32 sValidVal = pxMax - sPage;
		f32 sCur = (_valCur / (f32)(_valMax - _valMin)) * sValidVal;
		if(isHorizontal())	{	_rectKnob->set(_rectThis->x() + pxArw + sCur, _rectThis->y(), sPage, _rectThis->w());	}
		else				{	_rectKnob->set(_rectThis->x(), _rectThis->y() + pxArw + sCur, _rectThis->w(), sPage);	}
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
