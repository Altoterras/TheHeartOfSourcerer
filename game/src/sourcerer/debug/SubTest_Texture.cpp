/***********************************************************************//**
 *	SubTest_Texture.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SubTest_Texture.h"

// Friends
#include "../body/Game.h"
#include "../gui/GameFontSet.h"
#include "../gui/GameGui.h"

// External
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラスメソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SubTest_Texture::SubTest_Texture()
	: _texRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SubTest_Texture::~SubTest_Texture()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SubTest_Texture::create()
{
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SubTest_Texture::destroy()
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SubTest_Texture::exec(ExecRes* res, const ExecCtx* ec)
{
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void SubTest_Texture::render(const RenderCtx* rc)
{
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void SubTest_Texture::draw(const RenderCtx* rc)
{
	Renderer* rdr = rc->getRenderer();
//	Texture* tex = Game::getGame()->getGui()->getGuiTexture();
	Texture* tex = Game::getGame()->getFontSet()->getTexture(GameFontSet::JP);

//	static float vh = 1.0f;
//	vh -= 0.01f;	if(vh < 0.0f)	{	vh = 1.0f;	}

	rdr->setSolidColor(255, 255, 255, 255);
	rdr->bindTexture(tex);
	RendererUtils::draw2dTextureRect(
		rdr,
		0,
		0,
		16,
		16,
		32.0f / 1024.0f,
		48.0f / 1024.0f,
		16.0f / 1024.0f,
		16.0f / 1024.0f	);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS