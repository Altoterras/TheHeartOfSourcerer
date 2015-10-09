/***********************************************************************//**
 *	VwStringArray.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2012/12/06.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_VW_STRING_ARRAY_H_
#define _TFW_STRING_VW_STRING_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../collection/EasyArray.h"

// Friends
#include "VwString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ワイド文字列配列 クラス
 *
**//*---------------------------------------------------------------------*/
class VwStringArray : public EasyArray<VwString>
{
public:
	VwStringArray() : EasyArray() {}
	VwStringArray(VwString** arrEntr, int num) : EasyArray(arrEntr, num) {}
	explicit VwStringArray(const VwString* elm1) : EasyArray(elm1) {}
	VwStringArray(const VwString* elm1, const VwString* elm2) : EasyArray(elm1, elm2) {}
	VwStringArray(const VwString* elm1, const VwString* elm2, const VwString* elm3) : EasyArray(elm1, elm2, elm3) {};
	VwStringArray(const VwString* elm1, const VwString* elm2, const VwString* elm3, const VwString* elm4) : EasyArray(elm1, elm2, elm3, elm4) {}
	VwStringArray(const VwString* elm1, const VwString* elm2, const VwString* elm3, const VwString* elm4, const VwString* elm5) : EasyArray(elm1, elm2, elm3, elm4, elm5) {}
	VwStringArray(const EasyArray& src);
};

TFW_END_NS

#endif	// _TFW_STRING_VW_STRING_ARRAY_H_
