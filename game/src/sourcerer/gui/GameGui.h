/***********************************************************************//**
 *	GameGui.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_GAME_GUI_H_
#define _SRCR_GUI_GAME_GUI_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/gui/EtkGui.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;
class FedTaskSys;

TFW_END_NS

SRCR_BEGIN_NS

class AriaWindow;
class CharFronts;
class EventMovieFront;
class FocusCursor;
class FocusMenu;
class GameFontSet;
class Game;
class IngameFront;
class ItemListWindow;
class MoveCursor;
class SourceEditWindow;
class SourceListWindow;
class SubtitlesWindow;

/*---------------------------------------------------------------------*//**
 *	GUI（2D 表示物）
 *
**//*---------------------------------------------------------------------*/
class GameGui : public EtkGui
{
	//======================================================================
	// 定数
public:
	enum AriaWnd
	{
		ARIAWND_PC1,	// 自キャラ用詠唱ウインドウ
		ARIAWND_ENE1,	// 敵キャラ用詠唱ウインドウ
		NUM_ARIAWND
	};
	
	// 実行制御フラグ
	static const u32	EF_EXT_ENABLE_CTRL	=	0x00000001;	// 有効・無効化を外部コントロール
	static const u32	EF_HIDE_TEMPORARY	=	0x00000002;	// 一時的 GUI 表示オフ

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	外部ターゲット制御
	 *		タスクシステム的な外部 FrameExecDraw 制御
	 *		全部タスクシステム化したほうが良いようにも思われがちだが、
	 *		やりすぎると処理が煩雑になり、
	 *		デバッグ時にブレイクポイントの設定も困難になる･･･
	 *
	**//*-----------------------------------------------------------------*/
	class ExtCtrlTask
	{
		FrameExecDraw*	_trg;
		s32				_order;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline Texture*				getGuiTexture() const		{	return _texGui;					}

	inline IngameFront*			getIngameFront() const		{	return _igfr;					}
	inline EventMovieFront*		getEventMovieFront() const	{	return _evmfr;					}
	inline CharFronts*			getCharFronts() const		{	return _chrfrnt;				}
	inline MoveCursor*			getMoveCursor() const		{	return _mvcsr;					}
	inline FocusCursor*			getFocusCursor() const		{	return _fccsr;					}
	inline FocusMenu*			getFocusMenu() const		{	return _fcmenu;					}
	inline SubtitlesWindow*		getSubtitlesWindow() const	{	return _wndSbt;					}
	inline SourceEditWindow*	getSourceEditWindow() const	{	return _wndSrcedt;				}
	inline AriaWindow*			getMyAriaWindow() const		{	return _wndAria[ARIAWND_PC1];	}
	inline AriaWindow*			getEnemyAriaWindow() const	{	return _wndAria[ARIAWND_ENE1];	}
	inline SourceListWindow*	getMagicListWindow() const	{	return _wndMagicList;			}
	inline ItemListWindow*		getItemListWindow() const	{	return _wndItemList;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool isOpenedLockGui() const;
	void setExecFlags(u32 eflags, bool isValid);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	GameGui();
	~GameGui();
	bool create(GameFontSet* fontsetRef, FedTaskSys* tasksys);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	void notifyChangeLanguage();
	void notifyResizeView();
	void notifyEnterGame();
	void notifyChangeMapBefore();
	void notifyChangedMap();
	void notifyChangeLeader();
	void notifyDeserialize();

protected:
	virtual SysMsgWindow* makeSysMsgWindow();
	virtual ActionMsgWindow* makeActionMsgWindow();

	//======================================================================
	// 変数
private:
	Texture*			_texGui;				// GUI 用テクスチャ
	s32					_gmkPrevFrame;			// ゲームモード変更検知用
//	u32					_eflags;				// 実行制御フラグ

	IngameFront*		_igfr;
	EventMovieFront*	_evmfr;
	CharFronts*			_chrfrnt;
	MoveCursor*			_mvcsr;
	FocusCursor*		_fccsr;
	FocusMenu*			_fcmenu;
	AriaWindow*			_wndAria[NUM_ARIAWND];
	SubtitlesWindow*	_wndSbt;
	SourceEditWindow*	_wndSrcedt;
	SourceListWindow*	_wndMagicList;
	ItemListWindow*		_wndItemList;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_GAME_GUI_H_
