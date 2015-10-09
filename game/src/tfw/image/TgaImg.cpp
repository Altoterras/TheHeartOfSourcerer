/***********************************************************************//**
 *  TgaImg.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/09/01.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TgaImg.h"

// Friends
#include "../file/File.h"
#include "../file/IdFile.h"
#include "../file/MemoryFile.h"
#include "../string/CStringBase.h"
#include "../string/StringUtils.h"

// External

// Library
///#include <string.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TgaImg メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TgaImg::TgaImg() :
	_glid(0),
	_bits(0L),
	_bpp(0),
	_width(0),
	_height(0),
	_pxfmt(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TgaImg::~TgaImg()
{
	ASSERT(_bits == 0L);
}

/*---------------------------------------------------------------------*//**
	ファイルパスから作成
**//*---------------------------------------------------------------------*/
bool TgaImg::create(const CStringBase* filepath, u16 cflag)
{
	File file(filepath, File::MODE_READ);
	if(!file.isOpened())
	{
		return false;
	}

	return create(&file, cflag);
}

/*---------------------------------------------------------------------*//**
	バッファから作成
**//*---------------------------------------------------------------------*/
bool TgaImg::create(const char* fbuf, int size, u16 cflag)
{
	MemoryFile file(fbuf, size);
	if(!file.isOpened())
	{
		return false;
	}

	return create(&file, cflag);
}

/*---------------------------------------------------------------------*//**
	ファイル ID から作成
**//*---------------------------------------------------------------------*/
bool TgaImg::create(u32 fileid, u16 cflag)
{
	IdFile file(fileid);
	if(!file.isOpened())
	{
		return false;
	}

	return create(&file, cflag);
}

/*---------------------------------------------------------------------*//**
	ファイルから作成
**//*---------------------------------------------------------------------*/
bool TgaImg::create(FileBase* file, u16 cflag)
{
	u8* bits = 0L;

	bool isSucceeded = false;
	do
	{
		u8 tgaheaderDefault[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };	// 無圧縮の TGA ヘッダ
		u8 tgaheader[12];	// ヘッダの比較用
		size_t sizeRead = file->read(tgaheader, sizeof(tgaheader));
		if(sizeRead != sizeof(tgaheader))	{	break;	}	// 適正バイト数を読めたか

		if(::memcmp(tgaheaderDefault, tgaheader, sizeof(tgaheaderDefault))  != 0)	{	break;	}  // ヘッダの内容確認

		u8 infoheader[6];
		sizeRead = file->read(infoheader, sizeof(infoheader));
		if(sizeRead != sizeof(infoheader))	{	break;	}	// 適正バイト数を読めたか

		// 画像の幅 (上位バイト *  256  + 下位バイト) 
		u32 width  = infoheader[1] * 256 + infoheader[0];

		// 画像の高さ (上位バイト * 256 + 下位バイト) 
		u32 height = infoheader[3] * 256 + infoheader[2];

		// 画像の bpp（24 か 32 か）
		u32 bpp  = infoheader[4];

		// ピクセルフォーマット
		u32 pxfmt = GL_RGBA;
		if(bpp == 24)	{	pxfmt = GL_RGB;	}	// 24 bit / pixel

		// 不正データチェック
		if(width <= 0 || height <= 0 || ((bpp != 24) && (bpp != 32)))	{	break;	}

		// １ピクセルのバイト数
		u32 bytesperpixel = bpp / 8;

		// イメージサイズの計算
		u32 sizeImg = width * height * bytesperpixel;

		// メモリ確保
		bits = new u8[sizeImg];
		if(bits == 0L)	{	break;	}

		// ビット列を読み込む
		if(file->read(bits, sizeImg) != sizeImg)	{	break;	}

		// BRG -> RGB
		for(u32 i = 0; i < sizeImg; i += bytesperpixel)
		{
			u8 tmp = bits[i];
			bits[i] = bits[i + 2];
			bits[i + 2] = tmp;

			//bits[i + 0] = 0;
			//bits[i + 1] = 0;
			//bits[i + 2] = 0xff;
		}

		// OpenGL テクスチャの作成
		u32 glid = 0;
		if(TFW_IS_FLAG(cflag, CFLAG_GLTEXIMAGE))
		{
			::glGenTextures(1, &glid);
			if(glid != 0L)
			{
				::glBindTexture(GL_TEXTURE_2D, glid);
				::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				::glTexImage2D(GL_TEXTURE_2D, 0, pxfmt, width, height, 0, pxfmt, GL_UNSIGNED_BYTE, bits);
				::glBindTexture(GL_TEXTURE_2D, 0);
			}
			else
			{
				TRACE("{TgaImg::create} glGenTextures failed. pxfmt=0x%08x, width=%d, height=%d\n", pxfmt, width, height);
			}
		}

		// ビット配列を削除する
		if(!TFW_IS_FLAG(cflag, CFLAG_SAVE_BITS))
		{
			delete[] bits;
			bits = 0L;
		}

		// 成功時メンバに保存
		_bits = bits;
		_bpp = bpp;
		_width = width;
		_height = height;
		_pxfmt = pxfmt;
		_glid = glid;

		isSucceeded = true;
	}
	while(false);

	if(!isSucceeded)	{	delete[] bits;	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void TgaImg::destroy()
{
	// ビット配列を削除
	delete[] _bits;
	_bits = 0L;

	// OpenGL テクスチャの削除
	if(_glid != 0)
	{
		::glDeleteTextures(1, &_glid);
		_glid = 0L;
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS