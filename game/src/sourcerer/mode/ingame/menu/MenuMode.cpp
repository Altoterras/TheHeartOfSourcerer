/***********************************************************************//**
 *	MenuMode.cpp
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
#include "MenuMode.h"

// Friends
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/GameTaskModDef.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/GameFontSet.h"
#include "../../../gui/menu/materialized/MenuFuncTable_Main.h"
#include "../../../gui/menu/GameMenuCreateParam.h"
#include "../../../gui/menu/GameMenuShowParam.h"
#include "../../../gui/menu/GameMenuPanelFactory.h"
#include "../../../gui/panel/HeaderPanel.h"
#include "../../../gui/window/implement/PcAbilityWindow.h"
#include "../../../gui/window/implement/PcCharStatWindow.h"
#include "../../../gui/window/implement/PcEquipWindow.h"
#include "../../../gui/window/implement/ItemListWindow.h"
#include "../../../gui/window/implement/OptionsWindow.h"
#include "../../../gui/window/implement/QuestListWindow.h"
#include "../../../gui/window/implement/QuestManageWindow.h"
#include "../../../gui/window/implement/SourceListWindow.h"
#include "../../../gui/window/implement/SaveLoadWindow.h"

// External
#include "../../../../tfw/framemod/FedTaskSys.h"
#include "../../../../tfw/smenu/Menu.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// メニュー項目
#define MTNODEID_QUEST		1106

// GUI タスクシステムの処理順、兼 ID
#define GTSORDID_MENU		(GameTaskModDef::MOD__2D_ + 1)
#define GTSORDID_CHILD		(GameTaskModDef::MOD__2D_ + 2)
#define GTSORDID_HDPNL		(GameTaskModDef::MOD__2D_ + 3)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MenuMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	メニューでソースが選択された
**//*---------------------------------------------------------------------*/
void MenuMode::onCallMenuSource()
{
	// ソースリストウインドウを表示
	SourceListWindow* wndSourceList = Game::getGame()->getGui()->getMagicListWindow();
	wndSourceList->setCase(SourceListWindow::SLWCASE_MENU);
	wndSourceList->showWindow(true);

	_smode = SMODE_SOURCE;
}

/*---------------------------------------------------------------------*//**
	メニューでアイテムが選択された
**//*---------------------------------------------------------------------*/
void MenuMode::onCallMenuItem()
{
	// アイテムリストウインドウを表示
	ItemListWindow* wndItemList = Game::getGame()->getGui()->getItemListWindow();
	wndItemList->setCase(ItemListWindow::ILWCASE_MENU, 0);
	wndItemList->showWindow(true);

	_smode = SMODE_ITEM;
}

/*---------------------------------------------------------------------*//**
	メニューでステータスが選択された
**//*---------------------------------------------------------------------*/
void MenuMode::onCallMenuStatus()
{
	// キャラクタステータスウインドウを表示
	appearChidWindow(CWND_CHARSTAT);

	_smode = SMODE_STATUS;
}

/*---------------------------------------------------------------------*//**
	メニューでアビリティが選択された
**//*---------------------------------------------------------------------*/
void MenuMode::onCallMenuAbility()
{
	// アビリティウインドウを表示
	appearChidWindow(CWND_ABILITY);

	_smode = SMODE_ABILITY;
}

/*---------------------------------------------------------------------*//**
	メニューで装備が選択された
**//*---------------------------------------------------------------------*/
void MenuMode::onCallMenuEquip()
{
	// 装備ウインドウを表示
	appearChidWindow(CWND_EQUIP);

	_smode = SMODE_EQUIP;
}

/*---------------------------------------------------------------------*//**
	メニューでクエストが選択された
**//*---------------------------------------------------------------------*/
void MenuMode::onCallMenuQuest()
{
	// クエストウインドウを表示
	appearChidWindow(CWND_QUEST);

	_smode = SMODE_QUEST;
}

/*---------------------------------------------------------------------*//**
	メニューでセーブが選択された
**//*---------------------------------------------------------------------*/
void MenuMode::onCallMenuSave()
{
	// セーブウインドウを表示
	appearChidWindow(CWND_SAVE);

	_smode = SMODE_SAVE;
}

