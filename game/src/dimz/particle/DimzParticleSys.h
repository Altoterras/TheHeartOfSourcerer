/***********************************************************************//**
 *	DimzParticleSys.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/06/25.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _DIMZ_PARTICLE_DIMZ_PARTICLE_SYS_H_
#define _DIMZ_PARTICLE_DIMZ_PARTICLE_SYS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/particle/ParticleSys.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

DIMZ_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ゲーム用パーティクル システム
 *
**//*---------------------------------------------------------------------*/
class DimzParticleSys : public ParticleSys
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
private:
	Model* loadModel(s32 pmdlid);
};

DIMZ_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _DIMZ_PARTICLE_DIMZ_PARTICLE_SYS_H_
