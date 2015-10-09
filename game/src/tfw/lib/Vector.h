/***********************************************************************//**
 *	Vector.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2009/04/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_VECTOR_H_
#define _TFW_VECTOR_H_

#include "../Base.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	2 要素ベクトル
 *	
**//*---------------------------------------------------------------------*/
template<class TYPE> class Vector2
{
public:
	enum { NUM_COMPONENTS = 2 };

	TYPE		_v[NUM_COMPONENTS];

	Vector2()														{ set(0, 0); }
	Vector2(TYPE x, TYPE y)											{ set(x, y); }
	explicit Vector2(const Vector2* src)							{ copy(src); }
	Vector2(const Vector2& src)										{ copy(&src); }

	inline TYPE& x()												{ return _v[0]; }
	inline TYPE& y()												{ return _v[1]; }
	inline TYPE x() const											{ return _v[0]; }
	inline TYPE y() const											{ return _v[1]; }

	inline TYPE& operator [] (int i)								{ return _v[i]; }
	inline TYPE operator [] (int i) const							{ return _v[i]; }

	inline TYPE* ptr()												{ return _v; }
	inline const TYPE* ptr() const									{ return _v; }

	inline void set(TYPE x, TYPE y)									{ _v[0] = x; _v[1] = y; }
	inline void copy(const Vector2* p)								{ _v[0] = p->_v[0]; _v[1] = p->_v[1]; }
	inline Vector2& operator = (const Vector2& p)					{ _v[0] = p._v[0]; _v[1] = p._v[1]; return *this; }
	inline static Vector2* setout(Vector2* v, TYPE x, TYPE y)		{ v->set(x, y); return v; }

	inline bool equals(const Vector2* p) const						{ return (_v[0] == p->_v[0]) && (_v[1] == p->_v[1]); }
	inline bool operator == (const Vector2& p) const				{ return (_v[0] == p._v[0]) && (_v[1] == p._v[1]); }
	inline bool operator != (const Vector2& p) const				{ return (_v[0] != p._v[0]) || (_v[1] != p._v[1]); }

	// Negation operator. Returns the negative of the Vector2
	inline const Vector2 nega() const								{ return Vector2(- _v[0], - _v[1]); }
	inline const Vector2 operator - () const						{ return Vector2(- _v[0], - _v[1]); }

	// Binary vector add
	inline const Vector2 operator + (const Vector2& p) const		{ return Vector2(_v[0] + p._v[0], _v[1] + p._v[1]); }

	// Unary vector add. Slightly more efficient because no temporary intermediate object
	inline Vector2* add(const Vector2* p)							{ _v[0] += p->_v[0]; _v[1] += p->_v[1]; return this; }
	inline Vector2& operator += (const Vector2& p)					{ _v[0] += p._v[0]; _v[1] += p._v[1]; return *this; }

	// Binary vector subtract
	inline const Vector2 operator - (const Vector2& p) const		{ return Vector2(_v[0] - p._v[0], _v[1] - p._v[1]); }

	// Unary vector subtract
	inline Vector2* sub(const Vector2* p)							{ _v[0] -= p->_v[0]; _v[1] -= p->_v[1]; return this; }
	inline Vector2& operator -= (const Vector2& p)					{ _v[0] -= p._v[0]; _v[1] -= p._v[1]; return *this; }

	// Multiply by scalar
	inline const Vector2 operator * (TYPE scl) const				{ return Vector2(_v[0] * scl, _v[1] * scl); }

	// Unary multiply by scalar
	inline Vector2* mul(TYPE scl)									{ _v[0] *= scl; _v[1] *= scl; return this; }
	inline Vector2& operator *= (TYPE scl)							{ _v[0] *= scl; _v[1] *= scl; return *this; }

	// Divide by scalar
	inline const Vector2 operator / (TYPE scl) const				{ return Vector2(_v[0] / scl, _v[1] / scl); }

	// Unary divide by scalar
	inline Vector2* div(TYPE scl)									{ _v[0] /= scl; _v[1] /= scl; return this; }
	inline Vector2& operator /= (TYPE scl)							{ _v[0] /= scl; _v[1] /= scl; return *this; }

	// Length of the vector = sqrt( vec . vec )
	inline TYPE length() const										{ return (TYPE)::sqrtf( (f32)(_v[0] * _v[0] + _v[1] * _v[1]) ); }

	// Length squared of the vector = vec . vec
	inline TYPE lengthSq() const										{ return _v[0] * _v[0] + _v[1] * _v[1]; }

	// Normalize the vector so that it has length unity. Returns the previous length of the vector
	inline TYPE normalize()
	{
		TYPE norm = Vector2::length();
		if(norm > 0.0)
		{
			TYPE inv = 1.0f / norm;
			_v[0] *= inv;
			_v[1] *= inv;
		}
		return norm;
	}
};

