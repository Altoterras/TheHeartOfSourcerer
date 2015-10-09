/***********************************************************************//**
 *  MenuPanelReceptor.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SMENU_MENU_PANEL_RECEPTOR_H_
#define _TFW_SMENU_MENU_PANEL_RECEPTOR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "Base.h"
#include "../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS
class PanelBase;

SMENU_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	メニュー パネル レセプター
 *	
**//*---------------------------------------------------------------------*/
class MenuPanelReceptor : public FrameExecDraw
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 有効かどうかの判定
	inline bool isAvailable() const { return _isAvailable; }
	// パネル取得
	inline PanelBase* getPanel() const { return _panelCur; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MenuPanelReceptor();
	virtual ~MenuPanelReceptor();
	bool create(PanelBase* panel);
	void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);
	virtual void draw(const RenderCtx* rc, bool isCurrent);

	//======================================================================
	// 変数
private:
	PanelBase*		_panelCur;	
	bool			_isAvailable;
};

SMENU_END_NS
TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// #ifndef _TFW_SMENU_MENU_PANEL_RECEPTOR_H_
