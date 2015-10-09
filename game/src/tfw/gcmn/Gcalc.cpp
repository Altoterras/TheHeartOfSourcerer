/***********************************************************************//**
 *	Gcalc.cpp
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
#include "Gcalc.h"

// Friends
#include "../g3d/sg/node/implement/Camera.h"
#include "../gcmn/View.h"
#include "../lib/Color.h"
#include "../lib/Matrix.h"
#include "../lib/Rect.h"
#include "../lib/Vector.h"

// External

// Library
#include <math.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

/*---------------------------------------------------------------------*//**
	パースペクティブを計算する（縦視界角度版）
**//*---------------------------------------------------------------------*/
void Gcalc::perspectiveFovY(Matrix4F* mtx, f32 fov, f32 aspect, f32 cnear, f32 cfar)
{
	f32 s, c, d, r;
	d = cfar - cnear;
	r = TFW_DEG_TO_RAD(fov * 0.5f);
	
	s = ::sinf( r );
	c = ::cosf( r ) / s;

	mtx->_v[ 0 ][ 0 ] = c / aspect;
	mtx->_v[ 1 ][ 1 ] = c;
	mtx->_v[ 2 ][ 2 ] = - ( cfar + cnear ) / d;
	mtx->_v[ 2 ][ 3 ] = - 1.0f;
	mtx->_v[ 3 ][ 2 ] = - 2.0f * cfar * cnear / d;
	mtx->_v[ 3 ][ 3 ] = 1.0f;
}

/*---------------------------------------------------------------------*//**
	パースペクティブを計算する（横視界角度版）
**//*---------------------------------------------------------------------*/
void Gcalc::perspectiveFovX(Matrix4F* mtx, f32 fovx, f32 aspect, f32 cnear, f32 cfar)
{
	f32 s, c, d, r;
	d = cfar - cnear;
	r = TFW_DEG_TO_RAD(fovx * 0.5f);
	
	s = ::sinf( r );
	c = ::cosf( r ) / s;

	mtx->_v[ 0 ][ 0 ] = c;
	mtx->_v[ 1 ][ 1 ] = c / aspect;
	mtx->_v[ 2 ][ 2 ] = - ( cfar + cnear ) / d;
	mtx->_v[ 2 ][ 3 ] = - 1.0f;
	mtx->_v[ 3 ][ 2 ] = - 2.0f * cfar * cnear / d;
	mtx->_v[ 3 ][ 3 ] = 1.0f;
}

