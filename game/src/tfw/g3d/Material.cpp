/***********************************************************************//**
 *  Material.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/30.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Material.h"

// Friends
#include "MaterialCtx.h"
#include "../gcmn/Texture.h"
#include "../gcmn/TexAnim.h"
#include "../gcmn/Renderer.h"
#include "../lib/Vector.h"
#include "../string/VcString.h"
#include "../framemod/ExecCtx.h"
#include "../framemod/RenderCtx.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Material メソッド

/*---------------------------------------------------------------------*//**
	テクスチャアニメのフレームをリセットする
**//*---------------------------------------------------------------------*/
void Material::resetTexAnimFrame(MaterialCtx* ctx)
{
	if(_textureRef  == 0L)	{	return;	}
	_textureRef->resetTexAnimFrame(ctx->_texctx);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Material::Material()
	: _id(0)
	, _colorDiffuse(0L)
	, _colorSpecular(0L)
	, _colorAmbient(0L)
	, _opacity(1.0f)
	, _textureRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Material::~Material()
{
	ASSERT(_colorDiffuse == 0L);
	ASSERT(_colorSpecular == 0L);
	ASSERT(_colorAmbient == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Material::create(Vector3F* colorDiffuseEntr, Vector3F* colorSpecularEntr, Vector3F* colorAmbientEntr, f32 opacity, Texture* textureRef)
{
	_colorDiffuse = colorDiffuseEntr;
	_colorSpecular = colorSpecularEntr;
	_colorAmbient = colorAmbientEntr;
	_opacity = opacity;
	_textureRef = textureRef;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Material::destroy()
{
	delete _colorDiffuse;
	_colorDiffuse = 0L;
	delete _colorSpecular;
	_colorSpecular = 0L;
	delete _colorAmbient;
	_colorAmbient = 0L;
	_textureRef = 0L;
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを作成する
**//*---------------------------------------------------------------------*/
bool Material::createContext(MaterialCtx* ctx)
{
	return ctx->create(_textureRef);
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを破棄する
**//*---------------------------------------------------------------------*/
void Material::destroyContext(MaterialCtx* ctx)
{
	ctx->destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Material::exec(const ExecCtx* ec, MaterialCtx* mtrlctx)
{
	// テクスチャのフレーム制御
	if(_textureRef != 0L)
	{
		_textureRef->exec(ec, mtrlctx->_texctx);
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void Material::render(const RenderCtx* rc, MaterialCtx* mtrlctx)
{
	if(_textureRef != 0L)
	{
		rc->getRenderer()->bindTexture(_textureRef, mtrlctx->_texctx, false);
	}
	else
	{
		rc->getRenderer()->bindTexture(0L);
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS