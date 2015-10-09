/***********************************************************************//**
 *	RenderSystem.cpp
 *	Enlight Windows Application
 *
 *	Created by Ryoutarou Kishi on 2005/06/29.
 *	Copyright 2005 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

//--------------------------------------------------------------------------
// Basic

#include "BasicInclude.h"

//--------------------------------------------------------------------------
// Self

#include "RenderSystem.h"

//--------------------------------------------------------------------------
// Friend

#include "DebugWin.h"
#include "App.h"

//--------------------------------------------------------------------------
// External

#include "../etk/body/EtkBody.h"

//--------------------------------------------------------------------------
// Library

#include <mmsystem.h>
#include <gl/GL.h>
#if defined(_WINDOWS)
#include <OpenGL/glext.h>
#else
#include <glext.h>
#endif

namespace app
{

////////////////////////////////////////////////////////////////////////////
// クラスメンバ定義

/*---------------------------------------------------------------------*//**
	@brief		コンストラクタ
**//*---------------------------------------------------------------------*/
RenderSystem::RenderSystem()
	: m_hwndTrg(NULL)
	, m_hdcTrg(NULL)
	, m_hrcTrg(NULL)
	, m_widthClient(0)
	, m_heightClient(0)
{
}

/*---------------------------------------------------------------------*//**
	初期化
	@param		hWnd			ウインドウハンドル
	@param		hInstance		アプリケーションインスタンスハンドル
	@retval		true			成功
	@retval		false			失敗
**//*---------------------------------------------------------------------*/
bool RenderSystem::Init(HWND hwnd, HINSTANCE hInstance)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd 
		1,								// version number 
		PFD_DRAW_TO_WINDOW |			// support window 
		PFD_SUPPORT_OPENGL |			// support OpenGL 
		PFD_DOUBLEBUFFER,				// double buffered 
		PFD_TYPE_RGBA,					// RGBA type 
		24,								// 24-bit color depth 
		0, 0, 0, 0, 0, 0,				// color bits ignored 
		0,								// no alpha buffer 
		0,								// shift bit ignored 
		0,								// no accumulation buffer 
		0, 0, 0, 0,						// accum bits ignored 
		16,								// 16-bit z-buffer 
		0,								// no stencil buffer 
		0,								// no auxiliary buffer 
		PFD_MAIN_PLANE,					// main layer 
		0,								// reserved 
		0, 0, 0							// layer masks ignored 
	};

	m_hwndTrg = hwnd;
	m_hdcTrg = ::GetWindowDC(hwnd);

	// ウインドウサイズを取得する
	RECT rectClientWnd;
	::GetClientRect(m_hwndTrg, &rectClientWnd);

	// ピクセルフォーマットを指定する
	int pxformat = ::ChoosePixelFormat(m_hdcTrg, &pfd);
	::SetPixelFormat(m_hdcTrg, pxformat, &pfd);

	// OpenGL 初期化
	m_hrcTrg = ::wglCreateContext(m_hdcTrg);
	::wglMakeCurrent(m_hdcTrg, m_hrcTrg);
	initGlext();

	// 画面のクリア
	::glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	::glClearDepth(1.0f);

	// 画面サイズを保存
	m_widthClient = rectClientWnd.right - rectClientWnd.left;
	m_heightClient = rectClientWnd.bottom - rectClientWnd.top;

	return true;
}

/*---------------------------------------------------------------------*//**
	@brief		解放
**//*---------------------------------------------------------------------*/
void RenderSystem::Cleanup()
{
	if((m_hwndTrg != NULL) && (m_hdcTrg != NULL) && (m_hrcTrg != NULL))
	{
		::wglMakeCurrent(m_hdcTrg, 0);
		::wglDeleteContext(m_hrcTrg);
		::ReleaseDC(m_hwndTrg, m_hdcTrg);
		m_hwndTrg = NULL;
		m_hdcTrg = NULL;
		m_hrcTrg = NULL;
	}
}

/*---------------------------------------------------------------------*//**
	@brief		描画
**//*---------------------------------------------------------------------*/
void RenderSystem::Render()
{
	::SwapBuffers(m_hdcTrg);
}

/*---------------------------------------------------------------------*//**
	@brief		サイズ変更通知
**//*---------------------------------------------------------------------*/
void RenderSystem::OnResize(int width, int height)
{
	g_papp->GetBody()->resizeView((f32)width, (f32)height);
}

}	// namespace app
