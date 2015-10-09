/***********************************************************************//**
 *	ModelViewer.cpp
 *	Enlight BasicModel Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ModelViewer.h"

// Friends

// External
#include "../../etk/body/EtkBody.h"
#include "../../etk/body/EtkExecCtx.h"
#include "../../etk/camera/ModelViewCam.h"
#include "../../etk/sg/CamSgnc.h"
#include "../../etk/sg/SorSgm.h"
#include "../../tfw/debug/ScreenTrace.h"
#include "../../tfw/file/File.h"
#if defined(_ANDROID)
#include "../../tfw/file/FileAndroidAsset.h"
#endif
#include "../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/string/VcString.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

BASICMODEL_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ModelViewer メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ModelViewer::ModelViewer()
	: _camMvRef(0L)
	, _mdlarrRef(0L)
	, _numMdl(0)
	, _animid(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ModelViewer::~ModelViewer()
{
	ASSERT(_mdlarrRef == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ModelViewer::create()
{
	// シーングラフ及びリソースプールにモデルビュー標準カメラを追加
	etk::SorSgm* sgm = (etk::SorSgm*)EtkBody::getBody()->getSceneGraphManager();
	CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(etk::SorSgm::EK_CAM);
	{
		// モデルビュー専用カメラの追加
		ModelViewCam* mvcam = new ModelViewCam();
		if(!mvcam->create())
		{
			TRACE("{ModelViewer::create} create ModelViewCam failed.");
			return false;
		}
		if(sgm->addNode(mvcam, true))
		{
			nodecCam->switchVisible(mvcam->getId());	// カメラを有効化
			_camMvRef = mvcam;

			// 初期設定
			mvcam->loc()->y() = 100.0f;
			mvcam->loc()->z() = 1500.0f;
		}
	}

	// モデル読み込み
	VcString name = "modelview_test";
	ShapeModel** mdlarr = 0L;
	s32 numMdl = 0;
	if(!ModelLoader::loadModelsFromTbf(EtkBody::getBody()->getTfw(), &name, 0, ModelLoader::LFLAG_DISPOSE_SCENE_GRAPH, (Model***)&mdlarr, &numMdl))
	{
		TRACE("{ModelViewer::create} load model failed. %s", name.getRaw());
		return true;	// 失敗ではない
	}

	_mdlarrRef = mdlarr;
	_numMdl = numMdl;

	// アニメーションセット
	_animid = 20;//1;//33;
	for(int i = 0; i < _numMdl; i++)
	{
		if(_animid <= _mdlarrRef[i]->getMaxAnimId())
		{
			_mdlarrRef[i]->setAnimId(_animid, 0, false, true, true, false, false);
		}
	}

	// モデルが一つのときは強制表示（DEFTYPE_DECLARE だと表示されないので）
	if(_numMdl == 1)
	{
		_mdlarrRef[0]->setShow(true);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ModelViewer::destroy()
{
	// シーングラフのモデルを破棄
	etk::SorSgm* sgm = (etk::SorSgm*)EtkBody::getBody()->getSceneGraphManager();
	if(_mdlarrRef != 0L)
	{
		for(int i = 0; i < _numMdl; i++)
		{
			sgm->removeNode(_mdlarrRef[i]);
		}
		delete[] _mdlarrRef;
		_mdlarrRef = 0L;
		_numMdl = 0;
	}

	// カメラを破棄
	if(_camMvRef != 0L)
	{
		sgm->removeNode(_camMvRef);
		_camMvRef = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ModelViewer::exec(ExecRes* res, const ExecCtx* ec)
{
	EtkExecCtx* eec = (EtkExecCtx*)ec;
	TouchPanel* touchp = eec->getTouchPanel(); 
	if(!touchp->isReacted() && touchp->isClickRelease(0L))
	{
		{
			// アニメーション切替
			_animid++;
			bool isValidAnimId = false;
			for(int cnt = 1; cnt <= 2; cnt++)
			{
				for(int i = 0; i < _numMdl; i++)
				{
					if(_animid <= _mdlarrRef[i]->getMaxAnimId())
					{
						_mdlarrRef[i]->setAnimId(_animid, 30, false, true, true, false, false);
						isValidAnimId = true;
					}
				}
				if(isValidAnimId)	{	break;	}
				_animid = 1;
			}
		}
		touchp->setReacted(true);
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void ModelViewer::render(const RenderCtx* rc)
{
	ScreenTrace* st = EtkBody::getBody()->getScreenTrace();
	if(st != 0L)
	{
		st->traceFormat("_animid=%d", _animid);
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void ModelViewer::draw(const RenderCtx* rc)
{
	Renderer* rdr = rc->getRenderer();
}

////////////////////////////////////////////////////////////////////////////

BASICMODEL_END_NS