/***********************************************************************//**
 *	MainWindow.cpp
 *	Enlight Windows Application
 *	
 *	Created by Ryoutarou Kishi on 2009/06/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "BasicInclude.h"

// Self
#include "App.h"
#include "MainWindow.h"

// Friend
#include "Resource.h"					// IDS_APP_TITLE ...
#include "RenderSystem.h"
#include "DebugWin.h"
#if defined(_DEBUG)
	#include "../etk/debug/Test.h"
#endif

// External
#include "../etk/body/EtkBody.h"
#include "../tfw/Tfw.h"
#include "../tfw/gcmn/View.h"
#include "../tfw/lib/Point.h"
#include "../tfw/ui/Keyboard.h"
#include "../tfw/ui/TouchPanel.h"

namespace app
{

////////////////////////////////////////////////////////////////////////////
// クラスメンバ定義

//==========================================================================
// MainWindow

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MainWindow::MainWindow()
	: m_hMainWnd(NULL)
	, m_hSourceEditWnd(NULL)
	, m_prensysRef(0L)
{
	::memset(&m_rectWindowMode, 0, sizeof(RECT));
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MainWindow::~MainWindow()
{
	ASSERT(m_hMainWnd == NULL);
	ASSERT(m_hSourceEditWnd == NULL);
}

/*---------------------------------------------------------------------*//**
	作成
	@param		hInstance		インスタンスハンドル
	@param		nCmdShow		ウインドウ表示状態指定値	
	@retval		true			成功
	@retval		false			失敗
**//*---------------------------------------------------------------------*/
bool MainWindow::Create(HINSTANCE hInstance, WNDPROC pfuncWndProc, int widthScreen, int heightScreen, int nCmdShow, int aiflags)
{
	const int MAX_LOADSTRING = 100;
	TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
	TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

	// ウインドウ文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_APP, szWindowClass, MAX_LOADSTRING);

	// ウィンドウクラスを登録します。
	// アプリケーションが、関連付けられた 
	// 正しい形式の小さいアイコンを取得できるようにするには、
	// この関数を呼び出してください。
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)pfuncWndProc;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDC_APP);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= ::LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPICON);
	// メニューを付けたい場合は → wcex.lpszMenuName = (LPCTSTR)IDC_ENLIGHT;
	::RegisterClassEx(&wcex);

	// ウインドウサイズ
	DWORD styleWnd;
	int width, height;
	if(TFW_IS_FLAG(aiflags, App::AIF_FULL_SCREEN))
	{
		styleWnd = WS_POPUP | WS_BORDER;
		width = ::GetSystemMetrics(SM_CXSCREEN);
		height = ::GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		styleWnd = WS_OVERLAPPEDWINDOW;
		width = widthScreen;
		height = heightScreen;
#if 0
		if(TFW_IS_FLAG(aiflags, App::AIF_FORCE_RD))
		{
			width *= 2;
			height *= 2;
		}
		else if(TFW_IS_FLAG(aiflags, App::AIF_FORCE_HD))
		{
			width = 1024;
			height = 768;
		}
		if(TFW_IS_FLAG(aiflags, App::AIF_FORCE_WD))
		{
			width = (width * 16 * 2 / (9 * 3)) & ~1;
		}
#endif
	}

	// メインウインドウ作成
	m_hMainWnd = ::CreateWindow(
		szWindowClass,
		szTitle,
		styleWnd,
		CW_USEDEFAULT,
		0,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL	);
	if(m_hMainWnd == NULL)
	{
		return false;
	}

	// クライアントウインドウのサイズが、{WINDOW_HEIGHT, WINDOW_HEIGHT} になるように調整
	RECT rect;
	::GetClientRect(m_hMainWnd, &rect);
	rect.right = rect.left + width + (width - (rect.right - rect.left));
	rect.bottom = rect.top + height + (height - (rect.bottom - rect.top));
	::MoveWindow(m_hMainWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, FALSE);

	// ウインドウを表示
	::ShowWindow(m_hMainWnd, nCmdShow);
	::UpdateWindow(m_hMainWnd);

	// ウインドウの矩形を保存
	::GetWindowRect(m_hMainWnd, &m_rectWindowMode);

	// DirectX に登録
	#if defined(_DIRECTX)
	///DXUTSetWindow();
	#endif

	// ソースエディットウインドウを作成
	m_hSourceEditWnd = ::CreateWindowEx(
		WS_EX_CLIENTEDGE,
		TEXT("EDIT"),
		TEXT(""),
		WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE,
		0,
		0,
		0,
		0,
		m_hMainWnd,
		NULL,
		hInstance,
		NULL	);
	if(m_hSourceEditWnd == NULL)
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MainWindow::Destroy()
{
	if(m_hSourceEditWnd != NULL)
	{
		::DestroyWindow(m_hSourceEditWnd);
		m_hSourceEditWnd = NULL;
	}
	if(m_hMainWnd != NULL)
	{
		::DestroyWindow(m_hMainWnd);
		m_hMainWnd = NULL;
	}
}

