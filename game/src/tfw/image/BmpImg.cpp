/***********************************************************************//**
 *  BmpImg.cpp
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
#include "BmpImg.h"

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
// マクロ

#if !defined(_WINDOWS)	// maybe Apple
	#define PACKED __attribute__((packed))
#else
	#define PACKED
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BmpImg メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BmpImg::BmpImg() :
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
BmpImg::~BmpImg()
{
	ASSERT(_bits == 0L);
}

/*---------------------------------------------------------------------*//**
	ファイルパスから作成
**//*---------------------------------------------------------------------*/
bool BmpImg::create(const CStringBase* filepath, u16 cflag)
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
bool BmpImg::create(const char* fbuf, int size, u16 cflag)
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
bool BmpImg::create(u32 fileid, u16 cflag)
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
bool BmpImg::create(FileBase* file, u16 cflag)
{
	u8* bits = 0L;

	bool isSucceeded = false;
	do
	{
		// ファイルサイズを得る
		u32 lenFile = file->getLength();
		if(lenFile <= 0)						{	break;	}
		
		// ビットマップのファイルヘッダをメモり内に読み込む
		ASSERT(sizeof(BitmapFileHeader) == 14);
		BitmapFileHeader bmpfh;
		size_t sizeRead = file->read((void*)&bmpfh, sizeof(BitmapFileHeader));

		// ファイルが本当にビットマップであるかどうかをチェック
		if(sizeRead < sizeof(BitmapFileHeader))	{	break;	}
		if(bmpfh._type != 0x4d42)				{	break;	}

		// DIBのサイズを計算し、ファイルサイズからファイルヘッダのサイズを引く
		u32 sizeDib = lenFile - sizeof(BitmapFileHeader);

		// ビットマップにちょうど必要なメモリを割り当てる
		bits = new u8[sizeDib];

		// ビットマップをメモり内に読み込む
		sizeRead = file->read((void*)bits, sizeDib);
		if(sizeRead < sizeDib)					{	break;	}

		// BITMAPINFOHEADER構造体へのポインタを初期化する
		ASSERT(sizeof(BitmapInfoHeader) == 40);
		BitmapInfoHeader* pbmpih = (BitmapInfoHeader*)bits;

		// ビットマップのカラーテーブルへのポインタを計算する
		//RgbQuad* prgbtbl = (RgbQuad*)(bits + pbmpih->_size);

		// パレットの色数を得る
		int numPalleteColors = ((pbmpih->_clrUsed == 0) && (pbmpih->_bitcount <= 8)) ? (1 << pbmpih->_bitcount) : (int)pbmpih->_clrUsed;

		// ビットマップイメージのサイズを計算する
		if(pbmpih->_sizeImage == 0)
		{
			u32		bytewidth = (u32)pbmpih->_width;		// １列の幅をバイト単位で得る
			u32		height = (u32)pbmpih->_height;			// ビットマップの高さを得る
			pbmpih->_sizeImage = bytewidth * height;		// 列の数だけバイト幅をかける
		}
		else
		{
			pbmpih->_sizeImage = pbmpih->_sizeImage;		// それ以外の場合は、BITMAPINFOHEDER構造体に格納されているサイズをそのまま返す
		}

		// biClrUsedフィールドが、適切に初期化されていることを確認する
		if(pbmpih->_clrUsed == 0)	{	pbmpih->_clrUsed = numPalleteColors;	}

		// 実際のビットマップデータへのポインタを計算する
		u32 sizePallete = numPalleteColors * sizeof(RgbQuad);
		u8* pbits = bits + pbmpih->_size + sizePallete;

		if(pbmpih->_bitcount >= 8)
		{
			// １ピクセルのバイト数
			u32 bytesperpixel = pbmpih->_bitcount / 8;

			// BRG -> RGB
			for(u32 i= 0; i < pbmpih->_sizeImage; i += bytesperpixel)
			{
				u8 tmp = pbits[i];
				pbits[i] = pbits[i + 2];
				pbits[i + 2] = tmp;
			}

			#if 0
			// x y 入れ替え
			for(u32 y = 0; y < pbmpih->_height; y++)
			{
				for(u32 x = 0; x < pbmpih->_width; x++)
				{
					u32 i = (y * pbmpih->_width) + x;

				}
			}
			#endif
		}

		// ピクセルフォーマット
		u32 pxfmt = GL_RGB;

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
				::glTexImage2D(GL_TEXTURE_2D, 0, pxfmt, pbmpih->_width, pbmpih->_height, 0, pxfmt, GL_UNSIGNED_BYTE, bits);
				::glBindTexture(GL_TEXTURE_2D, 0);
			}
			else
			{
				TRACE("{BmpImg::create} glGenTextures failed. pxfmt=0x%08x, width=%d, height=%d\n", pxfmt, pbmpih->_width, pbmpih->_height);
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
		_bpp = pbmpih->_bitcount;
		_width = pbmpih->_width;
		_height = pbmpih->_height;
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
void BmpImg::destroy()
{
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