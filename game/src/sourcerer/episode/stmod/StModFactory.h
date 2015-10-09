/***********************************************************************//**
 *	StModFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STMOD_ST_MOD_FACTORY_H_
#define _SRCR_EPISODE_STMOD_ST_MOD_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class StMod;
class StoryManager;
class Game;

/*---------------------------------------------------------------------*//**
 *	ストーリー専用モジュール ファクトリー
 *
**//*---------------------------------------------------------------------*/
class StModFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static StMod* makeStMod(s32 storyid, StoryManager* strymngRef);
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_STMOD_ST_MOD_FACTORY_H_
