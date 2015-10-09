/***********************************************************************//**
 *	MapMod10181.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_MAPMOD_IMPLEMENT_MAP_MOD_10181_H_
#define _SRCR_EPISODE_MAPMOD_IMPLEMENT_MAP_MOD_10181_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "MapMod10000.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	マップ 10181 寺院入口
 *
**//*---------------------------------------------------------------------*/
class MapMod10181 : public MapMod10000
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MapMod10181();
	bool begin();
	bool end();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	//======================================================================
	// 変数
private:
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_MAPMOD_IMPLEMENT_MAP_MOD_10181_H_