/*---------------------------------------------------------------------*//**
 *	3 要素ベクトル
 *	
**//*---------------------------------------------------------------------*/
template<class TYPE> class Vector3
{
public:
	enum { NUM_COMPONENTS = 3 };

	TYPE		_v[NUM_COMPONENTS];

	Vector3()															{ set(0, 0, 0); }
	Vector3(TYPE x, TYPE y, TYPE z)										{ set(x, y, z); }
	explicit Vector3(const Vector3* src)								{ copy(src); }
	Vector3(const Vector3& src)											{ copy(&src); }

	inline TYPE& x()													{ return _v[0]; }
	inline TYPE& y()													{ return _v[1]; }
	inline TYPE& z()													{ return _v[2]; }
	inline TYPE& r()													{ return _v[0]; }
	inline TYPE& g()													{ return _v[1]; }
	inline TYPE& b()													{ return _v[2]; }
	inline TYPE x() const												{ return _v[0]; }
	inline TYPE y() const												{ return _v[1]; }
	inline TYPE z() const												{ return _v[2]; }
	inline TYPE r() const												{ return _v[0]; }
	inline TYPE g() const												{ return _v[1]; }
	inline TYPE b() const												{ return _v[2]; }

	inline TYPE& operator [] (int i)									{ return _v[i]; }
	inline TYPE operator [] (int i) const								{ return _v[i]; }

	inline TYPE* ptr()													{ return _v; }
	inline const TYPE* ptr() const										{ return _v; }

	inline void set(TYPE x, TYPE y, TYPE z)								{ _v[0] = x; _v[1] = y; _v[2] = z; }
	inline void copy(const Vector3* p)									{ _v[0] = p->_v[0]; _v[1] = p->_v[1]; _v[2] = p->_v[2]; }
	inline Vector3& operator = (const Vector3& p)						{ _v[0] = p._v[0]; _v[1] = p._v[1]; _v[2] = p._v[2]; return *this; }
	inline static Vector3* setout(Vector3* v, TYPE x, TYPE y, TYPE z)	{ v->set(x, y, z); return v; }

	inline bool equals(const Vector3* p) const							{ return (_v[0] == p->_v[0]) && (_v[1] == p->_v[1]) && (_v[2] == p->_v[2]); }
	inline bool operator == (const Vector3& p) const					{ return (_v[0] == p._v[0]) && (_v[1] == p._v[1]) && (_v[2] == p._v[2]); }
	inline bool operator != (const Vector3& p) const					{ return (_v[0] != p._v[0]) || (_v[1] != p._v[1]) || (_v[2] != p._v[2]); }

	// Negation operator. Returns the negative of the Vector3
	inline const Vector3 nega() const									{ return Vector3(- _v[0], - _v[1], - _v[2]); }
	inline const Vector3 operator - () const							{ return Vector3(- _v[0], - _v[1], - _v[2]); }

	// Binary vector add
	inline const Vector3 operator + (const Vector3& p) const			{ return Vector3(_v[0] + p._v[0], _v[1] + p._v[1], _v[2] + p._v[2]); }

	// Unary vector add. Slightly more efficient because no temporary intermediate object
	inline Vector3* add(const Vector3* p)								{ _v[0] += p->_v[0]; _v[1] += p->_v[1]; _v[2] += p->_v[2]; return this; }
	inline Vector3& operator += (const Vector3& p)						{ _v[0] += p._v[0]; _v[1] += p._v[1]; _v[2] += p._v[2]; return *this; }

	// Binary vector subtract
	inline const Vector3 operator - (const Vector3& p) const			{ return Vector3(_v[0] - p._v[0], _v[1] - p._v[1], _v[2] - p._v[2]); }

	// Unary vector subtract
	inline Vector3* sub(const Vector3* p)								{ _v[0] -= p->_v[0]; _v[1] -= p->_v[1]; _v[2] -= p->_v[2]; return this; }
	inline Vector3& operator -= (const Vector3& p)						{ _v[0] -= p._v[0]; _v[1] -= p._v[1]; _v[2] -= p._v[2]; return *this; }

	// Multiply by scalar
	inline const Vector3 operator * (TYPE scl) const					{ return Vector3(_v[0] * scl, _v[1] * scl, _v[2] * scl); }

	// Unary multiply by scalar
	inline Vector3* mul(TYPE scl)										{ _v[0] *= scl; _v[1] *= scl; _v[2] *= scl; return this; }
	inline Vector3& operator *= (TYPE scl)								{ _v[0] *= scl; _v[1] *= scl; _v[2] *= scl; return *this; }

	// Divide by scalar
	inline const Vector3 operator / (TYPE scl) const					{ return Vector3(_v[0] / scl, _v[1] / scl, _v[2] / scl); }

	// Unary divide by scalar
	inline Vector3* div(TYPE scl)										{ _v[0] /= scl; _v[1] /= scl; _v[2] /= scl; return this; }
	inline Vector3& operator /= (TYPE scl)								{ _v[0] /= scl; _v[1] /= scl; _v[2] /= scl; return *this; }

	// Dot product
	inline TYPE dot(const Vector3* p) const								{ return _v[0] * p->_v[0] + _v[1] * p->_v[1] + _v[2] * p->_v[2]; }
	inline TYPE operator * (const Vector3& p) const						{ return _v[0] * p._v[0] + _v[1] * p._v[1] + _v[2] * p._v[2]; }

	// Cross product
	inline const Vector3 cross(const Vector3* p) const					{ return Vector3(_v[1] * p->_v[2] - _v[2] * p->_v[1], _v[2] * p->_v[0] - _v[0] * p->_v[2], _v[0] * p->_v[1] - _v[1] * p->_v[0]); }
	inline const Vector3 operator ^ (const Vector3& p) const			{ return Vector3(_v[1] * p._v[2] - _v[2] * p._v[1], _v[2] * p._v[0] - _v[0] * p._v[2], _v[0] * p._v[1] - _v[1] * p._v[0]); }

	// Length of the vector = sqrt( vec . vec )
	inline TYPE length() const											{ return ::sqrtf( _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] ); }

	// Length squared of the vector = vec . vec
	inline TYPE lengthSq() const											{ return _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2]; }

	// Normalize the vector so that it has length unity. Returns the previous length of the vector
	inline TYPE normalize()
	{
		TYPE norm = Vector3::length();
		if(norm > 0.0)
		{
			TYPE inv = 1.0f / norm;
			_v[0] *= inv;
			_v[1] *= inv;
			_v[2] *= inv;
		}
		return norm;
	}
};

