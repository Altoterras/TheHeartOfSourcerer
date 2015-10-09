/***********************************************************************//**
 *	ItemIdDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_ITEM_ITEM_ID_DEF_H_
#define _SRCR_EPISODE_ITEM_ITEM_ID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	アイテム ID の定義
 *
**//*---------------------------------------------------------------------*/
class ItemIdDef
{
	//======================================================================
	// 定数
public:
	// アイテム定義 ID
	static const u16	ITEMDID_DUMMY			= 9;		// 予約用ダミー
	static const u16	ITEMDID_YELLOW_STONE	= 2005;		// 黄石
	static const u16	ITEMDID_GENBU_STONE		= 2050;		// 玄武石
	static const u16	ITEMDID_SOIL_GRASS		= 3009;		// 地草
	static const u16	ITEMDID_WEP_FIRST		= 6001;		// 最初の棍棒
	static const u16	ITEMDID_SHLD_DUMMY		= 7001;		// ダミー盾
	static const u16	ITEMDID_SHLD_FIRST		= 7002;		// 最初の盾
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_ITEM_ITEM_ID_DEF_H_
