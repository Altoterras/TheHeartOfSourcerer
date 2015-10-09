/***********************************************************************//**
 *  Texture.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/27.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Texture.h"

// Friends
#include "TextureCtx.h"
#include "TexAnim.h"
#include "../image/BmpImg.h"
#include "../image/TgaImg.h"
#include "../image/PvrtcImg.h"
#include "../image/PngImg.h"
#include "../file/File.h"
#include "../file/IdFile.h"
#include "../file/MemoryFile.h"
#include "../lib/Vector.h"
#include "../string/CStringBase.h"
#include "../string/StringUtils.h"
#include "../framemod/ExecCtx.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Texture 静的変数

bool Texture::_isSetTrans = false;

//==========================================================================
// Texture メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	テクスチャアニメを関連付ける
**//*---------------------------------------------------------------------*/
void Texture::bindTexAnim(TexAnim* texanimRef)
{
	_texanimRef = texanimRef;
}

/*---------------------------------------------------------------------*//**
	テクスチャアニメのフレームをリセットする
**//*---------------------------------------------------------------------*/
void Texture::resetTexAnimFrame(TextureCtx* ctx)
{
	if(_texanimRef  == 0L)	{	return;	}
	_texanimRef->resetTexAnimFrame(ctx->_tactx);
}

/*---------------------------------------------------------------------*//**
	リソースプール管理に設定する
**//*---------------------------------------------------------------------*/
void Texture::markResourcePoolManage()
{
	_isManagedByRp = true;
}

/*---------------------------------------------------------------------*//**
	フォーマットを示す３文字から、フォーマット定数を得る
**//*---------------------------------------------------------------------*/
Texture::Format Texture::getFormatFrom3Char(char fc1, char fc2, char fc3)
{
	if(			(fc1 == 'B') && (fc2 == 'M') && (fc3 == 'P') )	{	return FMT_BMP;		}
	else if(	(fc1 == 'T') && (fc2 == 'G') && (fc3 == 'A') )	{	return FMT_TGA;		}
	else if(	(fc1 == 'P') && (fc2 == 'V') && (fc3 == 'R') )	{	return FMT_PVR;		}
	else if(	(fc1 == 'P') && (fc2 == 'N') && (fc3 == 'G') )	{	return FMT_PNG;		}
	else														{	return FMT_NULL;	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Texture::Texture()
	: _id(0)
	, _texuid(0)
	, _img(0L)
	, _width(0)
	, _height(0)
	, _texanimRef(0L)
	, _glid(0)
	, _isManagedByRp(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Texture::~Texture()
{
	ASSERT(_img == 0L);
}

/*---------------------------------------------------------------------*//**
	ファイルパスから作成
**//*---------------------------------------------------------------------*/
bool Texture::create(const CStringBase* filepath)
{
	// 拡張子で判定してロード
	VcString ext;
	StringUtils::divideFileName(0L, &ext, filepath);
	ImageBase* img = 0L;
	if(ext.equalsIgnoreCase("bmp"))			{	img = new BmpImg();		}
	else if(ext.equalsIgnoreCase("tga"))	{	img = new TgaImg();		}
	else if(ext.equalsIgnoreCase("pvr"))	{	img = new PvrtcImg();	}
	else if(ext.equalsIgnoreCase("png"))	{	img = new PngImg();		}
	else									{	return false;			}
	ASSERT(img != 0L);
	if(!img->create(filepath, ImageBase::CFLAG_GLTEXIMAGE))
	{
		delete img;
		return false;
	}
	return copyImageInfo(img);
}

/*---------------------------------------------------------------------*//**
	バッファから作成
**//*---------------------------------------------------------------------*/
bool Texture::create(const char* fbuf, int size, Format fmt)
{
	// 指定タイプでロード
	ImageBase* img = 0L;
	switch(fmt)
	{
	case FMT_BMP:	img = new BmpImg();		break;
	case FMT_TGA:	img = new TgaImg();		break;
	case FMT_PVR:	img = new PvrtcImg();	break;
	case FMT_PNG:	img = new PngImg();		break;
	default:		return false;
	}
	ASSERT(img != 0L);
	if(!img->create(fbuf, size, ImageBase::CFLAG_GLTEXIMAGE))
	{
		TRACE("{Texture::create} failed. fbuf=0x%08x, size=%d, fmt=%d\n", fbuf, size, fmt);
		delete img;
		return false;
	}
	return copyImageInfo(img);
}

/*---------------------------------------------------------------------*//**
	ファイル ID から作成
**//*---------------------------------------------------------------------*/
bool Texture::create(u32 fileid, Format fmt)
{
	// 指定タイプでロード
	ImageBase* img = 0L;
	switch(fmt)
	{
	case FMT_BMP:	img = new BmpImg();		break;
	case FMT_TGA:	img = new TgaImg();		break;
	case FMT_PVR:	img = new PvrtcImg();	break;
	case FMT_PNG:	img = new PngImg();		break;
	default:		return false;
	}
	ASSERT(img != 0L);
	if(!img->create(fileid, ImageBase::CFLAG_GLTEXIMAGE))
	{
		TRACE("{Texture::create} failed. fileid=0x%08x, fmt=%d\n", fileid, fmt);
		delete img;
		return false;
	}
	return copyImageInfo(img);
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Texture::destroy()
{
	if(_isManagedByRp)	// ResourcePool 管理の場合
	{
		_img = 0L;
	}
	else				// ResourcePool 管理に任せていない場合
	{
		if(_img != 0L)
		{
	 		_img->destroy();
	 		delete _img;
	 		_img = 0L;
		}
	}
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを作成する
**//*---------------------------------------------------------------------*/
bool Texture::createContext(TextureCtx* ctx)
{
	return ctx->create(_texanimRef);
}

/*---------------------------------------------------------------------*//**
	可変値コンテキストを破棄する
**//*---------------------------------------------------------------------*/
void Texture::destroyContext(TextureCtx* ctx)
{
	ctx->destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Texture::exec(const ExecCtx* ec, TextureCtx* texctx)
{
	// テクスチャアニメーションのフレーム更新
	if((_texanimRef != 0L) && (texctx != 0L))
	{
		_texanimRef->advNextFrame(texctx->_tactx, ec->getDeltaFrame());
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void Texture::render(TextureCtx* texctx)
{
	if(_glid == 0)	{	return;	}

	if((_texanimRef != 0L) && (texctx != 0L))
	{
		// テクスチャアニメーションの座標適用
		Vector2F trs;
		_texanimRef->getCurrentTranslate(texctx->_tactx, &trs);
		::glMatrixMode(GL_TEXTURE);
		::glLoadIdentity();
		::glTranslatef(trs.x(), trs.y(), 0.0f);
		::glMatrixMode(GL_MODELVIEW);
		_isSetTrans = true;

		::glBindTexture(GL_TEXTURE_2D, _glid);
	}
	else
	{
		if(_isSetTrans)
		{
			::glMatrixMode(GL_TEXTURE);
			::glLoadIdentity();
			::glMatrixMode(GL_MODELVIEW);
			_isSetTrans = false;
		}

		::glBindTexture(GL_TEXTURE_2D, _glid);
	}
}

/*---------------------------------------------------------------------*//**
	イメージ情報のコピー
**//*---------------------------------------------------------------------*/
bool Texture::copyImageInfo(ImageBase* img)
{
	_img = img;
	_width = img->getWidth();
	_height = img->getHeight();
	_glid = img->getGlTextureId();
	return true;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS