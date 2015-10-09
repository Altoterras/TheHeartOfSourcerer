/***********************************************************************//**
 *	EdevParticleSys.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/06/25.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EdevParticleSys.h"

// Friends
#include "../body/EdevBody.h"
#include "../file/EdevFileIdDef.h"

// External
#include "../../etk/sg/TbfDefineEx.h"
#include "../../tfw/file/IdFile.h"
#include "../../tfw/g3d/sg/node/implement/ModelLoader.h"

EDEV_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	パーティクルモデルを読み込む
**//*---------------------------------------------------------------------*/
Model* EdevParticleSys::loadModel(s32 pmdlid)
{
	Model* model = 0L;
	if((PMDLID_PROG__START_ <= pmdlid) && (pmdlid <= PMDLID_PROG__END_))
	{
		/*
		model = ProgPtclMdlFactory::makeProgPrtlModel(pmdlid);
		if(model == 0L)
		*/
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
			TRACE("{EdevParticleSys::loadModel} no file: fileid=%d\n", fileid);
			return 0L;
		}

		if(!ModelLoader::loadModelFromTbf(EdevBody::getBody()->getTfw(), fileid, TbfDefineEx::MEXF_PTCL, ModelLoader::LFLAG_LOAD_BILLBOARD, (Model**)&model))
		{
			ASSERT(false);
			return 0L;
		}
	}

	return model;
}

////////////////////////////////////////////////////////////////////////////

EDEV_END_NS
