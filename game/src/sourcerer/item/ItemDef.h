/***********************************************************************//**
 *	ItemDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_ITEM_ITEM_DEF_H_
#define _SRCR_ITEM_ITEM_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../status/Elene.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;
class VcString;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	アイテム定義
 *
**//*---------------------------------------------------------------------*/
class ItemDef
{
	//======================================================================
	// 定数
public:
	enum Category
	{
		CAT_NULL = 0,
		CAT_OTHER = 1,
		CAT_STONE = 2,
		CAT_GRASS = 3,
		CAT_WEAPON = 6,
		CAT_SHIELD = 7,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// アイテム定義 ID を得る
	inline u16 getItemDefId() const { return _itmdid; }
	// カテゴリを得る
	inline Category getCategory() const { return (Category)_cat; }
	// 値段を得る
	inline u16 getPrice() const { return _price; }
	// 名前を得る
	inline const VcString* getName(LangId lang) const { return _name[lang]; }
	// 原質の五元素を得る
	inline const Elene* getOqlElene() const { return &_eleneOql; }
	// 種族特徴率を得る
	inline const Elene* getFamilyRatio() const { return &_eleneFamilyRatio; }
	// 汎用パラメータ１を得る
	inline u16 getParam1() const { return _param1; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ItemDef();
	~ItemDef();
	bool loadFromDataFile(FileBase* file);
	bool loadFromStringFile(FileBase* file);
	void cleanup();

private:
	bool loadString(VcString* str, FileBase* file);

	//======================================================================
	// 変数
private:
	u16 _itmdid;					// 定義 ID
	u16 _cat;						// カテゴリ
	u16 _price;						// 値段
	u16 _param1;					// 汎用パラメータ 1
	Elene _eleneOql;				// 五元素バランス
	Elene _eleneFamilyRatio;		// 種族特徴率
	VcString* _name[NUM_LANGID];	// 名前
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_ITEM_ITEM_DEF_H_
