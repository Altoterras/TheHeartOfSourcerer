/***********************************************************************//**
 *	App.cpp
 *	Enlight Windows Application
 *	
 *	Created by Ryoutarou Kishi on 2009/06/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Basic
#include "BasicInclude.h"

// Self
#include "App.h"

// Friends
#include "DebugWin.h"
#include "MainWindow.h"
#include "OsDepModWin.h"
#include "PsnsWin.h"
#include "RenderSystem.h"
#include "Resource.h"					// IDS_APP_TITLE ...
#include "StopWatch.h"

// External
#include "../etk/body/EtkBody.h"
#if defined(_ATAR)
#include "../atar/body/AtArBody.h"
#elif defined(_BASICMODEL)
#include "../basicmodel/body/BasicModelBody.h"
#elif defined(_EDEV)
#include "../edev/body/EdevBody.h"
#elif defined(_PUZZLE_ICHTHYS)
#include "../puzzle_ichthys/body/IchthysPuzzleBody.h"
#endif
#include "../tfw/file/File.h"
#include "../tfw/file/IdFile.h"
#include "../tfw/string/VcString.h"
#include "../tfw/string/StringUtils.h"
#include "../sourcerer/body/Game.h"

// Library
#include <locale.h>
#include <mmsystem.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////
// マクロ

#define RENDER_SPF		(60.0f)		// １秒間の描画フレーム数
#define LOGIC_SPF		(30.0f)		// １秒間で進む論理フレーム数

// 圧縮ファイル展開用リードバッファのサイズ
#define SIZE_UNCOMPRESS_READ_BUFFER_1		(8 * 1024 * 1024)	// 8MB
#define SIZE_UNCOMPRESS_READ_BUFFER_2		(12 * 1024 * 1024)	// 12MB

////////////////////////////////////////////////////////////////////////////
// グローバル変数定義

namespace app
{

App* g_papp = NULL;

}	// namespace app

////////////////////////////////////////////////////////////////////////////
// グローバル関数定義

/*---------------------------------------------------------------------*//**
	@brief		実行
	@param		hInstance		インスタンスハンドル
	@param		pszCmdLine		コマンドライン文字列
	@param		nCmdShow		ウインドウ表示状態指定値
**//*---------------------------------------------------------------------*/
int runApp(HINSTANCE hInstance, LPTSTR pszCmdLine, int nCmdShow)
{
	using namespace app;
	int aiflags = 0;
	int widthScreen = App::WIDTH_DEFAULT;
	int heightScreen = App::HEIGHT_DEFAULT;

	// ロケール指定
	setlocale(LC_ALL, "japanese");

	// コマンドラインパラメータ解析
	{
		STRING_CLASS strCmdParam(pszCmdLine);
		STRARRAY_CLASS saCmdParam = StringUtils::split(&strCmdParam, ' ');
		for(int i = 0; i < saCmdParam.getCount(); i++)
		{
			STRING_CLASS* strParam = saCmdParam.get(i);
			if(strParam->equalsIgnoreCase(_T("/sd")))
			{
				aiflags |= App::AIF_FORCE_SD;
			}
			if(strParam->equalsIgnoreCase(_T("/hd")))
			{
				aiflags |= App::AIF_FORCE_HD;
			}
			if(strParam->equalsIgnoreCase(_T("/rd")))
			{
				aiflags |=  App::AIF_FORCE_HD | App::AIF_FORCE_RD;
			}
#if 0
			if(strParam->equalsIgnoreCase(_T("/wd")))
			{
				aiflags |=  App::AIF_FORCE_WD;
			}
#endif
			if(strParam->equalsIgnoreCase(_T("/ipad")))
			{
				widthScreen = 1024;
				heightScreen = 748;// 768;
			}
			if(strParam->equalsIgnoreCase(_T("/ipad3")))
			{
				widthScreen = 2048;
				heightScreen = 1496;// 1536;
			}
			if(strParam->equalsIgnoreCase(_T("/iphone4")))
			{
				widthScreen = 960;
				heightScreen = 640;
			}
			if(strParam->equalsIgnoreCase(_T("/iphone5")))
			{
				widthScreen = 1136;
				heightScreen = 640;
			}
			if(strParam->equalsIgnoreCase(_T("/iphone6")))
			{
				widthScreen = 1334;
				heightScreen = 750;
			}
			if(strParam->equalsIgnoreCase(_T("/iphone6plus")))
			{
				widthScreen = 2208;// 1920;
				heightScreen = 1242;// 1080;
			}
			if(strParam->equalsIgnoreCase(_T("/debug_double_frame")))
			{
				aiflags |= App::AIF_DEBUG_DOUBLE_FRAME;
			}
		}
	}

	// デバッグモジュール初期化
	::app::DebugWin::init();

	// プロダクト定義
	VcString marker("altoterras.co.jp");
	VcString postfix("-w");
	#if defined(_ENLIGHT_V01)
		CcString prod("sourcerer");
		VcString appnam("sourcerer01");
		VcString fname("enlight1010w.dat");
	#elif defined(_ENLIGHT_V01_LITE)
		CcString prod("sourcerer");
		VcString appnam("sourcerer01");
		VcString fname("enlight1010wl.dat");
	#elif defined(_ENLIGHT_V02)
		CcString prod("sourcerer");
		VcString appnam("sourcerer02");
		VcString fname("enlight1020w.dat");
	#elif defined(_DIMZ) || defined(_EDEV) || defined(_ATAR) || defined(_BASICMODEL) || defined(_ENLIGHT_PRPZL01)
		CcString prod("sourcerer");
		VcString appnam("sourcerer01");
		VcString fname("enlight1010w.dat");
	#elif defined(_PUZZLE_ICHTHYS)
		CcString prod("puzzle_ichthys");
		VcString appnam("ichthyspuzzle01");
		VcString fname("puzzle1010w.dat");
		#if defined(_DEBUG)
			aiflags |= App::AIF_FORCE_HD | App::AIF_FORCE_RD;
		#else
			aiflags |= App::AIF_FORCE_HD | App::AIF_FORCE_RD | App::AIF_FULL_SCREEN;
		#endif
	#endif

	// 基底モジュールの作成
	g_papp = new App;
	OsDepModWin osdep(marker, appnam);
	PsnsWin psns;

	// リソースパスの取得
	VcString pathRes;
	if(!osdep.getResourceDirPath(&pathRes, prod))
	{
		return -1;
	}

	// TFW ファイルジュール初期化
	tfw::File::init(&pathRes);
	#if !defined(_ATAR)
		tfw::IdFile::init(&fname, &postfix, SIZE_UNCOMPRESS_READ_BUFFER_1, SIZE_UNCOMPRESS_READ_BUFFER_2, 0);
		//tfw::IdFile::init(&fname, &postfix, SIZE_UNCOMPRESS_READ_BUFFER_1, SIZE_UNCOMPRESS_READ_BUFFER_2, IdFile::PIFF_STORED_ZIP);
	#endif

	// アプリケーション初期化
	if(!g_papp->Init(hInstance, &osdep, &psns, widthScreen, heightScreen, nCmdShow, aiflags))
	{
		tfw::File::cleanup();
		g_papp->Cleanup();
		delete g_papp;
		return -1;
	}

	// アプリケーション実行
	int res = g_papp->Do();

	// アプリケーション解放
	g_papp->Cleanup();
	delete g_papp;

	// ファイルモジュールの後処理
	tfw::IdFile::cleanup();
	tfw::File::cleanup();

	return res;
}

