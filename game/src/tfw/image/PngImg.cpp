/***********************************************************************//**
 *  PngImg.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2010/03/18.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PngImg.h"

// Friends
#include "../collection/InsArray.h"
#include "../file/File.h"
#include "../file/IdFile.h"
#include "../file/MemoryFile.h"
#include "../string/CStringBase.h"
#include "../string/StringUtils.h"

// External

// Library
#include <lpng/png.h>
#include <setjmp.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 静的関数

#if SUPPORT_EXTLIB_LPNG

/*---------------------------------------------------------------------*//**
	ファイル読み込みコールバック関数
**//*---------------------------------------------------------------------*/
static void cbFileRead(png_structp png, png_bytep data, png_size_t length)
{
	FileBase* file = (FileBase*)png_get_io_ptr(png);
	file->read(data, (u32)length);
}

/*---------------------------------------------------------------------*//**
	4 色形式→ 16 色形式変換コールバック関数
**//*---------------------------------------------------------------------*/
static void cb4To16bpp(png_structp png, png_row_infop row_info, png_bytep data)
{
	static const png_byte pix[] =
	{
		0x00, 0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13,
		0x20, 0x21, 0x22, 0x23, 0x30, 0x31, 0x32, 0x33,
	};
	png_uint_32 rowb;
	png_byte *p, *q, c;

	rowb = (row_info->width + 1) / 2;
	q = data + rowb;
	p = data + rowb / 2;

	if (rowb % 2 == 1) {
		c = *p;
		*(--q) = pix[c >> 4];
	}
	while (p > data) {
		c = *(--p);
		*(--q) = pix[c & 0x0f];
		*(--q) = pix[c >> 4];
	}
	row_info->bit_depth   = 4;
	row_info->pixel_depth = 4;
	row_info->rowbytes    = rowb;
}

