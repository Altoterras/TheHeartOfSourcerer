/***********************************************************************//**
 *  Camera.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/13.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Camera.h"

// Friends
#include "../../../../lib/Vector.h"
#include "../../../../lib/Matrix.h"
#include "../../../../gcmn/Gcalc.h"
#include "../../../../gcmn/Renderer.h"
#include "../../../../gcmn/RendererUtils.h"
#include "../../../../gcmn/View.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	有効／無効化切替
**//*---------------------------------------------------------------------*/
void Camera::setVisible(bool isVisible)
{
	if(isVisible)
	{
		if(_cntVisible == 0)
		{
			notifyVisibleChange(true);
			_cntVisible = 1;	// 有効化
		}
	}
	else
	{
		if(_cntVisible != 0)
		{
			notifyVisibleChange(false);
			_cntVisible = 0;	// 無効化
		}
	}
}

/*---------------------------------------------------------------------*//**
	視錐台カリング判定

	@retval true 視野内にある
	@retval false 視野内にない
**//*---------------------------------------------------------------------*/
bool Camera::isClipWithWorldPos(const Vector3F* posWorld, f32 r)
{
	// Z 位置をビュー座標に変換
	f32 zView =
		_mtxView->_v[0][2] * posWorld->x() +
		_mtxView->_v[1][2] * posWorld->y() +
		_mtxView->_v[2][2] * posWorld->z() +
		_mtxView->_v[3][2];
	zView = - zView;

	// ニア平面とテスト
	if((zView + r) < _cnear)
	{
		return false;
	}

	// ファー平面とテスト
	if((zView - r) > _cfar)
	{
		return false;
	}

	// X 位置をビュー座標に変換
	f32 xView =
		_mtxView->_v[0][0] * posWorld->x() +
		_mtxView->_v[1][0] * posWorld->y() +
		_mtxView->_v[2][0] * posWorld->z() +
		_mtxView->_v[3][0];

	// 左クリップ平面とテスト
	f32 dist = (xView * _planeFovL->_v[0]) + (zView * _planeFovL->_v[2]);
	if(dist > r)
	{
		return false;
	}

	// 右クリップ平面とテスト
	dist = (xView * _planeFovR->_v[0]) + (zView * _planeFovR->_v[2]);
	if(dist > r)
	{
		return false;
	}

	// Y 位置をビュー座標に変換
	f32 yView =
		_mtxView->_v[0][1] * posWorld->x() +
		_mtxView->_v[1][1] * posWorld->y() +
		_mtxView->_v[2][1] * posWorld->z() +
		_mtxView->_v[3][1];

	// 上クリップ平面とテスト
	dist = (yView * _planeFovT->_v[1]) + (zView * _planeFovT->_v[2]);
	if(dist > r)
	{
		return false;
	}

	// 下クリップ平面とテスト
	dist = (yView * _planeFovB->_v[1]) + (zView * _planeFovB->_v[2]);
	if(dist > r)
	{
		return false;
	}

	// 視野内である
	return true;
}

