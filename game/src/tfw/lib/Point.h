/***********************************************************************//**
 *	Point.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/05.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_POINT_H_
#define _TFW_POINT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "Vector.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ポイント
 *
**//*---------------------------------------------------------------------*/
template<class TYPE> class Point : public Vector2<TYPE>
{
public:
	Point() { Vector2<TYPE>::set((TYPE)0, (TYPE)0); }
	Point(TYPE x, TYPE y) { Vector2<TYPE>::set(x, y); }
	explicit Point(const Point* src) { Vector2<TYPE>::copy((Vector2<TYPE>*)src); }
	Point(const Point& src) { Vector2<TYPE>::copy((Vector2<TYPE>*)&src); }
	inline static Point* setout(Point* v, TYPE x, TYPE y) { v->set(x, y); return v; }

	bool isNear(const Point* p2, TYPE xd, TYPE yd) const { return TFW_IS_NEAR(this->x(), p2->x(), xd) && TFW_IS_NEAR(this->y(), p2->y(), yd); }
};

typedef Point<s32> PointS32;
typedef Point<f32> PointF32;

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_POINT_H_

