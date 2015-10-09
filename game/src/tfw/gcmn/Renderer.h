
/***********************************************************************//**
 *	Renderer.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/04/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *	
**//***********************************************************************/

#ifndef _TFW_GCMN_RENDERER_H_
#define _TFW_GCMN_RENDERER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS
	
template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Vector4;
typedef Vector4<f32> Vector4F;
class View;
class Texture;
class TextureCtx;
class Camera;

/*---------------------------------------------------------------------*//**
	補助レンダラー クラス
		現時点では、全ての描画機能を提供するようなラッパークラスではない
		OpenGL の描画システムを補助するものユーティリティクラスに近い
		将来的には高レベル化するかもしれないので、「Renderer」とする

**//*---------------------------------------------------------------------*/
class Renderer
{
	//======================================================================
	// 定数
public:
	// アルファ合成設定
	enum AlphaFunc
	{
		AFUNC_NULL,		// 未設定
		AFUNC_NORMAL,	// 通常
		AFUNC_ADD,		// 加算
		AFUNC_SUB,		// 減算
	};

private:
	// 現在の描画ステートフラグ
	static const u32 RSF_LIGHTING	= 0x00000001;		// ライティング有効
	static const u32 RSF_ALPHABLEND	= 0x00000004;		// アルファブレンド有効
	static const u32 RSF_ALPHATEST	= 0x00000008;		// アルファテスト有効
	static const u32 RSF_CULLING	= 0x00000010;		// 背面カリング有効
	static const u32 RSF_TEXTURE	= 0x00000020;		// テクスチャ有効
	static const u32 RSF_FOG		= 0x00000040;		// フォグ有効
	static const u32 RSF_BLEQ_ADD	= 0x00000080;		// ブレンド式が加算
	static const u32 RSF_ZTEST		= 0x00000100;		// Z テスト有効
	static const u32 RSF_ZWRITING	= 0x00000200;		// Z 値書き込み有効
	static const u32 RSF_FOG_WHOLE	= 0x00400000;		// 全体に対するフォグ有効

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在のビューを得る
	inline View* getCurrentView() const { return _viewCurRef; }
	// 現在のカメラを得る
	inline Camera* getCurrentCamera() const { return _camCurRef; }
	
	// 設定色を得る
	inline const ColorU8* getSolidColor() const { return _colorSolidCur; }
	// クリアカラーを得る
	inline Vector4F* clearColor() const { return _colorClear; }

	// 現在のカメラを設定
	inline void setCurrentCamera(Camera* cam) { _camCurRef = cam; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void beginRenderFrame();
	void endRenderFrame();
	void setDefault3dRenderState();
	void setDefault2dRenderState();

	inline void bindTexture(Texture* tex)					{	bindTexture(tex, 0L, false);	}
	inline void bindTexture(Texture* tex, bool isForce)		{	bindTexture(tex, 0L, isForce);	}
	void bindTexture(Texture* tex, TextureCtx* texctx, bool isForce);

	void setSolidColor(u8 r, u8 g, u8 b, u8 a);
	void setSolidColor(u8 r, u8 g, u8 b);
	void setSolidColor(const ColorU8* col);
	void setSolidColorForce(u8 r, u8 g, u8 b, u8 a);
	void setSolidColorForce(const ColorU8* col);
	void setSolidColorMemoryOnly(u8 r, u8 g, u8 b, u8 a);
	void setSolidColorMemoryOnly(const ColorU8* col);
	void setColorAlpha(u8 a);

	void setLighting(bool isEnable);
	void setZTest(bool isEnable);
	void setZWriting(bool isEnable);
	void setAlphaBlend(bool isEnable);
	void setAlphaBlendFunc(AlphaFunc afunc);
	void setAlphaTest(bool isEnable);
	void setCulling(bool isEnable);
	void setEnableTexture(bool isEnable);
	void setFog(bool isEnable);
	void setFogWhole(bool isEnable);

	Renderer();
	~Renderer();
	bool create(View* viewRef);
	void destroy();

	//======================================================================
	// 変数
private:
	Vector4F*	_colorClear;
	ColorU8*	_colorSolidCur;
	AlphaFunc	_afuncCur;
	u32			_rsflagsCur;
	View*		_viewCurRef;
	Texture*	_texCurRef;
	Camera*		_camCurRef;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_GCMN_RENDERER_H_

