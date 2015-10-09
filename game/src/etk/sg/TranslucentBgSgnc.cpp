/***********************************************************************//**
 *  TranslucentBgSgnc.cpp
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2009/08/07.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TranslucentBgSgnc.h"

// Friends

// External
#include "../../tfw/gcmn/Renderer.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TranslucentBgSgnc メソッド

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void TranslucentBgSgnc::renderImplements(const RenderCtx* rc)
{
	if(getChildNodeNum() <= 0)	{	return;	}
	
	Renderer* rdr = rc->getRenderer();
	
	// 半透明背景用描画設定
	rdr->setAlphaTest(true);						// アルファテストあり
	::glAlphaFunc(GL_GREATER, 0.0f);				// α > 0 を受け付ける
	rdr->setAlphaBlend(true);						// アルファ合成あり
	rdr->setZTest(true);							// Z テストあり
	::glDepthFunc(GL_LEQUAL);						// Z 値が小さいもの（近いもの）が Z テスト合格
	rdr->setZWriting(false);						// Z 書き込みなし
	
	SgNodeChain::renderImplements(rc);
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS