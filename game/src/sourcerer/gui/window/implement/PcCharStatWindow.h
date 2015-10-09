/***********************************************************************//**
 *	PcCharStatWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_PC_CHAR_STAT_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_PC_CHAR_STAT_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "PcStatusWindowBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Panel;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	キャラクタ ステータス ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class PcCharStatWindow : public PcStatusWindowBase
{
	//======================================================================
	// 定数
private:
	enum Pnl
	{
		PNL_ENE_VAL,
		PNL_ENEDMG_VAL,
		PNL_PHSDMG_VAL,
		PNL_LV_VAL,
		PNL_UNI_VAL,
		PNL_ACT_VAL,
		PNL_STB_VAL,
		PNL_EXP_VAL,
		NUM_PNL
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PcCharStatWindow();
	~PcCharStatWindow();
	bool create(Font* fontRef, Texture* texRef);
	void destroy();

protected:
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);
	void onShowWindow(bool isShow);
	void onChangePc();

private:
	void updateValue();

	//======================================================================
	// 変数
private:
	Panel* _pnl[NUM_PNL];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_PC_CHAR_STAT_WINDOW_H_
