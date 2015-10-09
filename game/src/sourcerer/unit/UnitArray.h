/***********************************************************************//**
 *	UnitArray.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/22.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_UNIT_ARRAY_H_
#define _SRCR_UNIT_UNIT_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	ユニット配列
 *		※基本的に受け渡し用
 *
**//*---------------------------------------------------------------------*/
class UnitArray
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユニット取得
	const Unit* getUnit(int index) const;
	Unit* unit(int index) const;
	// ユニット数を得る
	int getCount() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void add(const UnitArray* src);
	void addUnit(Unit* unit);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	UnitArray();
	UnitArray(Unit* unit1);
	UnitArray(Unit* unit1, Unit* unit2);
	UnitArray(const UnitArray& src);
	~UnitArray();
	void copy(const UnitArray* src);
	void clear();
	inline UnitArray& operator = (const UnitArray& s) { copy(&s); return *this; }

	//======================================================================
	// メンバ変数
protected:
	List<Unit*>*	_listUnit;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_UNIT_ARRAY_H_
