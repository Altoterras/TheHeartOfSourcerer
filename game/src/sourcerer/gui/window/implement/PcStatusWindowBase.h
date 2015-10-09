/***********************************************************************//**
 *	PcStatusWindowBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_PC_STATUS_WINDOW_BASE_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_PC_STATUS_WINDOW_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/Window.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	キャラクタ ステータス メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class PcStatusWindowBase : public Window
{
public:
	static const s32	NUM_PCKIND = 3;

private:
	enum Button
	{
		BTN_LEFT,
		BTN_RIGHT,
		NUM_BTN
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PcStatusWindowBase();
	virtual ~PcStatusWindowBase();
	virtual bool create(Font* fontRef, Texture* texRef);
	virtual void destroy();
	virtual void execContents(ExecRes* res, const ExecCtx* ec);
	virtual void drawContents(const RenderCtx* rc, const RectF32* rectBase);

protected:
	void setEnablePc(s32 ipc, bool isEnable);
	virtual void onChangePc() {}

private:
	void nextPc();
	void prevPc();

	//======================================================================
	// 変数
protected:
	s32 _ipc;
	Font* _fontRef;
	Texture* _texRef;

private:
	Panel* _pnlBtn[NUM_BTN];
	bool _pcenb[NUM_PCKIND];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_PC_STATUS_WINDOW_BASE_H_
