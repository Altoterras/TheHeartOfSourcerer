/***********************************************************************//**
 *	ActStat.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/19.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_ACT_STAT_H_
#define _SRCR_STATUS_ACT_STAT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class RenderCtx;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	行動ステータス
 *
**//*---------------------------------------------------------------------*/
class ActStat
{
	//======================================================================
	// 定数
private:
	static const f32		FRAME_STOP;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 次のアクションまでの現在の残りフレーム数を得る
	inline f32				getRemainFrame() const		{	return _frameNext;			}
	// 次のアクションまでの最大フレーム数を得る
	inline f32				getMaxFrame() const			{	return _frameMax;			}
	// アクションを行うフレームかを得る
	inline bool				isActionFrame() const		{	return _frameNext == 0.0f;	}
	// 停止待ち
	inline bool				isStopFrame() const			{	return _frameNext < 0.0f;	}
	// 次のアクションまでの待ち中のフレームかを得る
	inline bool				isWaitFrame() const			{	return _frameNext > 0.0f;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void	setup(f32 frameMax, f32 frameRandDifMax, f32 frameFirstAdv);
	bool	advanceFrame(f32 frameDelta);
	void	stopFrame();
	void	restartFrame();

	void	draw(const RenderCtx* rc, f32 x, f32 y, f32 w) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ActStat()					{	clear();	}
	ActStat(const ActStat& src)	{	copy(&src);	}

	void copy(const ActStat* src)
	{
		_frameNext = src->_frameNext;
		_frameMax = src->_frameMax;
	}
	ActStat& operator =(const ActStat& src) { copy(&src); return *this; }

	void clear()
	{
		_frameNext = 0.0f;
		_frameMax = 0.0f;
	}

	//======================================================================
	// 変数
private:
	// [  0]
	f32				_frameNext;		// 次のアクションまでの残りフレーム数
	f32				_frameMax;		// 次のアクションまでの最大フレーム数
	// [ 12]
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_ACT_STAT_H_