/*---------------------------------------------------------------------*//**
	@brief		メイン関数
	@param		hInstance		インスタンスハンドル
	@param		hPrevInstance	以前のインスタンスハンドル（常に NULL）
	@param		pszCmdLine		コマンドライン文字列
	@param		nCmdShow		ウインドウ表示状態指定値
	@return		EXE 応答値
**//*---------------------------------------------------------------------*/
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR pszCmdLine, int nCmdShow)
{
	// 実行
	int res = runApp(hInstance, pszCmdLine, nCmdShow);

	#if DBGMODE
		// メモリリーク検知
		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);	// doexit (crt0dat.c) 内で _CrtDumpMemoryLeaks が呼ばれるように
	#endif

	return res;
}

////////////////////////////////////////////////////////////////////////////
// クラスメンバ定義

namespace app
{

/*---------------------------------------------------------------------*//**
	@brief		コンストラクタ
**//*---------------------------------------------------------------------*/
App::App() :
	m_hInstance(NULL),
	m_pwnd(NULL),
	m_prendsys(NULL),
	m_pbody(NULL),
	m_swFrame(NULL),
	m_idTimer(NULL),
	m_cntTimer(0)
{
	::srand((u32)::time(0L));
}

/*---------------------------------------------------------------------*//**
	@brief		APP インスタンスポインタを得る
	@return		APP インスタンスポインタ
**//*---------------------------------------------------------------------*/
App* App::GetApp()
{
	return g_papp;
}

/*---------------------------------------------------------------------*//**
	@brief		初期化
	@param		hInstance		インスタンスハンドル
	@param		osdep			OS 依存部
	@param		nCmdShow		ウインドウ表示状態指定値	
	@param		aiflags			アプリ初期化フラグ
	@retval		true			成功
	@retval		false			失敗
**//*---------------------------------------------------------------------*/
bool App::Init(HINSTANCE hInstance, etk::OsDepMod* osdep, PsnsWin* psns, int widthScreen, int heightScreen, int nCmdShow, int aiflags)
{
	// ウインドウ作成
	m_pwnd = new MainWindow;
	if(!m_pwnd->Create(hInstance, (WNDPROC)WindowProcedureStatic, widthScreen, heightScreen, nCmdShow, aiflags))
	{
		return false;
	}

	// 描画システム作成
	m_prendsys = new RenderSystem;
	if(!m_prendsys->Init(m_pwnd->GetMainWindowHandle(), hInstance))
	{
		return false;
	}
	m_pwnd->SetRenderSystem(m_prendsys);

	// マルチメディアタイマーの初期化
	m_idTimer = ::timeSetEvent(
		1,											// 間隔 [ms]
		0,											// 分解能
		(LPTIMECALLBACK)TimerProcedureStatic,		// コールバック関数
		(DWORD)&m_cntTimer,							// ユーザーパラメータ
		TIME_PERIODIC | TIME_CALLBACK_FUNCTION	);	// 動作フラグ
	if(m_idTimer == NULL)
	{
		return false;
	}

	// ボディ生成フラグ
	u32 bcflags = 0;
	if(TFW_IS_FLAG(aiflags, AIF_FORCE_HD))				{	bcflags |= etk::EtkBody::BCF_HD_RESOLUTION;							}
	else if(TFW_IS_FLAG(aiflags, AIF_FORCE_SD))			{																		}
	else												{	bcflags |= (::rand() & 1) ? etk::EtkBody::BCF_HD_RESOLUTION : 0;	}
	if(TFW_IS_FLAG(aiflags, AIF_FORCE_RD))				{	bcflags |= etk::EtkBody::BCF_RD_RESOLUTION;							}
///	if(TFW_IS_FLAG(aiflags, AIF_FORCE_WD))				{	bcflags |= etk::EtkBody::BCF_WIDE_DISPLAY;							}
	if(TFW_IS_FLAG(aiflags, AIF_DEBUG_DOUBLE_FRAME))	{	bcflags |= etk::EtkBody::BCF_DEBUG_DOUBLE_FRAME;					}

	// 表示フラグ
	u32 dispflags = TFW_DISPF_RANG_0;
///	if(TFW_IS_FLAG(aiflags, AIF_FORCE_WD))				{	dispflags |= TFW_DISPF_WIDE;										}

	// 製品定義 ID
	etk::EtkBody* body = 0L;
	#if defined(_ENLIGHT_V01)
		body = new sourcerer::Game();
	#elif defined(_ENLIGHT_V01_LITE)
		body = new sourcerer::Game();
	#elif defined(_ENLIGHT_V02)
		body = new sourcerer::Game();
	#elif defined(_ENLIGHT_PRPZL01)
		body = new sourcerer::Game();
	#elif defined(_DIMZ)
		body = new sourcerer::Game();
	#elif defined(_ATAR)
		body = new atar::AtArBody();
	#elif defined(_BASICMODEL)
		body = new basicmodel::BasicModelBody();
	#elif defined(_EDEV)
		body = new edev::EdevBody();
	#elif defined(_PUZZLE_ICHTHYS)
		body = new ichthyspzl::IchthysPuzzleBody();
	#endif

	// ゲームの論理画面サイズ
	f32 widthLogical = (f32)m_prendsys->GetClientWidth();
	f32 heightLogical = (f32)m_prendsys->GetClientHeight();
	{
		f32 qw = widthLogical / WIDTH_DEFAULT;
		f32 qh = heightLogical / HEIGHT_DEFAULT;
		if((qw > 1.0f) && (qh > 1.0f))
		{
			f32 q = qw > qh ? qh : qw;
			widthLogical /= q;
			heightLogical /= q;
		}
	}

	// ゲームアプリケーション作成
	if((body == 0L) || !body->init(
		widthLogical,
		heightLogical,
		(f32)m_prendsys->GetClientWidth(),
		(f32)m_prendsys->GetClientHeight(),
		1.0f,
		dispflags,
		RENDER_SPF,
		LOGIC_SPF,
		etk::LANGID_JP,
		etk::EtkBody::SBCASE_OPEN,
		bcflags,
		osdep,
		psns	))
	{
		delete body;
		::MessageBox(NULL, _T("Game initialize error."), _T("App::Init"), MB_ICONERROR);
		return false;
	}

	// フレーム計測のためのストップウォッチを作成
	m_swFrame = new StopWatch();
	body->setStopWatch(m_swFrame);
	DebugWin::setBody(body);

	m_pbody = body;
	return true;
}

/*---------------------------------------------------------------------*//**
	@brief		解放
	@param		hInstance		インスタンスハンドル
	@param		nCmdShow		ウインドウ表示状態指定値	
	@retval		true			成功
	@retval		false			失敗
**//*---------------------------------------------------------------------*/
void App::Cleanup()
{
	// ゲームアプリケーション解放
	if(m_pbody != NULL)
	{
		m_pbody->cleanup(etk::EtkBody::TBCASE_CLOSE);
	}

	// ストップウォッチ破棄
	if(m_swFrame != NULL)
	{
		delete m_swFrame;
		m_swFrame = NULL;
	}

	// マルチメディアタイマーの解放
	if(m_idTimer != NULL)
	{
		::timeKillEvent(m_idTimer);
	}

	// 描画システム破棄
	if(m_prendsys != NULL)
	{
		m_prendsys->Cleanup();
		delete m_prendsys;
		m_prendsys = NULL;
	}

	// ウインドウ破棄
	if(m_pwnd != NULL)
	{
		m_pwnd->Destroy();
		delete m_pwnd;
		m_pwnd = NULL;
	}

	// ゲームアプリケーション破棄
	if(m_pbody != NULL)
	{
		delete m_pbody;
		m_pbody = NULL;
	}
}

/*---------------------------------------------------------------------*//**
	@brief		実行
	@return		メッセージループ戻り値
**//*---------------------------------------------------------------------*/
int App::Do()
{
	HACCEL hAccelTable;
	hAccelTable = ::LoadAccelerators(m_hInstance, (LPCTSTR)IDC_APP);

	LARGE_INTEGER qpfreq;
	QueryPerformanceFrequency(&qpfreq);

	// メインメッセージループ
	MSG winmsg;
	ZeroMemory(&winmsg, sizeof(MSG));
	while(winmsg.message != WM_QUIT)
	{
		if(::PeekMessage(&winmsg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&winmsg);
			::DispatchMessage(&winmsg);
		}
		else if(m_cntTimer >= 1.0f)
		{
			// フレーム進行度を計算する
			float frameDelta = 1.0f;
			float timeElapse = m_swFrame->getElapsedSeconds();
			if(timeElapse != 0.0f)	{	frameDelta = timeElapse * LOGIC_SPF;	}
			m_swFrame->start();		// この時点をフレームの開始時点とする

			// 実行
			m_pwnd->Exec();
			m_pbody->renderFrame();
			m_pbody->updateFrame(frameDelta);
			if(m_pbody->isExitRequest())
			{
				m_pwnd->Destroy();
			}
			m_prendsys->Render();

			m_cntTimer = 0.0f;
		}
	}

	return (int)winmsg.wParam;
}

/*---------------------------------------------------------------------*//**
	@brief		ウインドウプロシージャ（コールバック）
	@param		hWnd			ウインドウハンドル
	@param		messeage		メッセージ ID
	@param		wparam			パラメータ１（WPARAM）
	@param		lparam			パラメータ２（LPARAM）
	@return		メッセージ応答値
**//*---------------------------------------------------------------------*/
LRESULT CALLBACK App::WindowProcedureStatic(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	if(g_papp->m_pwnd != NULL)
	{
		if(g_papp->m_pwnd->GetMainWindowHandle() == hWnd)
		{
			return g_papp->m_pwnd->WindowProcedure(message, wparam, lparam);
		}
	}

	switch(message) 
	{
	case WM_COMMAND:
		{
			int wmId, wmEvent;
			wmId    = LOWORD(wparam); 
			wmEvent = HIWORD(wparam); 
			// 選択されたメニューの解析
			switch (wmId)
			{
			case IDM_ABOUT:
				::DialogBox(g_papp->m_hInstance, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)AboutDialogWindowProcedureStatic);
				break;
			}
		}
		break;
	}

