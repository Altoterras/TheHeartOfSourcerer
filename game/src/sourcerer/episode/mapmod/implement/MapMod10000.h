/***********************************************************************//**
 *	MapMod10000.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/15.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_MAPMOD_IMPLEMENT_MAP_MOD_10000_H_
#define _SRCR_EPISODE_MAPMOD_IMPLEMENT_MAP_MOD_10000_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MapMod.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	１作目のマップ共通
 *
**//*---------------------------------------------------------------------*/
class MapMod10000 : public MapMod
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MapMod10000();

	virtual bool begin();
	virtual bool end();
	virtual void exec(ExecRes* res, const ExecCtx* ec);

protected:
	s32 playSurroundingsSe(u16 seid, f32 volume, bool isLoop, bool isPosRandom);

	//======================================================================
	// 変数
private:
	s32 _sechWaterSrrnd;
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_MAPMOD_IMPLEMENT_MAP_MOD_10000_H_
