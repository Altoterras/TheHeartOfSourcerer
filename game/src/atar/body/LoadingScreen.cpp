/***********************************************************************//**
 *	LoadingScreen.cpp
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/18.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "LoadingScreen.h"

// Friends

// External
#include "../../etk/body/EtkBody.h"
#include "../../tfw/file/IdFile.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/lib/Color.h"

#if defined(_DEBUG)
#include "../../etk/body/EtkExecCtx.h"
#include "../../tfw/ui/TouchPanel.h"
#endif

// Library

////////////////////////////////////////////////////////////////////////////

ATAR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define FCNT_FADE_IN	(30.0f)
#define FCNT_FADE_OUT	(15.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// LoadingScreen メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void LoadingScreen::begin()
{
	if(_active)		{	return;	}
	_active = true;
	_fcntAnim = 0.0f;
	_fcntFade = 0.0f;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void LoadingScreen::end()
{
	if(!_active)	{	return;	}
	_active = false;
	_fcntFade = 0.0f;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
LoadingScreen::LoadingScreen()
	: _tex(0L)
	, _fcntAnim(0.0f)
	, _fcntFade(0.0f)
	, _active(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
LoadingScreen::~LoadingScreen()
{
	ASSERT(_tex == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool LoadingScreen::create(u32 fileid)
{
	// テクスチャ読み込み
	Texture* tex = new Texture();
	if(!tex->create(fileid, Texture::FMT_PNG))
	{
		delete tex;
		ASSERT(false);
		return false;
	}
	_tex = tex;
	
	// その他初期化
	_active = false;
	_fcntFade = FCNT_FADE_OUT;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void LoadingScreen::destroy()
{
	// テクスチャの削除
	if (_tex != 0L)
	{
		_tex->destroy();
		delete _tex;
		_tex = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void LoadingScreen::exec(ExecRes* res, const ExecCtx* ec)
{
#if defined(_DEBUG) && 0	// test
	EtkExecCtx* eec = (EtkExecCtx*)ec;
	TouchPanel* touchp = eec->getTouchPanel(); 
	if(!touchp->isReacted() && touchp->isTapRelease(0L, 0L))
	{
		if(_active)	{	end();		}
		else		{	begin();	}
	}
#endif

	_fcntAnim += ec->getDeltaFrame();
	_fcntFade += ec->getDeltaFrame();
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void LoadingScreen::draw(const RenderCtx* rc)
{
	// フェードイン、フェードアウト
	f32 alphaFade = 1.0f;
	if(_active)
	{
		if(_fcntFade < FCNT_FADE_IN)
		{
			// フェードイン
			alphaFade = _fcntFade / (f32)FCNT_FADE_IN;
		}
	}
	else
	{
		if(_fcntFade < FCNT_FADE_OUT)
		{
			// フェードアウト
			alphaFade = 1.0f - (_fcntFade / (f32)FCNT_FADE_OUT);
		}
		else
		{
			return;
		}
	}
	rc->getRenderer()->setSolidColor(255, 255, 255, (u8)(255.0f * alphaFade));

	f32 wScr = EtkBody::getBody()->getLogicalWidth();
	f32 hScr = EtkBody::getBody()->getLogicalHeight();
	
	// 暗転
	ColorU8 col(0, 0, 0, (u8)(63.0f * alphaFade));
	RendererUtils::draw2dColorRect(rc->getRenderer(), 0.0f, 0.0f, wScr, hScr, &col);

	// テクスチャ描画
	s32 ifcntAnim = (s32)(_fcntAnim * 0.5f);
	f32 ang = (f32)(ifcntAnim % 12) * 30.0f;
	f32 wThis = TFW_MIN(wScr, hScr);
	::glPushMatrix();
	::glTranslatef(wScr * 0.5f, hScr * 0.5f, 0.0f);
	::glRotatef(ang, 0.0f, 0.0f, 1.0f);
	rc->getRenderer()->bindTexture(_tex);
	RendererUtils::draw2dTextureRect(rc->getRenderer(), - wThis * 0.5f, - wThis * 0.5f, wThis, wThis, 0.0f, 0.0f, 1.0f, 1.0f);
	::glPopMatrix();

	rc->getRenderer()->setSolidColor(255, 255, 255, 255);
}

////////////////////////////////////////////////////////////////////////////

ATAR_END_NS