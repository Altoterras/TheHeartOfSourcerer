/***********************************************************************//**
 *  Tfw.cpp
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
#include "Tfw.h"

// Friends
#include "common/ResourcePool.h"
#include "common/IdManager.h"
#include "gcmn/View.h"
#include "gcmn/Renderer.h"
#include "g3d/sg/SgManager.h"
#include "collection/List.h"
#include "sound/Sound.h"
#if defined(_DEBUG)
	#include "collection/List.h"
#endif

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 変数

#if ENABLE_GETCURTFW
// フレームワークリスト
static List<Tfw*>*	s_listTfw;
#endif

////////////////////////////////////////////////////////////////////////////
// 関数

#if ENABLE_GETCURTFW
// フレームワークリストから現在のフレームワークを取得
Tfw* getCurTfw()
{
	if(s_listTfw == 0L)	{	return 0L;	}
	ASSERT(s_listTfw->getCount() > 0);
	return s_listTfw->node(0)->getObject();	// 現状では１つに固定
}
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Tfw メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ビューのサイズ変更
**//*---------------------------------------------------------------------*/
void Tfw::resizeView(f32 width, f32 height, u32 dispflags)
{
	_view->setDisplayFlags(dispflags, false);
	_view->setRealSize(width, height, true);
}

/*---------------------------------------------------------------------*//**
	ビューのサイズ変更
**//*---------------------------------------------------------------------*/
void Tfw::resizeView(f32 width, f32 height)
{
	_view->setRealSize(width, height, true);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Tfw::Tfw() :
	_sgmng(0L),
	_view(0L),
	_respool(0L),
	_idmngr(0L),
	_renderer(0L)
{
	#if ENABLE_GETCURTFW
		// フレームワークリストに追加
		if(s_listTfw == 0L)
		{
			s_listTfw = new List<Tfw*>;
			s_listTfw->addTail(this);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Tfw::~Tfw()
{
	ASSERT(_renderer == 0L);
	ASSERT(_idmngr == 0L);
	ASSERT(_respool == 0L);
	ASSERT(_view == 0L);
	ASSERT(_sgmng == 0L);

	#if ENABLE_GETCURTFW
		// フレームワークリストから削除
		ASSERT(s_listTfw != 0L);
		s_listTfw->remove(this);
		if(s_listTfw->getCount() == 0)
		{
			delete s_listTfw;
			s_listTfw = 0L;
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Tfw::create(SgManager* sgmngEntr, View* viewEntr)
{
	// ビューの保持
	_view = viewEntr;

	// ID マネージャの作成
	IdManager* idmngr = new IdManager();
	if(idmngr != 0L)
	{
		const u32 NUM_ID = 32760;
		if(!idmngr->create(NUM_ID))
		{
			delete idmngr;
			ASSERTM(false, "idmngr::create failed.");
			return false;
		}
		_idmngr = idmngr;
	}

	// リソースプールの作成
	ResourcePool* respool = new ResourcePool();
	if(respool != 0L)
	{
		if(!respool->create(_idmngr))
		{
			delete respool;
			ASSERTM(false, "ResourcePool::create failed.");
			return false;
		}
		_respool = respool;
	}

	// レンダラーの作成
	Renderer* renderer = new Renderer();
	if(renderer != 0L)
	{
		if(!renderer->create(_view))
		{
			delete renderer;
			ASSERTM(false, "Renderer::create failed.");
			return false;
		}
		_renderer = renderer;
	}

	// シーングラフマネージャの保持と作成
	SgManager* sgmng = sgmngEntr;
	if(sgmng != 0L)
	{
		if(!sgmng->create(this))
		{
			delete sgmng;
			ASSERTM(false, "SgManager::create failed.");
			return false;
		}
		_sgmng = sgmng;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Tfw::destroy()
{
	// シーングラフマネージャの削除
	if(_sgmng != 0L)
	{
		_sgmng->destroy();
		delete _sgmng;
		_sgmng = 0L;
	}

	// レンダラーの削除
	if(_renderer != 0L)
	{
		_renderer->destroy();
		delete _renderer;
		_renderer = 0L;
	}

	// リソースプールの削除
	if(_respool != 0L)
	{
		_respool->destroy();
		delete _respool;
		_respool = 0L;
	}

	// ID マネージャの削除
	if(_idmngr != 0L)
	{
		_idmngr->destroy();
		delete _idmngr;
		_idmngr = 0L;
	}

	// ビューの削除
	if(_view != 0L)
	{
		_view->destroy();
		delete _view;
		_view = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void Tfw::exec(ExecRes* res, const ExecCtx* ec)
{
	_sgmng->exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void Tfw::render(const RenderCtx* rc)
{
	_sgmng->render(rc);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS