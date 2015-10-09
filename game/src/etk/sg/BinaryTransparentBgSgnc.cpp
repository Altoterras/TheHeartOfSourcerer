/***********************************************************************//**
 *  BinaryTransparentBgSgnc.cpp
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
#include "BinaryTransparentBgSgnc.h"

// Friends

// External
#include "../../tfw/gcmn/Renderer.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BinaryTransparentBgSgnc メソッド

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void BinaryTransparentBgSgnc::renderImplements(const RenderCtx* rc)
{
	if(getChildNodeNum() <= 0)	{	return;	}
	
	Renderer* rdr = rc->getRenderer();
	
	// 二値透明背景用描画設定
	rdr->setAlphaTest(true);						// アルファテストあり
//	::glAlphaFunc(GL_EQUAL, 1.0f);					// α=1.0f (a=255) のみ受け付ける
	::glAlphaFunc(GL_GEQUAL, 0.5f);					// α>=0.5f (a>=127) のみ受け付ける
	rdr->setAlphaBlend(true);						// アルファ合成あり
	rdr->setAlphaBlendFunc(Renderer::AFUNC_NORMAL);	// アルファ通常合成
	rdr->setZTest(true);							// Z テストあり
	::glDepthFunc(GL_LEQUAL);						// Z 値が小さいもの（近いもの）が Z テスト合格
	rdr->setZWriting(true);							// Z 書き込みあり

	SgNodeChain::renderImplements(rc);
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS