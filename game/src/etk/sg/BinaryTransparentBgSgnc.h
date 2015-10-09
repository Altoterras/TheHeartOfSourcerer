/***********************************************************************//**
 *  BinaryTransparentBgSgnc.h
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2009/08/07.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_SG_IMPLEMENT_SOR_BINARY_TRANSPARENT_BG_SGNC_H_
#define _ETK_SG_IMPLEMENT_SOR_BINARY_TRANSPARENT_BG_SGNC_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/g3d/sg/node/SgNodeChain.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	二値透明背景 ノード連結クラス
 *		“二値透明”とは、α値が 0 か 255 の二値のみで、
 *		ピクセルを完全に描くないか、不透明のように描くかの
 *		二種類のみの描画方法の事を指す
 *
**//*---------------------------------------------------------------------*/
class BinaryTransparentBgSgnc : public SgNodeChain
{
	//======================================================================
	// メソッド

public:
	virtual void renderImplements(const RenderCtx* rc);
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_SG_IMPLEMENT_SOR_BINARY_TRANSPARENT_BG_SGNC_H_

