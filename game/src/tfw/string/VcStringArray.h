/***********************************************************************//**
 *	VcStringArray.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/07/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_VC_STRING_ARRAY_H_
#define _TFW_STRING_VC_STRING_ARRAY_H_

#if 1
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../collection/EasyArray.h"

// Friends
#include "VcString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	文字列配列 クラス
 *
**//*---------------------------------------------------------------------*/
class VcStringArray : public EasyArray<VcString>
{
public:
	VcStringArray() : EasyArray() {}
	VcStringArray(VcString** arrEntr, int num) : EasyArray(arrEntr, num) {}
	explicit VcStringArray(const VcString* elm1) : EasyArray(elm1) {}
	VcStringArray(const VcString* elm1, const VcString* elm2) : EasyArray(elm1, elm2) {}
	VcStringArray(const VcString* elm1, const VcString* elm2, const VcString* elm3) : EasyArray(elm1, elm2, elm3) {};
	VcStringArray(const VcString* elm1, const VcString* elm2, const VcString* elm3, const VcString* elm4) : EasyArray(elm1, elm2, elm3, elm4) {}
	VcStringArray(const VcString* elm1, const VcString* elm2, const VcString* elm3, const VcString* elm4, const VcString* elm5) : EasyArray(elm1, elm2, elm3, elm4, elm5) {}
	VcStringArray(const EasyArray& src);
};

TFW_END_NS

#else
////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{

class VcString;

/*---------------------------------------------------------------------*//**
 *	文字列配列 クラス
 *
**//*---------------------------------------------------------------------*/
class VcStringArray
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字列取得
	inline VcString* getString(int index) const { return _strarr[index]; }
	// 文字列数を得る
	inline int getCount() const { return _num; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	VcStringArray();
	VcStringArray(VcString** strarrEntr, int num);
	VcStringArray(const VcStringArray& src);
	explicit VcStringArray(const VcString* str1);
	VcStringArray(const VcString* str1, const VcString* str2);
	VcStringArray(const VcString* str1, const VcString* str2, const VcString* str3);
	VcStringArray(const VcString* str1, const VcString* str2, const VcString* str3, const VcString* str4);
	VcStringArray(const VcString* str1, const VcString* str2, const VcString* str3, const VcString* str4, const VcString* str5);
	~VcStringArray();
	VcStringArray& operator=(const VcStringArray& src);
	void copy(const VcStringArray& src);
	void clear();
	void set(const VcString* str1, const VcString* str2, const VcString* str3, const VcString* str4, const VcString* str5, const VcString* str6, const VcString* str7, const VcString* str8, const VcString* str9, const VcString* str10);
	void set(VcString** strarr, int num);

	//======================================================================
	// メンバ変数
private:
	VcString**	_strarr;
	int			_num;
};

}	// namespace tfw

////////////////////////////////////////////////////////////////////////////
#endif

#endif	// _TFW_STRING_VC_STRING_ARRAY_H_
