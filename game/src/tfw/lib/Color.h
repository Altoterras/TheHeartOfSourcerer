/***********************************************************************//**
 *	Color.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/11/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COLOR_H_
#define _TFW_COLOR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "Vector.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	色
 *
**//*---------------------------------------------------------------------*/
template<class TYPE> class Color : public Vector4<TYPE>
{
public:
	Color()																	{ Vector4<TYPE>::set((TYPE)0, (TYPE)0, (TYPE)0, (TYPE)0); }
	Color(TYPE r, TYPE g, TYPE b, TYPE a)									{ Vector4<TYPE>::set(r, g, b, a); }
	explicit Color(const Color* src)										{ Vector4<TYPE>::copy((Vector4<TYPE>*)src); }
	Color(const Color& src)													{ Vector4<TYPE>::copy((Vector4<TYPE>*)&src); }
	inline static Color* setout(Color* v, TYPE r, TYPE g, TYPE b, TYPE a)	{ v->set(r, g, b, a); return v; }
};

typedef Color<f32>	ColorF;
typedef Color<u8>	ColorU8;
typedef Color<s32>	ColorS32;

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_COLOR_H_

