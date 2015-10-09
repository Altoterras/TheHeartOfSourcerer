/***********************************************************************//**
 *  HrAnimObjCtx.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/12/29.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HrAnimObjCtx.h"

// Friends

// External
#include "../lib/Matrix.h"
#include "../lib/Quaternion.h"
#include "../lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HrAnimObjCtx メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	フラグを設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjCtx::setFlags(u8 flags, bool isValid)
{
	// フラグを設定する
	TFW_SET_FLAG(_flags, flags, isValid);

	// スケールやモデルビュー行列を持つ
	if((_ex == 0L) && TFW_IS_FLAG(_flags, F_HAS_SCALE | F_SAVE_MV_MTX))
	{
		_ex = new HrAnimObjCtxEx();
	}
}

/*---------------------------------------------------------------------*//**
	フラグを設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjCtx::setTranslate(const Vector3F* trans)
{
	TFW_COPY_MEMORY(_trs, trans->_v, sizeof(f32) * 3);
}

/*---------------------------------------------------------------------*//**
	フラグを設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjCtx::setAngle(const Vector3F* angle)
{
	TFW_SET_FLAG(_flags, F_QUATERNION_ROT, false);
	TFW_COPY_MEMORY(_rot, angle->_v, sizeof(f32) * 3);
}

/*---------------------------------------------------------------------*//**
	フラグを設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjCtx::setQuaternion(const Vector4F* qtrn)
{
	TFW_SET_FLAG(_flags, F_QUATERNION_ROT, true);
	TFW_COPY_MEMORY(_rot, qtrn->_v, sizeof(f32) * 4);
}

/*---------------------------------------------------------------------*//**
	フラグを設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjCtx::setScale(const Vector3F* scale)
{
	if(_ex == 0L)	{	return;	}
	TFW_COPY_MEMORY(_ex->_scl, scale->_v, sizeof(f32) * 3);
}

/*---------------------------------------------------------------------*//**
	モデルビューマトリックを設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjCtx::setModelViewMatrix(const Matrix4F* mvmtx)
{
	if(_ex == 0L)	{	return;	}
	TFW_COPY_MEMORY(_ex->_mdlview, mvmtx->_v, sizeof(f32) * 16);
}

/*---------------------------------------------------------------------*//**
	モデルビューマトリックを計算する
**//*---------------------------------------------------------------------*/
void HrAnimObjCtx::calcAndSaveModelViewMatrix(Matrix4F* mtxMv)
{
	calcModelViewMatrixIgnoreSaveFlag(mtxMv);

	// モデルビュー行列を保存
	if(TFW_IS_FLAG(_flags, F_SAVE_MV_MTX))
	{
		GLfloat mtxGlMv[16];
		::glGetFloatv(GL_MODELVIEW_MATRIX, mtxGlMv);
		mtxMv->mult((const Matrix4F*)mtxGlMv);
		setModelViewMatrix(mtxMv);
	}
}

/*---------------------------------------------------------------------*//**
	モデルビューマトリックを計算し、ミドルウェアに設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjCtx::calcAndSaveAndSupplyModelViewMatrix(Matrix4F* mtxMv)
{
	calcModelViewMatrixIgnoreSaveFlag(mtxMv);

	::glMultMatrixf(mtxMv->ptr());	// OpenGL に反映

	// モデルビュー行列を保存
	if(TFW_IS_FLAG(_flags, F_SAVE_MV_MTX))
	{
		::glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)mtxMv->_v);
		setModelViewMatrix(mtxMv);
	}
}

/*---------------------------------------------------------------------*//**
	モデルビューマトリックを計算し、ミドルウェアに設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjCtx::calcModelViewMatrixIgnoreSaveFlag(Matrix4F* mtxMv)
{
	mtxMv->translate((Vector3F*)_trs);

	if(TFW_IS_FLAG(_flags, F_QUATERNION_ROT))
	{
		Matrix4F mtxRot;
		Quaternion::convToMatrix(&mtxRot, (Vector4F*)_rot);
		mtxMv->mult(&mtxRot);
	}
	else
	{
		mtxMv->rotateX(_rot[0]);
		mtxMv->rotateY(_rot[1]);
		mtxMv->rotateZ(_rot[2]);
	}

	///mtxMv->scale(_scl);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimObjCtx::HrAnimObjCtx()
	: _shpctxRef(0L)
	, _ex(0L)
	, _flags(0)
{
	TFW_ZERO_MEMORY(_trs, sizeof(f32) * 3);
	TFW_ZERO_MEMORY(_rot, sizeof(f32) * 4);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HrAnimObjCtx::~HrAnimObjCtx()
{
	ASSERT(_ex == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HrAnimObjCtx::create(ShapeCtx* shpctxRef)
{
	_shpctxRef = shpctxRef;
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void HrAnimObjCtx::destroy()
{
	delete _ex;
	_ex = 0L;
}

//==========================================================================
// HrAnimObjCtxEx メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimObjCtx::HrAnimObjCtxEx::HrAnimObjCtxEx()
{
	TFW_ZERO_MEMORY(_scl, sizeof(f32) * 3);
	TFW_ZERO_MEMORY(_mdlview, sizeof(f32) * 16);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS