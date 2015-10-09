/***********************************************************************//**
 *	Texture.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/05/27.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_GCMN_TEXTURE_H_
#define _TFW_GCMN_TEXTURE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class CStringBase;
class FileBase;
class ImageBase;
class TexAnim;
class ExecCtx;
class TextureCtx;

/*---------------------------------------------------------------------*//**
 *	テクスチャ
 *
**//*---------------------------------------------------------------------*/
class Texture
{
	//======================================================================
	// 定数

public:
	enum Format
	{
		FMT_NULL	= 0,
		FMT_BMP,
		FMT_TGA,
		FMT_PVR,
		FMT_PNG,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ID を得る
	u16				getId() const				{	return _id;				}
	// テクスチャユニーク ID を得る
	u32				getUniqueId() const			{	return _texuid;			}
	// OpenGL ID を得る
	u32				getGlId() const				{	return _glid;			}
	// 幅を得る
	u32				getWidth() const			{	return _width;			}
	// 高さを得る
	u32				getHeight() const			{	return _height;			}
	// イメージを得る
	ImageBase*		getImage() const			{	return _img;			}
	// テクスチャアニメーションを得る
	TexAnim*		getTexAnim() const			{	return _texanimRef;		}
	// 有効なテクスチャかどうかを得る
	bool			isValid() const				{	return _glid != 0;		}
	// 作成済みかどうかを得る
	bool			isCreated() const			{	return _img != 0;		}

	// ID を設定する
	void			setId(u16 id)				{	_id = id;				}
	// テクスチャユニーク ID を設定する
	void			setUniqueId(u32 texuid)		{	_texuid = texuid;		}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void			bindTexAnim(TexAnim* texanimRef);
	void			resetTexAnimFrame(TextureCtx* ctx);
	void			markResourcePoolManage();

	static Format	getFormatFrom3Char(char fc1, char fc2, char fc3);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Texture();
	virtual ~Texture();
	bool			create(const CStringBase* filepath);
	bool			create(const char* fbuf, int size, Format fmt);
	bool			create(u32 fileid, Format fmt);
	void			destroy();
	bool			createContext(TextureCtx* ctx);
	void			destroyContext(TextureCtx* ctx);
	void			exec(const ExecCtx* ec, TextureCtx* texctx);
	void			render(TextureCtx* texctx);

private:
	bool			copyImageInfo(ImageBase* img);

	//======================================================================
	// メンバ変数

private:
	u16			_id;			// テクスチャ ID (←プログラムで生成する内部 ID)
	u32			_texuid;		// テクスチャユニーク ID (←デザイナが指定する全ファイルを通したユニーク ID．この ID が同じ場合は同じテクスチャと見なせる)
	ImageBase*	_img;			// イメージ
	u32			_width;			// 幅
	u32			_height;		// 高さ
	TexAnim*	_texanimRef;	// テクスチャアニメ
	u32			_glid;			// OpenGL テクスチャ ID
	bool		_isManagedByRp;	// リソースプール管理かどうか

	static bool	_isSetTrans;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_GCMN_TEXTURE_H_