/***********************************************************************//**
 *	EquipmentItemUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_ITEM_EQUIPMENT_ITEM_UNIT_H_
#define _SRCR_UNIT_ITEM_EQUIPMENT_ITEM_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "ItemUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class HrAnimObjCtx;

TFW_END_NS

SRCR_BEGIN_NS

class CharStat;
class Item;

/*---------------------------------------------------------------------*//**
 *	所持アイテム ユニット
 *
**//*---------------------------------------------------------------------*/
class EquipmentItemUnit : public ItemUnit
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// アイテムを得る
	inline const Item* getItem() const { return _itmRef; }
	inline Item* item() { return _itmRef; }
	// 所持者を得る
	Unit* getOwnerUnit() const { return _unitOwnerRef; }

protected:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setGuiFocused(bool isEnable);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EquipmentItemUnit();
	~EquipmentItemUnit();
	bool create(Item* item, Unit* owner, u16 spid);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

protected:

	//======================================================================
	// メンバ変数
protected:
	Item* _itmRef;				// アイテム
	Unit* _unitOwnerRef;		// 所持者
	HrAnimObjCtx* _haocSpRef;	// 装備の物理的場所
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_ITEM_EQUIPMENT_ITEM_UNIT_H_
