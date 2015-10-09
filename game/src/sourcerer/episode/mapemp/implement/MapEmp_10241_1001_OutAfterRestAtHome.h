/***********************************************************************//**
 *	MapEmp_10241_1001_OutAfterRestAtHome.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/01.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10241_1001_OUT_AFTER_REST_AT_HOME_H_
#define _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10241_1001_OUT_AFTER_REST_AT_HOME_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MapEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	アサ宅で休息後に外に出たときのイベント
 *
**//*---------------------------------------------------------------------*/
class MapEmp_10241_1001_OutAfterRestAtHome : public MapEmp
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MapEmp_10241_1001_OutAfterRestAtHome();
	void begin(u32 param);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_MAPEMP_IMPLEMENT_MAP_EMP_10241_1001_OUT_AFTER_REST_AT_HOME_H_
