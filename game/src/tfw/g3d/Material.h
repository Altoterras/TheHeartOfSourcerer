/***********************************************************************//**
 *  Material.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/30.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_MATERIAL_H_
#define _TFW_G3D_MATERIAL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Texture;
class VcString;
class CStringBase;
class ExecCtx;
class RenderCtx;
class MaterialCtx;

/*---------------------------------------------------------------------*//**
 *	マテリアル（3D モデルの表面素材）
 *
**//*---------------------------------------------------------------------*/
class Material
{
	//======================================================================
	// アクセサ
public:
	// ID を得る
	inline u16				getId() const		{	return _id;				}
	// テクスチャを得る
	inline const Texture*	getTexture() const	{	return _textureRef;		}

	// ID を設定する
	void					setId(u16 id)		{	_id = id;				}

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void			resetTexAnimFrame(MaterialCtx* ctx);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Material();
	~Material();
	bool			create(Vector3F* colorDiffuseEntr, Vector3F* colorSpecularEntr, Vector3F* colorAmbientEntr, f32 opacity, Texture* textureRef);
	void			destroy();
	bool			createContext(MaterialCtx* ctx);
	void			destroyContext(MaterialCtx* ctx);
	void			exec(const ExecCtx* ec, MaterialCtx* mtrlctx);
	void			render(const RenderCtx* rc, MaterialCtx* mtrlctx);

private:

	//======================================================================
	// メンバ変数
private:
	u16			_id;
	Vector3F*	_colorDiffuse;
	Vector3F*	_colorSpecular;
	Vector3F*	_colorAmbient;
	f32			_opacity;
	Texture*	_textureRef;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_MATERIAL_H_