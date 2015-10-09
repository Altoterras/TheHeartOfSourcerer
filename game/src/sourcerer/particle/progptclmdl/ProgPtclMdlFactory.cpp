/***********************************************************************//**
 *	ProgPtclMdlFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/11.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ProgPtclMdlFactory.h"

// Friends
#include "implement/FanRangeModel.h"
#include "../../body/Game.h"

// External
#include "../../../tfw/common/ResourcePool.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	プログラム制御パーティクルモデルの作成
**//*---------------------------------------------------------------------*/
Model* ProgPtclMdlFactory::makeProgPrtlModel(s32 pmdlid)
{
	// モデル作成
	Model* model = 0L;
	switch(pmdlid)
	{
	case 20001:		model = new FanRangeModel();	break;
	}
	if(model == 0L)
	{
		return 0L;
	}

	// リソースプールに追加
	ResourcePool* respl = Game::getGame()->getResourcePool();
	respl->add((SgEntity**)&model, 1);

	return model;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
