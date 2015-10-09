/***********************************************************************//**
 *	Quaternion.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2012/11/14.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Quaternion.h"

// Friends
#include "Matrix.h"
#include "Vector.h"

// External

// Library
#include <math.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

const int kX = 0;
const int kY = 1;
const int kZ = 2;
const int kW = 3;

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Quaternion メソッド

/*---------------------------------------------------------------------*//**
	クォータニオンから行列へ変換
**//*---------------------------------------------------------------------*/
void Quaternion::convToMatrix(Matrix4F* mtxRot, const Vector4F* q)
{
	/*
	If q is guaranteed to be a unit Vector4F, s will always
	be 1.  In that case, this calculation can be optimized out.
	*/
	f32 norm = q->_v[kX] * q->_v[kX] + q->_v[kY] * q->_v[kY] + q->_v[kZ] * q->_v[kZ] + q->_v[kW] * q->_v[kW];
	f32 s = (norm > 0.0f) ? 2.0f / norm : 0.0f;
	
	/*
	Precalculate coordinate products
	*/
	f32 xx = q->_v[kX] * q->_v[kX] * s;
	f32 yy = q->_v[kY] * q->_v[kY] * s;
	f32 zz = q->_v[kZ] * q->_v[kZ] * s;
	f32 xy = q->_v[kX] * q->_v[kY] * s;
	f32 xz = q->_v[kX] * q->_v[kZ] * s;
	f32 yz = q->_v[kY] * q->_v[kZ] * s;
	f32 wx = q->_v[kW] * q->_v[kX] * s;
	f32 wy = q->_v[kW] * q->_v[kY] * s;
	f32 wz = q->_v[kW] * q->_v[kZ] * s;

	/*
	Calculate 3x3 matrix from orthonormal basis
	*/

	/*
	x axis
	*/
	mtxRot->_v[kX][kX] = 1.0f - (yy + zz);
	mtxRot->_v[kY][kX] = xy + wz;
	mtxRot->_v[kZ][kX] = xz - wy;
	
	/*
	y axis
	*/
	mtxRot->_v[kX][kY] = xy - wz;
	mtxRot->_v[kY][kY] = 1.0f - (xx + zz);
	mtxRot->_v[kZ][kY] = yz + wx;
	
	/*
	z axis
	*/
	mtxRot->_v[kX][kZ] = xz + wy;
	mtxRot->_v[kY][kZ] = yz - wx;
	mtxRot->_v[kZ][kZ] = 1.0f - (xx + yy);

	/*
	4th row and column of 4x4 matrix
	Translation and scale are not stored in quaternions, so these
	values are set to default (no scale, no translation).
	For systems where mtxRot comes pre-loaded with scale and translation
	factors, this code can be excluded.
	*/
	mtxRot->_v[kW][kX] = mtxRot->_v[kW][kY] = mtxRot->_v[kW][kZ] = mtxRot->_v[kX][kW] = mtxRot->_v[kY][kW] = mtxRot->_v[kZ][kW] = 0.0f;
	mtxRot->_v[kW][kW] = 1.0f;
}

/*---------------------------------------------------------------------*//**
	行列からクォータニオンへ変換
**//*---------------------------------------------------------------------*/
void Quaternion::convFromMatrix(Vector4F* q, const Matrix4F* mtxRot)
{
	/*
	This code can be optimized for mtxRot->_v[kW][kW] = 1, which 
	should always be true.  This optimization is excluded
	here for clarity.
	*/
	f32 tr = mtxRot->_v[kX][kX] + mtxRot->_v[kY][kY] + mtxRot->_v[kZ][kZ] + mtxRot->_v[kW][kW];
	f32 fourD;
	int i, j, k;
	
	/*
	w >= 0.5 ?
	*/
	if(tr >= 1.0f)
	{
		fourD = 2.0f * sqrtf(tr);
		q->_v[kW] = fourD / 4.0f;
		f32 fourDi = 1.0f / fourD;
		q->_v[kX] = (mtxRot->_v[kZ][kY] - mtxRot->_v[kY][kZ]) * fourDi;
		q->_v[kY] = (mtxRot->_v[kX][kZ] - mtxRot->_v[kZ][kX]) * fourDi;
		q->_v[kZ] = (mtxRot->_v[kY][kX] - mtxRot->_v[kX][kY]) * fourDi;
	}
	else
	{
		/*
		Find the largest component.  
		*/
		if(mtxRot->_v[kX][kX] > mtxRot->_v[kY][kY])
		{
			i = kX;
		}
		else
		{
			i = kY;
		}
		if(mtxRot->_v[kZ][kZ] > mtxRot->_v[i][i])
		{
			i = kZ;
		}
		
		/*
		Set j and k to point to the next two components
		*/
		j = (i + 1) % 3;
		k = (j + 1) % 3;

		/*
		fourD = 4 * largest component
		*/
		fourD = 2.0f * sqrtf(mtxRot->_v[i][i] - mtxRot->_v[j][j] - mtxRot->_v[k][k] + 1.0f);

		/*
		Set the largest component
		*/
		q->_v[i] = fourD * 0.25f;
		
		/*
		Calculate remaining components
		*/
		f32 fourDi = 1.0f / fourD;
		q->_v[j]  = (mtxRot->_v[j][i] + mtxRot->_v[i][j]) * fourDi;
		q->_v[k]  = (mtxRot->_v[k][i] + mtxRot->_v[i][k]) * fourDi;
		q->_v[kW] = (mtxRot->_v[k][j] - mtxRot->_v[j][k]) * fourDi;
	}
}

/*---------------------------------------------------------------------*//**
	オイラー角度からクォータニオンへ変換
**//*---------------------------------------------------------------------*/
void Quaternion::convFromEulerAngles(Vector4F* q, const Vector3F* angle)
{
	Matrix4F mtxRot;
	mtxRot.rotateX(angle->x());
	mtxRot.rotateY(angle->y());
	mtxRot.rotateZ(angle->z());
	convFromMatrix(q, &mtxRot);
}

/*---------------------------------------------------------------------*//**
	線形補間
**//*---------------------------------------------------------------------*/
void Quaternion::lerp(Vector4F* ret, const Vector4F* q1, const Vector4F* q2, f32 t)
{
	*ret = *q1 + (*q2 - *q1) * t;
	ret->normalize();
}

/*---------------------------------------------------------------------*//**
	球面線形補間
**//*---------------------------------------------------------------------*/
void Quaternion::slerp(Vector4F* ret, const Vector4F* q1, const Vector4F* q2, f32 t)
{
	Vector4F q3;
	f32 dot;
	dot = (q1->x() * q2->x()) + (q1->y() * q2->y()) + (q1->z() * q2->z()) + (q1->w() * q2->w());

	if(dot < 0.0f)
	{
		// 90°以上の場合は反転する
		dot = - dot;
		q3 = *q2 * -1;
	}
	else
	{
		q3 = *q2;
	}
	
	if(dot < 0.95f)
	{
		f32 angle = acosf(dot);
		f32 sina = sinf(angle);
		f32 sinat = sinf(angle * t);
		f32 sinaomt = sinf(angle * (1 - t));
		*ret = (*q1 * sinaomt + q3 * sinat) / sina;
	}
	else
	{
		// 角度が小さい場合は線形補間
		lerp(ret, q1, &q3, t);
	}
}

/*---------------------------------------------------------------------*//**
	球面三次補間における球面線形補間（90°以上の場合でも反転しない）
**//*---------------------------------------------------------------------*/
void Quaternion::slerpNoInvert(Vector4F* ret, const Vector4F* q1, const Vector4F* q2, f32 t)
{
	f32 dot = (q1->x() * q2->x()) + (q1->y() * q2->y()) + (q1->z() * q2->z()) + (q1->w() * q2->w());
	if((dot > -0.95f) && (dot < 0.95f))
	{
		f32 angle = acosf(dot);
		f32 sina = sinf(angle);
		f32 sinat = sinf(angle * t);
		f32 sinaomt = sinf(angle * (1 - t));
		*ret = (*q1 * sinaomt + *q2 * sinat) / sina;
	}
	else
	{
		// 角度が小さい場合は線形補間
		lerp(ret, q1, q2, t);
	}
}

/*---------------------------------------------------------------------*//**
	球面三次補間
**//*---------------------------------------------------------------------*/
void Quaternion::squad(Vector4F* ret, const Vector4F* q1, const Vector4F* q2, const Vector4F* a, const Vector4F* b, f32 t)
{
	Vector4F c, d;
	slerpNoInvert(&c, q1, q2, t);
	slerpNoInvert(&d, a, b, t);
	slerpNoInvert(ret, &c, &d, 2 * t * (1 - t));
}

/*---------------------------------------------------------------------*//**
	スプライン補間
**//*---------------------------------------------------------------------*/
void Quaternion::spline(Vector4F* ret, const Vector4F* qnm1, const Vector4F* qn, const Vector4F* qnp1)
{
	Vector4F qni;
	qni.x() = - qn->x();
	qni.y() = - qn->y();
	qni.z() = - qn->z();
	qni.w() = qn->w();
	*ret = multiply(*qn, exp((log(multiply(qni, *qnm1)) + log(multiply(qni, *qnp1))) * -0.25f));
}

/*---------------------------------------------------------------------*//**
	積
**//*---------------------------------------------------------------------*/
Vector4F Quaternion::multiply(const Vector4F& q1, const Vector4F& q2)
{
	Vector4F ret;
	ret.x() = q1.y() * q2.z() - q1.z() * q2.y() + q1.w() * q2.x() + q1.x() * q2.w();
	ret.y() = q1.z() * q2.x() - q1.x() * q2.z() + q1.w() * q2.y() + q1.y() * q2.w();
	ret.z() = q1.x() * q2.y() - q1.y() * q2.x() + q1.w() * q2.z() + q1.z() * q2.w();
	ret.w() = q1.w() * q2.w() - q1.x() * q2.x() - q1.y() * q2.y() - q1.z() * q2.z();
	return ret;
}

/*---------------------------------------------------------------------*//**
	log
**//*---------------------------------------------------------------------*/
Vector4F Quaternion::log(const Vector4F& q)
{
	f32 a = acosf(q.w());
	f32 sina = sinf(a);
	Vector4F ret;
	if(sina > 0.0f)
	{
		f32 sinai = 1.0f / sina;
		ret.x() = a * q.x() * sinai;
		ret.y() = a * q.y() * sinai;
		ret.z() = a * q.z() * sinai;
	}
	return ret;
}

/*---------------------------------------------------------------------*//**
	Exponential
**//*---------------------------------------------------------------------*/
Vector4F Quaternion::exp(const Vector4F& q)
{
	f32 a = sqrtf(q.x() * q.x() + q.y() * q.y() + q.z() * q.z());
	f32 sina = sinf(a);
	f32 cosa = cosf(a);
	Vector4F ret;
	ret.w() = cosa;
	if(a > 0.0f)
	{
		f32 ai = 1.0f / a;
		ret.x() = sina * q.x() * ai;
		ret.y() = sina * q.y() * ai;
		ret.z() = sina * q.z() * ai;
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
