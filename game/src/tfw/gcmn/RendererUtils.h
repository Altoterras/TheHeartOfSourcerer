/***********************************************************************//**
 *	RendererUtils.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *	
**//***********************************************************************/

#ifndef _TFW_GCMN_RENDERER_UTILS_H_
#define _TFW_GCMN_RENDERER_UTILS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../lib/Rect.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
template<class TYPE> class Vector2;
typedef Vector2<f32> Vector2F;
template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
typedef Rect<f32> RectF32;
class Renderer;
class View;

/*---------------------------------------------------------------------*//**
	補助レンダラー クラス
		現時点では、全ての描画機能を提供するようなラッパークラスではない
		OpenGL の描画システムを補助するものユーティリティクラスに近い
		将来的には高レベル化するかもしれないので、「Renderer」とする

**//*---------------------------------------------------------------------*/
class RendererUtils
{
	//======================================================================
	// 定数

public:
	enum LineKind
	{
		LINES = 1,
		LINE_LOOP,
		LINE_STRIP,
	};
	enum TriangleKind
	{
		TRIANGLES = 1,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
	};

	//======================================================================
	// メソッド

public:
	// 2D の線描画
	static void draw2dLine(Renderer* rdr, f32 vx1, f32 vy1, f32 vx2, f32 vy2);
	// 2D の円塗りつぶし描画
	static void draw2dFillArc(Renderer* rdr, f32 x, f32 y, f32 r, f32 thetaStart, f32 thetaEnd, f32 thetaInc);
	// 2D の塗りつぶし矩形描画
	static void draw2dRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh);
	static inline void draw2dRect(Renderer* rdr, const RectF32* vtx) { draw2dRect(rdr, vtx->x(), vtx->y(), vtx->w(), vtx->h()); }
	// 2D の線矩形描画
	static void draw2dRectLine(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh);
	static inline void draw2dRectLine(Renderer* rdr, const RectF32* vtx) { draw2dRectLine(rdr, vtx->x(), vtx->y(), vtx->w(), vtx->h()); }
	// 2D のテクスチャー矩形描画
	static void draw2dTextureRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh, f32 tu, f32 tv, f32 tw, f32 th);
	static inline void draw2dTextureRect(Renderer* rdr, const RectF32* vtx, const RectF32* uv) { draw2dTextureRect(rdr, vtx->x(), vtx->y(), vtx->w(), vtx->h(), uv->x(), uv->y(), uv->w(), uv->h()); }
	static void draw2dTextureRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh, f32 tu, f32 tv, f32 tw, f32 th, u32 rot);
	static inline void draw2dTextureRect(Renderer* rdr, const RectF32* vtx, const RectF32* uv, u32 rot) { draw2dTextureRect(rdr, vtx->x(), vtx->y(), vtx->w(), vtx->h(), uv->x(), uv->y(), uv->w(), uv->h(), rot); }
	// 2D の色指定塗りつぶし矩形描画
	static void draw2dColorRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh, const ColorU8* colLeftTop, const ColorU8* colRightTop, const ColorU8* colLeftBottom, const ColorU8* colRightBottom);
	static inline void draw2dColorRect(Renderer* rdr, const RectF32* vtx, const ColorU8* colLeftTop, const ColorU8* colRightTop, const ColorU8* colLeftBottom, const ColorU8* colRightBottom) { draw2dColorRect(rdr, vtx->x(), vtx->y(), vtx->w(), vtx->h(), colLeftTop, colRightTop, colLeftBottom, colRightBottom); }
	static inline void draw2dColorRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh, const ColorU8* col) { draw2dColorRect(rdr, vx, vy, vw, vh, col, col, col, col); }
	static inline void draw2dColorRect(Renderer* rdr, const RectF32* vtx, const ColorU8* col) { draw2dColorRect(rdr, vtx->x(), vtx->y(), vtx->w(), vtx->h(), col, col, col, col); }
	// 2D の色指定テクスチャー矩形描画
	static inline void draw2dColorTextureRect(Renderer* rdr, const RectF32* vtx, const RectF32* uv, const ColorU8* colLeftTop, const ColorU8* colRightTop, const ColorU8* colLeftBottom, const ColorU8* colRightBottom) { draw2dColorTextureRect(rdr, vtx->x(), vtx->y(), vtx->w(), vtx->h(), uv->x(), uv->y(), uv->w(), uv->h(), colLeftTop, colRightTop, colLeftBottom, colRightBottom); }
	static void draw2dColorTextureRect(Renderer* rdr, f32 vx, f32 vy, f32 vw, f32 vh, f32 tu, f32 tv, f32 tw, f32 th, const ColorU8* colLeftTop, const ColorU8* colRightTop, const ColorU8* colLeftBottom, const ColorU8* colRightBottom);

	// 3D の線描画
	static void render3dLines(Renderer* rdr, const Vector3F* vtxarr, s32 num, LineKind linekind);
	static void render3dTriangles(Renderer* rdr, const Vector3F* vtxarr, const Vector3F* nrmarr, const Vector2F* texarr, const ColorU8* colarr, s32 num, TriangleKind trikind);
	// 3D の線描画球
	static void render3dLineSphere(Renderer* rdr, f32 r, s32 subdiv);
private:
	static void render3dLineSphere_diveSubDiv(const Vector3F* v1, const Vector3F* v2, const Vector3F* v3, f32 r, s32 subdiv);

public:
	// 色変化
	static void boostColor(ColorU8* colOut, const ColorU8* colIn, f32 rate);

	// 画面にフィットさせて描画する
	static void draw2dTextureToScreenFitLongSide(Renderer* rdr, const View* view, const RectF32* uvTex, const ColorU8* colSide);
	static void draw2dTextureToScreenFitShortSide(Renderer* rdr, const View* view, const RectF32* uvTex);
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_GCMN_RENDERER_UTILS_H_

