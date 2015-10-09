/***********************************************************************//**
 *	HelpWindow.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/07/26.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_GUI_WINDOW_IMPLEMENT_HELP_WINDOW_H_
#define _ATAR_GUI_WINDOW_IMPLEMENT_HELP_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/Window.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;

TFW_END_NS

ATAR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	メッセージウインドウ
 *
**//*---------------------------------------------------------------------*/
class HelpWindow : public Window
{
	//======================================================================
	// 定数
public:
	enum ContentsKind
	{
		CNTNS_NULL,
		CNTNS_HELP,
		CNTNS_GETMARKER,
		CNTNS_GOSUPPORT_PHONE_NO,
		CNTNS_GOSUPPORT_PHONE_YES,
		NUM_CNTNS
	};

private:
	enum ButtonKind
	{
		BTN_BACK,
		BTN_PREV,
		BTN_NEXT,
		NUM_BTN
	};
	
	enum PageKind
	{
		PAGE_1,
		PAGE_2,
		NUM_PAGE
	};
	
	static const int NUM_LINK	= 2;

	static const u32 FILEID_CONTENTS_TEX[NUM_CNTNS][NUM_PAGE];
	static const s32 RECT_CONTENTS_LINK[NUM_CNTNS][NUM_PAGE][NUM_LINK][4];
	static const char* URL_CONTENTS_LINK[NUM_CNTNS][NUM_PAGE][NUM_LINK];

	//======================================================================
	// クラス
private:
	class Button
	{
	public:
		Panel* _pnl;
		bool _isEnable;
	public:
		Button();
		~Button();
		bool create(Texture* texGuiRef, const RectS32* rectPanel, const RectF* uvPanelNormal, const RectF* uvPanelHover);
		void destroy();
	};
	
	class Page
	{
	public:
		Texture* _tex;
		f32 _xOffset;
		Page();
		~Page();
	};
	
	class Link
	{
	public:
		RectS32* _rect;
		VcString* _url;
		Link();
		~Link();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 終了済みかを得る
	inline bool isDone() const { return _isDone; }
	// 入力完了したかを得る
	inline bool isInputDone() const { return _isInputDone; }

	// コンテンツ設定
	void setContents(ContentsKind contents) { _contents = contents; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HelpWindow();
	~HelpWindow();
	bool create(Font* fontRef, Texture* texGuiRef);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	void drawContents(const RenderCtx* rc, const RectS32* rectBase);

private:
	void updateButtonsState();
	void onShowWindow(bool isShow);

	//======================================================================
	// メンバ変数
private:
	Texture* _texGuiRef;
	bool _isDone;
	bool _isInputDone;
	Page _page[NUM_PAGE];
	Button _btn[NUM_BTN];
	Link _link[NUM_PAGE][NUM_LINK];
	ContentsKind _contents;
	int _idxCurPage;
	f32 _fcntFade;
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_GUI_WINDOW_IMPLEMENT_HELP_WINDOW_H_
