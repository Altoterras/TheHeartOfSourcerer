/***********************************************************************//**
 *	GamePauseScreen.cpp
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2012/09/04.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GamePauseScreen.h"

// Friends
#include "../body/Game.h"
#include "../body/GameFixedStrTbl.h"
#include "../body/GameRenderCtx.h"
#include "../file/GameFileIdDef.h"
#include "../gui/GameFontSet.h"

// External
#include "../../etk/body/OsDepMod.h"
#include "../../tfw/gcmn/EasyStringDrawer.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/lib/Color.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define W_PAUSE_TEX			(256.0f)
#define H_PAUSE_TEX			(256.0f)
#define W_IMG				(112.0f + 128.0f)
#define H_IMG_AND_MSG		(96.0f + 40.0f + 32.0f)
#define X_IMG_LEFT			((Game::getGame()->getLogicalWidth() - W_IMG) * 0.5f)
#define Y_IMG_AND_MSG_TOP	((Game::getGame()->getLogicalHeight() - H_IMG_AND_MSG) * 0.5f)
#define RECT_PAUSE			X_IMG_LEFT, Y_IMG_AND_MSG_TOP + 40.0f, 112.0f, 32.0f
#define UV_PAUSE			0.0f, 0.0f, 224.0f, 64.0f, W_PAUSE_TEX, H_PAUSE_TEX
#define RECT_KABAL			X_IMG_LEFT + 112.0f, Y_IMG_AND_MSG_TOP, 128.0f, 96.0f
#define UV_KABAL			0.0f, 64.0f, 256.0f, 192.0f, W_PAUSE_TEX, H_PAUSE_TEX
#define RECT_MSG			0.0f, Y_IMG_AND_MSG_TOP + 96.0f + 40.0f, (f32)Game::getGame()->getLogicalWidth(), 32.0f

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GamePauseScreen メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void GamePauseScreen::exec(ExecRes* res, const ExecCtx* ec)
{
	switch(_step)
	{
	default:
		break;
	case STEP_FADEIN:
		if(_fcntStep == 0.0f)
		{
			// 広告バナー ON
			Game::getGame()->getOsDepMod()->showAdBanner(true);
		}
		break;
	case STEP_FADEOUT:
		if(_fcntStep == 0.0f)
		{
			// 広告バナー OFF
			Game::getGame()->getOsDepMod()->showAdBanner(false);
		}
		break;
	}
	
	PauseScreen::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void GamePauseScreen::draw(const RenderCtx* rc)
{
	if(_tex == 0L)	{	return;	}

	switch(getStep())
	{
	case STEP_FADEIN:
	case STEP_STABLE:
	case STEP_FADEOUT:
		{
			f32 alpha = 1.0f;
			switch(getStep())
			{
			case STEP_FADEIN:
				alpha = getStepFrameCount() / 15.0f;
				if(alpha > 1.0f)	{	alpha = 1.0f;	}
				break;
			case STEP_FADEOUT:
				alpha = 1.0f - (getStepFrameCount() / 15.0f);
				if(alpha < 0.0f)	{	alpha = 0.0f;	}
				break;
			}

			RectF32 vtx, uv;
			ColorU8 col;
			GameRenderCtx* grc = (GameRenderCtx*)rc;
			Renderer* rdr = grc->getRenderer();
			Font* font = grc->getFontSet()->getFont(GameFontSet::JP);

			// 暗転幕
			col.set(0, 0, 0, (u8)(127.0f * alpha));
			RendererUtils::draw2dColorRect(rdr, RectF32::setout(&vtx, 0, 0, (f32)Game::getGame()->getLogicalWidth(), (f32)Game::getGame()->getLogicalHeight()), &col);

			// 絵
			rdr->bindTexture(_tex);	// テクスチャをバインド
			col.set(255, 255, 255, (u8)(255.0f * alpha));
			RendererUtils::draw2dColorTextureRect(rdr, RectF32::setout(&vtx, RECT_PAUSE), Gcalc::calcTexUv(&uv, UV_PAUSE, 1.0f), &col, &col, &col, &col);
			RendererUtils::draw2dColorTextureRect(rdr, RectF32::setout(&vtx, RECT_KABAL), Gcalc::calcTexUv(&uv, UV_KABAL, 1.0f), &col, &col, &col, &col);

			// メッセージ
			if(getStep() == STEP_STABLE)
			{
				rdr->setSolidColorForce(255, 255, 255, 255);
				EasyStringDrawer::draw(font, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_PAUSE_MESSAGE), RECT_MSG, 16, EasyStringDrawer::F_ALIGN_CENTER, rc);
			}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	テクスチャを読み込む
**//*---------------------------------------------------------------------*/
bool GamePauseScreen::loadTexture()
{
	u32 fileidTex = Env_isRdResolution() ? SYSTEM_PAUSESCREEN_RD_PNG : SYSTEM_PAUSESCREEN_HD_PNG;
	Texture* tex = new Texture();
	if(!tex->create(fileidTex, Texture::FMT_PNG))
	{
		delete tex;
		ASSERT(false);
		return false;
	}

	_tex = tex;
	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS