/***********************************************************************//**
 *  BaseTypes.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/13.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_BASETYPES_H_
#define _TFW_BASETYPES_H_

#include <memory.h>

////////////////////////////////////////////////////////////////////////////
// マクロ

#if (defined(_M_AMD64) && _M_AMD64) || (defined(__LP64__) && __LP64__)
	#define _64BIT		(1)
#endif

////////////////////////////////////////////////////////////////////////////
// 型

#if !defined(s64)
	#if defined(_WINDOWS)
		typedef signed __int64 s64;
	#else
		typedef signed long long s64;
	#endif
#endif
#if !defined(u64)
	#if defined(_WINDOWS)
		typedef unsigned __int64 u64;
	#else
		typedef unsigned long long u64;
	#endif
#endif
#if !defined(s32)
	typedef signed int s32;
#endif
#if !defined(u32)
	typedef unsigned int u32;
#endif
#if !defined(s16)
	typedef signed short s16;
#endif
#if !defined(u16)
	typedef unsigned short u16;
#endif
#if !defined(s8)
	typedef signed char s8;
#endif
#if !defined(u8)
	typedef unsigned char u8;
#endif

#if !defined(f64)
typedef double f64;
#endif
#if !defined(f32)
typedef float f32;
#endif

#if _64BIT
typedef u64 addr_size;
#else
typedef u32 addr_size;
#endif

////////////////////////////////////////////////////////////////////////////
// 定数

#define S8_MIN					(-128)
#define S8_MAX					(127)
#define U8_MIN					(0x00)
#define U8_MAX					(0xff)
#define S16_MIN					(-32768)
#define S16_MAX					(32767)
#define U16_MIN					(0x0000)
#define U16_MAX					(0xffff)
#define S32_MIN					(-2147483647 - 1)
#define S32_MAX					(2147483647)
#define U32_MIN					(0x00000000)
#define U32_MAX					(0xffffffff)
#define S64_MIN					(-9223372036854775807 - 1)
#define S64_MAX					(9223372036854775807)
#define U64_MIN					(0x0000000000000000)
#define U64_MAX					(0xffffffffffffffff)
#define	F32_PMAX				(3.402823466e+38F)
#define	F32_PMIN				(1.175494351e-38F)
#define	F64_PMAX				(1.7976931348623158e+308)
#define	F64_PMIN				(2.2250738585072014e-308)

////////////////////////////////////////////////////////////////////////////
// マクロ

#define TFW_U8_TO_U16(a1_, a2_)		((u16)(((a1_) << 8) & 0xff00) | (u16)(((a2_) << 0) & 0x00ff))

#define TFW_U16_TO_U8_HI(a_)		((u8)(((a_) >> 8) & 0xff))
#define TFW_U16_TO_U8_LO(a_)		((u8)((a_) & 0xff))

#define TFW_U32_TO_U8_B1(a_)		((u8)(((a_) >> 24) & 0xff))
#define TFW_U32_TO_U8_B2(a_)		((u8)(((a_) >> 16) & 0xff))
#define TFW_U32_TO_U8_B3(a_)		((u8)(((a_) >> 8) & 0xff))
#define TFW_U32_TO_U8_B4(a_)		((u8)(((a_) >> 0) & 0xff))

#define TFW_U32_CH_ENDIAN(a_)		( (((a_) << 24) & 0xff000000) | (((a_) << 8) & 0x00ff0000) | (((a_) >> 8) & 0x0000ff00) | (((a_) >> 24) & 0x000000ff) )
#define TFW_S32_CH_ENDIAN(a_)		TFW_U32_CH_ENDIAN(a_)
#define TFW_U16_CH_ENDIAN(a_)		( (((a_) << 8) & 0xff00) | (((a_) >> 8) & 0x00ff) )
#define TFW_S16_CH_ENDIAN(a_)		TFW_U16_CH_ENDIAN(a_)

#define TFW_F32_SIGN				0x80000000
#define TFW_F32_MANT				0x007fffff
#define TFW_F32_EXP					0x7f800000
#define TFW_F32_IS_FINITE(a_)		( (a_) != TFW_F32_EXP )
#define TFW_F32_IS_INF(a_)			( ((a_) & (TFW_F32_EXP | TFW_F32_MANT)) == TFW_F32_EXP )
#define TFW_F32_IS_NAN(a_)			( (((a_) & TFW_F32_EXP) == TFW_F32_EXP) && (((a_) & TFW_F32_MANT) != 0) )
#define TFW_F32_IS_NZERO(a_)		( (a_) == TFW_F32_SIGN )
#define TFW_F32_IS_INT(a_)			( TFW_F32_IS_FINITE(a_) && (!TFW_F32_IS_NZERO(a_)) && ((a_) == (s32)(a_)))
#define TFW_F32_IS_EQUAL(a_, b_)	( (TFW_F32_IS_NAN(a_) || TFW_F32_IS_NAN(b_)) ? false : ((a_) == (b_)) )

#define TFW_F64_HI_SIGN				0x80000000
#define TFW_F64_HI_MANT				0x000fffff
#define TFW_F64_HI_EXP				0x7ff00000
#define TFW_F64_TO_RAWU32_HI(a_)	(  *( ((u32*)&(a_)) + 1 )  )
#define TFW_F64_TO_RAWU32_LO(a_)	(  *( ((u32*)&(a_)) + 0 )  )
#define TFW_F64_IS_FINITE(a_)		( (TFW_F64_TO_RAWU32_HI(a_)) != TFW_F64_HI_EXP )
#define TFW_F64_IS_INF(a_)			( ((TFW_F64_TO_RAWU32_HI(a_) & (TFW_F64_HI_EXP | TFW_F64_HI_MANT)) == TFW_F64_HI_EXP) && (TFW_F64_TO_RAWU32_LO(a_) == 0) )
#define TFW_F64_IS_NAN(a_)			( ((TFW_F64_TO_RAWU32_HI(a_) & TFW_F64_HI_EXP) == TFW_F64_HI_EXP) && (((TFW_F64_TO_RAWU32_HI(a_) & TFW_F64_HI_MANT) != 0) || (TFW_F64_TO_RAWU32_LO(a_) != 0)) )
#define TFW_F64_IS_NZERO(a_)		( (TFW_F64_TO_RAWU32_HI(a_) == TFW_F64_HI_SIGN) && (TFW_F64_TO_RAWU32_LO(a_) == 0) )
#define TFW_F64_IS_INT(a_)			( TFW_F64_IS_FINITE(a_) && (!TFW_F64_IS_NZERO(a_)) && ((a_) == (s32)(a_)))
#define TFW_F64_IS_EQUAL(a_, b_)	( (TFW_F64_IS_NAN(a_) || TFW_F64_IS_NAN(b_)) ? false : ((a_) == (b_)) )

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	ボクシング用
 *
**//*---------------------------------------------------------------------*/
template<class TYPE> class TfwBoxingNumber
{
public:
	TYPE _v;

	TfwBoxingNumber()							{	_v = 0;			}
	TfwBoxingNumber(TYPE valueInit)				{	_v = valueInit;	}
	TfwBoxingNumber(const TfwBoxingNumber& s)	{	_v = s._v;		}

	TYPE v() const								{	return _v;		}

	static inline TYPE* toBasicArray(TfwBoxingNumber* parr, int len)
	{
		TYPE* parrNew = new TYPE[len];
		memcpy(parrNew, parr, sizeof(TYPE) * len);
		delete[] parr;
		return parrNew;
	}
};

typedef TfwBoxingNumber<f64> F64;
typedef TfwBoxingNumber<f32> F32;
typedef TfwBoxingNumber<s32> S32;
typedef TfwBoxingNumber<u32> U32;
typedef TfwBoxingNumber<s16> S16;
typedef TfwBoxingNumber<u16> U16;
typedef TfwBoxingNumber<s8> S8;
typedef TfwBoxingNumber<u8> U8;

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_BASETYPES_H_
