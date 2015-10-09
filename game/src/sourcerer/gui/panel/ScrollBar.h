/***********************************************************************//**
 *  ScrollBar.h
 *  Enlight Game Application
 *
 *  Created by Ryoutarou Kishi on 2010/07/05.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_PANEL_SCROLL_BAR_H_
#define _SRCR_GUI_PANEL_SCROLL_BAR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

#include "../../../tfw/gui/BasicScrollBar.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スクロールバー
 *	
**//*---------------------------------------------------------------------*/
class ScrollBar : public BasicScrollBar
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 矩形を設置する
#if 0
	virtual void			setRectangle(const RectF32* rect);
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ScrollBar();
	~ScrollBar();
	bool	create(const RectF32* rect, Texture* texRef);
	void	destroy();
	void	exec(ExecRes* res, TouchPanel* ui);

private:
	void	drawBackground(const RenderCtx* rc, const RectF32* rect);
	void	drawKnob(const RenderCtx* rc, const RectF32* rect);
	void	drawUpButton(const RenderCtx* rc, const RectF32* rect, bool isEnabled);
	void	drawDownButton(const RenderCtx* rc, const RectF32* rect, bool isEnabled);
	void	setKnobRectangle();

	//======================================================================
	// 変数
private:
	Texture*		_texRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_PANEL_SCROLL_BAR_H_
