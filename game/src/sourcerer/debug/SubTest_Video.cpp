/***********************************************************************//**
 *	SubTest_Video.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SubTest_Video.h"

// Friends
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../mode/GameMode.h"

// External

// Library
#include "../../tfw/file/File.h"
#if defined(_WINDOWS)
#include "../../windows/App.h"
#include "../../windows/MainWindow.h"
#endif

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラスメソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SubTest_Video::SubTest_Video()
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SubTest_Video::~SubTest_Video()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SubTest_Video::create()
{
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SubTest_Video::destroy()
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SubTest_Video::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	if(gec->getGameMode()->getCurrentGameModeKind() == GameMode::MODE_GAME_MOVIE)
	{
		Mode* mode = gec->getGameMode()->getChildEndMode();
		if(mode->getFrameCounter() == 2)
		{
			#if defined(_WINDOWS)
			{
				do
				{
					VcString pathBase(File::getBasePath());
					VcString pathVideo = pathBase + "movie_test.mpg";
					VcString strCmd = VcString::format("open %s type mpegvideo alias enlight_video", pathVideo.getRaw());
					MCIERROR ret = ::mciSendString(strCmd, 0L, 0, 0L);
					if(ret != 0)
					{
						goto Error;
					}

					HWND hwndMain = 0L;
					{
						app::App* app = app::App::GetApp();
						if(app != 0L)
						{
							app::MainWindow* wndMain = app->GetMainWindow();
							if(wndMain != 0L)
							{
								hwndMain = wndMain->GetMainWindowHandle();
								strCmd = VcString::format("window enlight_video handle %d", hwndMain);
								ret = ::mciSendString(strCmd, 0L, 0, 0L);
								if(ret != 0)
								{
									goto Error;
								}
							}
						}
					}

					ret = ::mciSendString("play enlight_video notify", 0L, 0, hwndMain);
					if(ret != 0)
					{
						goto Error;
					}

					MSG msg;
					::ZeroMemory(&msg, sizeof(MSG));
					bool isContinue = true;
					while((msg.message != WM_QUIT) && isContinue)
					{
						/*
						if(::GetMessage(&msg, hwndMain, 0L, 0L) <= 0)
						{
							break;
						}

						if(msg.message == MM_MCINOTIFY)
						{
							isContinue = false;
						}

						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
						*/
						
						if(::PeekMessage(&msg, hwndMain, 0L, 0L, PM_REMOVE))
						{
							if(msg.message == MM_MCINOTIFY)
							{
								if(	(msg.wParam == MCI_NOTIFY_SUCCESSFUL)
									|| (msg.wParam == MCI_NOTIFY_FAILURE)
									|| (msg.wParam == MCI_NOTIFY_ABORTED) )
								{
									isContinue = false;
								}
							}

							::TranslateMessage(&msg);
							::DispatchMessage(&msg);
						}
						else
						{
							::Sleep(100);
						}
					}

					// . . . . . . . . . . . . . . . . . . . . . . . . . . .
					// 再生終了

					/*
						【2009/11/13 r-kishi】
						The instruction at 727885A8 tried to read from an invalid address, FEEEFEEE
						というエラーの原因不明．
						途中放棄状態である．
					*/
					TRACE("{SubTest_Video::exec} end play video.\n");
					ret = ::mciSendString("close enlight_video", 0L, 0, 0L);
					if(ret != 0)
					{
						goto Error;
					}

					break;

					// . . . . . . . . . . . . . . . . . . . . . . . . . . .
					// エラー発生時
				Error:
					char buf[256];
					::mciGetErrorString(ret, buf, sizeof(buf));
					TRACE(buf);
				}
				while(false);
			}
			#endif
		}
	}

}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void SubTest_Video::render(const RenderCtx* rc)
{
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void SubTest_Video::draw(const RenderCtx* rc)
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
