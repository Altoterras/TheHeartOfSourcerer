/***********************************************************************//**
 *	MainWindow.h
 *	Enlight Windows Application
 *	
 *	Created by Ryoutarou Kishi on 2009/06/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#pragma once

////////////////////////////////////////////////////////////////////////////
// クラス定義

namespace app
{

class RenderSystem;

/*---------------------------------------------------------------------*//**
	@brief		メインウインドウクラス
**//*---------------------------------------------------------------------*/
class MainWindow
{
public:
	class MouseStat
	{
	public:
		static const int	DEF_RW = 10000;

		bool	m_isLbtnDown;
		bool	m_isRbtnDown;
		POINT	m_ptClick;
		POINT	m_ptNow;
		int		m_rWheelCur;
		int		m_rWheelDelta;
		int		m_cntWheelEnd;

		MouseStat() : m_isLbtnDown(false), m_isRbtnDown(false), m_rWheelCur(DEF_RW), m_rWheelDelta(0), m_cntWheelEnd(0)
		{
			ZeroMemory(&m_ptClick, sizeof(POINT));
			ZeroMemory(&m_ptNow, sizeof(POINT));
		}

		bool MakeTouchArray(void* parr, int* pnum);
		bool MakeWheelTouchArray(void* parr, const POINT& ptCenter, int r);
	};

public:
	MainWindow();
	~MainWindow();

	bool					Create(HINSTANCE hInstance, WNDPROC pfuncWndProc, int widthScreen, int heightScreen, int nCmdShow, int aiflags);
	void					Destroy();

	void					Exec();

	HWND					GetMainWindowHandle()					{	return m_hMainWnd;			}
	HWND					GetSourceEditWindowHandle()				{	return m_hSourceEditWnd;	}
	void					SetRenderSystem(RenderSystem* prensys)	{	m_prensysRef = prensys;		}

	LRESULT					WindowProcedure(UINT message, WPARAM wparam, LPARAM lparam);

private:
	void					ChangeWindowMode(RECT* rect, bool isWindowMode);
	void					MouseProcedure(UINT message, WPARAM wparam, LPARAM lparam);
	void					KeyboardProcedure(UINT message, WPARAM wparam, LPARAM lparam, bool isDown);
	void					KeyboardCharProcedure(UINT message, WPARAM wparam, LPARAM lparam);
	
private:
	HWND					m_hMainWnd;
	HWND					m_hSourceEditWnd;
	RenderSystem*			m_prensysRef;
	MouseStat				m_ms;
	RECT					m_rectWindowMode;
};

}	// namespace app