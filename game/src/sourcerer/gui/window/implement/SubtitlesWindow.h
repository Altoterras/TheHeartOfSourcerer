/***********************************************************************//**
 *	SubtitlesWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_SUBTITLES_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_SUBTITLES_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/MessageWindow.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Font;
class Texture;
class StringDrawer;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	字幕（台詞）ウインドウ
 *
**//*---------------------------------------------------------------------*/
class SubtitlesWindow : public MessageWindow
{
	//======================================================================
	// 定数
public:
	enum PanelKind
	{
		PANEL_LOCK,
		NUM_PANEL
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SubtitlesWindow();
	~SubtitlesWindow();
	bool create(Font* fontRef, Texture* texRef);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);
	void execContents(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);
	void onShowWindow(bool isShow);

	//======================================================================
	// 変数
private:
	Panel* _pnl[NUM_PANEL];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_SUBTITLES_WINDOW_H_
