/***********************************************************************//**
 *	ActStat.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/19.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ActStat.h"

// Friends
#include "TransStat.h"
#include "../common/CalcUtils.h"

// External
#include "../../tfw/framemod/RenderCtx.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Rect.h"

// Library


////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ActStat 静的変数

const f32 ActStat::FRAME_STOP = -1.0f;

//==========================================================================
// ActStat メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	設定
**//*---------------------------------------------------------------------*/
void ActStat::setup(f32 frameMax, f32 frameRandDifMax, f32 frameFirstAdv)
{
	_frameMax = frameMax;
	_frameMax += frameRandDifMax * CalcUtils::randF();

	_frameNext = frameMax - frameFirstAdv;
	if(_frameNext < 0.0f)	{	_frameNext = 0.0f;	}
}

/*---------------------------------------------------------------------*//**
	フレームを進める

	@retval	true	フレームがゼロになり、次のアクションを実行できる
	@retval false	残りフレームが存在し、次のアクションは実行できない
**//*---------------------------------------------------------------------*/
bool ActStat::advanceFrame(f32 frameDelta)
{
	if(_frameNext == 0.0f)				// アクションが起こった直後
	{
	}
	else if(_frameNext == FRAME_STOP)	// 停止中
	{
	}
	else								// 動作中
	{
		_frameNext -= frameDelta;
		if(_frameNext < 0.0f)
		{
			_frameNext = 0.0f;
		}
	}

	return _frameNext == 0.0f;
}

/*---------------------------------------------------------------------*//**
	停止状態へ移行
**//*---------------------------------------------------------------------*/
void ActStat::stopFrame()
{
	_frameNext = FRAME_STOP;
}

/*---------------------------------------------------------------------*//**
	フレームカウント再開
**//*---------------------------------------------------------------------*/
void ActStat::restartFrame()
{
	_frameNext = _frameMax;
}

/*---------------------------------------------------------------------*//**
	ステータス描画
**//*---------------------------------------------------------------------*/
void ActStat::draw(const RenderCtx* rc, f32 x, f32 y, f32 w) const
{
	Renderer* rdr = rc->getRenderer();
	f32 rateMax = w / (f32)_frameMax;
	f32 wRemain = _frameNext * rateMax;
	// 地
	{
		RectF32 vtx(x, y, w, 2.0f);
		ColorU8 col1(191, 255, 255, 241);
		ColorU8 col2(188, 255, 255, 241);
		ColorU8 col3(127, 163, 255, 241);
		ColorU8 col4(127, 147, 255, 241);
		RendererUtils::draw2dColorRect(rdr, &vtx, &col1, &col2, &col3, &col4);
	}
	// 残りゲージ
	{
		RectF32 vtx(x, y, wRemain, 2.0f);
		ColorU8 col1(0, 0, 255, 255);
		ColorU8 col2(0, 127, 255, 255);
		ColorU8 col3(191, 127, 255, 255);
		ColorU8 col4(127, 127, 255, 255);
		RendererUtils::draw2dColorRect(rdr, &vtx, &col1, &col2, &col3, &col4);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
