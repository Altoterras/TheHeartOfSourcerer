/***********************************************************************//**
 *	PrpzlPlayMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/07.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PrpzlPlayMode.h"

// Friends
#include "../../camera/BookViewCam.h"
#include "../../purepuzzle/PurePuzzle.h"
#include "../../purepuzzle/PrpzlPerf_Enlight.h"
#include "../../body/Game.h"
#include "../../body/GameRenderCtx.h"

// External
#include "../../../etk/sg/CamSgnc.h"
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/Tfw.h"
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/lib/Color.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PrpzlPlayMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PrpzlPlayMode::PrpzlPlayMode(PrpzlOperationData* opdat)
	: _opdatRef(opdat)
	, _pzl(0L)
	, _perf(0L)
	, _camRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PrpzlPlayMode::~PrpzlPlayMode()
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool PrpzlPlayMode::notifyBegin(void* objParamRef)
{
	// パズルモジュールの作成と開始
	PrpzlPerf_Enlight* perf = new PrpzlPerf_Enlight();	ASSERT(perf != 0L);
	PurePuzzle* pzl = new PurePuzzle();					ASSERT(pzl != 0L);
	if(!pzl->begin(_opdatRef, perf))
	{
		delete pzl;
		delete perf;
		return false;
	}
	_perf = perf;
	_pzl = pzl;

	// クリアカラー設定
	Vector4F* colorClear = Game::getGame()->getTfw()->getRenderer()->clearColor();
	colorClear->set(0.25f, 0.25f, 0.25f, 1.0f);

	// シーングラフ及びリソースプールにモデルビュー標準カメラを追加
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
	{
		// モデルビューカメラの追加
		BookViewCam* bvcam = new BookViewCam();
		if(!bvcam->create())
		{
			return false;
		}
		if(sgm->addNode(bvcam, true))
		{
			nodecCam->switchVisible(bvcam->getId());	// カメラを有効化
			_camRef = bvcam;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void PrpzlPlayMode::notifyEnd(void* objParamRef)
{
	// カメラを破棄
	if(_camRef != 0L)
	{
		SorSgm* sgm = Game::getGame()->getSceneGraph();
		sgm->removeNode(_camRef);
		_camRef = 0L;
	}

	// クリアカラー設定を戻す
	Vector4F* colorClear = Game::getGame()->getTfw()->getRenderer()->clearColor();
	colorClear->set(0.0f, 0.0f, 0.0f, 0.0f);

	// パズルモジュールの終了と削除
	if(_pzl != 0L)
	{
		_pzl->end();
		delete _pzl;
		_pzl = 0L;
		delete _perf;
		_perf = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void PrpzlPlayMode::exec(ExecRes* res, const ExecCtx* ec)
{
	GameModeBase::exec(res, ec);

	_pzl->exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void PrpzlPlayMode::render(const RenderCtx* rc)
{
	GameModeBase::render(rc);

	_pzl->render(rc);
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void PrpzlPlayMode::draw(const RenderCtx* rc)
{
	GameModeBase::draw(rc);

	_pzl->draw(rc);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
