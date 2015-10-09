/***********************************************************************//**
 *  ZsortModelSgnc.cpp
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2010/07/01.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ZsortModelSgnc.h"

// Friends

// External
#include "../../tfw/collection/List.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/g3d/sg/node/implement/Model.h"
#include "../../tfw/gcmn/Renderer.h"

// Library
#include <stdlib.h>	// qsort

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ZsortModelSgnc メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ZsortModelSgnc::ZsortModelSgnc()
	: _numMdl(0)
{
	TFW_ZERO_MEMORY(_qsdat, sizeof(Qsdat) * NUM_MDL_MAX);
}

/*---------------------------------------------------------------------*//**
	描画前処理
**//*---------------------------------------------------------------------*/
void ZsortModelSgnc::previseImplements(const RenderCtx* rc)
{
	// モデル数を得る
	s32 num = _listChild->getCount();
	ASSERT(num <= NUM_MDL_MAX);
	if(num > NUM_MDL_MAX)
	{
		// 万が一のための回避策
		SgNodeChain::previseImplements(rc);
		_numMdl = -1;
		return;
	}

	// 描画前処理と同時に Z 値を集める
	s32 cnt = 0;
	for(ListIterator<SgNode*> it = _listChild->iterator(); it.has(); it.next())
	{
		SgNode* sgn = it.object();
		sgn->previse(rc);

		if(sgn->getType() == SGNTYPE_MODEL)
		{
			Model* mdl = (Model*)sgn;
			const Vector3F* viewpos = mdl->getViewPosition();
			if(viewpos != 0L)
			{
				_qsdat[cnt]._mdl = mdl;
				_qsdat[cnt]._z = viewpos->z();
				cnt++;
			}
		}
	}
	_numMdl = cnt;

	// ソート
	::qsort(_qsdat, _numMdl, sizeof(Qsdat), (int (*)(const void*, const void*))compare);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void ZsortModelSgnc::renderImplements(const RenderCtx* rc)
{
	if(getChildNodeNum() <= 0)	{	return;	}
	
	Renderer* rdr = rc->getRenderer();
	
	// 半透明モデル用描画設定
	rdr->setAlphaTest(true);						// アルファテストあり
	::glAlphaFunc(GL_GREATER, 0.0f);				// α > 0 を受け付ける
	rdr->setAlphaBlend(true);						// アルファ合成あり
	rdr->setZTest(true);							// Z テストあり
	::glDepthFunc(GL_LEQUAL);						// Z 値が小さいもの（近いもの）が Z テスト合格
	#if TEST_ALL_ZSORT
		rdr->setZWriting(true);							// Z 書き込みあり
	#else
		rdr->setZWriting(false);						// Z 書き込みなし
	#endif
	rdr->setCulling(true);							// カリングあり
	
	if(_numMdl < 0)	// モデル数が最大値を超えた
	{
		// 回避
		SgNodeChain::renderImplements(rc);
	}
	else
	{
		// ソート情報に基づいて描画
		for(int i = 0; i < _numMdl; i++)
		{
			_qsdat[i]._mdl->render(rc);
		}
	}
}

/*---------------------------------------------------------------------*//**
	クイックソート比較関数
**//*---------------------------------------------------------------------*/
int ZsortModelSgnc::compare(const Qsdat* a, const Qsdat* b)
{
	if(a->_z > b->_z)	{	return 1;	}
	if(a->_z < b->_z)	{	return -1;	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS