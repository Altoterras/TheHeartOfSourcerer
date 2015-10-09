/***********************************************************************//**
 *  TypeUtils.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/03/23.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_TYPE_UTILS_H_
#define _TFW_LIB_TYPE_UTILS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

/*---------------------------------------------------------------------*//**
 *	文字列ユーティリティ
 *	
**//*---------------------------------------------------------------------*/
class TypeUtils
{
	//======================================================================
	// 共用体
public:
	/*-----------------------------------------------------------------*//**
	 *	64bit 共用体
	 *
	**//*-----------------------------------------------------------------*/
	union Union64
	{
		struct
		{
			u32	_a1;
			u32	_a2;
		} _f32_2;
		struct
		{
			u32	_a1;
			u32	_a2;
		} _u32_2;
		struct
		{
			u16	_a1;
			u16	_a2;
			u16	_a3;
			u16	_a4;
		} _u16_4;
		struct
		{
			u8	_a1;
			u8	_a2;
			u8	_a3;
			u8	_a4;
			u8	_a5;
			u8	_a6;
			u8	_a7;
			u8	_a8;
		} _u8_8;
		f64 _f;
	};

	/*-----------------------------------------------------------------*//**
	 *	32bit 共用体
	 *
	**//*-----------------------------------------------------------------*/
	union Union32
	{
		struct
		{
			u32	_a1;
		} _u32_1;
		struct
		{
			u16	_a1;
			u16	_a2;
		} _u16_2;
		struct
		{
			u8	_a1;
			u8	_a2;
			u8	_a3;
			u8	_a4;
		} _u8_4;
		f32 _f;
	};

	//======================================================================
	// 定数
public:
	static const Union32 s_f32PosInf;
	static const Union32 s_f32NegInf;
	static const Union32 s_f32Nan;
	static const Union64 s_f64PosInf;
	static const Union64 s_f64NegInf;
	static const Union64 s_f64Nan;

	//======================================================================
	// メソッド
public:
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ

	// 単精度浮動小数点の正の最大値を得る
	static inline const f32 getF32PositiveInfinity() { return s_f32PosInf._f; }
	// 単精度浮動小数点の負の最大値を得る
	static inline const f32 getF32NegativeInfinity() { return s_f32NegInf._f; }
	// 単精度浮動小数点の非数（NaN）を得る
	static inline const f32 getF32NaN() { return s_f32Nan._f; }
	// 倍精度浮動小数点の正の最大値を得る
	static inline const f64 getF64PositiveInfinity() { return s_f64PosInf._f; }
	// 倍精度浮動小数点の負の最大値を得る
	static inline const f64 getF64NegativeInfinity() { return s_f64NegInf._f; }
	// 倍精度浮動小数点の非数（NaN）を得る
	static inline const f64 getF64NaN() { return s_f64Nan._f; }

	// 空文字列を得る
	static inline const VcString* getEmptyString() { return _strEmpty; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	static bool init();
	static void cleanup();

	//======================================================================
	// 変数
private:
	static VcString* _strEmpty;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_LIB_TYPE_UTILS_H_
