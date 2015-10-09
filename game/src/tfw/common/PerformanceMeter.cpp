/***********************************************************************//**
 *	PerformanceMeter.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/06/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PerformanceMeter.h"

// Friends
#include "../framemod/RenderCtx.h"
#include "../gcmn/Renderer.h"
#include "../gcmn/RendererUtils.h"
#include "../lib/Color.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ


////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PerformanceMeter メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PerformanceMeter::PerformanceMeter(s32 numCheckPoint, f32 spfRender) :
	_cparrCheckPoint(0L),
	_numCheckPoint(numCheckPoint),
	_spfRender(spfRender),
	_idxCheckPointCount(0)
{
	_cparrCheckPoint = new CheckPoint[_numCheckPoint];
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PerformanceMeter::~PerformanceMeter()
{
	delete[] _cparrCheckPoint;
	_cparrCheckPoint = 0L;
	_numCheckPoint = 0;
}

/*---------------------------------------------------------------------*//**
	フレームスタート
**//*---------------------------------------------------------------------*/
void PerformanceMeter::startFrame()
{
	_idxCheckPointCount = 0;
	pushCheckPoint(0, 0x000000, 0x000000);
}

/*---------------------------------------------------------------------*//**
	計測
**//*---------------------------------------------------------------------*/
void PerformanceMeter::pushCheckPoint(f32 time, u32 colorCategory, u32 colorDetail)
{
	ASSERT(_idxCheckPointCount < _numCheckPoint);
	if(_idxCheckPointCount >= _numCheckPoint)	{	return;	}
	_cparrCheckPoint[_idxCheckPointCount]._time = time;
	_cparrCheckPoint[_idxCheckPointCount]._color1 = colorCategory;
	_cparrCheckPoint[_idxCheckPointCount]._color2 = colorDetail;
	_idxCheckPointCount++;
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void PerformanceMeter::render(const RenderCtx* rc)
{
	if(_numCheckPoint <= 0)	{	return;	}

	Renderer* rdr = rc->getRenderer();

	rdr->setZTest(false);
	rdr->setCulling(false);

	const f32 xBase = 30.0f;

	// 基点を描く
	rdr->setSolidColor(255, 255, 255, 255);
	RendererUtils::draw2dLine(rdr, xBase, 5.0f, xBase, 13.0f);

	// １フレームライン、x=100 が １フレームとして描く
	const f32 xOneFrame = 100.0f;
	rdr->setSolidColor(255, 255, 255, 255);
	RendererUtils::draw2dLine(rdr, xBase + xOneFrame, 5.0f, xBase + xOneFrame, 13.0f);

	// メーターを矩形として描く
	f32 x = xBase;
	f32 timePrev = _cparrCheckPoint[0]._time;
	const f32 xTimeToFrameLine = xOneFrame * _spfRender;
	for(int i = 1; i < _idxCheckPointCount; i++)
	{
		const CheckPoint& cp = _cparrCheckPoint[i];
		f32 w = (cp._time - timePrev) * xTimeToFrameLine;
		if(w > 0.0f)
		{
			ColorU8 col1((u8)((cp._color1 >> 16) & 0xff), (u8)((cp._color1 >> 8) & 0xff), (u8)((cp._color1 >> 0) & 0xff), 0xff);
			ColorU8 col2((u8)((cp._color2 >> 16) & 0xff), (u8)((cp._color2 >> 8) & 0xff), (u8)((cp._color2 >> 0) & 0xff), 0xff);
			RendererUtils::draw2dColorRect(rdr, x, 7.0f, w, 3.0f, &col1);
			RendererUtils::draw2dColorRect(rdr, x, 10.0f, w, 1.0f, &col2);
			x += w;
		}
		timePrev = cp._time;
	}

}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
