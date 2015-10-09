/***********************************************************************//**
 *	OptionsWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_OPTIONS_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_OPTIONS_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/Window.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS
	
TFW_END_NS

SRCR_BEGIN_NS

class Game;

#define ENABLE_OPT_MOVMSG_FLOW	1

/*---------------------------------------------------------------------*//**
 *	オプション メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class OptionsWindow : public Window
{
	//======================================================================
	// 定数
private:
	enum Button
	{
		BTN_SS,
		BTN_JS,
		BTN_RAW,
		BTN_SPELL,
#if ENABLE_OPT_MOVMSG_FLOW
		BTN_WAIT,
		BTN_AUTOFLOW,
#endif
		NUM_BTN
	};
	enum Parts
	{
		PRT_DUMMY,
		NUM_PRT
	};

	// YES / NO メッセージウインドウの問い合わせ内容
	enum ConfirmKind
	{
		CFK_NULL,
		CFK_CHANGE_TO_SS,
		CFK_CHANGE_TO_JS,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// （スタンドアローン時）終了済みかを得る
	inline bool isDone() const { return _isDone; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OptionsWindow();
	~OptionsWindow();
	bool create(Font* fontRef, Texture* texRef);
	void destroy();

protected:
	void execContents(ExecRes* res, const ExecCtx* ec);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);
	void onShowWindow(bool isShow);

private:
	void changeDefaultSoumaLang(s32 srckind);
	void updateDefaultSoumaLangButton();
	void changeChantWindowMode(bool isRawMode);
	void updateChantWindowModeButton();
#if ENABLE_OPT_MOVMSG_FLOW
	void changeMovieMsgFlowMode(bool isAutoFlow);
	void updateMovieMsgFlowModeButton();
#endif

	//======================================================================
	// 変数
private:
	bool _isDone;
	Font* _fontRef;
	Texture* _texRef;
	Panel* _pnlBtn[NUM_BTN];
	Panel* _pnlPrt[NUM_PRT];
	ConfirmKind _confirmkind;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_OPTIONS_WINDOW_H_
