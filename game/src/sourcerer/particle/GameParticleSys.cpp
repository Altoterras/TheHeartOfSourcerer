/***********************************************************************//**
 *	GameParticleSys.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/05/29.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GameParticleSys.h"

// Friends
#include "progptclmdl/ProgPtclMdlFactory.h"
#include "../body/Game.h"
#include "../file/GameFileIdDef.h"

// External
#include "../../etk/sg/TbfDefineEx.h"
#include "../../tfw/file/IdFile.h"
#include "../../tfw/g3d/sg/node/implement/ModelLoader.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	パーティクルモデルを読み込む
**//*---------------------------------------------------------------------*/
Model* GameParticleSys::loadModel(s32 pmdlid)
{
	Model* model = 0L;
	if((PMDLID_PROG__START_ <= pmdlid) && (pmdlid <= PMDLID_PROG__END_))
	{
		model = ProgPtclMdlFactory::makeProgPrtlModel(pmdlid);
		if(model == 0L)
		{
			ASSERT(false);
			return 0L;
		}
	}
	else
	{
		u32 fileid = FILEID_FROM_PARTICLE_MODEL(pmdlid);
		if(!IdFile::isIdFileExist(fileid))
		{
			TRACE("{GameParticleSys::loadModel} no file: fileid=%d\n", fileid);
			return 0L;
		}

		if(!ModelLoader::loadModelFromTbf(Game::getGame()->getTfw(), fileid, TbfDefineEx::MEXF_PTCL, ModelLoader::LFLAG_LOAD_BILLBOARD, (Model**)&model))
		{
			ASSERT(false);
			return 0L;
		}
	}

	return model;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
