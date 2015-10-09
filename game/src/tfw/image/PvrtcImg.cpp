/***********************************************************************//**
 *  PvrtcImg.cpp
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
#include "PvrtcImg.h"

// Friends
#include "../file/File.h"
#include "../file/IdFile.h"
#include "../file/MemoryFile.h"
#include "../string/CStringBase.h"
#include "../string/StringUtils.h"
#include "../collection/List.h"

// External

// Library
///#include <string.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

const u32 PVRTC_FLAG_TYPE_PVRTC_2	= 24;
const u32 PVRTC_FLAG_TYPE_PVRTC_4	= 25;
const u32 PVRTC_FLAG_TYPE_MASK		= 0xff;

////////////////////////////////////////////////////////////////////////////
// 構造体

struct PvrtcHeader
{
	u32	_sizeHeader;
	u32	_height;
	u32	_width;
	u32	_numMipmaps;
	u32	_flags;
	u32	_sizeData;
	u32	_bpp;
	u32	_bitmaskRed;
	u32	_bitmaskGreen;
	u32	_bitmaskBlue;
	u32	_bitmaskAlpha;
	u32	_tag;
	u32	_numSurfaces;
};

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PvrtcImg メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PvrtcImg::PvrtcImg() :
	_glid(0),
	_imgs(0L),
	_numImgs(0),
	_width(0),
	_height(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PvrtcImg::~PvrtcImg()
{
	ASSERT(_imgs == 0L);
}

/*---------------------------------------------------------------------*//**
	ファイルパスから作成
**//*---------------------------------------------------------------------*/
bool PvrtcImg::create(const CStringBase* filepath, u16 cflag)
{
	FTRACE("=== {PvrtcImg::create} 000.\n");
	File file(filepath, File::MODE_READ);
	if(!file.isOpened())
	{
		FTRACE("=== {PvrtcImg::create} -1.\n");
		return false;
	}

	FTRACE("=== {PvrtcImg::create} 002.\n");
	return create(&file, cflag);
}

/*---------------------------------------------------------------------*//**
	バッファから作成
**//*---------------------------------------------------------------------*/
bool PvrtcImg::create(const char* fbuf, int size, u16 cflag)
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
bool PvrtcImg::create(u32 fileid, u16 cflag)
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
bool PvrtcImg::create(FileBase* file, u16 cflag)
{
	u8* bufData = 0L;

	bool isSucceeded = false;
	do
	{
		// ヘッダを読み込む
		PvrtcHeader header;
		size_t sizeRead = file->read((void*)&header, sizeof(PvrtcHeader));
		if(sizeRead < sizeof(PvrtcHeader))	{	break;	}

		// ファイルフォーマット確認
		const char dslPvrtc[4] = { 'P', 'V', 'R', '!' };
		u32 tag = header._tag;
		if(dslPvrtc[0] != ((tag >>  0) & 0xff) || dslPvrtc[1] != ((tag >>  8) & 0xff) || dslPvrtc[2] != ((tag >> 16) & 0xff) || dslPvrtc[3] != ((tag >> 24) & 0xff))
		{
			break;
		}
	 
		// 画像フォーマットの確認
		u32 flagsFormat = header._flags & PVRTC_FLAG_TYPE_MASK;
		if((flagsFormat != PVRTC_FLAG_TYPE_PVRTC_4) && (flagsFormat != PVRTC_FLAG_TYPE_PVRTC_2))
		{
			break;
		}
		u32 format = 0;
		#if defined(_IOS)
			if(flagsFormat == PVRTC_FLAG_TYPE_PVRTC_4)			{	format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;	}
			else if(flagsFormat == PVRTC_FLAG_TYPE_PVRTC_2)		{	format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;	}
			else												{	break;											}
		#endif

		// 画像データの読み込み
		bufData = new u8[ header._sizeData ];
		sizeRead = file->read(bufData, header._sizeData);
		if(sizeRead < header._sizeData)		{	break;	}
		#if defined(_WINDOWS)
			break;	// 強制終了
		#endif

		// 基本データ
		u32 width       = header._width;
		u32 height      = header._height;
		///u32 hasAlpha    = header._bitmaskAlpha ? true : false;
 
		// 各ミップレベルの画像を読み込む
		u32			w			= width;
		u32			h			= height;
		u32			offset		= 0;
		u32			sizeData	= header._sizeData;
		u8*			bytes		= bufData;
		List<Img*>	listImg(false);
 		while(offset < sizeData)
		{
			u32   blockSize, widthBlocks, heightBlocks, bpp;
 
			if(flagsFormat == PVRTC_FLAG_TYPE_PVRTC_4)
			{
				blockSize    = 4 * 4;
				widthBlocks  = w / 4;
				heightBlocks = h / 4;
				bpp = 4;
			}
			else
			{
				blockSize    = 8 * 4;
				widthBlocks  = w / 8;
				heightBlocks = h / 4;
				bpp = 2;
			}
 
			if(widthBlocks < 2)		{	widthBlocks = 2;	}
			if(heightBlocks < 2)	{	heightBlocks = 2;	}
 
			Img* img = new Img();
			img->_size = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
			img->_bits = &bytes[offset];
			listImg.addTail(img);

			w = TFW_MAX(w >> 1, 1);
			h = TFW_MAX(h >> 1, 1);
			offset += img->_size;
		}
		Img** imgs = listImg.toArray();
		s32 numImgs = listImg.getCount();

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
				u32 w	= width;
				u32 h	= height;
				for(int i = 0; i < numImgs; i++)
				{
					#if defined(_IOS)
						::glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, imgs[i]->_size, imgs[i]->_bits);
					#endif
					w = TFW_MAX(w >> 1, 1);
					h = TFW_MAX(h >> 1, 1);
				}
				::glBindTexture(GL_TEXTURE_2D, 0);
			}
			else
			{
				TRACE("{PvrtcImg::create} glGenTextures failed. pxfmt=0x%08x, width=%d, height=%d\n", format, width, height);
			}
		}

		// ビット配列を保存するかどうか
		if(TFW_IS_FLAG(cflag, CFLAG_SAVE_BITS))	// 保存する
		{
			for(int i = 0; i < numImgs; i++)
			{
				// 後でデータバッファは削除されるので新しくバッファを確保しそこにコピーする
				u8* bitsNew = new u8[imgs[i]->_size];
				::memcpy(bitsNew, imgs[i]->_bits, imgs[i]->_size);
				imgs[i]->_bits = bitsNew;
			}
		}
		else									// 保存しない
		{
			for(int i = 0; i < numImgs; i++)
			{
				// 後でデータバッファは削除されるので NULL 化する
				imgs[i]->_bits = 0L;
			}
		}

		// 成功時メンバに保存
		_imgs = imgs;
		_numImgs = numImgs;
		_width = width;
		_height = height;
		_glid = glid;
		
		isSucceeded = true;
	}
	while(false);

	// データバッファを削除する
	delete[] bufData;

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PvrtcImg::destroy()
{
	for(int i = 0; i < _numImgs; i++)
	{
		delete _imgs[i];
	}
	delete[] _imgs;
	_imgs = 0L;

	// OpenGL テクスチャの削除
	if(_glid != 0)
	{
		::glDeleteTextures(1, &_glid);
		_glid = 0L;
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS