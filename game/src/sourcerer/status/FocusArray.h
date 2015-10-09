/***********************************************************************//**
 *	FocusArray.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/01.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_FOCUS_ARRAY_H_
#define _SRCR_STATUS_FOCUS_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector2;
typedef Vector2<f32> Vector2F;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	フォーカス配列
 *
**//*---------------------------------------------------------------------*/
class FocusArray
{
	//======================================================================
	// 定数
private:
	static const u16 F_CALC_SCR_POS		= 0x0001;	// 画面位置を計算する

	//======================================================================
	// クラス
protected:
	/*-----------------------------------------------------------------*//**
	 *	フォーカス配列要素
	 *
	**//*-----------------------------------------------------------------*/
	class Focus
	{
	public:
		Unit* _unit;
		Vector2F* _posScr;	

		Focus() : _unit(0L), _posScr(0L) {}
		void copy(const Focus* src);
		bool create(bool isCalcScreenPos);
		void destroy();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユニット取得
	inline const Unit* getUnit(s32 index) const { return _farr[index]._unit; }
	inline Unit* unit(s32 index) const { return _farr[index]._unit; }
	// スクリーン座標取得
	inline const Vector2F* getScreenPos(s32 index) const { return _farr[index]._posScr; }
	// 最大数取得
	inline s32 getMaxCount() const { return (s32)_numMax; }
	// スクリーン位置を計算・保持する設定かどうか
	inline bool isCalculateScreenPos() const { return TFW_IS_FLAG(_flags, F_CALC_SCR_POS); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	s32 getCount() const;
	
	void setUnit(s32 index, Unit* unit);
	void removeUnit(s32 index);

	void invalidateScreenPos(s32 index);
	bool isValidScreenPos(s32 index) const;
	void setCalculatedScreenPos(s32 index, const Vector2F* posScr);
	void calcScreenPos(s32 index);
	void calcScreenPosAll();

	void updateValidity();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FocusArray(u16 numMax, bool isCalcScreenPos);
	FocusArray(const FocusArray& src);
	~FocusArray();
	void copy(const FocusArray* src);
	void clear();
	bool recreate(u16 numMax);

	//======================================================================
	// メンバ変数
protected:
	Focus*	_farr;
	u16		_numMax;
	u16		_flags;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_FOCUS_ARRAY_H_
