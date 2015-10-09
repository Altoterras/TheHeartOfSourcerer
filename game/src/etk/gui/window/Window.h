/***********************************************************************//**
 *	Window.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/06/05.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_GUI_WINDOW_WINDOW_H_
#define _ETK_GUI_WINDOW_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../../tfw/gui/Panel.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
template<class TYPE> class Point;
typedef Point<f32> PointF32;

TFW_END_NS

ETK_BEGIN_NS

class WindowFrame;

/*---------------------------------------------------------------------*//**
 *	ウインドウ（高機能なパネル）
 *
**//*---------------------------------------------------------------------*/
class Window : public Panel
{
	//======================================================================
	// 定数
public:
	// 動作フラグ
	static const u16	AWNDF_TMP_HIDE		= 0x0001;	// 一時非表示
	static const u16	AWNDF_DRAW_ONLY		= 0x0002;	// 描画のみ（制御をまわさない）

private:
	// 制御フラグ
	static const u16	EF_SHOW				= 0x0001;	// 表示する
	static const u16	EF_HIDE_FRAME		= 0x0002;	// 非表示にしたフレーム

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 表示中かどうかを得る
	inline bool isShowWindow() const { return TFW_IS_FLAG(_eflagsWnd, EF_SHOW); }
	// フレーム制御カウンタの取得
	inline u32 getExecCount() const { return _cntExec; }

	// 共通ウインドウフレームの取得
	inline void setWindowFrame(WindowFrame* wndfrm)	{ _wndfrm = wndfrm; }
	// 動作フラグの設定
	void setActFlags(u16 aflags, bool isValid) { TFW_SET_FLAG(_aflagsWnd, aflags, isValid); }

protected:
	// 表示時カウンタを得る
	inline s32 getShowCount() const	{ return (_cntShow >= 0) ? _cntShow : 0; }
	// 非表示時カウンタを得る
	inline s32 getHideCount() const	{ return (_cntShow < 0) ? - _cntShow : 0; }
	// フェードフレームカウンタを得る（表示時フェードは正の値、非表示時フェードは負の値）
	inline f32 getFadeFrameCount() const { return _fcntFade; }
	// 一時非表示中かを得る
	inline bool isTemporaryHiding() const { return TFW_IS_FLAG(_aflagsWnd, AWNDF_TMP_HIDE); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void	showWindow(bool isShow);
	virtual void	showWindowWithFade(bool isShow, s32 fcntFade);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Window();
	virtual ~Window();
	virtual bool	create(const RectF32* rect);
	virtual void	destroy();
	virtual void	exec(ExecRes* res, const ExecCtx* ec);
	virtual void	draw(const RenderCtx* rc);
	virtual void	notifyChangeLanguage() {}
	virtual void	notifyChangeLogicalScreenSize() {}

protected:
	virtual void	execContents(ExecRes* res, const ExecCtx* ec) {}
	virtual void	drawContents(const RenderCtx* rc, const RectF32* rectBase) {}
	virtual void	onShowWindow(bool isShow) {}

	//======================================================================
	// メンバ変数
private:
	u16				_aflagsWnd;		// 動作フラグ
	u16				_eflagsWnd;		// 制御フラグ
	u32				_cntExec;		// フレーム制御カウンタ
	s32				_cntShow;		// 表示カウンタ（表示・非表示時のディゾルブ処理などに使う）
	f32				_fcntFade;		// フェードフレームカウンタ
	WindowFrame*	_wndfrm;		// ウインドウフレーム
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_GUI_WINDOW_WINDOW_H_