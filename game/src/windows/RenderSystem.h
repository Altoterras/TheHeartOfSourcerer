/***********************************************************************//**
 *	RenderSystem.h
 *	Enlight Windows Application
 *
 *	Created by Ryoutarou Kishi on 2005/06/29.
 *	Copyright 2005 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#pragma once

////////////////////////////////////////////////////////////////////////////
// クラス定義

namespace app
{

/*---------------------------------------------------------------------*//**
	@brief		描画システムクラス
**//*---------------------------------------------------------------------*/
class RenderSystem
{
public:
	inline int				GetClientWidth() const		{	return m_widthClient;	}
	inline int				GetClientHeight() const		{	return m_heightClient;	}

public:
	RenderSystem();

	bool					Init(HWND hWnd, HINSTANCE hInstance);
	void					Cleanup();

	void					Render();

	void					OnResize(int width, int height);

private:
	void					SetupMatrices();

private:
	HWND					m_hwndTrg;
	HDC						m_hdcTrg;
	HGLRC					m_hrcTrg;

	int						m_widthClient;
	int						m_heightClient;
};

}	// namespace app

