/***********************************************************************//**
 *	PcAbilityWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_PC_ABILITY_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_PC_ABILITY_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "PcStatusWindowBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS


TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	アビリティ ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class PcAbilityWindow : public PcStatusWindowBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	bool create(Font* fontRef, Texture* texRef);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);

	//======================================================================
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_PC_ABILITY_WINDOW_H_