	return ::DefWindowProc(hWnd, message, wparam, lparam);
}

/*---------------------------------------------------------------------*//**
	@brief		バージョン情報ボックスのメッセージハンドラ
	@param		hWnd			ウインドウハンドル
	@param		messeage		メッセージ ID
	@param		wparam			パラメータ１（WPARAM）
	@param		lparam			パラメータ２（LPARAM）
	@return		メッセージ応答値
**//*---------------------------------------------------------------------*/
LRESULT CALLBACK App::AboutDialogWindowProcedureStatic(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		if((LOWORD(wparam) == IDOK) || (LOWORD(wparam) == IDCANCEL))
		{
			::EndDialog(hWnd, LOWORD(wparam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

/*---------------------------------------------------------------------*//**
	@brief		マルチメディアタイマーのメッセージハンドラ
	@param		idTimer			タイマー ID
	@param		messeage		メッセージ ID
	@param		param			ユーザーパラメータ
**//*---------------------------------------------------------------------*/
void CALLBACK App::TimerProcedureStatic(UINT idTimer, UINT messeage, DWORD param, DWORD dummy1, DWORD dummy2)
{
	// タイマー割り込みの処理
	static const float FRAME_PER_MILLI_SECOND = RENDER_SPF * 0.001f;
	(*(float*)param) += FRAME_PER_MILLI_SECOND;
	return;
}

////////////////////////////////////////////////////////////////////////////

}	// namespace app
