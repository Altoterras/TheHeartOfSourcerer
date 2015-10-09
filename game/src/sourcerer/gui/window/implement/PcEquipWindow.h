/***********************************************************************//**
 *	PcEquipWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_PC_EQUIP_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_PC_EQUIP_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "PcStatusWindowBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS


TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	装備ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class PcEquipWindow : public PcStatusWindowBase
{
	//======================================================================
	// 定数
private:
	enum Button
	{
		BTN_ITEM_ACS,
		NUM_BTN
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PcEquipWindow();
	~PcEquipWindow();
	bool create(Font* fontRef, Texture* texRef);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);

	//======================================================================
	// 変数
private:
	Panel* _pnlBtn[NUM_BTN];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_PC_EQUIP_WINDOW_H_
