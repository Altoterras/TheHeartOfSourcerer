/***********************************************************************//**
 *	EtkGui.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_GUI_GUI_H_
#define _ETK_GUI_GUI_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;
class FedTaskSys;

TFW_END_NS

ETK_BEGIN_NS

class ActionMsgWindow;
class FontSet;
class SysMsgWindow;

/*---------------------------------------------------------------------*//**
 *	GUI（2D 表示物）
 *
**//*---------------------------------------------------------------------*/
class EtkGui : public FrameExecDraw
{
	//======================================================================
	// 定数
	
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
	inline SysMsgWindow*		getSysMsgWindow() const		{	return _wndSysMsg;				}
	inline ActionMsgWindow*		getActionMsgWindow() const	{	return _wndActMsg;				}
	inline FontSet*				getFontSet() const			{	return _fontsetRef;				}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool isOpenedLockGui() const;
	virtual void setExecFlags(u32 eflags, bool isValid);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EtkGui();
	virtual ~EtkGui();
	virtual bool create(FontSet* fontsetRef, FedTaskSys* tasksys);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec) = 0;

	virtual void notifyChangeLanguage() {}
	virtual void notifyChangeLogicalScreenSize() {}
	virtual void notifyResizeView();

protected:
	virtual SysMsgWindow* makeSysMsgWindow() = 0;
	virtual ActionMsgWindow* makeActionMsgWindow() = 0;

	//======================================================================
	// 変数
protected:
	u32					_eflags;				// 実行制御フラグ

	SysMsgWindow*		_wndSysMsg;
	ActionMsgWindow*	_wndActMsg;

	FontSet*			_fontsetRef;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_GUI_GUI_H_
