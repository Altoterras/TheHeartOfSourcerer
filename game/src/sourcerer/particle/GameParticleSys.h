/***********************************************************************//**
 *	GameParticleSys.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/05/29.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_PARTICLE_GAME_PARTICLE_SYS_H_
#define _SRCR_PARTICLE_GAME_PARTICLE_SYS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/particle/ParticleSys.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ゲーム用パーティクル システム
 *
**//*---------------------------------------------------------------------*/
class GameParticleSys : public ParticleSys
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
private:
	Model* loadModel(s32 pmdlid);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_PARTICLE_GAME_PARTICLE_SYS_H_