/*---------------------------------------------------------------------*//**
	視錐台カリング判定

	@retval true 視野内にある
	@retval false 視野内にない
**//*---------------------------------------------------------------------*/
bool Camera::isClipWithViewPos(const Vector3F* posView, f32 r)
{
	// Z 位置をビュー座標に変換
	f32 zView = posView->z();
	zView = - zView;

	// ニア平面とテスト
	if((zView + r) < _cnear)
	{
		return false;
	}

	// ファー平面とテスト
	if((zView - r) > _cfar)
	{
		return false;
	}

	// X 位置をビュー座標に変換
	f32 xView = posView->x();

	// 左クリップ平面とテスト
	f32 dist = (xView * _planeFovL->_v[0]) + (zView * _planeFovL->_v[2]);
	if(dist > r)
	{
		return false;
	}

	// 右クリップ平面とテスト
	dist = (xView * _planeFovR->_v[0]) + (zView * _planeFovR->_v[2]);
	if(dist > r)
	{
		return false;
	}

	// Y 位置をビュー座標に変換
	f32 yView = posView->y();

	// 上クリップ平面とテスト
	dist = (yView * _planeFovT->_v[1]) + (zView * _planeFovT->_v[2]);
	if(dist > r)
	{
		return false;
	}

	// 下クリップ平面とテスト
	dist = (yView * _planeFovB->_v[1]) + (zView * _planeFovB->_v[2]);
	if(dist > r)
	{
		return false;
	}

	// 視野内である
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Camera::Camera()
	: _camclass(CAMCLS_NULL)
	, _flags(0)
	, _cntVisible(0)
	, _fovy(0.0f)
	, _cnear(0.0f)
	, _cfar(0.0f)
	, _aspect(1.0f)
	, _mtxView(0L)
	, _mtxViewInv(0L)
	, _mtxBillboard(0L)
	, _planeFovL(0L)
	, _planeFovR(0L)
	, _planeFovT(0L)
	, _planeFovB(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Camera::~Camera()
{
	ASSERT(_mtxView == 0L);
	ASSERT(_mtxViewInv == 0L);
	ASSERT(_mtxBillboard == 0L);
	ASSERT(_planeFovL == 0L);
	ASSERT(_planeFovR == 0L);
	ASSERT(_planeFovT == 0L);
	ASSERT(_planeFovB == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Camera::create(f32 fovy, f32 cnear, f32 cfar, u16 flags)
{
	if(!SgEntity::create(SGNTYPE_CAMERA))
	{
		return false;
	}

	_mtxView = new Matrix4F();
	if(!TFW_IS_FLAG(_flags, F_NO_CALC_INVMTX))	{	_mtxViewInv = new Matrix4F();	}
	_mtxBillboard = new Matrix4F();
	_planeFovL = new Vector4F();
	_planeFovR = new Vector4F();
	_planeFovT = new Vector4F();
	_planeFovB = new Vector4F();

	_fovy = fovy;
	_cnear = cnear;
	_cfar = cfar;
	_flags = flags;

	_cntVisible = 0;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Camera::destroy()
{
	delete _planeFovB;
	_planeFovB = 0L;
	delete _planeFovT;
	_planeFovT = 0L;
	delete _planeFovR;
	_planeFovR = 0L;
	delete _planeFovL;
	_planeFovL = 0L;
	delete _mtxBillboard;
	_mtxBillboard = 0L;
	delete _mtxViewInv;
	_mtxViewInv = 0L;
	delete _mtxView;
	_mtxView = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Camera::exec(ExecRes* res, const ExecCtx* ec)
{
	if(_cntVisible == 0)	{	return;	}	// 無効化中

	// 有効化カウンタ
	if(_cntVisible < U32_MAX)	{	_cntVisible++;	}

	// カメラの姿勢計算
	calcViewMatrix();
	calcViewInverseMatrix();

	// ビルボードマトリックスを計算する
	calcBillboardMatrix();

	// クリップ平面を計算する
	calcFovClipPlanes();
}

/*---------------------------------------------------------------------*//**
	描画前処理
**//*---------------------------------------------------------------------*/
void Camera::previseImplements(const RenderCtx* rc)
{
	if(_cntVisible == 0)	{	return;	}	// 無効化中

	// 現在のカメラとしてレンダラーに設定
	rc->getRenderer()->setCurrentCamera(this);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void Camera::renderImplements(const RenderCtx* rc)
{
	if(_cntVisible == 0)	{	return;	}	// 無効化中
	
	Renderer* rdr = rc->getRenderer();
	View* view = rdr->getCurrentView();

	::glMatrixMode( GL_PROJECTION );
	::glLoadIdentity();
	
	// パースペクティブを設定する
	::glMultMatrixf(view->calcPerspectiveMatrix(_fovy, _cnear, _cfar)->ptr());

	::glMatrixMode( GL_MODELVIEW );
	::glLoadIdentity();

	// 画面の回転適用
	switch(view->getRotZ())
	{
	case TFW_DISPF_RANG_90:		::glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);	break;
	case TFW_DISPF_RANG_180:	::glRotatef(-180.0f, 0.0f, 0.0f, 1.0f);	break;
	case TFW_DISPF_RANG_270:	::glRotatef(-270.0f, 0.0f, 0.0f, 1.0f);	break;
	}
	
	// カメラの姿勢を設定する
	::glMultMatrixf(_mtxView->ptr());

	// アスペクト費の逆数を保存
	_aspect = view->getLogicalAspect();
}

/*---------------------------------------------------------------------*//**
	カメラ座標逆マトリックスの計算
**//*---------------------------------------------------------------------*/
void Camera::calcViewInverseMatrix()
{
	if(_mtxViewInv == 0L)	{	return;	}	// 計算しない

	_mtxViewInv->set(_mtxView);
	_mtxViewInv->inverse();
}

/*---------------------------------------------------------------------*//**
	ビルボードマトリックスの計算
**//*---------------------------------------------------------------------*/
void Camera::calcBillboardMatrix()
{
	_mtxBillboard->_v[0][0] = _mtxView->_v[0][0];
	_mtxBillboard->_v[0][1] = _mtxView->_v[0][1];
	_mtxBillboard->_v[0][2] = _mtxView->_v[0][2];
	_mtxBillboard->_v[0][3] = 0.0f;
	_mtxBillboard->_v[1][0] = _mtxView->_v[1][0];
	_mtxBillboard->_v[1][1] = _mtxView->_v[1][1];
	_mtxBillboard->_v[1][2] = _mtxView->_v[1][2];
	_mtxBillboard->_v[1][3] = 0.0f;
	_mtxBillboard->_v[2][0] = _mtxView->_v[2][0];
	_mtxBillboard->_v[2][1] = _mtxView->_v[2][1];
	_mtxBillboard->_v[2][2] = _mtxView->_v[2][2];
	_mtxBillboard->_v[2][3] = 0.0f;
	_mtxBillboard->_v[3][0] = 0.0f;	// 平行移動 X を無視
	_mtxBillboard->_v[3][1] = 0.0f;	// 平行移動 Y を無視
	_mtxBillboard->_v[3][2] = 0.0f;	// 平行移動 Z を無視
	_mtxBillboard->_v[3][3] = _mtxView->_v[3][3];
	_mtxBillboard->inverse();
}

/*---------------------------------------------------------------------*//**
	クリップ平面を計算する
**//*---------------------------------------------------------------------*/
void Camera::calcFovClipPlanes()
{
	Vector3F v1, v2, v3;

	f32 tanW = ::tanf( TFW_DEG_TO_RAD(_fovy) );	// クリップの実行が正しいかをテストするには _fovy を 1/4 などにしてみるとわかりやすい
	f32 ft = _cfar * tanW;
	f32 fta = ft * _aspect;

	// 左平面
	v1.x() = 0.0f;
	v1.y() = 0.0f;
	v1.z() = 0.0f;
	v2.x() = - fta;
	v2.y() = - ft;
	v2.z() = _cfar;
	v3.x() = v2.x();
	v3.y() = - v2.y();
	v3.z() = v2.z();
	Gcalc::makePlaneFromPoints( _planeFovL, &v1, &v2, &v3 );

	// 右平面
	v1.x() = 0.0f;
	v1.y() = 0.0f;
	v1.z() = 0.0f;
	v2.x() = fta;
	v2.y() = ft;
	v2.z() = _cfar;
	v3.x() = v2.x();
	v3.y() = - v2.y();
	v3.z() = v2.z();
	Gcalc::makePlaneFromPoints( _planeFovR, &v1, &v2, &v3 );

	// 上平面
	v1.x() = 0.0f;
	v1.y() = 0.0f;
	v1.z() = 0.0f;
	v2.x() = - fta;
	v2.y() = ft;
	v2.z() = _cfar;
	v3.x() = - v2.x();
	v3.y() = v2.y();
	v3.z() = v2.z();
	Gcalc::makePlaneFromPoints( _planeFovT, &v1, &v2, &v3 );

	// 下平面
	v1.x() = 0.0f;
	v1.y() = 0.0f;
	v1.z() = 0.0f;
	v2.x() = fta;
	v2.y() = - ft;
	v2.z() = _cfar;
	v3.x() = - v2.x();
	v3.y() = v2.y();
	v3.z() = v2.z();
	Gcalc::makePlaneFromPoints( _planeFovB, &v1, &v2, &v3 );
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS