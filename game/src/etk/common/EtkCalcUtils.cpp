/***********************************************************************//**
 *	EtkCalcUtils.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2013/08/22.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EtkCalcUtils.h"

// Friends
#include "../body/EtkBody.h"
#include "../sg/SorSgm.h"

// External
#include "../../tfw/Tfw.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/QuickSort.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EtkCalcUtils 静的メンバ変数

//==========================================================================
// EtkCalcUtils メソッド

/*---------------------------------------------------------------------*//**
	0.0f ～ 1.0f の乱数値を得る
**//*---------------------------------------------------------------------*/
f32 EtkCalcUtils::randF()
{
	#if defined(_DEBUG)
		f32 rf = ::rand() / (f32)RAND_MAX;
		return rf;
	#else
		return ::rand() / (f32)RAND_MAX;
	#endif
}

/*---------------------------------------------------------------------*//**
	-1.0f ～ 1.0f の乱数値を得る
**//*---------------------------------------------------------------------*/
f32 EtkCalcUtils::randPlusMinusOneF()
{
	#if defined(_DEBUG)
		f32 rf = (((::rand() / (f32)RAND_MAX) * 2.0f) - 1.0f);
		return rf;
	#else
		return (((::rand() / (f32)RAND_MAX) * 2.0f) - 1.0f);
	#endif
}

/*---------------------------------------------------------------------*//**
	3D 座標位置をスクリーン座標位置に変換する
**//*---------------------------------------------------------------------*/
bool EtkCalcUtils::calc3dPosToScreenPos(Vector2F* posScr, const Vector3F* pos3d)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = EtkBody::getBody()->getTfw();		if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::conv3dPosToScreenPos(posScr, pos3d, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	3D 座標位置をスクリーン座標位置に変換する（配列でまとめて計算）
**//*---------------------------------------------------------------------*/
bool EtkCalcUtils::calc3dPosToScreenPos(Vector2F* arrposScr, const Vector3F* arrpos3d, s32 numArr)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = EtkBody::getBody()->getTfw();		if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::conv3dPosToScreenPos(arrposScr, arrpos3d, numArr, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	スクリーン座標位置を 3D 座標位置に変換する
**//*---------------------------------------------------------------------*/
bool EtkCalcUtils::calcScreenPosTo3dPos(Vector3F* pos3d, const Vector2F* posScr, const f32 zScrPos)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = EtkBody::getBody()->getTfw();		if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::convScreenPosTo3dPos(pos3d, posScr, zScrPos, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	スクリーン座標位置を 3D 座標位置に変換する（配列でまとめて計算）
**//*---------------------------------------------------------------------*/
bool EtkCalcUtils::calcScreenPosTo3dPos(Vector3F* arrpos3d, const Vector2F* arrposScr, const f32* arrzScrPos, s32 numArr)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = EtkBody::getBody()->getTfw();		if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::convScreenPosTo3dPos(arrpos3d, arrposScr, arrzScrPos, numArr, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	スクリーン座標位置と 3D Z 座標位置を 3D 座標位置に変換する
**//*---------------------------------------------------------------------*/
bool EtkCalcUtils::calcScreenPosAnd3dZTo3dPos(Vector3F* pos3d, const Vector2F* posScr, const f32 z3dPos)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = EtkBody::getBody()->getTfw();		if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::convScreenPosAnd3dZTo3dPos(pos3d, posScr, z3dPos, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	スクリーン座標位置と 3D Z 座標位置を 3D 座標位置に変換する
	（配列でまとめて計算）
**//*---------------------------------------------------------------------*/
bool EtkCalcUtils::calcScreenPosAnd3dZTo3dPos(Vector3F* arrpos3d, const Vector2F* arrposScr, const f32* arrz3dPos, s32 numArr)
{
	// 現在のビュー・カメラを得る
	Tfw* tfw = EtkBody::getBody()->getTfw();		if(tfw == 0L)	{	return false;	}
	View* view = tfw->getView();					if(view == 0L)	{	return false;	}
	SorSgm* sgm = (SorSgm*)tfw->getSgManager();		if(sgm == 0L)	{	return false;	}
	Camera* cam = sgm->getVisibleCamera();			if(cam == 0L)	{	return false;	}

	// スクリーン座標に変換
	Gcalc::convScreenPosAnd3dZTo3dPos(arrpos3d, arrposScr, arrz3dPos, numArr, cam, view);

	return true;
}

/*---------------------------------------------------------------------*//**
	フルスクリーンの解像度に対応したテクスチャ座標を得る
**//*---------------------------------------------------------------------*/
void EtkCalcUtils::calcFullScreenMultiResolTexCoord(RectF* vtx, RectF* uv)
{
	f32 w = (f32)EtkBody::getBody()->getScreenWidth();
	f32 h = (f32)EtkBody::getBody()->getScreenHeight();

	if(vtx != 0L)
	{
		vtx->_v[0] = 0.0f;
		vtx->_v[1] = 0.0f;
		vtx->_v[2] = (f32)EtkBody::getBody()->getLogicalWidth();
		vtx->_v[3] = (f32)EtkBody::getBody()->getLogicalHeight();
	}
	
	if(uv != 0L)
	{
		f32 tw;
		f32 th;
		if((w > 1024) || (h > 1024))
		{
			tw = 2048.0f;
			th = 2048.0f;
		}
		else if((w > 512) && (h > 512))
		{
			tw = 1024.0f;
			th = 1024.0f;
		}
		else
		{
			tw = 512.0f;
			th = 512.0f;
		}
		Gcalc::calcTexUv(uv, 0.0f, 0.0f, w, h, tw, th, 1.0f);
	}
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
