/***********************************************************************//**
 *	ImageBase.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/09/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_IMAGE_IMAGE_BASE_H_
#define _TFW_IMAGE_IMAGE_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class CStringBase;
class FileBase;

/*---------------------------------------------------------------------*//**
	テクスチャ クラス

**//*---------------------------------------------------------------------*/
class ImageBase
{
	//======================================================================
	// 定数

public:
	// 作成フラグ
	static const u16	CFLAG_SAVE_BITS		= 0x0001;	// 作成後にビット配列を削除せず残す（サポートしている場合のみ）
	static const u16	CFLAG_GLTEXIMAGE	= 0x0002;	// OpenGL テクスチャを作成する（glTexImage2D する）

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	virtual u32		getWidth() const = 0;
	virtual u32		getHeight() const = 0;
	virtual u32		getGlTextureId() const = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual ~ImageBase() {}
	virtual bool	create(const CStringBase* filepath, u16 cflag) = 0;
	virtual bool	create(const char* fbuf, int size, u16 cflag) = 0;
	virtual bool	create(u32 fileid, u16 cflag) = 0;
	virtual void	destroy() = 0;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_IMAGE_IMAGE_BASE_H_