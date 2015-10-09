/***********************************************************************//**
 *  TranslucentBgSgnc.h
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2009/08/07.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_SG_IMPLEMENT_SOR_TRANSLUCENT_BG_SGNC_H_
#define _ETK_SG_IMPLEMENT_SOR_TRANSLUCENT_BG_SGNC_H_

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
class TranslucentBgSgnc : public SgNodeChain
{
	//======================================================================
	// メソッド

public:
	virtual void		renderImplements(const RenderCtx* rc);
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_SG_IMPLEMENT_SOR_TRANSLUCENT_BG_SGNC_H_

