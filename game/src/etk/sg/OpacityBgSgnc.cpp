/***********************************************************************//**
 *  OpacityBgSgnc.cpp
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2009/07/17.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "OpacityBgSgnc.h"

// Friends

// External
#include "../../tfw/gcmn/Renderer.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// OpacityBgSgnc メソッド

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void OpacityBgSgnc::renderImplements(const RenderCtx* rc)
{
	if(getChildNodeNum() <= 0)	{	return;	}

	Renderer* rdr = rc->getRenderer();
	
	// 不透明背景用描画設定
	rdr->setAlphaTest(false);						// アルファテストなし
	rdr->setAlphaBlend(false);						// アルファ合成なし
	rdr->setZTest(true);							// Z テストあり
	::glDepthFunc(/*GL_LESS*/GL_LEQUAL);			// Z 値が小さいもの（近いもの）が Z テスト合格
	rdr->setZWriting(true);							// Z 書き込みあり
	rdr->setFog(true);								// フォグあり

	SgNodeChain::renderImplements(rc);
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS