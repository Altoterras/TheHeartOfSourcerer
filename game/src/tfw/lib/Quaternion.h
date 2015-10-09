/***********************************************************************//**
 *	Quaternion.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2012/11/14.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_QUATERNION_H_
#define _TFW_LIB_QUATERNION_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include <math.h>

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
template<class TYPE> class Vector4;
typedef Vector3<f32> Vector3F;
typedef Vector4<f32> Vector4F;
class Matrix4F;

/*---------------------------------------------------------------------*//**
 *	クォータニオン
 *
**//*---------------------------------------------------------------------*/
class Quaternion
{
public:
	static void convToMatrix(Matrix4F* mtxRot, const Vector4F* q);
	static void convFromMatrix(Vector4F* q, const Matrix4F* mtxRot);
	static void convFromEulerAngles(Vector4F* q, const Vector3F* angle);

	static void lerp(Vector4F* ret, const Vector4F* q1, const Vector4F* q2, f32 t);

	static void slerp(Vector4F* ret, const Vector4F* q1, const Vector4F* q2, f32 t);
	static void slerpNoInvert(Vector4F* ret, const Vector4F* q1, const Vector4F* q2, f32 t);
	static void squad(Vector4F* ret, const Vector4F* q1, const Vector4F* q2, const Vector4F* a, const Vector4F* b, f32 t);
	static void spline(Vector4F* ret, const Vector4F* qnm1, const Vector4F* qn, const Vector4F* qnp1);

private:
	static Vector4F multiply(const Vector4F& q1, const Vector4F& q2);
	static Vector4F log(const Vector4F& q);
	static Vector4F exp(const Vector4F& q);


#if 0
public:
	typedef f32 TYPE;

	TYPE		_v[4][4];

	static const Matrix4F s_mtxIdentity;

	Matrix4F() { setIdentity(); }
	Matrix4F(
		TYPE a00, TYPE a01, TYPE a02, TYPE a03,
		TYPE a10, TYPE a11, TYPE a12, TYPE a13,
		TYPE a20, TYPE a21, TYPE a22, TYPE a23,
		TYPE a30, TYPE a31, TYPE a32, TYPE a33)
	{ set(a00, a01, a02, a03, a10, a11, a12, a13, a20, a21, a22, a23, a30, a31, a32, a33); }
	explicit Matrix4F(const Matrix4F* src)							{ set(src); }
	Matrix4F(const Matrix4F& src)									{ set(&src); }

	inline TYPE& operator()(int row, int col)						{ return _v[row][col]; }
	inline TYPE operator()(int row, int col) const					{ return _v[row][col]; }

	inline TYPE* ptr()												{ return (TYPE*)_v; }
	inline const TYPE* ptr() const									{ return (TYPE*)_v; }

	inline Matrix4F* set(
		TYPE a00, TYPE a01, TYPE a02, TYPE a03,
		TYPE a10, TYPE a11, TYPE a12, TYPE a13,
		TYPE a20, TYPE a21, TYPE a22, TYPE a23,
		TYPE a30, TYPE a31, TYPE a32, TYPE a33)
	{
		_v[0][0] = a00; _v[0][1] = a01; _v[0][2] = a02; _v[0][3] = a03;
		_v[1][0] = a10; _v[1][1] = a11; _v[1][2] = a12; _v[1][3] = a13;
		_v[2][0] = a20; _v[2][1] = a21; _v[2][2] = a22; _v[2][3] = a23;
		_v[3][0] = a30; _v[3][1] = a31; _v[3][2] = a32; _v[3][3] = a33;
		return this;
	}
	inline Matrix4F* set(const TYPE* const ptr)
	{
		TYPE* ptrDst = (TYPE*)_v;
		for(int i = 0; i < 16; i++) { ptrDst[i] = (TYPE)ptr[i]; }
		return this;
	}
	Matrix4F* set(const Vector4F* v1, const Vector4F* v2, const Vector4F* v3, const Vector4F* v4);
	Matrix4F* set(int raw, const Vector4F* v);
	Matrix4F* set(int raw, const Vector3F* v);
	inline Matrix4F* set(const Matrix4F* p)							{ set(p->ptr()); return this; }
	inline Matrix4F& operator = (const Matrix4F& p)					{ set(p.ptr()); return *this; }

	inline bool equals(const Matrix4F* p) const
	{
		return (_v[0][0] == p->_v[0][0]) && (_v[0][1] == p->_v[0][1]) && (_v[0][2] == p->_v[0][2]) && (_v[0][3] == p->_v[0][3])
			&& (_v[1][0] == p->_v[1][0]) && (_v[1][1] == p->_v[1][1]) && (_v[1][2] == p->_v[1][2]) && (_v[1][3] == p->_v[1][3])
			&& (_v[2][0] == p->_v[2][0]) && (_v[2][1] == p->_v[2][1]) && (_v[2][2] == p->_v[2][2]) && (_v[2][3] == p->_v[2][3])
			&& (_v[3][0] == p->_v[3][0]) && (_v[3][1] == p->_v[3][1]) && (_v[3][2] == p->_v[3][2]) && (_v[3][3] == p->_v[3][3]);
	}
	inline bool operator == (const Matrix4F& p) const				{ return equals(&p); }
	inline bool operator != (const Matrix4F& p) const				{ return !equals(&p); }

	inline static const Matrix4F* identity()						{ return &s_mtxIdentity; }
	inline void setIdentity()
	{
		set(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}
	inline bool isIdentity() const									{ return equals(&s_mtxIdentity); }

	// mult
	static void mult(Matrix4F* mtxRes, const Matrix4F* mtxL, const Matrix4F* mtxR);
	void mult(const Matrix4F* mtxR);
	void mult(Vector3F* vRes, const Vector3F* vR) const;
	void mult(Vector4F* vRes, const Vector4F* vR) const;

	// scale
	void setScale(const Vector3F* vec);
	void setScale(TYPE x, TYPE y, TYPE z);
	void scale(const Vector3F* vec);
	void scale(TYPE x, TYPE y, TYPE z);

	// translate
	void setTranslate(const Vector3F* vec);
	void setTranslate(TYPE x, TYPE y, TYPE z);
	void translate(const Vector3F* vec);
	void translate(TYPE x, TYPE y, TYPE z);

	// rotate
	void setRotateX(f32 angle);
	void rotateX(f32 angle);
	void setRotateY(f32 angle);
	void rotateY(f32 angle);
	void setRotateZ(f32 angle);
	void rotateZ(f32 angle);

	bool inverse();
	void ortho(f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar);
#endif
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_LIB_QUATERNION_H_

