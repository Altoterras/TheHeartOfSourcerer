/***********************************************************************//**
 *	LoadingScreen.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/18.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_BODY_LOADING_SCREEN_H_
#define _ATAR_BODY_LOADING_SCREEN_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameScene.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;

TFW_END_NS

ATAR_BEGIN_NS

class LoadingScreenInner;

/*---------------------------------------------------------------------*//**
 *	ムービー画面
 *
**//*---------------------------------------------------------------------*/
class LoadingScreen : public FrameScene
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void begin();
	void end();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	LoadingScreen();
	~LoadingScreen();
	bool create(u32 fileid);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void draw(const RenderCtx* rc);				// 2D 描画

	//======================================================================
	// 変数
private:
	Texture* _tex;
	f32 _fcntAnim;
	f32 _fcntFade;
	bool _active;
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_BODY_LOADING_SCREEN_H_
