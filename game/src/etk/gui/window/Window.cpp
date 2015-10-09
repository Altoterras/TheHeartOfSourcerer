/***********************************************************************//**
 *	Window.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/06/05.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/


////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Window.h"

// Friends
#include "WindowFrame.h"

// External
#include "../../../tfw/lib/Point.h"
#include "../../../tfw/lib/Rect.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	表示／非表示
**//*---------------------------------------------------------------------*/
void Window::showWindow(bool isShow)
{
	if(isShowWindow() && !isShow)
	{
		TFW_SET_FLAG(_eflagsWnd, EF_SHOW, false);
		TFW_SET_FLAG(_eflagsWnd, EF_HIDE_FRAME, true);
		onShowWindow(false);
		// 実行カウンタリセット
		_cntShow = 0;
	}
	else if(!isShowWindow() && isShow)
	{
		TFW_SET_FLAG(_eflagsWnd, EF_SHOW, true);
		onShowWindow(true);
		// 実行カウンタリセット
		_cntExec = 0;
		_cntShow = 0;
	}
}

/*---------------------------------------------------------------------*//**
	フェード表示／非表示
	※フェードに対応するか否かは派生クラスの実装による
**//*---------------------------------------------------------------------*/
void Window::showWindowWithFade(bool isShow, s32 fcntFade)
{
	if(isShow)
	{
		showWindow(true);
		// 表示フェードは正の値でカウンタをセットする
		_fcntFade = (f32)fcntFade;
		if(_fcntFade < 0.0f)	{	_fcntFade = - _fcntFade;	}
	}
	else
	{
		// 非表示フェードは負の値でカウンタをセットする
		_fcntFade = (f32)fcntFade;
		if(_fcntFade > 0.0f)	{	_fcntFade = - _fcntFade;	}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Window::Window()
	: _aflagsWnd(0)
	, _eflagsWnd(0)
	, _cntExec(0)
	, _cntShow(0)
	, _fcntFade(0.0f)
	, _wndfrm(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Window::~Window()
{
	ASSERT(_wndfrm == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Window::create(const RectF32* rect)
{
	if(!Panel::create(rect))	{	return false;	}

	_cntExec = 0;
	_cntShow = 0;
	_fcntFade = 0.0f;
	_wndfrm = new WindowFrame();		// デフォルトではウインドウフレームを作成する

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Window::destroy()
{
	delete _wndfrm;
	_wndfrm = 0L;

	Panel::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Window::exec(ExecRes* res, const ExecCtx* ec)
{
	// 非表示フレームフラグをおろす
	if(TFW_IS_FLAG(_eflagsWnd, EF_HIDE_FRAME))
	{
		TFW_SET_FLAG(_eflagsWnd, EF_HIDE_FRAME, false);
	}

	// フェードカウンタを更新する
	if(_fcntFade > 0.0f)		// 表示フェード（カウンタの値は正）
	{
		_fcntFade -= ec->getDeltaFrame();
		if(_fcntFade < 0.0f)	{	_fcntFade = 0.0f;	}
	}
	else if(_fcntFade < 0.0f)	// 非表示フェード（カウンタの値は負）
	{
		_fcntFade += ec->getDeltaFrame();
		if(_fcntFade > 0.0f)	{	_fcntFade = 0.0f;	}
		if(_fcntFade == 0.0f)
		{
			showWindow(false);	// 完全非表示へ
		}
	}

	// 表示カウンタの更新、非表示時は抜ける
	if(isShowWindow())
	{
		if((0 <= _cntShow) && (_cntShow < S32_MAX))
		{
			_cntShow++;
		}
	}
	else
	{
		if((S32_MIN < _cntShow) && (_cntShow <= 0))
		{
			_cntShow--;
		}
		// 非表示時は抜ける
		return;
	}

	// 描画のみのときは抜ける
	if(TFW_IS_FLAG(_aflagsWnd, AWNDF_DRAW_ONLY))	{	return;	}

	// パネルとしてのフレーム制御
	Panel::exec(res, ec);
	if(!isEnable())	{	return;	}
	
	// 派生クラスのフレーム制御
	execContents(res, ec);
	_cntExec++;
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void Window::draw(const RenderCtx* rc)
{
	// 非表示時は抜ける
	if(!isShowWindow())	{	return;	}
	// 一時非表示中は抜ける
	if(isTemporaryHiding())	{	return;	}

	// パネルとしての描画
	Panel::draw(rc);

	// フレーム描画
	if(_wndfrm != 0L)	{	_wndfrm->draw(rc, getRectangle());	}
	
	// 内容描画
	drawContents(rc, getRectangle());
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
