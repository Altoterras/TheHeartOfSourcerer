/***********************************************************************//**
 *	WorldMapMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_WORLDMAP_WORLD_MAP_MODE_H_
#define _SRCR_MODE_INGAME_WORLDMAP_WORLD_MAP_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class WorldMap;

/*---------------------------------------------------------------------*//**
 *	世界地図モード
 *
**//*---------------------------------------------------------------------*/
class WorldMapMode : public GameModeBase
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// クラス
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	WorldMapMode();
	~WorldMapMode();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);
	void render(const RenderCtx* rc);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);

	//======================================================================
	// 変数
private:
	WorldMap* _wmapRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_WORLDMAP_WORLD_MAP_MODE_H_
