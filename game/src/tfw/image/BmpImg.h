/***********************************************************************//**
 *	BmpImg.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/09/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_IMAGE_BMP_IMG_H_
#define _TFW_IMAGE_BMP_IMG_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "ImageBase.h"


////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class CStringBase;
class FileBase;

/*---------------------------------------------------------------------*//**
 *	ビットマップ
 *
**//*---------------------------------------------------------------------*/
class BmpImg : public ImageBase
{
	//======================================================================
	// 構造体
private:
	/*-----------------------------------------------------------------*//**
	 *	ビットマップ ファイル ヘッダ
	 *
	**//*-----------------------------------------------------------------*/
	struct BitmapFileHeader
	{
		u16					_type;
		u32					_size;
		u16					_reserved1;
		u16					_reserved2;
		u32					_offbits;
	};

	/*-----------------------------------------------------------------*//**
	 *	ビットマップ情報ヘッダ
	 *
	**//*-----------------------------------------------------------------*/
	struct BitmapInfoHeader
	{
		u32					_size;
		s32					_width;
		s32					_height;
		u16					_planes;
		u16					_bitcount;
		u32					_compression;
		u32					_sizeImage;
		s32					_xPelsPerMeter;
		s32					_yPelsPerMeter;
		u32					_clrUsed;
		u32					_clrImportant;
	};

	/*-----------------------------------------------------------------*//**
	 *	RGB 色情報
	 *
	**//*-----------------------------------------------------------------*/
	struct RgbQuad
	{
		u8					_b;
		u8					_g;
		u8					_r;
		u8					_reserved;
	};

	/*-----------------------------------------------------------------*//**
	 *	ビットマップ情報
	 *
	**//*-----------------------------------------------------------------*/
	struct BitmapInfo
	{
		BitmapInfoHeader	_header;
		RgbQuad				_colors[1];
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	virtual u32		getWidth() const			{	return _width;			}
	virtual u32		getHeight() const			{	return _height;			}
	virtual u32		getGlTextureId() const		{	return _glid;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	BmpImg();
	virtual ~BmpImg();

	virtual bool	create(const CStringBase* filepath, u16 cflag);
	virtual bool	create(const char* fbuf, int size, u16 cflag);
	virtual bool	create(u32 fileid, u16 cflag);
	virtual void	destroy();

private:
	bool			create(FileBase* file, u16 cflag);

	//======================================================================
	// 変数
private:
	u32		_glid;		// OpenGL テクスチャ ID
	u8*		_bits;		// イメージデータ
	u32		_bpp;		// ピクセル当たりのビット数（bit per pixel）
	u32		_width;		// 幅
	u32		_height;	// 高さ
	u32		_pxfmt;		// ピクセルフォーマット
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_IMAGE_BMP_IMG_H_