/*---------------------------------------------------------------------*//**
	ウインドウプロシージャ（実装）
	@param		messeage		メッセージ ID
	@param		wparam			パラメータ１（WPARAM）
	@param		lparam			パラメータ２（LPARAM）
	@return		メッセージ応答値
**//*---------------------------------------------------------------------*/
LRESULT MainWindow::WindowProcedure(UINT message, WPARAM wparam, LPARAM lparam)
{
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
			case IDM_EXIT:
				Destroy();
				break;
			}
		}
		break;
#if 0
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = ::BeginPaint(m_hMainWnd, &ps);
			// TODO: 描画コードをここに追加してください...
			::EndPaint(m_hMainWnd, &ps);
		}
		break;
#endif
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_SIZE:
		{
			int width, height;
			if(wparam == SIZE_MAXIMIZED)
			{
				RECT rect;
				ChangeWindowMode(&rect, false);
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
			}
			else
			{
				width = LOWORD(lparam); 
				height = HIWORD(lparam); 

				// 最大化時以外にウインドウの矩形を保存する
				if(::GetWindowLong(m_hMainWnd, GWL_STYLE) & WS_MAXIMIZEBOX)
				{
					::GetWindowRect(m_hMainWnd, &m_rectWindowMode);
				}
			}
			TRACE(tfw::VcString::format("%d, %d", width, height));
			if(m_prensysRef != 0L)
			{
				u32 rotz = g_papp->GetBody()->getTfw()->getView()->getDisplayFlags() & TFW_DISPF_RANG_MASK;
				if((rotz == TFW_DISPF_RANG_270) || (rotz == TFW_DISPF_RANG_90))
				{
					m_prensysRef->OnResize(height, width);
				}
				else
				{
					m_prensysRef->OnResize(width, height);
				}
			}
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		MouseProcedure(message, wparam, lparam);
		break;
	case WM_KEYDOWN:
		KeyboardProcedure(message, wparam, lparam, true);
		break;
	case WM_KEYUP:
		KeyboardProcedure(message, wparam, lparam, false);
		break;
	case WM_SYSKEYDOWN:
		KeyboardProcedure(message, wparam, lparam, true);
		break;
	case WM_SYSKEYUP:
		KeyboardProcedure(message, wparam, lparam, false);
		break;
	case WM_CHAR:
		KeyboardCharProcedure(message, wparam, lparam);
		break;
	}

	return ::DefWindowProc(m_hMainWnd, message, wparam, lparam);
}

/*---------------------------------------------------------------------*//**
	ウインドウ モードの変更
	@param		rect			変更後の矩形を受け取るためのポインタ
	@param		isWindowMode	ウインドウ モードかどうか
								false の場合はフルスクリーン モード
**//*---------------------------------------------------------------------*/
void MainWindow::ChangeWindowMode(RECT* rect, bool isWindowMode)
{
	int xBorder, yBorder;
	POINT pt;
	RECT rc;

	if(isWindowMode)
	{
		// ウインドウ スタイル
		::SetWindowLong(m_hMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		///::SetWindowLong(m_hMainWnd, GWL_EXSTYLE, ::GetWindowLong(m_hMainWnd, GWL_EXSTYLE) & ~WS_EX_TOPMOST);

		xBorder = 0;
		yBorder = 0;
		::memcpy(&rc, &m_rectWindowMode, sizeof(RECT));
		///xBorder = ::GetSystemMetrics(SM_CXFRAME) - ::GetSystemMetrics(SM_CXBORDER);
		///yBorder = ::GetSystemMetrics(SM_CYFRAME) - ::GetSystemMetrics(SM_CYBORDER);
	}
	else
	{
		// フルスクリーン スタイル
		::SetWindowLong(m_hMainWnd, GWL_STYLE, WS_POPUP | WS_BORDER | WS_VISIBLE);
		///::SetWindowLong(m_hMainWnd, GWL_EXSTYLE, ::GetWindowLong(m_hMainWnd, GWL_EXSTYLE) | WS_EX_TOPMOST);

		xBorder = 0;
		yBorder = 0;
		rc.left = 0;
		rc.top = 0;
		rc.right = ::GetSystemMetrics(SM_CXSCREEN);
		rc.bottom = ::GetSystemMetrics(SM_CYSCREEN);
	}

	pt.x = rc.left + xBorder;
	pt.y = rc.top + yBorder;
	::ScreenToClient( ::GetParent( m_hMainWnd ), &pt );

	::SetWindowPos(
		m_hMainWnd,
		NULL,
		pt.x,
		pt.y,
		(rc.right - rc.left) - xBorder * 2,
		(rc.bottom - rc.top) - yBorder * 2,
		SWP_DRAWFRAME | SWP_NOZORDER );

	if(rect != 0L)
	{
		rect->left = pt.x;
		rect->top = pt.y;
		rect->right = rect->left + (rc.right - rc.left) - xBorder * 2;
		rect->bottom = rect->top + (rc.bottom - rc.top) - yBorder * 2;
	}
}

/*---------------------------------------------------------------------*//**
	マウス イベント プロシージャ
	@param		messeage		メッセージ ID
	@param		wparam			パラメータ１（WPARAM）
	@param		lparam			パラメータ２（LPARAM）
**//*---------------------------------------------------------------------*/
void MainWindow::MouseProcedure(UINT message, WPARAM wparam, LPARAM lparam)
{
	etk::EtkBody* body = g_papp->GetBody();
	if(body == 0L)	{	return;	}
	tfw::TouchPanel* pui = body->getTouchPanel();
	tfw::TouchPanel::Touch ptcArr[2];
	const tfw::View* view = body->getTfw()->getView();
	int numArr = 0;

	int xRaw = (int)(LOWORD(lparam));
	int yRaw = (int)(HIWORD(lparam));
	u32 rotz = g_papp->GetBody()->getTfw()->getView()->getDisplayFlags() & TFW_DISPF_RANG_MASK;
	int x, y;
	/*
	if((rotz == TFW_DISPF_RANG_270) || (rotz == TFW_DISPF_RANG_90))
	{
		y = xRaw;
		x = yRaw;
	}
	else
	*/
	{
		x = xRaw;
		y = yRaw;
	}

	switch(message) 
	{
	case WM_LBUTTONDOWN:
		::SetCapture(m_hMainWnd);
		m_ms.m_isLbtnDown = true;
		m_ms.m_ptNow.x = m_ms.m_ptClick.x = x;
		m_ms.m_ptNow.y = m_ms.m_ptClick.y = y;
		if(m_ms.MakeTouchArray((void**)&ptcArr, &numArr))	{	pui->beganTouches(ptcArr, numArr);	}
		break;
	case WM_LBUTTONUP:
		::ReleaseCapture();
		m_ms.m_isLbtnDown = false;
		if(m_ms.MakeTouchArray((void**)&ptcArr, &numArr))	{	pui->endedTouches(ptcArr, numArr);	}
		break;
	case WM_RBUTTONDOWN:
		::SetCapture(m_hMainWnd);
		m_ms.m_isRbtnDown = true;
		if(m_ms.MakeTouchArray((void**)&ptcArr, &numArr))	{	pui->beganTouches(ptcArr, numArr);	}
		break;
	case WM_RBUTTONUP:
		::ReleaseCapture();
		m_ms.m_isRbtnDown = false;
		if(m_ms.MakeTouchArray((void**)&ptcArr, &numArr))	{	pui->endedTouches(ptcArr, numArr);	}
		break;
	case WM_MOUSEMOVE:
		m_ms.m_ptNow.x = x;
		m_ms.m_ptNow.y = y;
		m_ms.m_isLbtnDown = TFW_IS_FLAG(wparam, MK_LBUTTON);
		m_ms.m_isRbtnDown = TFW_IS_FLAG(wparam, MK_RBUTTON);
		if(m_ms.m_isLbtnDown || m_ms.m_isRbtnDown)
		{
			RECT rect;
			::GetClientRect(m_hMainWnd, &rect);
			if(	(0 <= xRaw) && (xRaw < rect.right)
				&& (0 <= yRaw) && (yRaw < rect.bottom))
			{
				// ウインドウ内に入っている
				if(m_ms.MakeTouchArray((void**)&ptcArr, &numArr))	{	pui->movedTouches(ptcArr, numArr);	}
			}
			else
			{
				// ウインドウ外に移動した
				::ReleaseCapture();
				if(m_ms.MakeTouchArray((void**)&ptcArr, &numArr))	{	pui->cancelledTouches(ptcArr, numArr);	}
				m_ms.m_isLbtnDown = false;
				m_ms.m_isRbtnDown = false;
			}
		}
		pui->moveCursor((f32)x, (f32)y);
		break;
	case WM_MOUSEWHEEL:
		m_ms.m_ptNow.x = x;
		m_ms.m_ptNow.y = y;
		m_ms.m_isLbtnDown = m_ms.m_isRbtnDown = true;
		if(m_ms.MakeTouchArray((void**)&ptcArr, &numArr))								{	pui->beganTouches(ptcArr, numArr);	}
		if(m_ms.MakeWheelTouchArray((void**)&ptcArr, m_ms.m_ptNow, m_ms.m_rWheelCur))	{	pui->movedTouches(ptcArr, 2);		}
		int delta = GET_WHEEL_DELTA_WPARAM(wparam);
		m_ms.m_rWheelDelta = -delta / 10;	///- delta / 5;
		m_ms.m_cntWheelEnd = 2;
		break;
	}
}

/*---------------------------------------------------------------------*//**
	キーボード イベント プロシージャ
	@param		messeage		メッセージ ID
	@param		wparam			パラメータ１（WPARAM）
	@param		lparam			パラメータ２（LPARAM）
	@param		isDown			キーボードが押されたかどうか
**//*---------------------------------------------------------------------*/
void MainWindow::KeyboardProcedure(UINT message, WPARAM wparam, LPARAM lparam, bool isDown)
{
	etk::EtkBody* body = g_papp->GetBody();
	if(body == 0L)	{	return;	}
	tfw::Keyboard* kui = body->getKeyboard();

	int vkey = (int)wparam;
	int cntRepeat = lparam & 0xffff;
	switch(vkey)
	{
	case VK_LEFT:
	case VK_RIGHT:
		kui->setOn(vkey == VK_LEFT ? Keyboard::SYSKEY_LEFT : Keyboard::SYSKEY_RIGHT, isDown);
		if(isDown && (::GetKeyState(VK_CONTROL) < 0))
		{
			tfw::View* view = body->getTfw()->getView();
			RECT rectWnd, rectCl;
			::GetWindowRect(m_hMainWnd, &rectWnd);
			::GetClientRect(m_hMainWnd, &rectCl);
			// 回転変更
			int rotzOld = view->getRotZ();
			int rotzNew = 0;
			switch(vkey)
			{
			case VK_LEFT:
				if(rotzOld == TFW_DISPF_RANG_270)	{	rotzNew = TFW_DISPF_RANG_0;		}
				else								{	rotzNew = rotzOld + 1;			}
				break;
			case VK_RIGHT:
				if(rotzOld == TFW_DISPF_RANG_0)		{	rotzNew = TFW_DISPF_RANG_270;	}
				else								{	rotzNew = rotzOld - 1;			}
				break;
			}
			// ウインドウリサイズ
			int widthCl, heightCl;
			if((rotzOld == TFW_DISPF_RANG_0) || (rotzOld == TFW_DISPF_RANG_180))
			{
				widthCl = rectCl.right - rectCl.left;
				heightCl = rectCl.bottom - rectCl.top;
			}
			else
			{
				widthCl = rectCl.bottom - rectCl.top;
				heightCl = rectCl.right - rectCl.left;
			}
			int widthWnd, heightWnd;
			if((rotzNew == TFW_DISPF_RANG_0) || (rotzNew == TFW_DISPF_RANG_180))
			{
				widthWnd = widthCl;
				heightWnd = heightCl;
			}
			else
			{
				widthWnd = heightCl;
				heightWnd = widthCl;
			}
			widthWnd += (rectCl.left - rectWnd.left) + (rectWnd.right - rectCl.right);
			heightWnd += (rectCl.top - rectWnd.top) + (rectWnd.bottom - rectCl.bottom);
			::MoveWindow(m_hMainWnd, rectWnd.left, rectWnd.top, widthWnd, heightWnd, false);
			// ビューポートの変更
			::GetClientRect(m_hMainWnd, &rectCl);
			u32 dispflags = /*(view->getDisplayFlags() & TFW_DISPF_WIDE) |*/ rotzNew;
			if((rotzNew == TFW_DISPF_RANG_270) || (rotzNew == TFW_DISPF_RANG_90))
			{
				widthCl = rectCl.bottom - rectCl.top;
				heightCl = rectCl.right - rectCl.left;
				dispflags |= TFW_DISPF_ROT_COORD;
			}
			else
			{
				widthCl = rectCl.right - rectCl.left;
				heightCl = rectCl.bottom - rectCl.top;
			}
			body->getTfw()->resizeView((f32)widthCl, (f32)heightCl, dispflags);
		}
		break;
	case VK_UP:
	case VK_DOWN:
		kui->setOn(vkey == VK_UP ? Keyboard::SYSKEY_UP : Keyboard::SYSKEY_DOWN, isDown);
		break;
	case VK_PRIOR:	// PageUp
	case VK_NEXT:	// PageDown
		kui->setOn(vkey == VK_PRIOR ? Keyboard::SYSKEY_PGUP : Keyboard::SYSKEY_PGDOWN, isDown);
		break;
	case VK_HOME:
		kui->setOn(Keyboard::SYSKEY_HOME, isDown);
		break;
	case VK_END:
		kui->setOn(Keyboard::SYSKEY_END, isDown);
		break;
	case VK_F1:		case VK_F2:		case VK_F3:		case VK_F4:		case VK_F5:
	case VK_F6:		case VK_F7:		case VK_F8:		case VK_F9:		case VK_F10:
	case VK_F11:	case VK_F12:
		#if defined(_DEBUG)
			if(isDown)
			{
				// ファンクションキーによるテスト実行
				etk::Test* test = body->getTest();
				if(test != 0L)
				{
					test->onKeydownFunctionKey(vkey - VK_F1 + 1);
				}
			}
		#endif
		break;
	case VK_SHIFT:
		kui->setOn(Keyboard::SYSKEY_SHIFT, isDown);
		break;
	case VK_CONTROL:
		kui->setOn(Keyboard::SYSKEY_CTRL, isDown);
		break;
	case VK_MENU:
		kui->setOn(Keyboard::SYSKEY_ALT, isDown);
		break;
	case VK_ESCAPE:
		///kui->setOn(Keyboard::SYSKEY_ESC, isDown);
		// ウインドウモードへ復帰
		ChangeWindowMode(0L, true);
		break;
	case VK_DELETE:
		kui->setOn(Keyboard::SYSKEY_DEL, isDown);
		break;
	case VK_BACK:
		kui->setOn(Keyboard::SYSKEY_BS, isDown);
		break;
	case VK_SPACE:
		kui->setOn(' ', isDown);
		break;
	case VK_TAB:
		kui->setOn('\t', isDown);
		break;
	case VK_RETURN:
		kui->setOn('\n', isDown);
		break;
	case VK_PAUSE:
		if(cntRepeat == 1)	// リピートを無視して、最初の押下に反応
		{
			if(body->isBackground())
			{
				body->leaveBackground();
			}
			else
			{
				body->enterBackground();
			}
		}
		break;
	default:
		if((::GetKeyState(VK_CONTROL) < 0) || (::GetKeyState(VK_MENU) < 0))	// メモ：GetAsyncKeyState はハードウェアレベルで割り込み処理をして現在の状態を検出する
		{
			if(('A' <= vkey) && (vkey <= 'Z'))
			{
				kui->setOn(vkey - 'A' + 'a', isDown);
			}
			else if(('0' <= vkey) && (vkey <= '9'))
			{
				kui->setOn(vkey, isDown);
			}
		}
		break;

	}
}

/*---------------------------------------------------------------------*//**
	キーボード文字イベント プロシージャ
	@param		messeage		メッセージ ID
	@param		wparam			パラメータ１（WPARAM）
	@param		lparam			パラメータ２（LPARAM）
**//*---------------------------------------------------------------------*/
void MainWindow::KeyboardCharProcedure(UINT message, WPARAM wparam, LPARAM lparam)
{
	#if 1	// WM_CHAR を解釈する方法
	{
		if((0x20 <= wparam) && (wparam <= 0x7e))
		{
			etk::EtkBody* body = g_papp->GetBody();
			if(body != 0L)
			{
				tfw::Keyboard* kui = body->getKeyboard();
				if(kui != 0L)
				{
					kui->setOnOnlyOneFrame((int)wparam);
				}
			}
		}
	}
	#else	// WM_KEYDOWN を解釈する方法
	{
		if(::GetKeyState(VK_SHIFT))
		{
			if(vkey == '0')
			{
				return;
			}
			else if(('1' <= vkey) && (vkey <= ';'))
			{
				vkey -= 0x10;
			}
			else if((',' <= vkey) && (vkey <= '/'))
			{
				vkey += 0x10;
			}
			else if('@' == vkey)
			{
				vkey += 0x20;
			}
			else if(('A' <= vkey) && (vkey <= 'Z'))
			{
			}
			else if(('[' <= vkey) && (vkey <= '^'))
			{
				vkey += 0x20;
			}
			else if('\\' == vkey)
			{
				vkey = '_';
			}
			else switch(vkey)
			{
			case 0xba:	vkey = 0x2a;	break;	// '*'
			case 0xbb:	vkey = 0x2b;	break;	// '+'
			case 0xbc:	vkey = 0x3c;	break;	// '<'
			case 0xbd:	vkey = 0x3d;	break;	// '='
			case 0xbe:	vkey = 0x3e;	break;	// '>'
			case 0xbf:	vkey = 0x3f;	break;	// '?'
			case 0xc0:	vkey = 0x60;	break;	// '逆スラッシュ'
			case 0xdb:	vkey = 0x7b;	break;	// '{'
			case 0xdc:	vkey = 0x7c;	break;	// '|'
			case 0xdd:	vkey = 0x7d;	break;	// '}'
			case 0xde:	vkey = 0x7e;	break;	// '~'
			case 0xe2:	vkey = 0x5f;	break;	// '_'
			}
		}
		else
		{
			if(('A' <= vkey) && (vkey <= 'Z'))
			{
				vkey += 0x20; 
			}
			else switch(vkey)
			{
			case 0xba:	vkey = 0x3a;	break;	// ':'
			case 0xbb:	vkey = 0x3b;	break;	// ';'
			case 0xbc:	vkey = 0x2c;	break;	// ','
			case 0xbd:	vkey = 0x2d;	break;	// '-'
			case 0xbe:	vkey = 0x2e;	break;	// '.'
			case 0xbf:	vkey = 0x2f;	break;	// '/'
			case 0xc0:	vkey = 0x40;	break;	// '@'
			case 0xdb:	vkey = 0x5b;	break;	// '['
			case 0xdc:	vkey = 0x5c;	break;	// '\'
			case 0xdd:	vkey = 0x5d;	break;	// ']'
			case 0xde:	vkey = 0x5e;	break;	// '^'
			case 0xe2:	vkey = 0x5c;	break;	// '逆スラッシュ'
			}
		}
		TRACE("> %c\n", vkey);
		kui->setOn(vkey, isDown);
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	実行
**//*---------------------------------------------------------------------*/
void MainWindow::Exec()
{
	// ホイール終了のタッチキャンセルを送信
	if(m_ms.m_cntWheelEnd > 0)
	{
		tfw::TouchPanel* pui = g_papp->GetBody()->getTouchPanel();

		m_ms.m_cntWheelEnd--;
		if(m_ms.m_cntWheelEnd == 0)
		{
			tfw::TouchPanel::Touch ptcArr[2];
			pui->cancelledTouches(ptcArr, 2);
			m_ms.m_rWheelCur = MouseStat::DEF_RW;
			m_ms.m_isLbtnDown = m_ms.m_isRbtnDown = false;
		}
		else
		{
			tfw::TouchPanel::Touch ptcArr[2];
			m_ms.m_rWheelCur += m_ms.m_rWheelDelta;
			if(m_ms.MakeWheelTouchArray((void**)&ptcArr, m_ms.m_ptNow, m_ms.m_rWheelCur))	{	pui->movedTouches(ptcArr, 2);	}
		}
	}
}

//==========================================================================
// MainWindow::MouseStat

bool MainWindow::MouseStat::MakeTouchArray(void* parr, int* pnum)
{
	int numArr = (m_isLbtnDown && m_isRbtnDown) ? 2 : 1;
	tfw::TouchPanel::Touch* ptcArr = (tfw::TouchPanel::Touch*)parr;

	ptcArr[0]._pt->x() = (f32)m_ptNow.x;
	ptcArr[0]._pt->y() = (f32)m_ptNow.y;

	if(numArr >= 2)
	{
		ptcArr[1]._pt->x() = (f32)(m_ptClick.x - (m_ptNow.x - m_ptClick.x));
		ptcArr[1]._pt->y() = (f32)(m_ptClick.y - (m_ptNow.y - m_ptClick.y));
	}

	*pnum = numArr;
	return true;
}

bool MainWindow::MouseStat::MakeWheelTouchArray(void* parr, const POINT& ptCenter, int r)
{
	tfw::TouchPanel::Touch* ptcArr = (tfw::TouchPanel::Touch*)parr;

	ptcArr[0]._pt->x() = (f32)(ptCenter.x - r);
	ptcArr[0]._pt->y() = (f32)(ptCenter.y - r);
	ptcArr[1]._pt->x() = (f32)(ptCenter.x + r);
	ptcArr[1]._pt->y() = (f32)(ptCenter.y + r);

	return true;
}

}	// namespace app
