/***********************************************************************//**
 *	Elene.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_ELENE_H_
#define _SRCR_STATUS_ELENE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	元素エネルギー量
 *		※シリアライズは SerializerUtils で行う
 *
**//*---------------------------------------------------------------------*/
class Elene
{
	//======================================================================
	// 定数
public:
	// 元素エネルギー種別
	enum ElKind
	{
		EL_SO,		// 地（Soil）
		EL_WA,		// 水（Water）
		EL_FI,		// 火（Fire）
		EL_WI,		// 風（Wind）
		EL_AE,		// 空（Aethel）
		NUM_EL
	};

	// 元素エネルギーの有効桁
	static const s32	ELENE_VALID_FIGURE	=        100;				// 表示上は 1
	// 元素エネルギーの基準値
	static const s32	ELEN_DEFAULT		=      10000;				// 表示上は 100
	// 元素エネルギー最大値
	static const s32	ELEN_MAX			=    1000000;				// 表示上は 10000
	// <参考>			INT_MAX				: 2147483647;

	// 全体シフトの基準値
	static const s32	SHIFT_STDVAL		= ELEN_MAX;					// この値で１種別分シフトする
	// 全体シフトの最大値
	static const s32	SHIFT_MAX			= SHIFT_STDVAL * NUM_EL;	// この値で地元素が空元素を超える

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 各元素エネルギーを得る
	inline s32 getSoil() const					{	return _ee[EL_SO];		}
	inline s32 getWater() const					{	return _ee[EL_WA];		}
	inline s32 getFire() const					{	return _ee[EL_FI];		}
	inline s32 getWind() const					{	return _ee[EL_WI];		}
	inline s32 getAethel() const				{	return _ee[EL_AE];		}
	// 元素のエネルギーを得る
	inline s32 get(ElKind el) const				{	return _ee[el];		}
	inline s32 get(s32 el) const				{	ASSERT((EL_SO <= el) && (el < NUM_EL)); 	return _ee[el];		}

	// 初期値（全てゼロ）かどうかを得る
	bool isInitialValue() const;
	// 物質スコアを得る
	s32 getMaterialScore() const;
	// 物質率を得る
	f32 getMaterialRatio() const;
	// エネルギースコアを得る
	s32 getEngyScore() const;
	// エネルギー率を得る
	f32 getEngyRatio() const;
	// 重さ [g]（グラム）を得る
	s32 getWeight() const;
	// 体積 [l]（リットル）を得る
	s32 getVolume() const;
	// 有効半径を得る
	f32 getEffectiveRange() const;

	// 各元素エネルギーを設定する
	inline s32 setSoil(s32 so)					{	return set(EL_SO, so);	}
	inline s32 setWater(s32 wa)					{	return set(EL_WA, wa);	}
	inline s32 setFire(s32 fi)					{	return set(EL_FI, fi);	}
	inline s32 setWind(s32 wi)					{	return set(EL_WI, wi);	}
	inline s32 setAethel(s32 ae)				{	return set(EL_AE, ae);	}
	// 元素のエネルギーを設定する
	inline s32 set(ElKind el, s32 en)
	{
		if(en >= ELEN_MAX)	{ _ee[el] = ELEN_MAX; }
		else if(en <= 0)	{ _ee[el] = 0; }
		else				{ _ee[el] = en; }
		return _ee[el];
	}
	inline s32 set(s32 el, s32 en)
	{	
		ASSERT((EL_SO <= el) && (el < NUM_EL));
		return set((ElKind)el, en);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool equals(const Elene* s) const;

	// エネルギーの単純合計値を得る
	inline s32 sum() const
	{
		s32 res = 0;
		for(int i = 0; i < NUM_EL; i++) { res += _ee[i]; }
		return res;
	}
	// 空以外のエネルギーの単純合計値を得る
	inline s32 sumExceptAethel() const
	{
		s32 res = 0;
		for(int i = 0; i < NUM_EL; i++) { res += _ee[i]; }
		return res;
	}

	void shift(Elene* res, s32 shiftval) const;
	s32 getSoumaMaterializeLifeEnergy() const;

	// 要素ごとの加算
	inline s32 add(ElKind el, s32 en)	{	return set(el, _ee[el] + en);			}
	inline s32 add(s32 el, s32 en)		{	return set(el, _ee[el] + en);			}
	inline s32 addSoil(s32 so)			{	return set(EL_SO, _ee[EL_SO] + so);		}
	inline s32 addWater(s32 wa)			{	return set(EL_WA, _ee[EL_WA] + wa);		}
	inline s32 addFire(s32 fi)			{	return set(EL_FI, _ee[EL_FI] + fi);		}
	inline s32 addWind(s32 wi)			{	return set(EL_WI, _ee[EL_WI] + wi);		}
	inline s32 addAethel(s32 ae)		{	return set(EL_AE, _ee[EL_AE] + ae);		}
	// 要素ごとの減算
	inline s32 sub(ElKind el, s32 en)	{	return set(el, _ee[el] - en);			}
	inline s32 sub(s32 el, s32 en)		{	return set(el, _ee[el] - en);			}
	inline s32 subSoil(s32 so)			{	return set(EL_SO, _ee[EL_SO] - so);		}
	inline s32 subWater(s32 wa)			{	return set(EL_WA, _ee[EL_WA] - wa);		}
	inline s32 subFire(s32 fi)			{	return set(EL_FI, _ee[EL_FI] - fi);		}
	inline s32 subWind(s32 wi)			{	return set(EL_WI, _ee[EL_WI] - wi);		}
	inline s32 subAethel(s32 ae)		{	return set(EL_AE, _ee[EL_AE] - ae);		}
	// 要素ごとのスケーリング
	void scale(s32 ratio);
	void scale(f32 ratio);

	#if defined(_DEBUG)
		// デバッグ出力
		void _debug_traceValue();
	#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	inline Elene()											{	clear();					}
	inline Elene(s32 so, s32 wa, s32 fi, s32 wi, s32 ae)	{	set(so, wa, fi, wi, ae);	}
	inline Elene(const Elene& src)							{	copy(&src);					}

	inline void copy(const Elene* src)						{	for(int i = 0; i < NUM_EL; i++) { _ee[i] = src->_ee[i]; }								}
	inline Elene& operator =(const Elene& src)				{	copy(&src);	return *this;																}
	inline void clear()										{	for(int i = 0; i < NUM_EL; i++) { _ee[i] = 0; }											}
	inline void set(s32 so, s32 wa, s32 fi, s32 wi, s32 ae)	{	_ee[EL_SO] = so; _ee[EL_WA] = wa; _ee[EL_FI] = fi; _ee[EL_WI] = wi; _ee[EL_AE] = ae;	}
	inline void add(const Elene* src)						{	for(int i = 0; i < NUM_EL; i++) { _ee[i] += src->_ee[i]; }								}
	inline void sub(const Elene* src)						{	for(int i = 0; i < NUM_EL; i++) { _ee[i] -= src->_ee[i]; }								}

	//======================================================================
	// 変数
private:
	// [  0]
	s32 _ee[NUM_EL];		// 各元素のエネルギー量	【2010/01/29 r-kishi】現在、元素エネルギーがマイナスになるかは定かではないので s32 にしている…
	// [ 20]
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_ELENE_H_
