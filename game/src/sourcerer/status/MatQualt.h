/***********************************************************************//**
 *	MatQualt.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_MAT_QUALT_H_
#define _SRCR_STATUS_MAT_QUALT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class Elene;

/*---------------------------------------------------------------------*//**
 *	物質の質（Material Qualities）
 *
**//*---------------------------------------------------------------------*/
class MatQualt
{
	//======================================================================
	// 定数
public:
	enum Quality
	{
		//								精妙 0 <-> 10000 重鈍
		WT,		// 重さ（weight）		｛軽い - 重い｝
		HT,		// 熱さ（heat）			｛熱い - 冷たい｝
		UC,		// 油性（unctuousness）	｛乾燥 - 油性｝
		SP,		// 鋭さ（sharpness）	｛鋭い - 鈍い｝
		HD,		// 硬さ（hardness）		｛柔らかい - 硬い｝
		MN,		// 精妙さ（minuteness）	｛精妙 - 荒い｝
		LQ,		// 液性（liquidness）	｛液性 - 固形｝
		RG,		// 粗さ（roughness）	｛滑らか - 粗い｝
		SC, 	// 粘性（stickiness）	｛澄んだ - 粘性｝
		MV,		// 動き（move）			｛動き - 安定｝
		NUM_QUALITY
	};

	static const	u16	MAX_VALUE	= 10000;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在の値を得る
	inline u16		getValue(s32 q) const		{	ASSERT((0 <= q) && (q < NUM_QUALITY));	return _val[q];	}
	// 値を設定する
	inline void		setValue(s32 q, u16 val)	{	ASSERT((0 <= q) && (q < NUM_QUALITY));	_val[q] = val;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// 全ての値がゼロであるか判定する
	inline bool		isZero() const
	{
		for(int i = 0; i < NUM_QUALITY; i++)	{	if(_val[i] != 0) { return false; }	}
		return true;
	}
	// 絶対値の総合計を計算する
	inline u32		getTotal() const
	{
		u32 val = 0;
		for(int i = 0; i < NUM_QUALITY; i++)	{	val += _val[i];						}
		return val;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MatQualt()									{	clear();							}
	MatQualt(const MatQualt& src)				{	copy(&src);							}
	void copy(const MatQualt* src)				{	for(int i = 0; i < NUM_QUALITY; i++)	{	_val[i] = src->_val[i];	}	}
	MatQualt& operator =(const MatQualt& src)	{	copy(&src);		return *this;		}
	void clear()								{	for(int i = 0; i < NUM_QUALITY; i++)	{	_val[i] = 0;	}	}
	void setup(const Elene* elene);

	//======================================================================
	// 変数
private:
	u16			_val[NUM_QUALITY];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_MAT_QUALT_H_
