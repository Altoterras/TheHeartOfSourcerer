/***********************************************************************//**
 *	App.h
 *	Enlight Windows Application
 *	
 *	Created by Ryoutarou Kishi on 2009/06/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#pragma once

////////////////////////////////////////////////////////////////////////////
// 型

namespace app
{

class App;
class MainWindow;
class RenderSystem;

}	// namespace app

////////////////////////////////////////////////////////////////////////////
// グローバル変数

namespace app
{

extern App* g_papp;

}	// namespace app

////////////////////////////////////////////////////////////////////////////
// クラス

namespace etk
{
	class EtkBody;
	class OsDepMod;
}	// namespace etk

namespace app
{

class PsnsWin;
class StopWatch;

/*---------------------------------------------------------------------*//**
	アプリケーション

**//*---------------------------------------------------------------------*/
class App
{
	//======================================================================
	// 定数
public:
	static const int		AIF_FORCE_SD			= 0x00000001;
	static const int		AIF_FORCE_HD			= 0x00000002;
	static const int		AIF_FORCE_RD			= 0x00000004;
///	static const int		AIF_FORCE_WD			= 0x00000008;	// ワイド画面（16:9）
	static const int		AIF_FULL_SCREEN			= 0x00000010;	// フルスクリーン
	static const int		AIF_DEBUG_DOUBLE_FRAME	= 0x00010000;

	static const int		WIDTH_DEFAULT			= 480;
	static const int		HEIGHT_DEFAULT			= 320;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	etk::EtkBody*			GetBody() const			{	return m_pbody;	};
	MainWindow*				GetMainWindow() const	{	return m_pwnd;	}
	static App*				GetApp();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	App();

	bool					Init(HINSTANCE hInstance, etk::OsDepMod* osdep, PsnsWin* psns, int widthScreen, int heightScreen, int nCmdShow, int aiflags);
	void					Cleanup();
	int						Do();

	static LRESULT CALLBACK	WindowProcedureStatic(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK AboutDialogWindowProcedureStatic(HWND, UINT, WPARAM, LPARAM);
	static void CALLBACK TimerProcedureStatic(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dummy1, DWORD dummy2);

	//======================================================================
	// 変数
private:
	HINSTANCE				m_hInstance;

	MainWindow*				m_pwnd;
	RenderSystem*			m_prendsys;
	etk::EtkBody*			m_pbody;
	StopWatch*				m_swFrame;

	UINT					m_idTimer;
	float					m_cntTimer;
};

}	// namespace app

////////////////////////////////////////////////////////////////////////////