#endif

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PngImg メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	保持されているビット配列を削除する
**//*---------------------------------------------------------------------*/
void PngImg::deleteBits()
{
	delete[] _bits;
	_bits = 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PngImg::PngImg() :
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
PngImg::~PngImg()
{
	ASSERT(_bits == 0L);
}

/*---------------------------------------------------------------------*//**
	ファイルパスから作成
**//*---------------------------------------------------------------------*/
bool PngImg::create(const CStringBase* filepath, u16 cflag)
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
bool PngImg::create(const char* fbuf, int size, u16 cflag)
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
bool PngImg::create(u32 fileid, u16 cflag)
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
bool PngImg::create(FileBase* file, u16 cflag)
{
	u8* bits = 0L;

	bool isSucceeded = false;
	do
	{
		#if SUPPORT_EXTLIB_LPNG
			// LIBPNG 情報構造体の準備
			png_struct* png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0L, 0L, 0L);
			if(png == 0L)
			{
				TRACE("{PngImg::create} png_create_read_struct failed. file=0x%08x, cflag=0x%04x\n", file, cflag);
				break;
			}
			png_info* pnginf = png_create_info_struct(png);
			if(pnginf == 0L)
			{
				TRACE("{PngImg::create} png_create_info_struct failed. (pnginf) png=0x%08x\n", png);
				png_destroy_read_struct(&png, 0L, 0L);
				break;
			}
			png_info* pngend = png_create_info_struct(png);
			if(pngend == 0L)
			{
				TRACE("{PngImg::create} png_create_info_struct failed. (pngend) png=0x%08x\n", png);
				png_destroy_read_struct(&png, &pnginf, 0L);
				break;
			}

			// 読み込み関数をセットする
			png_set_read_fn(png, (void*)file, cbFileRead);

			// 各種情報をヘッダから読み込む
			png_uint_32 width, height;
			int numDepth, numPalette;
			int nPngDepth, nColorType, nInterlaceType, nCompType, nFilterType;
			png_read_info(png, pnginf);
			png_get_IHDR(png, pnginf, &width, &height, &nPngDepth, &nColorType, &nInterlaceType, &nCompType, &nFilterType);

			// 色数、ピクセルフォーマット
			u32 pxfmt = GL_RGB;
			if(nColorType == PNG_COLOR_TYPE_RGB)
			{
				nPngDepth = 24;
				numDepth = 24;
				numPalette = 0;
				pxfmt = GL_RGB;
			}
			else if(nColorType == PNG_COLOR_TYPE_RGB_ALPHA)
			{
				nPngDepth = 32;
				numDepth = 32;
				numPalette = 0;
				pxfmt = GL_RGBA;
			}
			else
			{
				switch(nPngDepth)
				{
				case 2:		numDepth = 4;			break;
				case 16:	numDepth = 8;			break;
				default:	numDepth = nPngDepth;	break;
				}
				numPalette = 1 << numDepth;
			}

			// パレット配列の用意
			png_color* palets = 0L;
			if(numPalette > 0)
			{
				palets = new png_color[numPalette];
			}

			// ビット配列の用意
			int nRowBytes = (width * numDepth + 31) / 32 * 4;
			int nImgBytes = nRowBytes * height;
			u8* bits = new u8[nImgBytes];
			u8** rows = new u8*[height];
			s32 cntRowPtr = 0;
			for(int y = height - 1; y >= 0; --y)
			{
				rows[cntRowPtr++] = bits + (y * nRowBytes);
			}

			// 各種設定
			/*
			if(nColorType & PNG_COLOR_MASK_ALPHA)
			{
				png_set_strip_alpha(png);
			}
			*/
			if(nPngDepth == 2)
			{
				png_set_read_user_transform_fn(png, cb4To16bpp);
			}
			else if(nPngDepth == 16)
			{
				png_set_strip_16(png);
			}
			/*
			if(nColorType== PNG_COLOR_TYPE_RGB || nColorType == PNG_COLOR_TYPE_RGB_ALPHA)
			{
				png_set_bgr(png);
			}
			*/
			if(png_get_valid(png, pnginf, PNG_INFO_tRNS))
			{
				png_set_tRNS_to_alpha(png);
			}

			png_read_update_info(png, pnginf);

			// パレット情報を作成する
			if(numPalette > 0)
			{
				if(nColorType == PNG_COLOR_TYPE_PALETTE)
				{
					png_color *palette;
					int num_palette;
					png_get_PLTE(png, pnginf, &palette, &num_palette);
					if (num_palette > numPalette)
					{
						num_palette = numPalette;
					}
					memset(palets,       0, numPalette * sizeof(png_color));
					memcpy(palets, palette, num_palette * sizeof(png_color));
				}
				else
				{
					int numDepth = (nPngDepth == 16) ? 8 : nPngDepth;
					png_build_grayscale_palette(numDepth, palets);
				}
			}

			// イメージの読み込み
			png_read_image(png, (png_bytepp)rows);

			// LIBPNG 処理終了
			png_read_end(png, pngend);
			png_destroy_read_struct(&png, &pnginf, &pngend);
			delete[] palets;
			delete[] rows;

			// 強制的に２のべき乗サイズに変換
			#if defined(_FORCE_TEXTURE_POWER)
			{
				u32 widthPow = calcPowerValue(width);
				u32 heightPow = calcPowerValue(height);
				if((width != widthPow) || (height != heightPow))
				{
					int nRowBytesPow = (widthPow * numDepth + 31) / 32 * 4;
					int nImgBytesPow = nRowBytesPow * heightPow;
					u8* bitsPow = new u8[nImgBytesPow];
					::memset(bitsPow, 0, sizeof(u8) * nImgBytesPow);
					u32 wL = (widthPow - width) / 2;
					u32 hT = (heightPow - height) / 2;
					u32 xL = (wL * numDepth + 31) / 32 * 4;
					u32 yOrg = 0;
					u32 yPow = hT * nRowBytesPow;
					for(u32 j = 0; j < height; j++)
					{
						::memcpy(&bitsPow[yPow + xL], &bits[yOrg], nRowBytes);
						yOrg += nRowBytes;
						yPow += nRowBytesPow;
					}
					delete[] bits;
					bits = bitsPow;
					width = widthPow;
					height = heightPow;
				}
			}
			#endif	// #if defined(_FORCE_TEXTURE_POWER)

			// OpenGL テクスチャの作成
			u32 glid = 0;
			if(TFW_IS_FLAG(cflag, CFLAG_GLTEXIMAGE))
			{
				::glGenTextures(1, &glid);
				if(glid != 0L)
				{
					::glBindTexture(GL_TEXTURE_2D, glid);
					::glTexImage2D(GL_TEXTURE_2D, 0, pxfmt, width, height, 0, pxfmt, GL_UNSIGNED_BYTE, bits);
					::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					::glBindTexture(GL_TEXTURE_2D, 0);
				}
				else
				{
					TRACE("{PngImg::create} glGenTextures failed. pxfmt=0x%08x, width=%d, height=%d\n", pxfmt, width, height);
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
			_width = width;
			_height = height;
			_pxfmt = pxfmt;
			_bpp = numDepth;
			_glid = glid;

			isSucceeded = true;
		#endif
	}
	while(false);

	if(!isSucceeded)	{	delete[] bits;	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PngImg::destroy()
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

/*---------------------------------------------------------------------*//**
	最も小さい２のべき乗に収まる値を得る
**//*---------------------------------------------------------------------*/
u32 PngImg::calcPowerValue(u32 val)
{
	u32 c = 1;
	while(c < val)
	{
		c <<= 1;
	}
	return c;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS