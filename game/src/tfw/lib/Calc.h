/***********************************************************************//**
 *	Calc.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/10/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_CALC_H_
#define _TFW_LIB_CALC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	小計算関数集
 *	
**//*---------------------------------------------------------------------*/
class Calc
{
public:
	// 簡易 sin
	static f32 sin(f32 radian);

	// 値を強制的に範囲に収める
	inline static s32 clamp(s32 val, s32 valMin, s32 valMax)
	{
		if(val < valMin)	{	return valMin;	}
		if(val > valMax)	{	return valMax;	}
		return val;
	}

	// エンディアン変換
	inline static void convEndian4(u8* val1, const u8* val2)
	{
		ASSERT(val1 != val2);
		val1[0] = val2[3];
		val1[1] = val2[2];
		val1[2] = val2[1];
		val1[3] = val2[0];
	}
	inline static void convEndian8(u8* val1, const u8* val2)
	{
		ASSERT(val1 != val2);
		val1[0] = val2[7];
		val1[1] = val2[6];
		val1[2] = val2[5];
		val1[3] = val2[4];
		val1[4] = val2[3];
		val1[5] = val2[2];
		val1[6] = val2[1];
		val1[7] = val2[0];
	}

	// log2
	inline static u32 log2Ceiling32(u32 val)
	{
		u32 l = 0;
		u32 wk = val;
		if(wk & (wk - 1))	{	l += 1;					}
		if(wk >> 16)		{	l += 16;	wk >>= 16;	}
		if(wk >> 8)			{	l += 8;		wk >>= 8;	}
		if(wk >> 4)			{	l += 4;		wk >>= 4;	}
		if(wk >> 2)			{	l += 2;		wk >>= 2;	}
		if(wk >> 1)			{	l += 1;					}
		return l;
	}

	// 角度を -PI ～ PI の間におさめる
	inline static f32 adjustAngle(f32 angle)
	{
		while(angle > TFW_PI)	{	angle -= 2.0f * TFW_PI;	}
		while(angle < -TFW_PI)	{	angle += 2.0f * TFW_PI;	}
		return angle;
	}

	// 角度計算
	inline static bool calcAngle(f32* angle, f32 x, f32 y)
	{
		if(TFW_IS_NEAR(x, 0.0f, 0.0001f))
		{
			if(y > 0.01f)		{	*angle = TFW_HPI;		}
			else if(y < -0.01f)	{	*angle = - TFW_HPI;		}
			else				{	return false;			}	// 角度不明
		}
		else if(x >= 0.0f)	{	*angle = ::atanf( y / x );			}
		else				{	*angle = TFW_PI + ::atanf( y / x );	}
		*angle = adjustAngle(*angle);
		return true;
	}

	// 角度を近づける
	inline static f32 closeAngle(f32 angle1, f32 angle2, f32 rate1)
	{
#if 0
		f32 d1 = TFW_FABS( angle1 - angle2 );
		f32 d2 = TFW_FABS( (angle1 + TFW_DPI) - angle2 );
		f32 d3 = TFW_FABS( (angle1 - TFW_DPI) - angle2 );
		if(d1 < d2)
		{
			if(d1 < d3)
			{
				return adjustAngle((angle1 * rate1) + (angle2 * (1.0f - rate1)));
			}
			else
			{
				return adjustAngle(((angle1 - TFW_DPI) * rate1) + (angle2 * (1.0f - rate1)));
			}
		}
		else
		{
			if(d2 < d3)
			{
				return adjustAngle(((angle1 + TFW_DPI) * rate1) + (angle2 * (1.0f - rate1)));
			}
			else
			{
				return adjustAngle(((angle1 - TFW_DPI) * rate1) + (angle2 * (1.0f - rate1)));
			}
		}
#else
		f32 d1 = angle1 - angle2;
		f32 d2 = (angle1 + TFW_DPI) - angle2;
		f32 d3 = (angle1 - TFW_DPI) - angle2;
		f32 d1a = TFW_FABS( d1 );
		f32 d2a = TFW_FABS( d2 );
		f32 d3a = TFW_FABS( d3 );
		if(d1a < d2a)
		{
			if(d1a < d3a)
			{
				return adjustAngle(angle2 + (d1 * rate1));
			}
			else
			{
				return adjustAngle(angle2 + (d3 * rate1));
			}
		}
		else
		{
			if(d2a < d3a)
			{
				return adjustAngle(angle2 + (d2 * rate1));
			}
			else
			{
				return adjustAngle(angle2 + (d3 * rate1));
			}
		}
#endif
	}

	// 値をのこぎり波に変換する（周期は２の累乗でなければならない）
	inline static s32 convSawValueBinary(s32 val, s32 valCycle2i)
	{
		s32 vc = val & (valCycle2i - 1);
		if(vc <= (valCycle2i >> 1))	// 前半．「>> 1」は「÷ 2」と同義
		{
			return vc << 1;		// 「<< 1」は「× 2」と同義
		}
		else						// 後半
		{
			return (valCycle2i - vc) << 1;
		}
	}
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_LIB_CALC_H_

