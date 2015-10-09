/***********************************************************************//**
 *	MapEmp.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/31.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_MAPEMP_MAP_EMP_H_
#define _SRCR_EPISODE_MAPEMP_MAP_EMP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../event/matter/implement/ProgMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class EventSys;

/*---------------------------------------------------------------------*//**
 *	マップ用イベント マター プログラム
 *
**//*---------------------------------------------------------------------*/
class MapEmp : public ProgMatter
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MapEmp(u16 emptype);
	virtual void begin(u32 param);
	virtual void end();
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_MAPEMP_MAP_EMP_H_