/*---------------------------------------------------------------------*//**
	メニューでオプションが選択された
**//*---------------------------------------------------------------------*/
void MenuMode::onCallMenuOptions()
{
	// オプションウインドウを表示
	appearChidWindow(CWND_OPTIONS);

	_smode = SMODE_OPTIONS;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MenuMode::MenuMode()
	: _smode(SMODE_TOP)
	, _menu(0L)
	, _showprm(0L)
	, _hpnlHbar(0L)
{
	for(int i = 0; i < NUM_CWND; i++)
	{
		_cwnd[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MenuMode::~MenuMode()
{
	ASSERT(_menu == 0L);
	ASSERT(_showprm == 0L);
	ASSERT(_hpnlHbar == 0L);
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_CWND; i++)
		{
			ASSERT(_cwnd[i] == 0L);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MenuMode::create()
{
	// 自身の作成
	if(!GameModeBase::create(GameMode::MODE_TEMPLATE__START_, GameMode::MODE_TEMPLATE__END_, 0L))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool MenuMode::notifyBegin(void* objParamRef)
{
	Game::getGame()->setSkip3d(true);	// 3D 処理を止める

	// メニュー表示パラメータの作成
	_showprm = new GameMenuShowParam();

	// メニューの作成
	MenuFuncTable* ftmv = new MenuFuncTable_Main();
	MenuPanelFactory* pf = new GameMenuPanelFactory();
	GameMenuCreateParam* mcprm = new GameMenuCreateParam();
	mcprm->_case = GameMenuCreateParam::CASE_INGAME_MAIN;
	mcprm->_owner = this;
	_menu = new Menu();
	if(!_menu->create(FILEID_CONV_LANG(MENU_MENU_INGAMEMAIN_EN_XML), ftmv, pf, mcprm))
	{	
		ASSERT(false);
		return false;
	}
	// メニュー項目の設定
	#if !ENABLE_QUEST
		MenuTreeNode* mtnode = _menu->findMenuTreeNode(MTNODEID_QUEST);
		if(mtnode != 0L)	{	mtnode->setDisabled(true);	}	// 無効に
	#endif

	// ヘッダーパネル作成
	HeaderPanel* hpnlHbar = new HeaderPanel();
	GameGui* gui = Game::getGame()->getGui();
	if(!hpnlHbar->create(
		gui->getFontSet()->getFont(GameFontSet::JP),
		gui->getGuiTexture(),
		GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU),
		GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_BACK),
		0L, 0L, 0	))
	{
		delete hpnlHbar;
		ASSERT(false);
		return false;
	}
	_hpnlHbar = hpnlHbar;

	// メニューを表示
	_showprm->_xBase = 0;
	_showprm->_yBase = 56;
	_showprm->_ownerRef = this;
	_menu->showMenu(_showprm);

	// GUI タスクにメニューとヘッダーパネルを登録
	FedTaskSys* tasksys = Game::getGame()->getTaskSys(); ASSERT(tasksys != 0L);
	if(!tasksys->registerExecModule(_menu, GameTaskModDef::calcExecOrder(GTSORDID_MENU), GTSORDID_MENU, 0))					{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_menu, GameTaskModDef::calcDrawOrder(GTSORDID_MENU), false, GTSORDID_MENU, 0))			{	ASSERT(false);	}
	if(!tasksys->registerExecModule(_hpnlHbar, GameTaskModDef::calcExecOrder(GTSORDID_HDPNL), GTSORDID_HDPNL, 0))			{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_hpnlHbar, GameTaskModDef::calcDrawOrder(GTSORDID_HDPNL), false, GTSORDID_HDPNL, 0))	{	ASSERT(false);	}

	// 変数初期化
	_smode = SMODE_TOP;

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void MenuMode::notifyEnd(void* objParamRef)
{
	// GUI タスクからメニューとヘッダーパネルを削除
	FedTaskSys* tasksys = Game::getGame()->getTaskSys(); ASSERT(tasksys != 0L);
	if(!tasksys->removeModule(GTSORDID_HDPNL))	{	ASSERT(false);	}
	if(!tasksys->removeModule(GTSORDID_MENU))	{	ASSERT(false);	}

	// 子ウインドウを削除
	for(int i = 0; i < NUM_CWND; i++)
	{
		disappearChildWindow((ChildWnd)i);
	}
	switch(_smode)
	{
	case SMODE_SOURCE:
		{
			SourceListWindow* wndMlist = Game::getGame()->getGui()->getMagicListWindow(); ASSERT(wndMlist != 0L);
			if(wndMlist->isShowWindow())	{	wndMlist->showWindow(false);	}	// ソースリストウインドウを閉じる
		}
		break;
	case SMODE_ITEM:
		{
			ItemListWindow* wndIlist = Game::getGame()->getGui()->getItemListWindow(); ASSERT(wndIlist != 0L);
			if(wndIlist->isShowWindow())	{	wndIlist->showWindow(false);	}	// アイテムリストウインドウを閉じる
		}
		break;
	}

	// ヘッダーパネル削除
	if(_hpnlHbar != 0L)
	{
		_hpnlHbar->destroy();
		delete _hpnlHbar;
		_hpnlHbar = 0L;
	}

	// メニュー表示パラメータの削除
	delete _showprm;
	_showprm = 0L;

	// メニューの削除
	if(_menu != 0L)
	{
		MenuFuncTable* ftmv = _menu->getFuncTable();
		MenuPanelFactory* pf = _menu->getPanelFactory();
		GameMenuCreateParam* mcprm = (GameMenuCreateParam*)_menu->getCreateParam();
		_menu->destroy();
		delete ftmv;
		delete pf;
		delete mcprm;
		delete _menu;
		_menu = 0L;
	}

	Game::getGame()->setSkip3d(false);	// 3D 処理を再開
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void MenuMode::exec(ExecRes* res, const ExecCtx* ec)
{
	GameModeBase::exec(res, ec);

	switch(_smode)
	{
	case SMODE_TOP:
		{
			// メニュー制御
			if(_menu->isDone())
			{
///				Mode* modeParent = getParentMode();
///				modeParent->reserveMode(modeParent->getPrevMode());	// モードを戻す
///				getParentMode()->setPrevMode();
				Game::getGame()->getGameMode()->changePrevEndMode(GameMode::MODE_NULL);
			}

			// ヘッダーパネル制御
			const HeaderPanel::Res* cres = _hpnlHbar->getResult();
			if(cres->isClickLeftButton())	// 左ボタンクリック
			{
				_hpnlHbar->resetResult();
///				Mode* modeParent = getParentMode();
///				modeParent->reserveMode(modeParent->getPrevMode());	// モードを戻す
///				getParentMode()->setPrevMode();
				Game::getGame()->getGameMode()->changePrevEndMode(GameMode::MODE_NULL);
			}
		}
		break;
	case SMODE_SOURCE:
		{
			SourceListWindow* wndMlist = Game::getGame()->getGui()->getMagicListWindow(); ASSERT(wndMlist != 0L);
			if(wndMlist->isDone())	// 完了検知
			{
				// ソースリストウインドウを閉じる
				wndMlist->showWindow(false);

				// トップメニューへ
				_smode = SMODE_TOP;
				_menu->showMenu(_showprm);
			}
		}
		break;
	case SMODE_ITEM:
		{
			ItemListWindow* wndIlist = Game::getGame()->getGui()->getItemListWindow(); ASSERT(wndIlist != 0L);
			if(wndIlist->isDone())	// 完了検知
			{
				// アイテムリストウインドウを閉じる
				wndIlist->showWindow(false);

				// トップメニューへ
				_smode = SMODE_TOP;
				_menu->showMenu(_showprm);
			}
		}
		break;
	case SMODE_STATUS:
	case SMODE_ABILITY:
	case SMODE_EQUIP:
	case SMODE_QUEST:
	case SMODE_SAVE:
	case SMODE_OPTIONS:
		{
			// ヘッダーパネル制御
			const HeaderPanel::Res* cres = _hpnlHbar->getResult();
			if(cres->isClickLeftButton())	// 左ボタンクリック
			{
				_hpnlHbar->resetResult();

				// 子ウインドウの削除
				ChildWnd cwnd = NUM_CWND;
				switch(_smode)
				{
				case SMODE_STATUS:		cwnd = CWND_CHARSTAT;	break;
				case SMODE_ABILITY:		cwnd = CWND_ABILITY;	break;
				case SMODE_EQUIP:		cwnd = CWND_EQUIP;		break;
				case SMODE_QUEST:		cwnd = CWND_QUEST;		break;
				case SMODE_SAVE:		cwnd = CWND_SAVE;		break;
				case SMODE_OPTIONS:		cwnd = CWND_OPTIONS;	break;
				}
				if(cwnd != NUM_CWND)
				{
					ASSERT(_cwnd[cwnd] != 0L);
					disappearChildWindow(cwnd);
				}

				// トップメニューへ
				_smode = SMODE_TOP;
				_menu->showMenu(_showprm);
			}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void MenuMode::draw(const RenderCtx* rc)
{
	GameModeBase::draw(rc);
}

/*---------------------------------------------------------------------*//**
	子ウインドウを表示する
**//*---------------------------------------------------------------------*/
bool MenuMode::appearChidWindow(ChildWnd cwnd)
{
	disappearChildWindow(cwnd);	// 念のため閉じる

	GameGui* gui = Game::getGame()->getGui();
	const VcString* title = 0L;

	// 子ウインドウを作成
	ASSERT(_cwnd[cwnd] == 0L);
	switch(cwnd)
	{
	case CWND_ABILITY:
		_cwnd[cwnd] = new PcAbilityWindow();
		((PcAbilityWindow*)_cwnd[cwnd])->create(gui->getFontSet()->getFont(GameFontSet::JP), gui->getGuiTexture());
		title = GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_ABILITY);
		break;
	case CWND_CHARSTAT:
		_cwnd[cwnd] = new PcCharStatWindow();
		((PcCharStatWindow*)_cwnd[cwnd])->create(gui->getFontSet()->getFont(GameFontSet::JP), gui->getGuiTexture());
		title = GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_CHAR_STAT);
		break;
	case CWND_EQUIP:
		_cwnd[cwnd] = new PcEquipWindow();
		((PcEquipWindow*)_cwnd[cwnd])->create(gui->getFontSet()->getFont(GameFontSet::JP), gui->getGuiTexture());
		title = GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_EQUIP);
		break;
	case CWND_QUEST:
		_cwnd[cwnd] = new QuestManageWindow();
		((QuestManageWindow*)_cwnd[cwnd])->create(gui->getFontSet()->getFont(GameFontSet::JP), gui->getGuiTexture());
		title = GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_QUEST);
		break;
	case CWND_SAVE:
		_cwnd[cwnd] = new SaveLoadWindow();
		((SaveLoadWindow*)_cwnd[cwnd])->create(gui->getFontSet()->getFont(GameFontSet::JP), gui->getGuiTexture(), SaveLoadWindow::PAGE_SAVE, false, 0);
		title = GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_SAVE);
		break;
	case CWND_OPTIONS:
		_cwnd[cwnd] = new OptionsWindow();
		((OptionsWindow*)_cwnd[cwnd])->create(gui->getFontSet()->getFont(GameFontSet::JP), gui->getGuiTexture());
		title = GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU_OPTIONS);
		break;
	}
	// 子ウインドウを表示
	_cwnd[cwnd]->showWindow(true);

	// GUI タスクにウインドウを登録
	FedTaskSys* tasksys = Game::getGame()->getTaskSys(); ASSERT(tasksys != 0L);
	ASSERT(!tasksys->isExistModule(GTSORDID_CHILD));
	if(!tasksys->registerExecModule(_cwnd[cwnd], GameTaskModDef::calcExecOrder(GTSORDID_CHILD), GTSORDID_CHILD, EtkTaskModDef::MGF_WINDOW))			{ ASSERT(false); }
	if (!tasksys->registerDrawModule(_cwnd[cwnd], GameTaskModDef::calcDrawOrder(GTSORDID_CHILD), false, GTSORDID_CHILD, EtkTaskModDef::MGF_WINDOW))	{ ASSERT(false); }

	// ヘッダーパネルのテキストを戻す
	_hpnlHbar->changeGuiString(title, 0L, 0L, 0L);

	return true;
}

/*---------------------------------------------------------------------*//**
	子ウインドウを閉じる
**//*---------------------------------------------------------------------*/
void MenuMode::disappearChildWindow(ChildWnd cwnd)
{
	if(_cwnd[cwnd] == 0L)	{	return;	}

	// 子ウインドウを非表示に
	_cwnd[cwnd]->showWindow(false);

	// GUI タスクからウインドウを削除
	FedTaskSys* tasksys = Game::getGame()->getTaskSys(); ASSERT(tasksys != 0L);
	if(!tasksys->removeModule(GTSORDID_CHILD))	{	ASSERT(false);	}
	ASSERT(!tasksys->isExistModule(GTSORDID_CHILD));

	// キャラクタステータスウインドウを削除
	_cwnd[cwnd]->destroy();
	delete _cwnd[cwnd];
	_cwnd[cwnd] = 0L;

	// ヘッダーパネルのテキストを戻す
	_hpnlHbar->changeGuiString(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU), 0L, 0L, 0L);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
