/***********************************************************************//**
 *  ModelLoader.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/13.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G3D_SG_NODE_IMPLEMENT_MODEL_LOADER_H_
#define _TFW_G3D_SG_NODE_IMPLEMENT_MODEL_LOADER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Model;
class ShapeModel;
class VcString;

/*---------------------------------------------------------------------*//**
	モデル ローダー クラス

**//*---------------------------------------------------------------------*/
class ModelLoader
{
	//======================================================================
	// 定数
public:
	static const u16	LFLAG_NULL					= 0x0000;
	static const u16	LFLAG_LOAD_BILLBOARD		= 0x0001;	// ビルボードモデルの読み込み
	static const u16	LFLAG_DISPOSE_SCENE_GRAPH	= 0x0002;	// 読み込み後、シーングラフに自動登録

	//======================================================================
	// メソッド
public:
	static bool			loadModelFromTbf(Tfw* tfwRef, const CStringBase* filename, u16 exflags, u16 lflag, Model** mdlRes);
	static bool			loadModelFromTbf(Tfw* tfwRef, u32 fileid, u16 exflags, u16 lflag, Model** mdlRes);
	static bool			loadModelsFromTbf(Tfw* tfwRef, const CStringBase* filename, u16 exflags, u16 lflag, Model*** mdlarrRes, s32* numModelRes);
	static bool			loadModelsFromTbf(Tfw* tfwRef, u32 fileid, u16 exflags, u16 lflag, Model*** mdlarrRes, s32* numModelRes);

private:
	static bool			loadAfter(Tfw* tfwRef, Model** mdlarr, s32 numModel, u16 exflags, bool isDisposeSceneGraph);
	static bool			disposeToSceneGraph(Tfw* tfwRef, Model** mdlarr, s32 numModel);
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G3D_SG_NODE_IMPLEMENT_MODEL_LOADER_H_
