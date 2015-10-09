/***********************************************************************//**
 *  PanelBase.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/11.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_GUI_PANEL_BASE_H_
#define _TFW_GUI_PANEL_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../framemod/FrameExecDraw.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	パネル基底クラス
 *	
**//*---------------------------------------------------------------------*/
TFW_INTERFACE PanelBase : public FrameExecDraw
{
	//======================================================================
	// メソッド
public:
	virtual ~PanelBase() {}
	virtual void setFront(bool isFront) {}
	virtual void onCreatePanel() {}
	virtual void onDestroyPanel() {}
	virtual void exec(ExecRes* res, const ExecCtx* ec) {}
	virtual void render(const RenderCtx* rc) {}
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_GUI_PANEL_BASE_H_
