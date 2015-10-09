/***********************************************************************//**
 *	FocusMenu.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FocusMenu.h"

// Friends
#include "menu/materialized/MenuFuncTable_Focus.h"
#include "menu/GameMenuCreateParam.h"
#include "menu/GameMenuShowParam.h"
#include "menu/GameMenuPanelFactory.h"
#include "window/menu/MenuWindow.h"
#include "../common/CalcUtils.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../mode/GameMode.h"
#include "../file/GameFileIdDef.h"
#include "../unit/Unit.h"

// External

// Library
#include "../../tfw/lib/Point.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/ui/TouchPanel.h"
#include "../../tfw/smenu/Menu.h"
#include "../../tfw/smenu/MenuTreeNode.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// FocusMenu メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	メニューを表示する
**//*---------------------------------------------------------------------*/
void FocusMenu::showMenu(MenuKind mk, const PointF32* ptBase, Unit* unitRef, bool isFollow)
{
	for(int i = 0; i < NUM_MENU; i++)
	{
		if(_menu[i] == 0L)	{	continue;	}
		if(_menu[i]->isShow())
		{
			if(i != mk)
			{
				_menu[i]->closeMenu();
			}
		}
		else
		{
			if(i == mk)
			{
				// メニューを開く
				_showprm->_xBase = ptBase->x();
				_showprm->_yBase = ptBase->y();
				_showprm->_unitRef = unitRef;
				_menu[i]->showMenu(_showprm);
				_unitFllowRef[i] = isFollow ? unitRef : 0L;

				// メニューの位置調整
				adjustMenuPos((MenuKind)i);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	全てのメニューを隠す
**//*---------------------------------------------------------------------*/
void FocusMenu::hideAllMenu()
{
	for(int i = 0; i < NUM_MENU; i++)
	{
		if(_menu[i] == 0L)	{	continue;	}
		if(_menu[i]->isShow())
		{
			_menu[i]->closeMenu();
		}
	}
}

/*---------------------------------------------------------------------*//**
	メニュー項目の有効／無効化
**//*---------------------------------------------------------------------*/
bool FocusMenu::setMenuElementEnable(MenuKind mk, int mtnodeid, bool isEnable)
{
	Menu* menu = _menu[mk];
	MenuTreeNode* mtnode = menu->findMenuTreeNode(mtnodeid);
	if(mtnode == 0L)	{	return false;	}
	mtnode->setDisabled(!isEnable);
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FocusMenu::FocusMenu()
	: _isEnableOut(true)
	, _isEnableSelf(false)
	, _showprm(0L)
{
	for(int i = 0; i < NUM_MENU; i++)
	{
		_menu[i] = 0L;
		_unitFllowRef[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FocusMenu::~FocusMenu()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_MENU; i++)
		{
			ASSERT(_menu[i] == 0L);
		}
	#endif
	ASSERT(_showprm == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool FocusMenu::create()
{
	// メニュー表示パラメータの作成
	_showprm = new GameMenuShowParam();

	// ここではメニューの作成しない（タイトルでの言語設定を反映させるため）
	// notifyEnterGame() で作成する

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void FocusMenu::destroy()
{
	_isEnableSelf = false;

	// メニュー破棄
	destroyMenu();

	// メニュー表示パラメータの削除
	delete _showprm;
	_showprm = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void FocusMenu::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!_isEnableOut)	{	return;	}	// 外部からの無効化

	GameExecCtx* gec = (GameExecCtx*)ec;
	_isEnableSelf = !gec->isOpenedLockGui();
	if(_isEnableSelf)
	{
		// メニューのフレーム処理
		for(int i = 0; i < NUM_MENU; i++)
		{	
			if(_menu[i] == 0L)		{	continue;	}
			if(!_menu[i]->isShow())	{	continue;	}
			ExecRes resMenu;
			_menu[i]->exec(&resMenu, ec);
			if(resMenu.isDone())
			{
			}
			else
			{
				adjustMenuPos((MenuKind)i);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void FocusMenu::draw(const RenderCtx* rc)
{
	if(!_isEnableOut || !_isEnableSelf)	{	return;	}	// 無効化中

	// メニューの描画
	for(int i = 0; i < NUM_MENU; i++)
	{
		if(_menu[i] == 0L)	{	continue;	}
		_menu[i]->draw(rc);
	}
}

/*---------------------------------------------------------------------*//**
	ゲーム開始通知
**//*---------------------------------------------------------------------*/
void FocusMenu::notifyEnterGame()
{
	// メニュー再作成
	destroyMenu();
	createMenu();
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void FocusMenu::notifyChangedMap()
{
	// メニューを閉じ、値をリセット
	for(int i = 0; i < NUM_MENU; i++)
	{
		if(_menu[i]->isShow())
		{
			_menu[i]->closeMenu();
		}
		_unitFllowRef[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	メニュー作成
**//*---------------------------------------------------------------------*/
bool FocusMenu::createMenu()
{
	// メニューの作成
	MenuFuncTable* ftmv = new MenuFuncTable_Focus();
	MenuPanelFactory* pf = new GameMenuPanelFactory();
	GameMenuCreateParam* mcprm = new GameMenuCreateParam();
	mcprm->_case = GameMenuCreateParam::CASE_INGAME_FOCUS;
	mcprm->_owner = this;
	_menu[MENU_PC1] = new Menu();
	if(!_menu[MENU_PC1]->create(FILEID_CONV_LANG(MENU_MENU_PC1_EN_XML), ftmv, pf, mcprm))					{	ASSERT(false);	return false;	}
	_menu[MENU_PC2] = new Menu();
	if(!_menu[MENU_PC2]->create(FILEID_CONV_LANG(MENU_MENU_PC2_EN_XML), ftmv, pf, mcprm))					{	ASSERT(false);	return false;	}
	_menu[MENU_PC2_WEP] = new Menu();
	if(!_menu[MENU_PC2_WEP]->create(FILEID_CONV_LANG(MENU_MENU_EQUIPPEDITEM_EN_XML), ftmv, pf, mcprm))		{ ASSERT(false);	return false; }
	_menu[MENU_PC3] = new Menu();
	if(!_menu[MENU_PC3]->create(FILEID_CONV_LANG(MENU_MENU_PC3_EN_XML), ftmv, pf, mcprm))					{	ASSERT(false);	return false;	}
	_menu[MENU_ENEMY] = new Menu();
	if(!_menu[MENU_ENEMY]->create(FILEID_CONV_LANG(MENU_MENU_FOCUSENEMY_EN_XML), ftmv, pf, mcprm))			{	ASSERT(false);	return false;	}
	_menu[MENU_EXTRA] = new Menu();
	if(!_menu[MENU_EXTRA]->create(FILEID_CONV_LANG(MENU_MENU_FOCUSEXTRA_EN_XML), ftmv, pf, mcprm))			{	ASSERT(false);	return false;	}
	_menu[MENU_MAPOBJ] = new Menu();
	if(!_menu[MENU_MAPOBJ]->create(FILEID_CONV_LANG(MENU_MENU_FOCUSMAPOBJ_EN_XML), ftmv, pf, mcprm))		{	ASSERT(false);	return false;	}
	_menu[MENU_DROPITEM] = new Menu();
	if(!_menu[MENU_DROPITEM]->create(FILEID_CONV_LANG(MENU_MENU_FOCUSDROPITEM_EN_XML), ftmv, pf, mcprm))	{	ASSERT(false);	return false;	}

	_isEnableSelf = true;

	return true;
}

/*---------------------------------------------------------------------*//**
	メニュー破棄
**//*---------------------------------------------------------------------*/
void FocusMenu::destroyMenu()
{
	_isEnableSelf = false;

	// メニューの削除
	for(int i = 0; i < NUM_MENU; i++)
	{
		if(_menu[i] != 0L)
		{
			if(i == (NUM_MENU - 1))	// メニュー付加オブジェ類は共有しているので最後の要素で削除する
			{
				MenuFuncTable* ftmv = _menu[i]->getFuncTable();
				MenuPanelFactory* pf = _menu[i]->getPanelFactory();
				GameMenuCreateParam* mcprm = (GameMenuCreateParam*)_menu[i]->getCreateParam();
				_menu[i]->destroy();
				delete ftmv;
				delete pf;
				delete mcprm;
			}
			else
			{
				_menu[i]->destroy();
			}
			delete _menu[i];
			_menu[i] = 0L;
		}
	}
}

/*---------------------------------------------------------------------*//**
	メニューの位置調整
**//*---------------------------------------------------------------------*/
void FocusMenu::adjustMenuPos(MenuKind mkind)
{
	// フォローなしの場合は必要なし
	if(_unitFllowRef[mkind] == 0)			{	return;	}

	do
	{
		MenuTreeNode* mtnode = _menu[mkind]->getCurrentMenuNode();
		if(mtnode == 0L)						{	break;	}
		MenuWindow* mwnd = (MenuWindow*)mtnode->getPanel();
		if(mwnd == 0L)							{	break;	}
		const RectF32* rectMwnd = mwnd->getRectangle();

		// 対象がいなくなった
		if(!_unitFllowRef[mkind]->isEnable())
		{
			break;	// メニューを閉じる
		}

		// スクリーン座標に変換
		Vector2F vScr;
		CalcUtils::calc3dPosToScreenPos(&vScr, _unitFllowRef[mkind]->getCenterPos());
		f32 x = vScr.x();
		f32 y = vScr.y();

		// ユニット自体が画面外に出たときはメニューを閉じる
		if((x < 0) || (x > Game::getGame()->getLogicalWidth()) || (y < 0) || (y > Game::getGame()->getLogicalHeight()))
		{
			break;	// メニューを閉じる
		}

		// 画面内に収めるように位置修正
		if((x + rectMwnd->w()) > Game::getGame()->getLogicalWidth())	{	x = Game::getGame()->getLogicalWidth() - rectMwnd->w();	}
		else if(x < 0)					{	x = 0;						}
		if((y + rectMwnd->h()) > Game::getGame()->getLogicalHeight())	{	y = Game::getGame()->getLogicalHeight() - rectMwnd->h();	}
		else if(y < 0)					{	y = 0;						}

		// 修正を反映
		if((x != rectMwnd->x()) || (y != rectMwnd->y()))
		{
			PointF32 pt(x, y);
			mwnd->move(&pt);
		}

		return;	// 正常終了
	}
	while(false);

	// なんらかエラーが発生したのでメニューを閉じる
	_menu[mkind]->closeMenu();
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
