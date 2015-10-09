/***********************************************************************//**
 *  HrAnimObjPosture.cpp
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
#include "HrAnimObjPosture.h"

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
// HrAnimObjPosture メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	クォータニオンを算出する
**//*---------------------------------------------------------------------*/
void HrAnimObjPosture::calcQuaternion(Vector4F* qtrn) const
{
	if(TFW_IS_FLAG(_flags, F_HAS_QUATERNION))
	{
		qtrn->copy((Vector4F*)_rot);
		return;
	}

	// 角度ベクトルの場合はクォータニオンに変換
	Quaternion::convFromEulerAngles(qtrn, (Vector3F*)_rot);
}

/*---------------------------------------------------------------------*//**
	平行移動ベクトルを設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjPosture::setTranslate(const Vector3F* trans)
{
	TFW_COPY_MEMORY(_trs, trans->_v, sizeof(f32) * 3);
}

/*---------------------------------------------------------------------*//**
	角度ベクトルを設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjPosture::setAngle(const Vector3F* angle)
{
	TFW_SET_FLAG(_flags, F_HAS_QUATERNION, false);
	TFW_COPY_MEMORY(_rot, angle->_v, sizeof(f32) * 3);
}

/*---------------------------------------------------------------------*//**
	クォータニオンを設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjPosture::setQuaternion(const Vector4F* qtrn)
{
	TFW_SET_FLAG(_flags, F_HAS_QUATERNION, true);
	TFW_COPY_MEMORY(_rot, qtrn->_v, sizeof(f32) * 4);
}

/*---------------------------------------------------------------------*//**
	スケールを設定する
**//*---------------------------------------------------------------------*/
void HrAnimObjPosture::setScale(const Vector3F* scale)
{
	if(scale == 0L)
	{
		TFW_SET_FLAG(_flags, F_HAS_SCALE, false);
		return;
	}
	TFW_SET_FLAG(_flags, F_HAS_SCALE, true);
	TFW_COPY_MEMORY(_scl, scale->_v, sizeof(f32) * 3);
}

/*---------------------------------------------------------------------*//**
	値の設定
**//*---------------------------------------------------------------------*/
void HrAnimObjPosture::set(const Vector3F* trans, const Vector3F* angle, const Vector3F* scale)
{
	TFW_COPY_MEMORY(_trs, trans->_v, sizeof(f32) * 3);
	TFW_SET_FLAG(_flags, F_HAS_QUATERNION, false);
	TFW_COPY_MEMORY(_rot, angle->_v, sizeof(f32) * 3);
	TFW_SET_FLAG(_flags, F_HAS_SCALE, true);
	TFW_COPY_MEMORY(_scl, scale->_v, sizeof(f32) * 3);

	#if PRE_CALC_QUATERNION			// クォータニオンを計算する
	{
		Quaternion::convFromEulerAngles((Vector4F*)_rot, (Vector3F*)_rot);
		TFW_SET_FLAG(_flags, F_HAS_QUATERNION, true);
	}
	#endif

	#if VALIDATION_SCALE_DEFAULT	// スケールを有効化する
	{
		TFW_SET_FLAG(_flags, F_HAS_SCALE, true);
	}
	#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimObjPosture::HrAnimObjPosture()
	: _flags(0)
{
	TFW_ZERO_MEMORY(_trs, sizeof(f32) * 3);
	TFW_ZERO_MEMORY(_rot, sizeof(f32) * 4);
	TFW_ZERO_MEMORY(_scl, sizeof(f32) * 3);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HrAnimObjPosture::HrAnimObjPosture(const Vector3F* trans, const Vector3F* angle, const Vector4F* quaternion, const Vector3F* scale, bool isQuaternion)
	: _flags(0)
{
	setTranslate(trans);
	if(isQuaternion)	{	setQuaternion(quaternion);	}
	else				{	setAngle(angle);			}
	if(scale != 0L)		{	setScale(scale);			}
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void HrAnimObjPosture::copy(const HrAnimObjPosture* src)
{
	this->_flags = src->_flags;
	TFW_COPY_MEMORY(this->_trs, src->_trs, sizeof(f32) * 3);
	TFW_COPY_MEMORY(this->_rot, src->_rot, sizeof(f32) * 4);
	TFW_COPY_MEMORY(this->_scl, src->_scl, sizeof(f32) * 3);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS