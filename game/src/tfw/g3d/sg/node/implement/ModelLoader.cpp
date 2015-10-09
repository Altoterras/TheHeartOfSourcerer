/***********************************************************************//**
 *  ModelLoader.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/18.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ModelLoader.h"

// Friends
#include "ModelLoader_tbf.h"
#include "ShapeModel.h"
#include "../../SgManager.h"
#include "../../../Shape.h"
#include "../../../Material.h"
#include "../../../../Tfw.h"
#include "../../../../gcmn/Texture.h"
#include "../../../../lib/Vector.h"
#include "../../../../lib/Matrix.h"
#include "../../../../collection/List.h"
#include "../../../../common/ResourcePool.h"
#include "../../../../string/VcString.h"
#include "../../../../string/CcString.h"
#include "../../../../string/StringUtils.h"
#include "../../../../file/File.h"

// External

// Library
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ModelLoader メソッド

/*---------------------------------------------------------------------*//**
	TBF モデルをファイルから読み込む
**//*---------------------------------------------------------------------*/
bool ModelLoader::loadModelFromTbf(Tfw* tfwRef, const CStringBase* filename, u16 exflags, u16 lflags, Model** mdlRes)
{
	// 読み込み処理
	ModelLoader_tbf loader(tfwRef);
	if(!loader.load(filename, true, TFW_IS_FLAG(lflags, LFLAG_LOAD_BILLBOARD)))
	{
		return false;
	}
	ASSERT(loader.getLoadedModelNum() == 1);

	// 読み込み後処理
	if(!loadAfter(tfwRef, loader.getLoadedModels(), loader.getLoadedModelNum(), exflags, TFW_IS_FLAG(lflags, LFLAG_DISPOSE_SCENE_GRAPH)))
	{
		return false;
	}

	if(mdlRes != 0L)	{	*mdlRes = loader.getLoadedModels()[0];	}
	return true;
}

/*---------------------------------------------------------------------*//**
	TBF モデルをパックファイルから読み込む
**//*---------------------------------------------------------------------*/
bool ModelLoader::loadModelFromTbf(Tfw* tfwRef, u32 fileid, u16 exflags, u16 lflags, Model** mdlRes)
{
	// 読み込み処理
	ModelLoader_tbf loader(tfwRef);
	if(!loader.load(fileid, true, TFW_IS_FLAG(lflags, LFLAG_LOAD_BILLBOARD)))
	{
		return false;
	}
	ASSERT(loader.getLoadedModelNum() == 1);

	// 読み込み後処理
	if(!loadAfter(tfwRef, loader.getLoadedModels(), loader.getLoadedModelNum(), exflags, TFW_IS_FLAG(lflags, LFLAG_DISPOSE_SCENE_GRAPH)))
	{
		return false;
	}

	if(mdlRes != 0L)	{	*mdlRes = loader.getLoadedModels()[0];	}
	return true;
}

/*---------------------------------------------------------------------*//**
	複数の TBF モデルをファイルから読み込む
**//*---------------------------------------------------------------------*/
bool ModelLoader::loadModelsFromTbf(Tfw* tfwRef, const CStringBase* filename, u16 exflags, u16 lflags, Model*** mdlarrRes, s32* numModelRes)
{
	// 読み込み処理
	ModelLoader_tbf loader(tfwRef);
	if(!loader.load(filename, false, TFW_IS_FLAG(lflags, LFLAG_LOAD_BILLBOARD)))
	{
		TRACE("{ModelLoader::loadModelsFromTbf} load file. filename=%s, lflags=0x%04x", filename->getRaw(), lflags);
		return false;
	}

	// 読み込み後処理
	if(!loadAfter(tfwRef, loader.getLoadedModels(), loader.getLoadedModelNum(), exflags, TFW_IS_FLAG(lflags, LFLAG_DISPOSE_SCENE_GRAPH)))
	{
		TRACE("{ModelLoader::loadModelsFromTbf} loadAfter file. exflags=0x%04x, lflags=0x%04x", exflags, lflags);
		return false;
	}

	if(mdlarrRes != 0L)
	{
		*mdlarrRes = new Model*[loader.getLoadedModelNum()];
		for(int i = 0; i < loader.getLoadedModelNum(); i++)
		{
			(*mdlarrRes)[i] = loader.getLoadedModels()[i];
		}
	}
	if(numModelRes != 0L)	{	*numModelRes = loader.getLoadedModelNum();	}
	return true;
}

/*---------------------------------------------------------------------*//**
	複数の TBF モデルをパックファイルから読み込む
**//*---------------------------------------------------------------------*/
bool ModelLoader::loadModelsFromTbf(Tfw* tfwRef, u32 fileid, u16 exflags, u16 lflags, Model*** mdlarrRes, s32* numModelRes)
{
	// 読み込み処理
	ModelLoader_tbf loader(tfwRef);
	if(!loader.load(fileid, false, TFW_IS_FLAG(lflags, LFLAG_LOAD_BILLBOARD)))
	{
		return false;
	}

	// 読み込み後処理
	if(!loadAfter(tfwRef, loader.getLoadedModels(), loader.getLoadedModelNum(), exflags, TFW_IS_FLAG(lflags, LFLAG_DISPOSE_SCENE_GRAPH)))
	{
		return false;
	}

	if(mdlarrRes != 0L)
	{
		*mdlarrRes = new Model*[loader.getLoadedModelNum()];
		for(int i = 0; i < loader.getLoadedModelNum(); i++)
		{
			(*mdlarrRes)[i] = loader.getLoadedModels()[i];
		}
	}
	if(numModelRes != 0L)	{	*numModelRes = loader.getLoadedModelNum();	}
	return true;
}

/*---------------------------------------------------------------------*//**
	読み込み後処理
**//*---------------------------------------------------------------------*/
bool ModelLoader::loadAfter(Tfw* tfwRef, Model** mdlarr, s32 numModel, u16 exflags, bool isDisposeSceneGraph)
{
	bool isSucceeded = true;

	// モデルタイプを設定
	for(int i = 0; i < numModel; i++)
	{
		Model* mdl = mdlarr[i];
		mdl->setExFlag(exflags, true);
	}

	// リソースプールに追加
	ResourcePool* respl = tfwRef->getResourcePool();
	respl->add((SgEntity**)mdlarr, numModel);

	// シーングラフへ配置
	if(isDisposeSceneGraph)
	{
		if(!disposeToSceneGraph(tfwRef, mdlarr, numModel))
		{
			isSucceeded = false;
			ASSERT(false);
		}
	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	モデルをシーングラフに配置する
**//*---------------------------------------------------------------------*/
bool ModelLoader::disposeToSceneGraph(Tfw* tfwRef, Model** mdlarr, s32 numModel)
{
	bool isSucceeded = true;

	SgManager* sgmng = tfwRef->getSgManager();
	for(int i = 0; i < numModel; i++)
	{
		if(!sgmng->addNode(mdlarr[i]))
		{
			isSucceeded = false;
		}
	}

	return isSucceeded;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS