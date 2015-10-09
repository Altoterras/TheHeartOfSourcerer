/***********************************************************************//**
 *	ItemArray.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/19.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_ITEM_ITEM_ARRAY_H_
#define _SRCR_ITEM_ITEM_ARRAY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

SRCR_BEGIN_NS

class Item;

/*---------------------------------------------------------------------*//**
 *	ユニット配列
 *		※基本的に受け渡し用
 *
**//*---------------------------------------------------------------------*/
class ItemArray
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユニット取得
	const Item* getItem(int index) const;
	Item* item(int index) const;
	// ユニット数を得る
	int getCount() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void addItem(Item* item);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ItemArray();
	ItemArray(Item* item1);
	ItemArray(Item* item1, Item* item2);
	ItemArray(const ItemArray& src);
	~ItemArray();
	void copy(const ItemArray* src);
	void clear();
	inline ItemArray& operator = (const ItemArray& s) { copy(&s); return *this; }

	//======================================================================
	// メンバ変数
protected:
	List<Item*>*	_listItem;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_ITEM_ITEM_ARRAY_H_
