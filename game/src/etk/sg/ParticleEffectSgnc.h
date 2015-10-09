/***********************************************************************//**
 *  ParticleEffectSgnc.h
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2009/09/10.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_SG_IMPLEMENT_SOR_PARTICLE_EFFECT_SGNC_H_
#define _ETK_SG_IMPLEMENT_SOR_PARTICLE_EFFECT_SGNC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/SgNodeChain.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	半透明背景 ノード連結クラス
 *
**//*---------------------------------------------------------------------*/
class ParticleEffectSgnc : public SgNodeChain
{
	//======================================================================
	// メソッド

public:
	virtual void		renderImplements(const RenderCtx* rc);
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_SG_IMPLEMENT_SOR_PARTICLE_EFFECT_SGNC_H_

