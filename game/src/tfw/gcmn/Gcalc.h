/***********************************************************************//**
 *	Gcalc.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/04/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *	
**//***********************************************************************/

#ifndef _TFW_GCMN_CALC_H_
#define _TFW_GCMN_CALC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Vector2;
typedef Vector2<f32> Vector2F;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
template<class TYPE> class Vector4;
typedef Vector4<f32> Vector4F;
template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class Matrix4F;
class Camera;
class View;

/*---------------------------------------------------------------------*//**
 *	計算関数クラス
 *
**//*---------------------------------------------------------------------*/
class Gcalc
{
public:
	// パースペクティブを計算する
	static void			perspectiveFovY(Matrix4F* mtx, f32 fov, f32 aspect, f32 cnear, f32 cfar);
	static void			perspectiveFovX(Matrix4F* mtx, f32 fovx, f32 aspect, f32 cnear, f32 cfar);
	// 視野行列を計算する（gluLookat 相当）
	static void			lookat(Matrix4F* mtx, const Vector3F* eye, const Vector3F* aim, const Vector3F* up);
	// 角度を有効範囲内へ変換する
	static f32			adjustAngle(f32 angle);
	// 補間計算
	static inline f32	interpolate(f32 v1, f32 v2, f32 rate) { return v1 + ((v2 - v1) * rate); }
	static f32			interpolateAngle(f32 v1, f32 v2, f32 rate);
	static Vector3F*	interpolate(Vector3F* vdst, const Vector3F* v1, const Vector3F* v2, f32 rate);
	static Vector4F*	interpolate(Vector4F* vdst, const Vector4F* v1, const Vector4F* v2, f32 rate);
	static ColorU8*		interpolate(ColorU8* vdst, const ColorU8* v1, const ColorU8* v2, f32 rate);
	// スプライン補間計算
	static f32			splineintrp(f32 xt, f32 x_M2, f32 x_M1, f32 x_P0, f32 x_P1, f32 y_M2, f32 y_M1, f32 y_P0, f32 y_P1);
	static Vector3F*	splineintrp(Vector3F* vyDst, f32 xt, f32 x_M2, f32 x_M1, f32 x_P0, f32 x_P1, const Vector3F* vy_M2, const Vector3F* vy_M1, const Vector3F* vy_P0, const Vector3F* vy_P1);
	// 3D 座標とスクリーン座標の相互変換計算する
	static inline void	conv3dPosToScreenPos(Vector2F* ptScr, const Vector3F* pos3d, const Camera* cam, const View* view) { conv3dPosToScreenPos(ptScr, pos3d, 1, cam, view); }
	static void 		conv3dPosToScreenPos(Vector2F* arrptScr, const Vector3F* arrpos3d, s32 numArr, const Camera* cam, const View* view);
	static inline void	convScreenPosTo3dPos(Vector3F* pos3d, const Vector2F* ptScr, const f32 zScrPos, const Camera* cam, const View* view) { convScreenPosTo3dPos(pos3d, ptScr, &zScrPos, 1, cam, view); }
	static void			convScreenPosTo3dPos(Vector3F* arrpos3d, const Vector2F* arrptScr, const f32* arrzScrPos, s32 numArr, const Camera* cam, const View* view);
	static inline void	convScreenPosAnd3dZTo3dPos(Vector3F* pos3d, const Vector2F* ptScr, const f32 z3dPos, const Camera* cam, const View* view) { convScreenPosAnd3dZTo3dPos(pos3d, ptScr, &z3dPos, 1, cam, view); }
	static void			convScreenPosAnd3dZTo3dPos(Vector3F* arrpos, const Vector2F* arrptScr, const f32* arrz3dPos, s32 numArr, const Camera* cam, const View* view);
	// ３点から平面を作成する
	static void			makePlaneFromPoints(Vector4F* plane, const Vector3F* p1, const Vector3F* p2, const Vector3F* p3);
	// テクスチャ座標を計算する
	static RectF32*		calcTexUv(RectF32* uv, f32 uPixel, f32 vPixel, f32 wPixel, f32 hPixel, f32 wTex, f32 hTex, f32 scaleResolutionInv);
	static RectF32*		calcTexUvNoAa(RectF32* uv, f32 uPixel, f32 vPixel, f32 wPixel, f32 hPixel, f32 wTex, f32 hTex);
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_CALC_H_

