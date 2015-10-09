/***********************************************************************//**
 *	DimzTest.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "DimzTest.h"

// Friends
#include "../particle/DimzParticleSys.h" 

// External
#include "../../etk/body/EtkBody.h"
#include "../../etk/body/EtkExecCtx.h"
#include "../../etk/camera/ModelViewCam.h"
#include "../../etk/sound/SoundMngr.h" 
#include "../../sourcerer/sg/CamSgnc.h"
#include "../../sourcerer/sg/SorSgm.h"
#include "../../tfw/common/PerformanceMeter.h"
#include "../../tfw/file/File.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/modemng/Mode.h"
#include "../../tfw/smenu/Menu.h"
#include "../../tfw/smenu/MenuTreeNode.h"
#include "../../tfw/string/StringUtils.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

DIMZ_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define TEST_FONT_PERFORMANCE	(1)
#define TEST_TEXTURE			(2)
#define TEST_VIDEO				(3)
#define TEST_MESSAGE_WINDOW		(4)
#define TEST_ES					(0) //(0)
#define TEST_ES_SS				(0)

#define ACTIVE_TEST				(0)	//TEST_MESSAGE_WINDOW//0

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// DimzTest メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ファンクションキーによるテスト実行
**//*---------------------------------------------------------------------*/
void DimzTest::onKeydownFunctionKey(s32 funckey)
{
	switch(funckey)
	{
	case 1:		// F1
		break;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
DimzTest::DimzTest()
	: _pmeter(0L)
	, _menu(0L)
	, _camMvRef(0L)
{
	setTestFlags(F_SHOW_PMETER | F_SHOW_DBGMENUST, true);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
DimzTest::~DimzTest()
{
	ASSERT(_menu == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool DimzTest::create(f32 spfRender, f32 spfLogic)
{
	// パフォーマンスメーターの作成
	_pmeter = new PerformanceMeter(35, spfRender);

	#if 0
		// デバッグメニュー作成
		Menu* menu = new Menu();
		MenuFuncTable* ftmv = new MenuFuncTable_Debug();
		MenuPanelFactory* pf = new GameMenuPanelFactory();
		GameMenuCreateParam* mcprm = new GameMenuCreateParam();
		mcprm->_case = GameMenuCreateParam::CASE_DEBUG;
		mcprm->_owner = this;
		if(!menu->create(MENU_MENU_DEBUG_XML, ftmv, pf, mcprm))
		{
			delete menu;
			delete ftmv;
			delete pf;
			delete mcprm;
			ASSERT(false);
			return false;
		}
		_menu = menu;
	#endif

	// シーングラフ及びリソースプールにモデルビュー標準カメラを追加
	sourcerer::SorSgm* sgm = (sourcerer::SorSgm*)EtkBody::getBody()->getSceneGraphManager();
	CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(sourcerer::SorSgm::EK_CAM);
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
			_camMvRef = mvcam;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void DimzTest::destroy()
{
	// カメラを破棄
	sourcerer::SorSgm* sgm = (sourcerer::SorSgm*)EtkBody::getBody()->getSceneGraphManager();
	if(_camMvRef != 0L)
	{
		sgm->removeNode(_camMvRef);
		_camMvRef = 0L;
	}

	#if 0
		// メニュー破棄
		if(_menu != 0L)
		{
			MenuFuncTable* ftmv = _menu->getFuncTable();
			MenuPanelFactory* pf = _menu->getPanelFactory();
			GameMenuCreateParam* mcprm = (GameMenuCreateParam*)_menu->getCreateParam();
			_menu->destroy();
			delete _menu;
			delete ftmv;
			delete pf;
			delete mcprm;
			_menu = 0L;
		}
	#endif

	// パフォーマンスメーター破棄
	if(_pmeter != 0L)
	{
		delete _pmeter;
		_pmeter = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御前処理
**//*---------------------------------------------------------------------*/
void DimzTest::prepareExec()
{
	if(_pmeter != 0L)	{	_pmeter->startFrame();	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void DimzTest::exec(ExecRes* res, const ExecCtx* ec)
{
	EtkExecCtx* eec = (EtkExecCtx*)ec;
	TouchPanel* ui = eec->getTouchPanel(); 

//	if(_subtest != 0L)	{	_subtest->exec(res, ec);	}

	// デバッグメニュー制御
	#if 0
		ExecRes resMenu;
		_menu->exec(&resMenu, ec);
		if(!resMenu.isDone())
		{
			PointS32 ptTap;
			if(ui->isClickRelease(&ptTap))
			{
				PointF ptfTap((f32)ptTap.x(), (f32)ptTap.y());
				if(_rectTmenu->isPointIn(&ptfTap))
				{
					if(!_menu->isShow())
					{
						// メニュー表示
						_menu->showMenu(0L);

						// SE
						Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_CSR_MOVE, false);
					}
				}
			}
		}
	#endif

	// デモ
	#if 1
		if((eec->getBodyExecCount() & 7) == 0)
		{
			DimzParticleSys* ptclsys = (DimzParticleSys*)EtkBody::getBody()->getParticleSys();
			Vector3F pos(::rand() / (f32)RAND_MAX * 100.0f, ::rand() / (f32)RAND_MAX * 100.0f, ::rand() / (f32)RAND_MAX * 100.0f);
			ColorU8 col((u8)(::rand() / (f32)RAND_MAX * 255.0f), (u8)(::rand() / (f32)RAND_MAX * 255.0f), (u8)(::rand() / (f32)RAND_MAX * 255.0f), 255);
			ptclsys->appearParticle(307, &pos, &col, 0);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void DimzTest::render(const RenderCtx* rc)
{
//	if(_subtest != 0L)	{	_subtest->render(rc);	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void DimzTest::draw(const RenderCtx* rc)
{
	Renderer* rdr = rc->getRenderer();

	#if 0
		if(_menu != 0L)
		{
			_menu->draw(rc);
			if(!_menu->isShow() && TFW_IS_FLAG(getTestFlags(), F_SHOW_DBGMENUST))
			{
				ColorU8 col1, col2, col3, col4;
				RendererUtils::renderColor2dRect(rdr, _rectTmenu, ColorU8::setout(&col1, 255, 255, 255, 63), ColorU8::setout(&col2, 204, 255, 255, 63), ColorU8::setout(&col3, 200, 212, 212, 63), ColorU8::setout(&col4, 180, 202, 202, 63));
			}
		}
	#endif

//	if(_subtest != 0L)	{	_subtest->draw(rc);	}

	if(TFW_IS_FLAG(getTestFlags(), F_SHOW_PMETER))
	{
		if(_pmeter != 0L)	{	_pmeter->render(rc);	}
	}
}

////////////////////////////////////////////////////////////////////////////

DIMZ_END_NS