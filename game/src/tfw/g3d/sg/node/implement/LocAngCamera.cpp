/***********************************************************************//**
 *  LocAngCamera.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/16.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "LocAngCamera.h"

// Friends
#include "../../../../lib/Vector.h"
#include "../../../../lib/Matrix.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	位置を設定
**//*---------------------------------------------------------------------*/
void LocAngCamera::setLoc(const Vector3F* loc)
{
	_loc->copy(loc);
}

/*---------------------------------------------------------------------*//**
	角度を設定
**//*---------------------------------------------------------------------*/
void LocAngCamera::setAngle(const Vector3F* ang)
{
	_ang->copy(ang);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
LocAngCamera::LocAngCamera()
	: _loc(0L)
	, _ang(0L)
{
	_camclass = CAMCLS_LOCANG;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
LocAngCamera::~LocAngCamera()
{
	ASSERT(_loc == 0L);
	ASSERT(_ang == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool LocAngCamera::create(f32 fovy, f32 cnear, f32 cfar, u16 flags)
{
	if(!Camera::create(fovy, cnear, cfar, flags))
	{
		return false;
	}

	_loc = new Vector3F();
	_ang = new Vector3F();

	_fovy = fovy;
	_cnear = cnear;
	_cfar = cfar;

	_cntVisible = 1;	// 有効化

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void LocAngCamera::destroy()
{
	delete _ang;
	_ang = 0L;
	delete _loc;
	_loc = 0L;

	Camera::destroy();
}

/*---------------------------------------------------------------------*//**
	カメラ座標（ビュー座標）マトリックスの計算
**//*---------------------------------------------------------------------*/
void LocAngCamera::calcViewMatrix()
{
	// カメラ行列の計算
	Vector3F trans(- *_loc);
	_mtxView->setIdentity();
	_mtxView->translate(&trans);
	_mtxView->rotateX(- _ang->x());
	_mtxView->rotateY(- _ang->y());
	_mtxView->rotateZ(- _ang->z());
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS