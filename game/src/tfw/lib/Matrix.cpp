/***********************************************************************//**
 *	Matrix.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/06/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Matrix.h"

// Friends
#include "Vector.h"

// External

// Library
#include <math.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define R 0

#define INNER_PRODUCT(m1_, m2_, r_, c_) \
      ((m1_)->_v[r_][0] * (m2_)->_v[0][c_]) \
    + ((m1_)->_v[r_][1] * (m2_)->_v[1][c_]) \
    + ((m1_)->_v[r_][2] * (m2_)->_v[2][c_]) \
    + ((m1_)->_v[r_][3] * (m2_)->_v[3][c_])

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Matrix4F 静的メンバ変数

const Matrix4F Matrix4F::s_mtxIdentity;

//==========================================================================
// Matrix4F メソッド

Matrix4F* Matrix4F::set(const Vector4F* v1, const Vector4F* v2, const Vector4F* v3, const Vector4F* v4)
{
	::memcpy(_v[0], v1, sizeof(Vector4F));
	::memcpy(_v[1], v2, sizeof(Vector4F));
	::memcpy(_v[2], v3, sizeof(Vector4F));
	::memcpy(_v[3], v4, sizeof(Vector4F));
	return this;
}

Matrix4F* Matrix4F::set(int raw, const Vector4F* v)
{
	::memcpy(_v[raw], v, sizeof(Vector4F));
	return this;
}

Matrix4F* Matrix4F::set(int raw, const Vector3F* v)
{
	::memcpy(_v[raw], v, sizeof(Vector3F));
	return this;
}

void Matrix4F::mult(Matrix4F* mtxRes, const Matrix4F* mtxL, const Matrix4F* mtxR)
{
#if R
	mtxRes->_v[0][0] = INNER_PRODUCT(mtxR, mtxL, 0, 0);
	mtxRes->_v[1][0] = INNER_PRODUCT(mtxR, mtxL, 1, 0);
	mtxRes->_v[2][0] = INNER_PRODUCT(mtxR, mtxL, 2, 0);
	mtxRes->_v[3][0] = INNER_PRODUCT(mtxR, mtxL, 3, 0);
	mtxRes->_v[0][1] = INNER_PRODUCT(mtxR, mtxL, 0, 1);
	mtxRes->_v[1][1] = INNER_PRODUCT(mtxR, mtxL, 1, 1);
	mtxRes->_v[2][1] = INNER_PRODUCT(mtxR, mtxL, 2, 1);
	mtxRes->_v[3][1] = INNER_PRODUCT(mtxR, mtxL, 3, 1);
	mtxRes->_v[0][2] = INNER_PRODUCT(mtxR, mtxL, 0, 2);
	mtxRes->_v[1][2] = INNER_PRODUCT(mtxR, mtxL, 1, 2);
	mtxRes->_v[2][2] = INNER_PRODUCT(mtxR, mtxL, 2, 2);
	mtxRes->_v[3][2] = INNER_PRODUCT(mtxR, mtxL, 3, 2);
	mtxRes->_v[0][3] = INNER_PRODUCT(mtxR, mtxL, 0, 3);
	mtxRes->_v[1][3] = INNER_PRODUCT(mtxR, mtxL, 1, 3);
	mtxRes->_v[2][3] = INNER_PRODUCT(mtxR, mtxL, 2, 3);
	mtxRes->_v[3][3] = INNER_PRODUCT(mtxR, mtxL, 3, 3);
#else
	mtxRes->_v[0][0] = INNER_PRODUCT(mtxL, mtxR, 0, 0);
	mtxRes->_v[0][1] = INNER_PRODUCT(mtxL, mtxR, 0, 1);
	mtxRes->_v[0][2] = INNER_PRODUCT(mtxL, mtxR, 0, 2);
	mtxRes->_v[0][3] = INNER_PRODUCT(mtxL, mtxR, 0, 3);
	mtxRes->_v[1][0] = INNER_PRODUCT(mtxL, mtxR, 1, 0);
	mtxRes->_v[1][1] = INNER_PRODUCT(mtxL, mtxR, 1, 1);
	mtxRes->_v[1][2] = INNER_PRODUCT(mtxL, mtxR, 1, 2);
	mtxRes->_v[1][3] = INNER_PRODUCT(mtxL, mtxR, 1, 3);
	mtxRes->_v[2][0] = INNER_PRODUCT(mtxL, mtxR, 2, 0);
	mtxRes->_v[2][1] = INNER_PRODUCT(mtxL, mtxR, 2, 1);
	mtxRes->_v[2][2] = INNER_PRODUCT(mtxL, mtxR, 2, 2);
	mtxRes->_v[2][3] = INNER_PRODUCT(mtxL, mtxR, 2, 3);
	mtxRes->_v[3][0] = INNER_PRODUCT(mtxL, mtxR, 3, 0);
	mtxRes->_v[3][1] = INNER_PRODUCT(mtxL, mtxR, 3, 1);
	mtxRes->_v[3][2] = INNER_PRODUCT(mtxL, mtxR, 3, 2);
	mtxRes->_v[3][3] = INNER_PRODUCT(mtxL, mtxR, 3, 3);
#endif
}

void Matrix4F::mult(const Matrix4F* mtxR)
{
#if R	// preMult
	TYPE tmp[4];
	tmp[0] = INNER_PRODUCT(mtxR, this, 0, 0);
	tmp[1] = INNER_PRODUCT(mtxR, this, 1, 0);
	tmp[2] = INNER_PRODUCT(mtxR, this, 2, 0);
	tmp[3] = INNER_PRODUCT(mtxR, this, 3, 0);
	_v[0][0] = tmp[0];
	_v[1][0] = tmp[1];
	_v[2][0] = tmp[2];
	_v[3][0] = tmp[3];
	tmp[0] = INNER_PRODUCT(mtxR, this, 0, 1);
	tmp[1] = INNER_PRODUCT(mtxR, this, 1, 1);
	tmp[2] = INNER_PRODUCT(mtxR, this, 2, 1);
	tmp[3] = INNER_PRODUCT(mtxR, this, 3, 1);
	_v[0][1] = tmp[0];
	_v[1][1] = tmp[1];
	_v[2][1] = tmp[2];
	_v[3][1] = tmp[3];
	tmp[0] = INNER_PRODUCT(mtxR, this, 0, 2);
	tmp[1] = INNER_PRODUCT(mtxR, this, 1, 2);
	tmp[2] = INNER_PRODUCT(mtxR, this, 2, 2);
	tmp[3] = INNER_PRODUCT(mtxR, this, 3, 2);
	_v[0][2] = tmp[0];
	_v[1][2] = tmp[1];
	_v[2][2] = tmp[2];
	_v[3][2] = tmp[3];
	tmp[0] = INNER_PRODUCT(mtxR, this, 0, 3);
	tmp[1] = INNER_PRODUCT(mtxR, this, 1, 3);
	tmp[2] = INNER_PRODUCT(mtxR, this, 2, 3);
	tmp[3] = INNER_PRODUCT(mtxR, this, 3, 3);
	_v[0][3] = tmp[0];
	_v[1][3] = tmp[1];
	_v[2][3] = tmp[2];
	_v[3][3] = tmp[3];
#else	// postMult
	TYPE tmp[4];
	tmp[0] = INNER_PRODUCT(this, mtxR, 0, 0);
	tmp[1] = INNER_PRODUCT(this, mtxR, 0, 1);
	tmp[2] = INNER_PRODUCT(this, mtxR, 0, 2);
	tmp[3] = INNER_PRODUCT(this, mtxR, 0, 3);
	_v[0][0] = tmp[0];
	_v[0][1] = tmp[1];
	_v[0][2] = tmp[2];
	_v[0][3] = tmp[3];
	tmp[0] = INNER_PRODUCT(this, mtxR, 1, 0);
	tmp[1] = INNER_PRODUCT(this, mtxR, 1, 1);
	tmp[2] = INNER_PRODUCT(this, mtxR, 1, 2);
	tmp[3] = INNER_PRODUCT(this, mtxR, 1, 3);
	_v[1][0] = tmp[0];
	_v[1][1] = tmp[1];
	_v[1][2] = tmp[2];
	_v[1][3] = tmp[3];
	tmp[0] = INNER_PRODUCT(this, mtxR, 2, 0);
	tmp[1] = INNER_PRODUCT(this, mtxR, 2, 1);
	tmp[2] = INNER_PRODUCT(this, mtxR, 2, 2);
	tmp[3] = INNER_PRODUCT(this, mtxR, 2, 3);
	_v[2][0] = tmp[0];
	_v[2][1] = tmp[1];
	_v[2][2] = tmp[2];
	_v[2][3] = tmp[3];
	tmp[0] = INNER_PRODUCT(this, mtxR, 3, 0);
	tmp[1] = INNER_PRODUCT(this, mtxR, 3, 1);
	tmp[2] = INNER_PRODUCT(this, mtxR, 3, 2);
	tmp[3] = INNER_PRODUCT(this, mtxR, 3, 3);
	_v[3][0] = tmp[0];
	_v[3][1] = tmp[1];
	_v[3][2] = tmp[2];
	_v[3][3] = tmp[3];
#endif
}

void Matrix4F::mult(Vector3F* vRes, const Vector3F* vR) const
{
	if(vRes == vR)
	{
		#if 1	// preMult
			f32 d = 1.0f / (_v[0][3] * vR->_v[0] + _v[1][3] * vR->_v[1] + _v[2][3] * vR->_v[2] + _v[3][3]);
			f32 x = (_v[0][0] * vR->_v[0] + _v[1][0] * vR->_v[1] + _v[2][0] * vR->_v[2] + _v[3][0]) * d;
			f32 y = (_v[0][1] * vR->_v[0] + _v[1][1] * vR->_v[1] + _v[2][1] * vR->_v[2] + _v[3][1]) * d;
			f32 z = (_v[0][2] * vR->_v[0] + _v[1][2] * vR->_v[1] + _v[2][2] * vR->_v[2] + _v[3][2]) * d;
		#else	// postMult
			f32 d = 1.0f / (_v[3][0] * vR->_v[0] + _v[3][1] * vR->_v[1] + _v[3][2] * vR->_v[2] + _v[3][3]);
			f32 x = (_v[0][0] * vR->_v[0] + _v[0][1] * vR->_v[1] + _v[0][2] * vR->_v[2] + _v[0][3]) * d;
			f32 y = (_v[1][0] * vR->_v[0] + _v[1][1] * vR->_v[1] + _v[1][2] * vR->_v[2] + _v[1][3]) * d;
			f32 z = (_v[2][0] * vR->_v[0] + _v[2][1] * vR->_v[1] + _v[2][2] * vR->_v[2] + _v[2][3]) * d;
		#endif
		vRes->_v[0] = x;
		vRes->_v[1] = y;
		vRes->_v[2] = z;
	}
	else
	{
		#if 1	// preMult
			f32 d = 1.0f / (_v[0][3] * vR->_v[0] + _v[1][3] * vR->_v[1] + _v[2][3] * vR->_v[2] + _v[3][3]);
			vRes->_v[0] = (_v[0][0] * vR->_v[0] + _v[1][0] * vR->_v[1] + _v[2][0] * vR->_v[2] + _v[3][0]) * d;
			vRes->_v[1] = (_v[0][1] * vR->_v[0] + _v[1][1] * vR->_v[1] + _v[2][1] * vR->_v[2] + _v[3][1]) * d;
			vRes->_v[2] = (_v[0][2] * vR->_v[0] + _v[1][2] * vR->_v[1] + _v[2][2] * vR->_v[2] + _v[3][2]) * d;
		#else	// postMult
			f32 d = 1.0f / (_v[3][0] * vR->_v[0] + _v[3][1] * vR->_v[1] + _v[3][2] * vR->_v[2] + _v[3][3]);
			vRes->_v[0] = (_v[0][0] * vR->_v[0] + _v[0][1] * vR->_v[1] + _v[0][2] * vR->_v[2] + _v[0][3]) * d;
			vRes->_v[1] = (_v[1][0] * vR->_v[0] + _v[1][1] * vR->_v[1] + _v[1][2] * vR->_v[2] + _v[1][3]) * d;
			vRes->_v[2] = (_v[2][0] * vR->_v[0] + _v[2][1] * vR->_v[1] + _v[2][2] * vR->_v[2] + _v[2][3]) * d;
		#endif
	}
}

void Matrix4F::mult(Vector4F* vRes, const Vector4F* vR) const
{
	if(vRes == vR)
	{
		#if 1	// preMult
			f32 x = _v[0][0] * vR->_v[0] + _v[1][0] * vR->_v[1] + _v[2][0] * vR->_v[2] + _v[3][0] * vR->_v[3];
			f32 y = _v[0][1] * vR->_v[0] + _v[1][1] * vR->_v[1] + _v[2][1] * vR->_v[2] + _v[3][1] * vR->_v[3];
			f32 z = _v[0][2] * vR->_v[0] + _v[1][2] * vR->_v[1] + _v[2][2] * vR->_v[2] + _v[3][2] * vR->_v[3];
			f32 w = _v[0][3] * vR->_v[0] + _v[1][3] * vR->_v[1] + _v[2][3] * vR->_v[2] + _v[3][3] * vR->_v[3];
		#else	// postMult
			f32 x = _v[0][0] * vR->_v[0] + _v[0][1] * vR->_v[1] + _v[0][2] * vR->_v[2] + _v[0][3] * vR->_v[3];
			f32 y = _v[1][0] * vR->_v[0] + _v[1][1] * vR->_v[1] + _v[1][2] * vR->_v[2] + _v[1][3] * vR->_v[3];
			f32 z = _v[2][0] * vR->_v[0] + _v[2][1] * vR->_v[1] + _v[2][2] * vR->_v[2] + _v[2][3] * vR->_v[3];
			f32 w = _v[3][0] * vR->_v[0] + _v[3][1] * vR->_v[1] + _v[3][2] * vR->_v[2] + _v[3][3] * vR->_v[3];
		#endif
		vRes->_v[0] = x;
		vRes->_v[1] = y;
		vRes->_v[2] = z;
		vRes->_v[3] = w;
	}
	else
	{
		#if 1	// preMult
			vRes->_v[0] = _v[0][0] * vR->_v[0] + _v[1][0] * vR->_v[1] + _v[2][0] * vR->_v[2] + _v[3][0] * vR->_v[3];
			vRes->_v[1] = _v[0][1] * vR->_v[0] + _v[1][1] * vR->_v[1] + _v[2][1] * vR->_v[2] + _v[3][1] * vR->_v[3];
			vRes->_v[2] = _v[0][2] * vR->_v[0] + _v[1][2] * vR->_v[1] + _v[2][2] * vR->_v[2] + _v[3][2] * vR->_v[3];
			vRes->_v[3] = _v[0][3] * vR->_v[0] + _v[1][3] * vR->_v[1] + _v[2][3] * vR->_v[2] + _v[3][3] * vR->_v[3];
		#else	// postMult
			vRes->_v[0] = _v[0][0] * vR->_v[0] + _v[0][1] * vR->_v[1] + _v[0][2] * vR->_v[2] + _v[0][3] * vR->_v[3];
			vRes->_v[1] = _v[1][0] * vR->_v[0] + _v[1][1] * vR->_v[1] + _v[1][2] * vR->_v[2] + _v[1][3] * vR->_v[3];
			vRes->_v[2] = _v[2][0] * vR->_v[0] + _v[2][1] * vR->_v[1] + _v[2][2] * vR->_v[2] + _v[2][3] * vR->_v[3];
			vRes->_v[3] = _v[3][0] * vR->_v[0] + _v[3][1] * vR->_v[1] + _v[3][2] * vR->_v[2] + _v[3][3] * vR->_v[3];
		#endif
	}
}

void Matrix4F::setScale(const Vector3F* vec)
{
	setScale(vec->_v[0], vec->_v[1], vec->_v[2]);
}

void Matrix4F::setScale(TYPE x, TYPE y, TYPE z)
{
	set(	x,					0.0f,				0.0f,				0.0f,
			0.0f,				y,					0.0f,				0.0f,
			0.0f,				0.0f,				z,					0.0f,
			0.0f,				0.0f,				0.0f,				1.0f				);
}

void Matrix4F::scale(const Vector3F* vec)
{
	_v[0][0] *= vec->x();
	_v[1][1] *= vec->y();
	_v[2][2] *= vec->z();
}

void Matrix4F::scale(TYPE x, TYPE y, TYPE z)
{
	_v[0][0] *= x;
	_v[1][1] *= y;
	_v[2][2] *= z;
}

void Matrix4F::setTranslate(const Vector3F* vec)
{
	setTranslate(vec->_v[0], vec->_v[1], vec->_v[2]);
}

void Matrix4F::setTranslate(TYPE x, TYPE y, TYPE z)
{
#if R
	set(	1.0f,				0.0f,				0.0f,				x,
			0.0f,				1.0f,				0.0f,				y,
			0.0f,				0.0f,				1.0f,				z,
			0.0f,				0.0f,				0.0f,				1.0f				);
#else
	set(	1.0f,				0.0f,				0.0f,				0.0f,
			0.0f,				1.0f,				0.0f,				0.0f,
			0.0f,				0.0f,				1.0f,				0.0f,
			x,					y,					z,					1.0f				);
#endif
}

void Matrix4F::translate(const Vector3F* vec)
{
	Matrix4F mtxTrans;
	mtxTrans.setTranslate(vec->_v[0], vec->_v[1], vec->_v[2]);
	mult(&mtxTrans);
}

void Matrix4F::translate(TYPE x, TYPE y, TYPE z)
{
	Matrix4F mtxTrans;
	mtxTrans.setTranslate(x, y, z);
	mult(&mtxTrans);
}

void Matrix4F::setRotateX(f32 angle)
{
	f32 cos = ::cosf(angle);
	f32 sin = ::sinf(angle);
#if R
	set(	1.0f,				0.0f,				0.0f,				0.0f,
			0.0f,				cos,				- sin,				0.0f,
			0.0f,				sin,				cos,				0.0f,
			0.0f,				0.0f,				0.0f,				1.0f		      );
#else
	set(	1.0f,				0.0f,				0.0f,				0.0f,
			0.0f,				cos,				sin,				0.0f,
			0.0f,				- sin,				cos,				0.0f,
			0.0f,				0.0f,				0.0f,				1.0f		      );
#endif
}

void Matrix4F::rotateX(f32 angle)
{
	Matrix4F mtxRot;
	mtxRot.setRotateX(angle);
	mult(&mtxRot);
}

void Matrix4F::setRotateY(f32 angle)
{
	f32 cos = ::cosf(angle);
	f32 sin = ::sinf(angle);
#if R
	set(	cos,				0.0f,				sin,				0.0f,
			0.0f,				1.0f,				0.0f,				0.0f,
			- sin,				0.0f,				cos,				0.0f,
			0.0f,				0.0f,				0.0f,				1.0f		       );
#else
	set(	cos,				0.0f,				- sin,				0.0f,
			0.0f,				1.0f,				0.0f,				0.0f,
			sin,				0.0f,				cos,				0.0f,
			0.0f,				0.0f,				0.0f,				1.0f		       );
#endif
}

void Matrix4F::rotateY(f32 angle)
{
	Matrix4F mtxRot;
	mtxRot.setRotateY(angle);
	mult(&mtxRot);
}

void Matrix4F::setRotateZ(f32 angle)
{
	f32 cos = ::cosf(angle);
	f32 sin = ::sinf(angle);
#if R
	set(	cos,				- sin,				0.0f,				0.0f,
			sin,				cos,				0.0f,				0.0f,
			0.0f,				0.0f,				1.0f,				0.0f,
			0.0f,				0.0f,				0.0f,				1.0f		       );
#else
	set(	cos,				sin,				0.0f,				0.0f,
			- sin,				cos,				0.0f,				0.0f,
			0.0f,				0.0f,				1.0f,				0.0f,
			0.0f,				0.0f,				0.0f,				1.0f		       );
#endif
}

void Matrix4F::rotateZ(f32 angle)
{
	Matrix4F mtxRot;
	mtxRot.setRotateZ(angle);
	mult(&mtxRot);
}

bool Matrix4F::inverse()
{
	unsigned int indxc[4], indxr[4], ipiv[4];
	unsigned int i, j, k, l, ll;
	unsigned int icol = 0;
	unsigned int irow = 0;
	TYPE temp, pivinv, dum, big;

	for(j = 0; j < 4; j++)	{	ipiv[j] = 0;	}

	for(i = 0; i < 4; i++)
	{
		big = 0.0f;
		for(j = 0; j < 4; j++)
		{
			if(ipiv[j] != 1)
			{
				for(k = 0; k < 4; k++)
				{
					if(ipiv[k] == 0)
					{
						if(TFW_ABS(_v[j][k]) >= big)
						{
							big = TFW_ABS(_v[j][k]);
							irow=j;
							icol=k;
						}
					}
					else if(ipiv[k] > 1)
					{
						return false;
					}
				}
			}
		}
		++(ipiv[icol]);
		if(irow != icol)
		{
		   for(l = 0; l < 4; l++)	{	TFW_SWAP(_v[irow][l], _v[icol][l], temp);	}
		}

		indxr[i] = irow;
		indxc[i] = icol;
		if(_v[icol][icol] == 0)
		{
			return false;
		}

		pivinv = 1.0f / _v[icol][icol];
		_v[icol][icol] = 1;
		for(l = 0; l < 4; l++)	{	_v[icol][l] *= pivinv;	}
		for(ll = 0; ll < 4; ll++)
		{
			if(ll != icol)
			{
				dum=_v[ll][icol];
				_v[ll][icol] = 0;
				for(l = 0; l < 4; l++)	{	_v[ll][l] -= _v[icol][l] * dum;	}
			}
		}
	}
	for(int lx = 4; lx > 0; --lx)
	{
		if(indxr[lx-1] != indxc[lx-1])
		{
			for(k = 0; k < 4; k++)
			{
				TFW_SWAP(_v[k][indxr[lx-1]], _v[k][indxc[lx-1]], temp);
			}
		}
	}

	return true;
}

void Matrix4F::ortho(f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar)
{
	set(
		2.0f / (right - left),				0.0f,								0.0f,								0.0f,
		0.0f,								2.0f / (top - bottom),				0.0f,								0.0f,
		0.0f,								0.0f,								- 2.0f / (zfar - znear),			0.0f,
		- (right + left) / (right - left),	- (top + bottom) / (top - bottom),	- (zfar + znear) / (zfar - znear),	1.0f	);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
