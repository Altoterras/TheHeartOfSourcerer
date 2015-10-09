/***********************************************************************//**
 *	SaveLoadWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_SAVE_LOAD_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_SAVE_LOAD_WINDOW_H_

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
class HeaderPanel;

/*---------------------------------------------------------------------*//**
 *	セーブ／ロード メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class SaveLoadWindow : public Window
{
	//======================================================================
	// 定数
public:
	// ページ
	enum Page
	{
		PAGE_SAVE,
		PAGE_LOAD,
		NUM_PAGE
	};

private:
	// ボタン
	enum Button
	{
		BTN_FILE1,
		BTN_FILE2,
		BTN_GOTITLE,
		NUM_BTN
	};
	// パネル
	enum Pnl
	{
		PNL_SCORE,
		PNL_FILE1_PSNS_ON,
		PNL_FILE2_PSNS_ON,
		PNL_CAP_GOTITLE,
		NUM_PNL
	};

	// YES / NO メッセージウインドウの問い合わせ内容
	enum ConfirmKind
	{
		CFK_NULL,
		CFK_GOTITLE,
	};

	static const s32 NUM_SAVEFILE	= 2;

	//======================================================================
	// クラス
private:
	class SaveFileInfo
	{
	public:
		u32 _framePlay;
		bool _isCleared;
		u32 _score;
	public:
		SaveFileInfo() : _framePlay(0), _isCleared(false), _score(0) {}
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
	SaveLoadWindow();
	~SaveLoadWindow();
	bool create(Font* fontRef, Texture* texRef, Page page, bool isStandAlone, u32 saveflags);
	void destroy();

protected:
	void execContents(ExecRes* res, const ExecCtx* ec);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);
	void onShowWindow(bool isShow);
	void notifyChangeLogicalScreenSize();

private:
	bool doSave(s32 svid);
	void updateSaveFileInfo();
	void complyChangeScreenSize();

	//======================================================================
	// 変数
private:
	u32 _saveflags;
	ConfirmKind _confirmkind;
	Page _page;
	bool _isDone;
	Font* _fontRef;
	Texture* _texRef;
	Panel* _btn[NUM_BTN];
	Panel* _pnl[NUM_PNL];
	HeaderPanel* _hpnlHbarSa;			// スタンドアローンモード時のヘッダーパネル
	SaveFileInfo _sfi[NUM_SAVEFILE];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_SAVE_LOAD_WINDOW_H_
