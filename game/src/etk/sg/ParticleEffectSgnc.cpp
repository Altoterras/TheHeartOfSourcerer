/***********************************************************************//**
 *  ParticleEffectSgnc.cpp
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2009/09/10.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ParticleEffectSgnc.h"

// Friends

// External
#include "../../tfw/gcmn/Renderer.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ParticleEffectSgnc メソッド

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void ParticleEffectSgnc::renderImplements(const RenderCtx* rc)
{
	if(getChildNodeNum() <= 0)	{	return;	}
	
	Renderer* rdr = rc->getRenderer();
	
	// パーティクルエフェクト用描画設定
	rdr->setAlphaTest(true);						// アルファテストあり
	::glAlphaFunc(GL_GREATER, 0.0f);				// α > 0 を受け付ける
	rdr->setAlphaBlend(true);						// アルファ合成あり
	rdr->setZTest(true);							// Z テストあり
	::glDepthFunc(GL_LEQUAL);						// Z 値が小さいもの（近いもの）が Z テスト合格
	rdr->setZWriting(false);						// Z 書き込みなし
	
#if TEST_PTCL
	// パーティクルテスト
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
#endif
	
	SgNodeChain::renderImplements(rc);
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS