/***********************************************************************//**
 *	Rect.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/05.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_RECT_H_
#define _TFW_RECT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "Point.h"
#include "Size.h"
#include "Vector.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	矩形
 *	
**//*---------------------------------------------------------------------*/
template<class TYPE> class Rect : public Vector4<TYPE>
{
#if 1
public:
	Rect()																{ Vector4<TYPE>::set((TYPE)0, (TYPE)0, (TYPE)0, (TYPE)0); }
	Rect(TYPE left, TYPE right, TYPE width, TYPE height)				{ Vector4<TYPE>::set(left, right, width, height); }
	explicit Rect(const Rect* src)										{ Vector4<TYPE>::copy((Vector4<TYPE>*)src); }
	Rect(const Rect& src)												{ Vector4<TYPE>::copy((Vector4<TYPE>*)&src); }
	inline static Rect* setout(Rect* v, TYPE x, TYPE y, TYPE w, TYPE h)	{ v->set(x, y, w, h); return v; }

	inline TYPE& left()													{ return this->_v[0]; }
	inline TYPE& top()													{ return this->_v[1]; }
	inline TYPE& width()												{ return this->_v[2]; }
	inline TYPE& height()												{ return this->_v[3]; }
	inline TYPE& w()													{ return this->_v[2]; }
	inline TYPE& h()													{ return this->_v[3]; }
	inline TYPE left() const											{ return this->_v[0]; }
	inline TYPE top() const												{ return this->_v[1]; }
	inline TYPE width() const											{ return this->_v[2]; }
	inline TYPE height() const											{ return this->_v[3]; }
	inline TYPE w() const												{ return this->_v[2]; }
	inline TYPE h() const												{ return this->_v[3]; }
	inline TYPE right() const											{ return this->_v[0] + this->_v[2]; }
	inline TYPE bottom() const											{ return this->_v[1] + this->_v[3]; }

	inline Rect* setPoint(TYPE left, TYPE right)						{ this->_v[0] = left; this->_v[1] = right; return this; }
	inline Rect* setPoint(const Point<TYPE>* pt)						{ this->_v[0] = pt->_v[0]; this->_v[1] = pt->_v[1]; return this; }
	inline Rect* setSize(TYPE width, TYPE height)						{ this->_v[2] = width; this->_v[3] = height; return this; }
	inline Rect* setSize(const Size<TYPE>* size)						{ this->_v[2] = size->_v[0]; this->_v[3] = size->_v[1]; return this; }

	inline bool isEmpty() const											{ return (this->_v[2] <= 0) || (this->_v[3] <= 0); }

	inline Rect* add(const Rect* rv)
	{
		if(this->_v[0] > rv->_v[0])
		{
			this->_v[2] = this->_v[0] + this->_v[2] - rv->_v[0];
			this->_v[0] = rv->_v[0];
		}
		if(this->_v[1] > rv->_v[1])
		{
			this->_v[3] = this->_v[1] + this->_v[3] - rv->_v[1];
			this->_v[1] = rv->_v[1];
		}
		if((this->_v[0] + this->_v[2]) < (rv->_v[0] + rv->_v[2]))
		{
			this->_v[2] = rv->_v[0] + rv->_v[2] - this->_v[0];
		}
		if((this->_v[1] + this->_v[3]) < (rv->_v[1] + rv->_v[3]))
		{
			this->_v[3] = rv->_v[1] + rv->_v[3] - this->_v[1];
		}
		return this;
	}

	inline bool isPointIn(const Point<TYPE>* pt) const
	{
		return ( left() <= pt->x() ) && ( pt->x() < right() ) && ( top() <= pt->y() ) && ( pt->y() < bottom() );
	}

	inline bool isPointIn(const Point<TYPE>* pt, TYPE marginH, TYPE marginV) const
	{
		return ( (left() - marginH) <= pt->x() ) && ( pt->x() < (right() + marginH) ) && ( (top() - marginV) <= pt->y() ) && ( pt->y() < (bottom() + marginV) );
	}

	inline Point<TYPE> center() const
	{
		return Point<TYPE>(left() + (width() / 2), top() + (height() / 2));
	}
#else
public:
	enum { NUM_COMPONENTS = 4 };

	TYPE _v[NUM_COMPONENTS];

	Rect()																{ set(0, 0, 0, 0); }
	Rect(TYPE left, TYPE right, TYPE width, TYPE height)				{ set(left, right, width, height); }

	inline TYPE& left()													{ return _v[0]; }
	inline TYPE& top()													{ return _v[1]; }
	inline TYPE& width()												{ return _v[2]; }
	inline TYPE& height()												{ return _v[3]; }
	inline TYPE& x()													{ return _v[0]; }
	inline TYPE& y()													{ return _v[1]; }
	inline TYPE& w()													{ return _v[2]; }
	inline TYPE& h()													{ return _v[3]; }
	inline TYPE left() const											{ return _v[0]; }
	inline TYPE top() const												{ return _v[1]; }
	inline TYPE width() const											{ return _v[2]; }
	inline TYPE height() const											{ return _v[3]; }
	inline TYPE x() const												{ return _v[0]; }
	inline TYPE y() const												{ return _v[1]; }
	inline TYPE w() const												{ return _v[2]; }
	inline TYPE h() const												{ return _v[3]; }
	inline TYPE right() const											{ return _v[0] + _v[2]; }
	inline TYPE bottom() const											{ return _v[1] + _v[3]; }

	inline TYPE& operator [] (int i)									{ return _v[i]; }
	inline TYPE operator [] (int i) const								{ return _v[i]; }

	inline TYPE* ptr()													{ return _v; }
	inline const TYPE* ptr() const										{ return _v; }

	inline Rect* set(TYPE left, TYPE right, TYPE width, TYPE height)	{ _v[0] = left; _v[1] = right; _v[2] = width; _v[3] = height; return this; }
	inline Rect* set(const Rect* rv)									{ _v[0] = rv->_v[0]; _v[1] = rv->_v[1]; _v[2] = rv->_v[2]; _v[3] = rv->_v[3]; return this; }
	inline Rect& operator = (const Rect& rv)							{ _v[0] = rv._v[0]; _v[1] = rv._v[1]; _v[2] = rv._v[2]; _v[3] = rv._v[3]; return *this; }
	inline Rect* setPoint(TYPE left, TYPE right)						{ _v[0] = left; _v[1] = right; return this; }
	inline Rect* setPoint(const Point<TYPE>* pt)						{ _v[0] = pt->_v[0]; _v[1] = pt->_v[1]; return this; }
	inline Rect* setSize(TYPE width, TYPE height)						{ _v[2] = width; _v[3] = height; return this; }
	inline Rect* setSize(const Size<TYPE>* size)						{ _v[2] = size->_v[0]; _v[3] = size->_v[1]; return this; }

	inline bool equals(const Rect* rv) const							{ return (_v[0] == rv->_v[0]) && (_v[1] == rv->_v[1]) && (_v[2] == rv->_v[2]) && (_v[3] == rv->_v[3]); }
	inline bool operator == (const Rect& rv) const						{ return (_v[0] == rv._v[0]) && (_v[1] == rv._v[1]) && (_v[2] == rv._v[2]) && (_v[3] == rv._v[3]); }
	inline bool operator != (const Rect& rv) const						{ return (_v[0] != rv._v[0]) || (_v[1] != rv._v[1]) || (_v[2] != rv._v[2]) || (_v[3] != rv._v[3]); }
	
	inline bool isEmpty() const											{ return (_v[2] <= 0) && (_v[3] <= 0); }
	
	inline Rect* add(const Rect* rv)
	{
		if(_v[0] > rv->_v[0])
		{
			_v[2] = _v[0] + _v[2] - rv->_v[0];
			_v[0] = rv->_v[0];
		}
		if(_v[1] > rv->_v[1])
		{
			_v[3] = _v[1] + _v[3] - rv->_v[1];
			_v[1] = rv->_v[1];
		}
		if((_v[0] + _v[2]) < (rv->_v[0] + rv->_v[2]))
		{
			_v[2] = rv->_v[0] + rv->_v[2] - _v[0];
		}
		if((_v[1] + _v[3]) < (rv->_v[1] + rv->_v[3]))
		{
			_v[3] = rv->_v[1] + rv->_v[3] - _v[1];
		}
		return this;
	}

	inline bool isPointIn(const Point<TYPE>* pt) const
	{
		return ( left() <= pt->x() ) && ( pt->x() < right() ) && ( top() <= pt->y() ) && ( pt->y() < bottom() );
	}

	inline Point<TYPE> center() const
	{
		return Point<TYPE>(x() + (w() / 2), y() + (h() / 2));
	}
#endif
};

typedef Rect<s32> RectS32;
typedef Rect<f32> RectF32;
	
TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_RECT_H_

