/***********************************************************************//**
 *	BasicModelTest.cpp
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
#include "BasicModelTest.h"

// Friends

// External
#include "../../etk/body/EtkExecCtx.h"
#include "../../tfw/common/PerformanceMeter.h"
#include "../../tfw/smenu/Menu.h"
#include "../../tfw/smenu/MenuTreeNode.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

BASICMODEL_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BasicModelTest メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ファンクションキーによるテスト実行
**//*---------------------------------------------------------------------*/
void BasicModelTest::onKeydownFunctionKey(s32 funckey)
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
BasicModelTest::BasicModelTest()
	: _pmeter(0L)
	, _menu(0L)
{
	setTestFlags(F_SHOW_PMETER | F_SHOW_DBGMENUST, true);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BasicModelTest::~BasicModelTest()
{
	ASSERT(_menu == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool BasicModelTest::create(f32 spfRender, f32 spfLogic)
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

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void BasicModelTest::destroy()
{
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
void BasicModelTest::prepareExec()
{
	if(_pmeter != 0L)	{	_pmeter->startFrame();	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void BasicModelTest::exec(ExecRes* res, const ExecCtx* ec)
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
	#if 0
		if((eec->getBodyExecCount() & 7) == 0)
		{
			BasicModelParticleSys* ptclsys = (BasicModelParticleSys*)EtkBody::getBody()->getParticleSys();
			Vector3F pos(::rand() / (f32)RAND_MAX * 100.0f, ::rand() / (f32)RAND_MAX * 100.0f, ::rand() / (f32)RAND_MAX * 100.0f);
			ColorU8 col((u8)(::rand() / (f32)RAND_MAX * 255.0f), (u8)(::rand() / (f32)RAND_MAX * 255.0f), (u8)(::rand() / (f32)RAND_MAX * 255.0f), 255);
			ptclsys->appearParticle(307, &pos, &col, 0);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void BasicModelTest::render(const RenderCtx* rc)
{
//	if(_subtest != 0L)	{	_subtest->render(rc);	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void BasicModelTest::draw(const RenderCtx* rc)
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

BASICMODEL_END_NS