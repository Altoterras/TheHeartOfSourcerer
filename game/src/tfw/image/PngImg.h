/***********************************************************************//**
 *	PngImg.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2010/03/18.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_IMAGE_PNG_IMG_H_
#define _TFW_IMAGE_PNG_IMG_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "ImageBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class CStringBase;
class FileBase;

/*---------------------------------------------------------------------*//**
	PNG

**//*---------------------------------------------------------------------*/
class PngImg : public ImageBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	virtual u32		getWidth() const			{	return _width;			}
	virtual u32		getHeight() const			{	return _height;			}
	virtual u32		getGlTextureId() const		{	return _glid;			}
	u8*				getBits() const				{	return _bits;			}
	u32				getBitPerPixel() const		{	return _bpp;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void deleteBits();
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PngImg();
	virtual ~PngImg();

	virtual bool	create(const CStringBase* filepath, u16 cflag);
	virtual bool	create(const char* fbuf, int size, u16 cflag);
	virtual bool	create(u32 fileid, u16 cflag);
	virtual void	destroy();

private:
	bool			create(FileBase* file, u16 cflag);
	u32				calcPowerValue(u32 val);

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

#endif	// _TFW_IMAGE_PNG_IMG_H_