/*---------------------------------------------------------------------*//**
 *	4 要素ベクトル
 *	
**//*---------------------------------------------------------------------*/
template<class TYPE> class Vector4
{
public:
	enum { NUM_COMPONENTS = 4 };

	TYPE		_v[NUM_COMPONENTS];

	Vector4()																	{ set(0, 0, 0, 0); }
	Vector4(TYPE x, TYPE y, TYPE z, TYPE w)										{ set(x, y, z, w); }
	explicit Vector4(const Vector4* src)										{ copy(src); }
	Vector4(const Vector4& src)													{ copy(&src); }

	inline TYPE& x()															{ return _v[0]; }
	inline TYPE& y()															{ return _v[1]; }
	inline TYPE& z()															{ return _v[2]; }
	inline TYPE& w()															{ return _v[3]; }
	inline TYPE& r()															{ return _v[0]; }
	inline TYPE& g()															{ return _v[1]; }
	inline TYPE& b()															{ return _v[2]; }
	inline TYPE& a()															{ return _v[3]; }
	inline TYPE x() const														{ return _v[0]; }
	inline TYPE y() const														{ return _v[1]; }
	inline TYPE z() const														{ return _v[2]; }
	inline TYPE w() const														{ return _v[3]; }
	inline TYPE r() const														{ return _v[0]; }
	inline TYPE g() const														{ return _v[1]; }
	inline TYPE b() const														{ return _v[2]; }
	inline TYPE a() const														{ return _v[3]; }

	inline TYPE& operator [] (int i)											{ return _v[i]; }
	inline TYPE operator [] (int i) const										{ return _v[i]; }

	inline TYPE* ptr()															{ return _v; }
	inline const TYPE* ptr() const												{ return _v; }

	inline void set(TYPE x, TYPE y, TYPE z, TYPE w)								{ _v[0] = x; _v[1] = y; _v[2] = z; _v[3] = w; }
	inline void copy(const Vector4* p)											{ _v[0] = p->_v[0]; _v[1] = p->_v[1]; _v[2] = p->_v[2]; _v[3] = p->_v[3]; }
	inline Vector4& operator = (const Vector4& p)								{ _v[0] = p._v[0]; _v[1] = p._v[1]; _v[2] = p._v[2]; _v[3] = p._v[3]; return *this; }
	inline static Vector4* setout(Vector4* v, TYPE x, TYPE y, TYPE z, TYPE w)	{ v->set(x, y, z, w); return v; }

	inline bool equals(const Vector4* p) const									{ return (_v[0] == p->_v[0]) && (_v[1] == p->_v[1]) && (_v[2] == p->_v[2]) && (_v[3] == p->_v[3]); }
	inline bool operator == (const Vector4& p) const							{ return (_v[0] == p._v[0]) && (_v[1] == p._v[1]) && (_v[2] == p._v[2]) && (_v[3] == p._v[3]); }
	inline bool operator != (const Vector4& p) const							{ return (_v[0] != p._v[0]) || (_v[1] != p._v[1]) || (_v[2] != p._v[2]) || (_v[3] != p._v[3]); }

	// Negation operator. Returns the negative of the Vector4
	inline const Vector4 nega() const											{ return Vector4(- _v[0], - _v[1], - _v[2], - _v[3]); }
	inline const Vector4 operator - () const									{ return Vector4(- _v[0], - _v[1], - _v[2], - _v[3]); }

	// Binary vector add
	inline const Vector4 operator + (const Vector4& p) const					{ return Vector4(_v[0] + p._v[0], _v[1] + p._v[1], _v[2] + p._v[2], _v[3] + p._v[3]); }

	// Unary vector add. Slightly more efficient because no temporary intermediate object
	inline Vector4* add(const Vector4* p)										{ _v[0] += p->_v[0]; _v[1] += p->_v[1]; _v[2] += p->_v[2]; _v[3] += p->_v[3]; return this; }
	inline Vector4& operator += (const Vector4& p)								{ _v[0] += p._v[0]; _v[1] += p._v[1]; _v[2] += p._v[2]; _v[3] += p._v[3]; return *this; }

	// Binary vector subtract
	inline const Vector4 operator - (const Vector4& p) const					{ return Vector4(_v[0] - p._v[0], _v[1] - p._v[1], _v[2] - p._v[2], _v[3] - p._v[3]); }

	// Unary vector subtract
	inline Vector4* sub(const Vector4* p)										{ _v[0] -= p->_v[0]; _v[1] -= p->_v[1]; _v[2] -= p->_v[2]; _v[3] -= p->_v[3]; return this; }
	inline Vector4& operator -= (const Vector4& p)								{ _v[0] -= p._v[0]; _v[1] -= p._v[1]; _v[2] -= p._v[2]; _v[3] -= p._v[3]; return *this; }

	// Multiply by scalar
	inline const Vector4 operator * (TYPE scl) const							{ return Vector4(_v[0] * scl, _v[1] * scl, _v[2] * scl, _v[3] * scl); }

	// Unary multiply by scalar
	inline Vector4* mul(TYPE scl)												{ _v[0] *= scl; _v[1] *= scl; _v[2] *= scl; _v[3] *= scl; return this; }
	inline Vector4& operator *= (TYPE scl)										{ _v[0] *= scl; _v[1] *= scl; _v[2] *= scl; _v[3] *= scl; return *this; }

	// Divide by scalar
	inline const Vector4 operator / (TYPE scl) const							{ return Vector4(_v[0] / scl, _v[1] / scl, _v[2] / scl, _v[3] / scl); }

	// Unary divide by scalar
	inline Vector4* div(TYPE scl)												{ _v[0] /= scl; _v[1] /= scl; _v[2] /= scl; _v[3] /= scl; return this; }
	inline Vector4& operator /= (TYPE scl)										{ _v[0] /= scl; _v[1] /= scl; _v[2] /= scl; _v[3] /= scl; return *this; }

	// Dot product
	inline TYPE dot(const Vector4* p) const										{ return _v[0] * p->_v[0] + _v[1] * p->_v[1] + _v[2] * p->_v[2] + _v[3] * p->_v[3]; }
	inline TYPE operator * (const Vector4& p) const								{ return _v[0] * p._v[0] + _v[1] * p._v[1] + _v[2] * p._v[2] + _v[3] * p._v[3]; }

	// Length of the vector = sqrt( vec . vec )
	inline TYPE length() const													{ return ::sqrtf( _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] + _v[3] * _v[3] ); }

	// Length squared of the vector = vec . vec
	inline TYPE lengthSq() const													{ return _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] + _v[3] * _v[3]; }

	// Normalize the vector so that it has length unity. Returns the previous length of the vector
	inline TYPE normalize()
	{
		TYPE norm = Vector4::length();
		if(norm > 0.0)
		{
			TYPE inv = 1.0f / norm;
			_v[0] *= inv;
			_v[1] *= inv;
			_v[2] *= inv;
			_v[3] *= inv;
		}
		return norm;
	}
};

typedef Vector2<f32> Vector2F;
typedef Vector3<f32> Vector3F;
typedef Vector4<f32> Vector4F;
typedef Vector2<s32> Vector2S32;
typedef Vector3<s32> Vector3S32;
typedef Vector4<s32> Vector4S32;
typedef Vector2<s16> Vector2S16;
typedef Vector3<s16> Vector3S16;
typedef Vector4<s16> Vector4S16;
typedef Vector2<s8> Vector2S8;
typedef Vector3<s8> Vector3S8;
typedef Vector4<s8> Vector4S8;
typedef Vector2<u32> Vector2U32;
typedef Vector3<u32> Vector3U32;
typedef Vector4<u32> Vector4U32;
typedef Vector2<u16> Vector2U16;
typedef Vector3<u16> Vector3U16;
typedef Vector4<u16> Vector4U16;
typedef Vector2<u8> Vector2U8;
typedef Vector3<u8> Vector3U8;
typedef Vector4<u8> Vector4U8;

////////////////////////////////////////////////////////////////////////////
	
TFW_END_NS

#endif	// _TFW_VECTOR_H_

