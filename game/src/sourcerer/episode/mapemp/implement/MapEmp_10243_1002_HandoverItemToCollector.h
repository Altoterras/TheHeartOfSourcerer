/***********************************************************************//**
 *	MapEmp_10243_1002_HandoverItemToCollector.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/21.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10243_1002_HAND_OVER_ITEM_TO_COLLECTOR_H_
#define _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10243_1002_HAND_OVER_ITEM_TO_COLLECTOR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MapEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Item;

/*---------------------------------------------------------------------*//**
 *	石コレクター（タケリ）に石を渡すイベント
 *
**//*---------------------------------------------------------------------*/
class MapEmp_10243_1002_HandoverItemToCollector : public MapEmp
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MapEmp_10243_1002_HandoverItemToCollector();
	void begin(u32 param);
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

private:
	void handoverItem(const Item* item);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10243_1002_HAND_OVER_ITEM_TO_COLLECTOR_H_
