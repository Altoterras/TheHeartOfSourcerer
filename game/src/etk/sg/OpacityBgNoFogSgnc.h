/***********************************************************************//**
 *  OpacityBgNoFogSgnc.h
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2011/12/16.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_SG_IMPLEMENT_SOR_OPACITY_BG_NO_FOG_SGNC_H_
#define _ETK_SG_IMPLEMENT_SOR_OPACITY_BG_NO_FOG_SGNC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/SgNodeChain.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	不透明背景 ノード連結クラス
 *
**//*---------------------------------------------------------------------*/
class OpacityBgNoFogSgnc : public SgNodeChain
{
	//======================================================================
	// メソッド

public:
	virtual void		renderImplements(const RenderCtx* rc);
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_SG_IMPLEMENT_SOR_OPACITY_BG_NO_FOG_SGNC_H_

