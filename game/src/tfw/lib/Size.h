/***********************************************************************//**
 *	Size.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/05.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SIZE_H_
#define _TFW_SIZE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "Vector.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	サイズ
 *	
**//*---------------------------------------------------------------------*/
template<class TYPE> class Size : public Vector2<TYPE>
{
#if 1
public:
	Size()																{ Vector2<TYPE>::set((TYPE)0, (TYPE)0); }
	Size(TYPE w, TYPE h)												{ Vector2<TYPE>::set(w, h); }
	explicit Size(const Size* src)										{ Vector2<TYPE>::copy((Vector2<TYPE>*)src); }
	Size(const Size& src)												{ Vector2<TYPE>::copy((Vector2<TYPE>*)&src); }
	inline static Size* setout(Size* v, TYPE w, TYPE h)					{ v->set(w, h); return v; }

	inline TYPE& width()												{ return this->_v[0]; }
	inline TYPE& height()												{ return this->_v[1]; }
	inline TYPE& w()													{ return this->_v[0]; }
	inline TYPE& h()													{ return this->_v[1]; }
	inline TYPE width() const											{ return this->_v[0]; }
	inline TYPE height() const											{ return this->_v[1]; }
	inline TYPE w() const												{ return this->_v[0]; }
	inline TYPE h() const												{ return this->_v[1]; }
#else
public:
	enum { NUM_COMPONENTS = 2 };

	TYPE		_v[NUM_COMPONENTS];

	Size()																{ set(0, 0); }
	Size(TYPE w, TYPE h)												{ set(w, h); }

	inline TYPE& width()												{ return _v[0]; }
	inline TYPE& height()												{ return _v[1]; }
	inline TYPE& w()													{ return _v[0]; }
	inline TYPE& h()													{ return _v[1]; }
	inline TYPE width() const											{ return _v[0]; }
	inline TYPE height() const											{ return _v[1]; }
	inline TYPE w() const												{ return _v[0]; }
	inline TYPE h() const												{ return _v[1]; }

	inline TYPE& operator [] (int i)									{ return _v[i]; }
	inline TYPE operator [] (int i) const								{ return _v[i]; }

	inline TYPE* ptr()													{ return _v; }
	inline const TYPE* ptr() const										{ return _v; }

	inline Size* set(TYPE w, TYPE h)									{ _v[0] = w; _v[1] = h; return this; }
	inline Size* set(const Size* rv)									{ _v[0] = rv->_v[0]; _v[1] = rv->_v[1]; return this; }
	inline Size& operator = (const Size& rv)							{ _v[0] = rv._v[0]; _v[1] = rv._v[1]; return *this; }

	inline bool equals(const Size* rv) const							{ return (_v[0] == rv->_v[0]) && (_v[1] == rv->_v[1]); }
	inline bool operator == (const Size& rv) const						{ return (_v[0] == rv._v[0]) && (_v[1] == rv._v[1]); }
	inline bool operator != (const Size& rv) const						{ return (_v[0] != rv._v[0]) || (_v[1] != rv._v[1]); }
	
	inline bool isEmpty()												{ return (_v[0] <= 0) && (_v[1] <= 0); }

	inline Size* add(const Size* rv)									{ _v[0] += rv->_v[0]; _v[1] += rv->_v[1]; return this; }
#endif
};

typedef Size<f32> SizeF32;
typedef Size<s32> SizeS32;

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_SIZE_H_

