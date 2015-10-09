/***********************************************************************//**
 *	Renderer.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/04/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Renderer.h"

// Friends
#include "../gcmn/Texture.h"
#include "../gcmn/View.h"
#include "../lib/Color.h"
#include "../lib/Vector.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Renderer::Renderer()
	: _colorClear(0L)
	, _colorSolidCur(0L)
	, _afuncCur(AFUNC_NULL)
	, _rsflagsCur(0)
	, _viewCurRef(0L)
	, _texCurRef(0L)
	, _camCurRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Renderer::~Renderer()
{
	ASSERT(_colorClear == 0L);
	ASSERT(_colorSolidCur == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Renderer::create(View* viewRef)
{
	_viewCurRef = viewRef;

	// クリアカラーベクトルの作成
	_colorClear = new Vector4F(0.0f, 0.0f, 0.0f, 0.0f);

	// ソリッドカラー現在値を作成
	_colorSolidCur = new ColorU8(255, 255, 255, 255);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// OpenGL のステイト初期化
	
	::glClearColor(_colorClear->r(), _colorClear->g(), _colorClear->b(), _colorClear->a());
	::glColor4ub(_colorSolidCur->r(), _colorSolidCur->g(), _colorSolidCur->b(), _colorSolidCur->a());
	::glDisable(GL_CULL_FACE);
	::glDisable(GL_ALPHA_TEST);
	::glDisable(GL_FOG);
	::glDisable(GL_BLEND);
	::glDisable(GL_LIGHTING);
	::glDisable(GL_DEPTH_TEST);
	::glDepthMask(false);
	::glDisable(GL_TEXTURE_2D);
	::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);		// テクスチャのラップモードを
	::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		// リピートに
	//::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//::glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#if defined(_IOS) || defined(_ANDROID)
	::glDepthRangef(0.0f, 1.0f);	// Z 値の範囲
#else
	::glDepthRange(0.0, 1.0);		// Z 値の範囲
#endif
	::glMatrixMode(GL_TEXTURE);
	::glLoadIdentity();
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
	
	setEnableTexture(true);		// テクスチャを有効に
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Renderer::destroy()
{
	// ソリッドカラー現在値を削除
	delete _colorSolidCur;
	_colorSolidCur = 0;

	// クリアカラーベクトルの削除
	delete _colorClear;
	_colorClear = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム描画の開始
**//*---------------------------------------------------------------------*/
void Renderer::beginRenderFrame()
{
	setZWriting(true);		// Z 書き込みを ON にしてから
	::glClearColor(_colorClear->r(), _colorClear->g(), _colorClear->b(), _colorClear->a());
	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_camCurRef = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム描画の終了
**//*---------------------------------------------------------------------*/
void Renderer::endRenderFrame()
{
#if defined(_WINDOWS) && 0
	::glFinish();
	::glFlush();
#endif
}

/*---------------------------------------------------------------------*//**
	デフォルトの 3D レンダリングステイトを設定する
		フレーム 3D 描画処理の最初に一回呼び出すものとし、
		その後の描画はここにある設定の差分から行う
		基本的にここに記述されるステイトを変える場合は、
		デフォルトのステイトに戻すものとする
**//*---------------------------------------------------------------------*/
void Renderer::setDefault3dRenderState()
{
	// ライティング設定
	setLighting(false);			// ライティングなし

	// Z 設定
	setZTest(true);				// Z 判定あり
	setZWriting(true);			// Z 書き込みあり
	::glDepthFunc(GL_LEQUAL);	// Z 値が低いもの（手前のもの）が合格
	
	// カリング設定
	setCulling(true);			// カリングあり
	::glCullFace(GL_BACK);		// 背面カリング

	// アルファ合成設定
	setAlphaBlend(false);		// アルファ合成なし

	// フォグ設定
	setFog(true);				// フォグ有効
	
	// 頂点カラー設定
	setSolidColorForce(255, 255, 255, 255);	// 不透明白に

	// テクスチャ設定をクリア
	bindTexture(0L, true);
}

/*---------------------------------------------------------------------*//**
	デフォルトのレンダリングステイトを設定する
		フレーム 2D 描画処理の最初に一回呼び出すものとし、
		その後の描画はここにある設定の差分から行う
		基本的にここに記述されるステイトを変える場合は、
		デフォルトのステイトに戻すものとする
**//*---------------------------------------------------------------------*/
void Renderer::setDefault2dRenderState()
{
	// 左上が原点の２Ｄ座標系を設定
	_viewCurRef->apply2dDrawSetting();

	// ライティング設定
	setLighting(false);			// ライティングなし

	// Z 設定
	setZTest(false);			// Z 判定なし

	// カリング設定
	setCulling(false);			// カリングなし

	// アルファ合成設定
	setAlphaBlend(true);		// アルファ合成あり
	setAlphaBlendFunc(AFUNC_NORMAL);	// 通常合成

	// アルファテスト設定
	setAlphaTest(true);			// アルファテストあり
	::glAlphaFunc(GL_GREATER, 0.0f);	// アルファ値 0 を超えるものが合格

	// フォグ設定
	setFog(false);				// フォグ無効
	
	// 頂点カラー設定
	setSolidColorForce(255, 255, 255, 255);	// 不透明白に

	// テクスチャ設定をクリア
	bindTexture(0L, true);
}

/*---------------------------------------------------------------------*//**
	テクスチャ転送
		バインド済みのテクスチャは転送しない
**//*---------------------------------------------------------------------*/
void Renderer::bindTexture(Texture* tex, TextureCtx* texctx, bool isForce)
{
	// TEST-TextureOptimize-
	if(tex != 0L)
	{
		setEnableTexture(true);
	}

	if((_texCurRef != tex) || isForce)	// 変更チェック
	{
		if(tex != 0L)
		{
			tex->render(texctx);
		}
		else
		{
			setEnableTexture(false);
		}
		_texCurRef = tex;
	}
}

/*---------------------------------------------------------------------*//**
	色の指定
**//*---------------------------------------------------------------------*/
void Renderer::setSolidColor(u8 r, u8 g, u8 b, u8 a)
{
	// 変更チェック
	if((_colorSolidCur->r() == r) && (_colorSolidCur->g() == g) && (_colorSolidCur->b() == b) && (_colorSolidCur->a() == a))	{	return;	}
	_colorSolidCur->set(r, g, b, a);

	// 設定の変更
	::glColor4ub(_colorSolidCur->r(), _colorSolidCur->g(), _colorSolidCur->b(), _colorSolidCur->a());
}

/*---------------------------------------------------------------------*//**
	色の指定
**//*---------------------------------------------------------------------*/
void Renderer::setSolidColor(u8 r, u8 g, u8 b)
{
	// 変更チェック
	if((_colorSolidCur->r() == r) && (_colorSolidCur->g() == g) && (_colorSolidCur->b() == b))	{	return;	}
	_colorSolidCur->r() = r;
	_colorSolidCur->g() = g;
	_colorSolidCur->b() = b;

	// 設定の変更
	::glColor4ub(_colorSolidCur->r(), _colorSolidCur->g(), _colorSolidCur->b(), _colorSolidCur->a());
}

/*---------------------------------------------------------------------*//**
	色の指定
**//*---------------------------------------------------------------------*/
void Renderer::setSolidColor(const ColorU8* col)
{
	// 変更チェック
	if(_colorSolidCur->equals(col))	{	return;	}
	_colorSolidCur->copy(col);

	// 設定の変更
	::glColor4ub(_colorSolidCur->r(), _colorSolidCur->g(), _colorSolidCur->b(), _colorSolidCur->a());
}

/*---------------------------------------------------------------------*//**
	強制的色指定
**//*---------------------------------------------------------------------*/
void Renderer::setSolidColorForce(u8 r, u8 g, u8 b, u8 a)
{
	_colorSolidCur->set(r, g, b, a);
	::glColor4ub(_colorSolidCur->r(), _colorSolidCur->g(), _colorSolidCur->b(), _colorSolidCur->a());
}

/*---------------------------------------------------------------------*//**
	強制的色指定
**//*---------------------------------------------------------------------*/
void Renderer::setSolidColorForce(const ColorU8* col)
{
	_colorSolidCur->copy(col);
	::glColor4ub(_colorSolidCur->r(), _colorSolidCur->g(), _colorSolidCur->b(), _colorSolidCur->a());
}

/*---------------------------------------------------------------------*//**
	使用した色を記憶する
		※	glEnableClientState(GL_COLOR_ARRAY) 時などに、
			強制的に色が書き換わった場合にソリッドカラーを記憶させる
**//*---------------------------------------------------------------------*/
void Renderer::setSolidColorMemoryOnly(u8 r, u8 g, u8 b, u8 a)
{
	if((_colorSolidCur->r() == r) && (_colorSolidCur->g() == g) && (_colorSolidCur->b() == b) && (_colorSolidCur->a() == a))	{	return;	}
	_colorSolidCur->set(r, g, b, a);
}

/*---------------------------------------------------------------------*//**
	使用した色を記憶する
		※	glEnableClientState(GL_COLOR_ARRAY) 時などに、
			強制的に色が書き換わった場合にソリッドカラーを記憶させる
**//*---------------------------------------------------------------------*/
void Renderer::setSolidColorMemoryOnly(const ColorU8* col)
{
	if(_colorSolidCur->equals(col))	{	return;	}
	_colorSolidCur->copy(col);
}

/*---------------------------------------------------------------------*//**
	アルファ値のみの指定
**//*---------------------------------------------------------------------*/
void Renderer::setColorAlpha(u8 a)
{
	// 変更チェック
	if(_colorSolidCur->a() == a)	{	return;	}
	_colorSolidCur->a() = a;

	// 設定の変更
	::glColor4ub(_colorSolidCur->r(), _colorSolidCur->g(), _colorSolidCur->b(), _colorSolidCur->a());
}

/*---------------------------------------------------------------------*//**
	ライティングの有効／無効設定
**//*---------------------------------------------------------------------*/
void Renderer::setLighting(bool isEnable)
{
	// 変更チェック
	if(TFW_IS_FLAG(_rsflagsCur, RSF_LIGHTING) == isEnable)	{	return;	}
	TFW_SET_FLAG(_rsflagsCur, RSF_LIGHTING, isEnable);

	// 設定の変更
	if(isEnable)	{	::glEnable(GL_LIGHTING);	}
	else			{	::glDisable(GL_LIGHTING);	}
}

/*---------------------------------------------------------------------*//**
	Z テストの有効／無効設定
**//*---------------------------------------------------------------------*/
void Renderer::setZTest(bool isEnable)
{
	// 変更チェック
	if(TFW_IS_FLAG(_rsflagsCur, RSF_ZTEST) == isEnable)	{	return;	}
	TFW_SET_FLAG(_rsflagsCur, RSF_ZTEST, isEnable);

	// 設定の変更
	if(isEnable)	{	::glEnable(GL_DEPTH_TEST);	}
	else			{	::glDisable(GL_DEPTH_TEST);	}
}

/*---------------------------------------------------------------------*//**
	Z 値書き込みの有効／無効設定
**//*---------------------------------------------------------------------*/
void Renderer::setZWriting(bool isEnable)
{
	// 変更チェック
	if(TFW_IS_FLAG(_rsflagsCur, RSF_ZWRITING) == isEnable)	{	return;	}
	TFW_SET_FLAG(_rsflagsCur, RSF_ZWRITING, isEnable);

	// 設定の変更
	::glDepthMask(isEnable);
}

/*---------------------------------------------------------------------*//**
	アルファブレンドの有効／無効設定
**//*---------------------------------------------------------------------*/
void Renderer::setAlphaBlend(bool isEnable)
{
	// 変更チェック
	if(TFW_IS_FLAG(_rsflagsCur, RSF_ALPHABLEND) == isEnable)	{	return;	}
	TFW_SET_FLAG(_rsflagsCur, RSF_ALPHABLEND, isEnable);

	// 設定の変更
	if(isEnable)	{	::glEnable(GL_BLEND);		}
	else			{	::glDisable(GL_BLEND);		}
}

/*---------------------------------------------------------------------*//**
	アルファブレンドファンクションの設定
**//*---------------------------------------------------------------------*/
void Renderer::setAlphaBlendFunc(AlphaFunc afunc)
{
	// 変更チェック
	if(_afuncCur == afunc)	{	return;	}
	_afuncCur = afunc;

	// 設定の変更
	switch(afunc)
	{
	case AFUNC_NORMAL:	// 通常
		if(!TFW_IS_FLAG(_rsflagsCur, RSF_BLEQ_ADD))
		{
			TFW_SET_FLAG(_rsflagsCur, RSF_BLEQ_ADD, true);
			#if defined(_WINDOWS)
				::glBlendEquation(GL_FUNC_ADD);
			#elif defined(_IOS)
				::glBlendEquationOES(GL_FUNC_ADD_OES);
			#endif
		}
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;

	case AFUNC_ADD:		// 加算
		if(!TFW_IS_FLAG(_rsflagsCur, RSF_BLEQ_ADD))
		{
			TFW_SET_FLAG(_rsflagsCur, RSF_BLEQ_ADD, true);
			#if defined(_WINDOWS)
				::glBlendEquation(GL_FUNC_ADD);
			#elif defined(_IOS)
				::glBlendEquationOES(GL_FUNC_ADD_OES);
			#endif
		}
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;

	case AFUNC_SUB:		// 減算
		if(TFW_IS_FLAG(_rsflagsCur, RSF_BLEQ_ADD))
		{
			TFW_SET_FLAG(_rsflagsCur, RSF_BLEQ_ADD, false);
			#if defined(_WINDOWS)
				::glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
			#elif defined(_IOS)
				::glBlendEquationOES(GL_FUNC_REVERSE_SUBTRACT_OES);
			#endif
		}
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	}
}

/*---------------------------------------------------------------------*//**
	アルファテストの有効／無効設定
**//*---------------------------------------------------------------------*/
void Renderer::setAlphaTest(bool isEnable)
{
	// 変更チェック
	if(TFW_IS_FLAG(_rsflagsCur, RSF_ALPHATEST) == isEnable)	{	return;	}
	TFW_SET_FLAG(_rsflagsCur, RSF_ALPHATEST, isEnable);

	// 設定の変更
	if(isEnable)	{	::glEnable(GL_ALPHA_TEST);		}
	else			{	::glDisable(GL_ALPHA_TEST);		}
}

/*---------------------------------------------------------------------*//**
	ポリゴンの背面カリングの有効／無効設定
**//*---------------------------------------------------------------------*/
void Renderer::setCulling(bool isEnable)
{
	// 変更チェック
	if(TFW_IS_FLAG(_rsflagsCur, RSF_CULLING) == isEnable)	{	return;	}
	TFW_SET_FLAG(_rsflagsCur, RSF_CULLING, isEnable);

	// 設定の変更
	if(isEnable)	{	::glEnable(GL_CULL_FACE);	}
	else			{	::glDisable(GL_CULL_FACE);	}
}

/*---------------------------------------------------------------------*//**
	テクスチャの有効／無効設定
**//*---------------------------------------------------------------------*/
void Renderer::setEnableTexture(bool isEnable)
{
	// 変更チェック
	if(TFW_IS_FLAG(_rsflagsCur, RSF_TEXTURE) == isEnable)	{	return;	}
	TFW_SET_FLAG(_rsflagsCur, RSF_TEXTURE, isEnable);

	// 設定の変更
	if(isEnable)	{	::glEnable(GL_TEXTURE_2D);	}
	else			{	::glDisable(GL_TEXTURE_2D);	}
}

/*---------------------------------------------------------------------*//**
	フォグの有効／無効設定
**//*---------------------------------------------------------------------*/
void Renderer::setFog(bool isEnable)
{
	// 全体に対するフォグが有効でない限り値変更は出来ない
	if(!TFW_IS_FLAG(_rsflagsCur, RSF_FOG_WHOLE))		{	return;	}

	// 変更チェック
	if(TFW_IS_FLAG(_rsflagsCur, RSF_FOG) == isEnable)	{	return;	}
	TFW_SET_FLAG(_rsflagsCur, RSF_FOG, isEnable);

	// 設定の変更
	if(isEnable)	{	::glEnable(GL_FOG);		}
	else			{	::glDisable(GL_FOG);	}
}

/*---------------------------------------------------------------------*//**
	全体に対するフォグの有効／無効設定
**//*---------------------------------------------------------------------*/
void Renderer::setFogWhole(bool isEnable)
{
	// 変更チェック
	if(TFW_IS_FLAG(_rsflagsCur, RSF_FOG_WHOLE) == isEnable)	{	return;	}
	TFW_SET_FLAG(_rsflagsCur, RSF_FOG_WHOLE, isEnable);

	// 設定の変更
	if(isEnable)	{	::glEnable(GL_FOG);		}
	else			{	::glDisable(GL_FOG);	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
