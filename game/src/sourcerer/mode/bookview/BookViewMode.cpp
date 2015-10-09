/***********************************************************************//**
 *	BookViewMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BookViewMode.h"

// Friends
#include "../../bookviewer/BookViewer.h"
#include "../../camera/BookViewCam.h"
#include "../../file/GameFileIdDef.h"
#include "../../body/Game.h"
#include "../../body/GameExecCtx.h"
#include "../../body/GameRenderCtx.h"

// External
#include "../../../etk/sg/CamSgnc.h"
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/Tfw.h"
#include "../../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/gcmn/Texture.h"
#include "../../../tfw/file/File.h"
#include "../../../tfw/lib/Vector.h"
#include "../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BookViewMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	カスタムイベント呼び出しハンドラ
**//*---------------------------------------------------------------------*/
void BookViewMode::callCustomEvent(void* objRes, const VcString* strParam, const void* objParam)
{
	if(strParam->equals("Test::changeTestViewMode"))
	{
		// 制御モード変更
		endActMode();
		/*
		switch(_amode)
		{
		case AMODE__DUMMY_1:
			beginActMode(AMODE__DUMMY_2, false);
			break;
		case AMODE__DUMMY_2:
			beginActMode(AMODE__DUMMY_1, false);
			break;
		}
		*/

		///this->getParentMode()->reserveMode(GameMode::MODE_GAME_TXTEDITOR);	// 次のデバッグモードへ
		///this->getParentMode()->reserveNextMode();	// 次のデバッグモードへ
		Game::getGame()->getGameMode()->changeEndMode((GameMode::ModeKind)getParentMode()->getNextMode());	// 次のデバッグモードへ
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BookViewMode::BookViewMode()
	: _bookvw(0L)
	, _camRef(0L)
	, _dispCnt(0)
	, _amode(AMODE__DUMMY_1)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BookViewMode::~BookViewMode()
{
	ASSERT(_bookvw == 0L);
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool BookViewMode::notifyBegin(void* objParamRef)
{
	// ブックビューワーを作成
	BookViewer* bookvw = new BookViewer();
	if(!bookvw->create())
	{
		delete bookvw;
		ASSERT(false);
		return false;
	}
	_bookvw = bookvw;
	// ブックビューワーの設定と開始
	BookViewer::Config conf;
	conf._numPage = 6;
	conf._fidFirst = BOOK_BOOK01_PAGE01_PNG;
	_bookvw->begin(&conf, 0L);

	// 制御モード開始
	beginActMode(AMODE__DUMMY_1, true);

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

	// 値の初期化
	_dispCnt = 0;

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void BookViewMode::notifyEnd(void* objParamRef)
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

	// 制御モード終了
	endActMode();

	// ブックビューワーを削除
	if(_bookvw != 0L)
	{
		_bookvw->end(0L);
		_bookvw->destroy();
		delete _bookvw;
		_bookvw = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	制御モード開始
**//*---------------------------------------------------------------------*/
bool BookViewMode::beginActMode(ActMode amode, bool isFirst)
{
	SorSgm* sgm = Game::getGame()->getSceneGraph();

#if 0
	// シーングラフ及びリソースプールにモデルビュー標準カメラを追加
	CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
	{
		// モデルビュー専用カメラの追加
		ModelViewCam* mvcam = new ModelViewCam();
		if(!mvcam->create())
		{
			return false;
		}
		if(sgm->addNode(mvcam, true))
		{
			nodecCam->switchVisible(mvcam->getId());	// カメラを有効化
			_camRef = mvcam;
		}
	}
#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	制御モード終了
**//*---------------------------------------------------------------------*/
void BookViewMode::endActMode()
{
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void BookViewMode::exec(ExecRes* res, const ExecCtx* ec)
{
	GameModeBase::exec(res, ec);

	// ブックビューワーの制御
	_bookvw->exec(res, ec);

	// ユーザー操作
	TouchPanel* touchp = ((GameExecCtx*)ec)->getTouchPanel();
	if(!touchp->isReacted() && touchp->isClickRelease(0L))
	{
		switch(_amode)
		{
		case AMODE__DUMMY_1:
			break;
		}
	}

	// モードをスクリーントレース
	if(_dispCnt < 60)
	{
		VcString stmsg = ": BOOKVIEW :\n";
		STRACE(stmsg.getRaw());
	}
	_dispCnt++;
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void BookViewMode::render(const RenderCtx* rc)
{
	GameModeBase::render(rc);

	// ブックビューワーの描画
	_bookvw->render(rc);
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void BookViewMode::draw(const RenderCtx* rc)
{
	GameModeBase::draw(rc);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
