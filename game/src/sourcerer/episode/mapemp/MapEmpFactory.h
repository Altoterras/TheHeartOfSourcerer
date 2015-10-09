/***********************************************************************//**
 *	MapEmpFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/31.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_MAPEMP_MAP_EMP_FACTORY_H_
#define _SRCR_EPISODE_MAPEMP_MAP_EMP_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class EventMatter;

/*---------------------------------------------------------------------*//**
 *	ストーリー用イベント マター プログラム ファクトリー
 *
**//*---------------------------------------------------------------------*/
class MapEmpFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static EventMatter* newEventMatter(const CStringBase* nameMatter);
	static EventMatter* newEventMatter(s32 mapid, u32 caseid);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_MAPEMP_MAP_EMP_FACTORY_H_
