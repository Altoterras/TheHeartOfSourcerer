/***********************************************************************//**
 *	FocusMenu.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_FOCUS_MENU_H_
#define _SRCR_GUI_FOCUS_MENU_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
template<class TYPE> class Point;
typedef Point<f32> PointF32;
namespace smenu
{
	class Menu;
}

TFW_END_NS
using namespace smenu;

SRCR_BEGIN_NS

class Game;
class GameMenuShowParam;
class Unit;

/*---------------------------------------------------------------------*//**
 *	フォーカスメニュー
 *
**//*---------------------------------------------------------------------*/
class FocusMenu : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	enum MenuKind
	{
		MENU_PC1,
		MENU_PC2,
		MENU_PC3,
		MENU_PC2_WEP,
		MENU_ENEMY,
		MENU_EXTRA,
		MENU_MAPOBJ,
		MENU_DROPITEM,
		NUM_MENU
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 外部有効かどうかを得る
	inline bool		isEnable() const			{ return _isEnableOut; }
	// メニューを得る
	inline Menu*	menu(MenuKind mkind)		{ return _menu[mkind]; }

	// 外部有効かどうかをs設定する
	inline void		setEnable(bool isEnable)	{ _isEnableOut = isEnable; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void showMenu(MenuKind mk, const PointF32* ptBase, Unit* unitRef, bool isFollow);
	void hideAllMenu();
	bool setMenuElementEnable(MenuKind mk, int mtnodeid, bool isEnable);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FocusMenu();
	virtual ~FocusMenu();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void draw(const RenderCtx* rc);				// 描画
	void notifyEnterGame();
	void notifyChangedMap();

private:
	bool createMenu();
	void destroyMenu();
	void adjustMenuPos(MenuKind mkind);

	//======================================================================
	// 変数
private:
	bool				_isEnableOut;
	bool				_isEnableSelf;
	Menu*				_menu[NUM_MENU];
	Unit*				_unitFllowRef[NUM_MENU];	// 位置追従用ユニット参照
	GameMenuShowParam*	_showprm;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_FOCUS_MENU_H_
