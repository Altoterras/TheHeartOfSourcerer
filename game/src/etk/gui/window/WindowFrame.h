/***********************************************************************//**
 *	WindowFrame.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/06/05.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_GUI_WINDOW_WINDOW_FRAME_H_
#define _ETK_GUI_WINDOW_WINDOW_FRAME_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
template<class TYPE> class Point;
typedef Point<f32> PointF32;
class RenderCtx;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
	ウインドウ フレーム クラス
**//*---------------------------------------------------------------------*/
class WindowFrame
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	WindowFrame();
	virtual ~WindowFrame();

	virtual void		draw(const RenderCtx* rc, const RectF32* rect);
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_GUI_WINDOW_WINDOW_FRAME_H_