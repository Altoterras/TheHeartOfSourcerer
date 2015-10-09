/***********************************************************************//**
 *	MapEmp_10243_1001_DoorControl.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/18.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10243_1001_DOOR_CONTROL_H_
#define _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10243_1001_DOOR_CONTROL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MapEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	井戸舎（アイテム合成場）のドア制御イベント
 *
**//*---------------------------------------------------------------------*/
class MapEmp_10243_1001_DoorControl : public MapEmp
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MapEmp_10243_1001_DoorControl();
	void begin(u32 param);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10243_1001_DOOR_CONTROL_H_
