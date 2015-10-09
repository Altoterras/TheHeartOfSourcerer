/***********************************************************************//**
 *	PerformanceMeter.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/06/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COMMON_PERFORMANCE_METER_H_
#define _TFW_COMMON_PERFORMANCE_METER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class RenderCtx;

/*---------------------------------------------------------------------*//**
 *	パフォーマンスメーター
 *
**//*---------------------------------------------------------------------*/
class PerformanceMeter
{
	//======================================================================
	// クラス
private:
	class CheckPoint
	{
	public:
		f32		_time;
		u32		_color1;
		u32		_color2;
		CheckPoint() : _time(0.0f), _color1(0), _color2(0) {}
	};

	//======================================================================
	// メソッド
public:
	PerformanceMeter(s32 numCheckPoint, f32 spfRender);
	~PerformanceMeter();
	void startFrame();
	void pushCheckPoint(f32 time, u32 colorCategory, u32 colorDetail);
	void render(const RenderCtx* rc);

	//======================================================================
	// 変数
private:
	CheckPoint*	_cparrCheckPoint;
	s32			_numCheckPoint;
	f32			_spfRender;

	s32			_idxCheckPointCount;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_COMMON_PERFORMANCE_METER_H_