/*---------------------------------------------------------------------*//**
	視野行列を計算する（gluLookat 相当）
**//*---------------------------------------------------------------------*/
void Gcalc::lookat(Matrix4F* mtx, const Vector3F* eye, const Vector3F* aim, const Vector3F* up)
{
#if 1
	Vector3F vz = *aim - *eye;
	vz.normalize();
	Vector3F vx = vz ^ *up;
	Vector3F vy = vx ^ vz;
	Matrix4F mtxWk(
		vx._v[0], vy._v[0], - vz._v[0], 0.0f,
		vx._v[1], vy._v[1], - vz._v[1], 0.0f,
		vx._v[2], vy._v[2], - vz._v[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	mtx->set(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		- eye->_v[0], - eye->_v[1], - eye->_v[2], 1.0f);
	mtx->mult(&mtxWk);
#else
	mtx->_v[0][3] = 0.0f;
	mtx->_v[1][3] = 0.0f;
	mtx->_v[2][3] = 0.0f;
	mtx->_v[3][3] = 1.0f;
	Vector3F* axx = (Vector3F*)&mtx->_v[0][0];
	Vector3F* axy = (Vector3F*)&mtx->_v[1][0];
	Vector3F* axz = (Vector3F*)&mtx->_v[2][0];
	Vector3F* trs = (Vector3F*)&mtx->_v[3][0];
	*axz = *eye - *aim;
	axz->normalize();
	*axx = *up ^ *axz;
	*axy = *axz ^ *axx;
	*trs = *eye;
	mtx->inverse();
#endif
}

/*---------------------------------------------------------------------*//**
	角度を -PI ～ PI の間におさめる
**//*---------------------------------------------------------------------*/
f32 Gcalc::adjustAngle(f32 angle)
{
	while(angle > TFW_PI)	{	angle -= 2.0f * TFW_PI;	}
	while(angle < -TFW_PI)	{	angle += 2.0f * TFW_PI;	}
	return angle;
}

/*---------------------------------------------------------------------*//**
	角度補間
**//*---------------------------------------------------------------------*/
f32 Gcalc::interpolateAngle(f32 v1, f32 v2, f32 rate)
{
	return adjustAngle( v1 + ((v2 - v1) * rate) );
}

/*---------------------------------------------------------------------*//**
	ベクトル補間
**//*---------------------------------------------------------------------*/
Vector3F* Gcalc::interpolate(Vector3F* vdst, const Vector3F* v1, const Vector3F* v2, f32 rate)
{
	vdst->_v[0] = interpolate(v1->_v[0], v2->_v[0], rate);
	vdst->_v[1] = interpolate(v1->_v[1], v2->_v[1], rate);
	vdst->_v[2] = interpolate(v1->_v[2], v2->_v[2], rate);
	return vdst;
}

/*---------------------------------------------------------------------*//**
	ベクトル補間
**//*---------------------------------------------------------------------*/
Vector4F* Gcalc::interpolate(Vector4F* vdst, const Vector4F* v1, const Vector4F* v2, f32 rate)
{
	vdst->_v[0] = interpolate(v1->_v[0], v2->_v[0], rate);
	vdst->_v[1] = interpolate(v1->_v[1], v2->_v[1], rate);
	vdst->_v[2] = interpolate(v1->_v[2], v2->_v[2], rate);
	vdst->_v[3] = interpolate(v1->_v[3], v2->_v[3], rate);
	return vdst;
}

/*---------------------------------------------------------------------*//**
	色補間
**//*---------------------------------------------------------------------*/
ColorU8* Gcalc::interpolate(ColorU8* vdst, const ColorU8* v1, const ColorU8* v2, f32 rate)
{
	vdst->_v[0] = (u8)interpolate(v1->_v[0], v2->_v[0], rate);
	vdst->_v[1] = (u8)interpolate(v1->_v[1], v2->_v[1], rate);
	vdst->_v[2] = (u8)interpolate(v1->_v[2], v2->_v[2], rate);
	vdst->_v[3] = (u8)interpolate(v1->_v[3], v2->_v[3], rate);
	return vdst;
}

/*---------------------------------------------------------------------*//**
	スプライン補間、基本式

	@param xt 求めたい Y 値における X 値
	@param x_M2 補間区間の２つ前の X 値
	@param x_M1 補間区間左端の X 値
	@param x_P0 補間区間右端の X 値
	@param x_P1 補間区間の１つ先の X 値
	@param y_M2 x_M2 に対する Y 値
	@param y_M1 x_M1 に対する Y 値
	@param y_P0 x_P0 に対する Y 値
	@param y_P1 x_P1 に対する Y 値
	@return 補間 Y 値
**//*---------------------------------------------------------------------*/
f32 Gcalc::splineintrp(f32 xt, f32 x_M2, f32 x_M1, f32 x_P0, f32 x_P1, f32 y_M2, f32 y_M1, f32 y_P0, f32 y_P1)
{
	f32 h_M1 = x_M1 - x_M2;
	f32 h_P0 = x_P0 - x_M1;
	f32 h_P1 = x_P1 - x_P0;

	f32 dy1_M1 = (y_M1 - y_M2) / h_M1;
	f32 dy1_P0 = (y_P0 - y_M1) / h_P0;
	f32 dy1_P1 = (y_P1 - y_P0) / h_P1;

	f32 dy2_M1 = (dy1_P0 - dy1_M1) / (x_P0 - x_M2);
	f32 dy2_P0 = (dy1_P1 - dy1_P0) / (x_P1 - x_M1);

	f32 xtl = xt - x_M1;
	f32 xtr = x_P0 - xt;
	f32 yi0 = dy2_M1 / (6.0f * h_P0) * xtr * xtr * xtr;
	f32 yi1 = dy2_P0 / (6.0f * h_P0) * xtl * xtl * xtl;
	f32 yi2 = (y_M1 / h_P0 - h_P0 * dy2_M1 / 6.0f) * xtr;
	f32 yi3 = (y_P0 / h_P0 - h_P0 * dy2_P0 / 6.0f) * xtl;

	return yi0 + yi1 + yi2 + yi3;
}

/*---------------------------------------------------------------------*//**
	ベクトルのスプライン補間

	@param xt 求めたい Y 値における X 値
	@param x_M2 補間区間の２つ前の X 値
	@param x_M1 補間区間左端の X 値
	@param x_P0 補間区間右端の X 値
	@param x_P1 補間区間の１つ先の X 値
	@param vy_M2 x_M2 に対するベクトル値
	@param vy_M1 x_M1 に対するベクトル値
	@param vy_P0 x_P0 に対するベクトル値
	@param vy_P1 x_P1 に対するベクトル値
	@return 補間ベクトル値
**//*---------------------------------------------------------------------*/
Vector3F* Gcalc::splineintrp(Vector3F* vyDst, f32 xt, f32 x_M2, f32 x_M1, f32 x_P0, f32 x_P1, const Vector3F* vy_M2, const Vector3F* vy_M1, const Vector3F* vy_P0, const Vector3F* vy_P1)
{
	f32 h_M1 = x_M1 - x_M2;
	f32 h_P0 = x_P0 - x_M1;
	f32 h_P1 = x_P1 - x_P0;

	f32 dy1_M1[4], dy1_P0[4], dy1_P1[4];
	for(int i = 0; i < 3; i++)
	{
		dy1_M1[i] = (vy_M1->_v[i] - vy_M2->_v[i]) / h_M1;
		dy1_P0[i] = (vy_P0->_v[i] - vy_M1->_v[i]) / h_P0;
		dy1_P1[i] = (vy_P1->_v[i] - vy_P0->_v[i]) / h_P1;
	}

	f32 dy2_M1[4], dy2_P0[4];
	for(int i = 0; i < 3; i++)
	{
		dy2_M1[i] = (dy1_P0[i] - dy1_M1[i]) / (x_P0 - x_M2);
		dy2_P0[i] = (dy1_P1[i] - dy1_P0[i]) / (x_P1 - x_M1);
	}

	f32 xtl = xt - x_M1;
	f32 xtr = x_P0 - xt;
	for(int i = 0; i < 3; i++)
	{
		f32 yi0 = dy2_M1[i] / (6.0f * h_P0) * xtr * xtr * xtr;
		f32 yi1 = dy2_P0[i] / (6.0f * h_P0) * xtl * xtl * xtl;
		f32 yi2 = (vy_M1->_v[i] / h_P0 - h_P0 * dy2_M1[i] / 6.0f) * xtr;
		f32 yi3 = (vy_P0->_v[i] / h_P0 - h_P0 * dy2_P0[i] / 6.0f) * xtl;
		vyDst->_v[i] = yi0 + yi1 + yi2 + yi3;
	}

	return vyDst;
}

/*---------------------------------------------------------------------*//**
	3D 座標からスクリーン座標を計算する（配列でまとめて計算）
**//*---------------------------------------------------------------------*/
void Gcalc::conv3dPosToScreenPos(Vector2F* arrptScr, const Vector3F* arrpos3d, s32 numArr, const Camera* cam, const View* view)
{
	const Matrix4F* mtxView = cam->getViewMatrix();			ASSERT(mtxView != 0L);
	const Matrix4F* mtxPrs = view->getPerspectiveMatrix();	ASSERT(mtxPrs != 0L);
	f32 hw = view->getLogicalWidth() * 0.5f;
	f32 hh = view->getLogicalHeight() * 0.5f;
	for(s32 i = 0; i < numArr; i++)
	{
		Vector3F vWk(arrpos3d[i]);
		mtxView->mult(&vWk, &vWk);
		mtxPrs->mult(&vWk, &vWk);
		switch(view->getRotZ())
		{
		default:
		case TFW_DISPF_RANG_0:
		case TFW_DISPF_RANG_180:
			arrptScr[i].x() = (vWk.x() * hw) + hw;
			arrptScr[i].y() = - (vWk.y() * hh) + hh;
			break;
		case TFW_DISPF_RANG_90:
		case TFW_DISPF_RANG_270:
			arrptScr[i].x() = (vWk.x() * hw * view->getInvLogicalAspect()) + hw;
			arrptScr[i].y() = - (vWk.y() * hh * view->getLogicalAspect()) + hh;
			break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	スクリーン座標から 3D 座標を計算する（配列でまとめて計算）
**//*---------------------------------------------------------------------*/
void Gcalc::convScreenPosTo3dPos(Vector3F* arrpos3d, const Vector2F* arrptScr, const f32* arrzScrPos, s32 numArr, const Camera* cam, const View* view)
{
	if(cam->getViewInverseMatrix() == 0L)	{	return;	}
	const Matrix4F* mtxViewInv = cam->getViewInverseMatrix();	ASSERT(mtxViewInv != 0L);
	Matrix4F mtxPrsInv(*view->getPerspectiveMatrix());
	mtxPrsInv.inverse();
	f32 hw = view->getLogicalWidth() * 0.5f;
	f32 hh = view->getLogicalHeight() * 0.5f;
	for(s32 i = 0; i < numArr; i++)
	{
		Vector3F vWk;
		switch(view->getRotZ())
		{
		default:
		case TFW_DISPF_RANG_0:
		case TFW_DISPF_RANG_180:
			vWk.x() = (arrptScr[i].x() - hw) / hw;
			vWk.y() = - (arrptScr[i].y() - hh) / hh;
			break;
		case TFW_DISPF_RANG_90:
		case TFW_DISPF_RANG_270:
			vWk.x() = (arrptScr[i].x() - hw) / (hw * view->getInvLogicalAspect());
			vWk.y() = - (arrptScr[i].y() - hh) / (hh * view->getLogicalAspect());
			break;
		}
		vWk.z() = arrzScrPos[i];
		mtxPrsInv.mult(&vWk, &vWk);
		mtxViewInv->mult(&arrpos3d[i], &vWk);
	}
}

/*---------------------------------------------------------------------*//**
	スクリーン座標位置と 3D Z 座標位置を 3D 座標位置に変換する
**//*---------------------------------------------------------------------*/
void Gcalc::convScreenPosAnd3dZTo3dPos(Vector3F* arrpos, const Vector2F* arrptScr, const f32* arrz3dPos, s32 numArr, const Camera* cam, const View* view)
{
	if(cam->getViewInverseMatrix() == 0L)	{	return;	}
	const Matrix4F* mtxView = cam->getViewMatrix();				ASSERT(mtxView != 0L);
	const Matrix4F* mtxViewInv = cam->getViewInverseMatrix();	ASSERT(mtxViewInv != 0L);
	const Matrix4F* mtxPrs = view->getPerspectiveMatrix();		ASSERT(mtxPrs != 0L);
	Matrix4F mtxPrsInv(*view->getPerspectiveMatrix());
	mtxPrsInv.inverse();
	f32 hw = view->getLogicalWidth() * 0.5f;
	f32 hh = view->getLogicalHeight() * 0.5f;
	for(s32 i = 0; i < numArr; i++)
	{
		Vector3F vWk;
		switch(view->getRotZ())
		{
		default:
		case TFW_DISPF_RANG_0:
		case TFW_DISPF_RANG_180:
			vWk.x() = (arrptScr[i].x() - hw) / hw;
			vWk.y() = - (arrptScr[i].y() - hh) / hh;
			break;
		case TFW_DISPF_RANG_90:
		case TFW_DISPF_RANG_270:
			vWk.x() = (arrptScr[i].x() - hw) / (hw * view->getInvLogicalAspect());
			vWk.y() = - (arrptScr[i].y() - hh) / (hh * view->getLogicalAspect());
			break;
		}

		vWk.z() = (mtxView->_v[2][2] * arrz3dPos[i] + mtxView->_v[3][2]) / (mtxView->_v[2][3] * arrz3dPos[i] + mtxView->_v[3][3]);
		vWk.z() = (mtxPrs->_v[2][2] * vWk.z() + mtxPrs->_v[3][2]) / (mtxPrs->_v[2][3] * vWk.z() + mtxPrs->_v[3][3]);

		mtxPrsInv.mult(&vWk, &vWk);
		mtxViewInv->mult(&arrpos[i], &vWk);

#if 0	// 計算実験
		Vector3F vWk2;
		cam->getViewMatrix()->mult(&vWk2, &vWk2);
		view->getPerspectiveMatrix()->mult(&vWk2, &vWk2);
		TRACE("v2 {%f, %f, %f}\n", vWk2.x(), vWk2.y(), vWk2.z());

		const Matrix4F* mtxPrs = view->getPerspectiveMatrix();
		f32 z3d = cam->getViewMatrix()->_v[3][2];
		f32 z2d = (mtxPrs->_v[2][2] * z3d + mtxPrs->_v[3][2]) / (mtxPrs->_v[2][3] * z3d + mtxPrs->_v[3][3]);

		vWk.x() = 100.0f;
		vWk.y() = 100.0f;
		vWk.z() = 0.0f;
		cam->getViewMatrix()->mult(&vWk, &vWk);
		view->getPerspectiveMatrix()->mult(&vWk, &vWk);
		switch(view->getRotZ())
		{
		default:
		case TFW_RANG_0:
		case TFW_RANG_180:
			vWk.x() = (vWk.x() * hw) + hw;
			vWk.y() = (vWk.y() * hh) + hh;
			break;
		case TFW_RANG_90:
		case TFW_RANG_270:
			vWk.x() = (vWk.x() * hw / view->getLogicalAcpect()) + hw;
			vWk.y() = (vWk.y() * hh * view->getLogicalAcpect()) + hh;
			break;
		}
		TRACE("v (2d) {%f, %f, %f}\n", vWk.x(), vWk.y(), vWk.z());

		switch(view->getRotZ())
		{
		default:
		case TFW_RANG_0:
		case TFW_RANG_180:
			vWk.x() = (vWk.x() - hw) / hw;
			vWk.y() = (vWk.y() - hh) / hh;
			break;
		case TFW_RANG_90:
		case TFW_RANG_270:
			vWk.x() = (vWk.x() - hw) / (hw / view->getLogicalAcpect());
			vWk.y() = (vWk.y() - hh) / (hh * view->getLogicalAcpect());
			break;
		}
		mtxPrsInv.mult(&vWk, &vWk);
		mtxViewInv->mult(&vWk, &vWk);
		TRACE("v (3d) {%f, %f, %f}\n", vWk.x(), vWk.y(), vWk.z());
#endif
	}
}

/*---------------------------------------------------------------------*//**
	３点から平面を作成する
**//*---------------------------------------------------------------------*/
void Gcalc::makePlaneFromPoints(Vector4F* plane, const Vector3F* p1, const Vector3F* p2, const Vector3F* p3)
{
	Vector3F v1, v2, v3;
	v1.x() = p2->x() - p1->x();
	v1.y() = p2->y() - p1->y();
	v1.z() = p2->z() - p1->z();
	v2.x() = p3->x() - p1->x();
	v2.y() = p3->y() - p1->y();
	v2.z() = p3->z() - p1->z();
	v3 = v1.cross(&v2);
	v3.normalize();
	plane->x() = v3.x();
	plane->y() = v3.y();
	plane->z() = v3.z();
	plane->w() = -( v3.x() * p1->x() + v3.y() * p1->y() + v3.z() * p1->z() );
}

#if 0
/*---------------------------------------------------------------------*//**
	テクスチャ座標を計算する
**//*---------------------------------------------------------------------*/
RectF32 Gcalc::calcTexUv(f32 uPixel, f32 vPixel, f32 wPixel, f32 hPixel, f32 wTex, f32 hTex, f32 scaleResolutionInv)
{
	RectF32 uv;
	calcTexUv(&uv, uPixel, vPixel, wPixel, hPixel, wTex, hTex, scaleResolutionInv);
	return uv;
}
#endif

/*---------------------------------------------------------------------*//**
	テクスチャ座標を計算する
**//*---------------------------------------------------------------------*/
RectF32* Gcalc::calcTexUv(RectF32* uv, f32 uPixel, f32 vPixel, f32 wPixel, f32 hPixel, f32 wTex, f32 hTex, f32 scaleResolutionInv)
{
	f32 wiTex = 1.0f / wTex;
	f32 hiTex = 1.0f / hTex;
	f32 wiTexAa = wiTex * scaleResolutionInv;
	f32 hiTexAa = hiTex * scaleResolutionInv;
	uv->_v[0] = (uPixel * wiTex) + wiTexAa;
	uv->_v[1] = (vPixel * hiTex) + hiTexAa;
	uv->_v[2] = (wPixel * wiTex) - wiTexAa - wiTexAa;
	uv->_v[3] = (hPixel * hiTex) - hiTexAa - hiTexAa;
	return uv;
}

/*---------------------------------------------------------------------*//**
	テクスチャ座標を計算する（アンチエイリアシングを考慮しない）
**//*---------------------------------------------------------------------*/
RectF32* Gcalc::calcTexUvNoAa(RectF32* uv, f32 uPixel, f32 vPixel, f32 wPixel, f32 hPixel, f32 wTex, f32 hTex)
{
	f32 wiTex = 1.0f / wTex;
	f32 hiTex = 1.0f / hTex;
	uv->_v[0] = (uPixel * wiTex);
	uv->_v[1] = (vPixel * hiTex);
	uv->_v[2] = (wPixel * wiTex);
	uv->_v[3] = (hPixel * hiTex);
	return uv;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
