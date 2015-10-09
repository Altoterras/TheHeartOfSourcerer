/***********************************************************************//**
 *	ListWindowBase.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/03.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ListWindowBase.h"

// Friends
#include "../panel/HeaderPanel.h"
#include "../menu/GameMenuCreateParam.h"
#include "../menu/GameMenuShowParam.h"
#include "../menu/GameMenuPanelFactory.h"
#include "../window/implement/GameSysMsgWindow.h"
#include "../window/menu/MenuWindow.h"
#include "../../body/Game.h"
#include "../../body/GameExecCtx.h"
#include "../../body/GameFixedStrTbl.h"
#include "../../gui/GameGui.h"

// External

// Library
#include "../../../tfw/collection/List.h"
#include "../../../tfw/lib/Rect.h"
#include "../../../tfw/lib/TypeUtils.h"
#include "../../../tfw/g2d/font/Font.h"
#include "../../../tfw/gcmn/Texture.h"
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/gcmn/RendererUtils.h"
#include "../../../tfw/ui/TouchPanel.h"
#include "../../../tfw/smenu/Menu.h"
#include "../../../tfw/smenu/MenuTreeNode.h"
#include "../../../tfw/string/StringUtils.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 表示位置
#define THIS_H				(320)

#define PARAM_LIST			"List_"

#define MSGWND_CTRL_HINT	675431

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ListWindowBase メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	フォーカスインデックスを得る
**//*---------------------------------------------------------------------*/
s32 ListWindowBase::getFocusedIndex() const
{
	MenuTreeNode* mtnode = _menuList->getCurrentMenuNode();
	if(mtnode == 0L)	{	return -1;	}
	MenuWindow* mwnd = (MenuWindow*)mtnode->getPanel();
	if(mwnd == 0L)		{	return -1;	}
	return mwnd->getFocusedElementIndex();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	モードを設定する
**//*---------------------------------------------------------------------*/
void ListWindowBase::setMode(u8 lwbmode, const VcString* title)
{
	_lwmode = lwbmode;
	_hpnlHbar->changeGuiString(title, 0L, 0L, 0L);

	if(_idxSelected != -1)	// 現時点で選択されている場合
	{
		switch(_lwmode)
		{
		case LWMODE_SORT:
			selectSortFirst();	// 最初の要素の選択へ
			break;
		case LWMODE_CLONE:
			confirmClone();		// 要素編集へ
			break;
		case LWMODE_DELETE:
			confirmDelete();	// 削除確認へ
			break;
		case LWMODE_EDIT:
			confirmEdit();		// 編集確認へ
			break;
		}
	}

	if((_lwmode != LWMODE_SORT) && (_idxSortSelected != -1))
	{
		cancelSort(-1);	// ソートキャンセル
	}
}

/*---------------------------------------------------------------------*//**
	リスト要素を選択する
**//*---------------------------------------------------------------------*/
void ListWindowBase::setSelectedIndex(s32 index)
{
	_idxSelected = index;
	_res.setSelectedIndex(_idxSelected);		// 結果値の設定

	if(_menuList != 0L)
	{
		MenuTreeNode* mtnode = _menuList->getCurrentMenuNode();
		if(mtnode != 0L)
		{
			MenuWindow* mwnd = (MenuWindow*)mtnode->getPanel();
			if(mwnd != 0L)
			{
				mwnd->setFocusedElementIndex(index);
			}
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ListWindowBase::ListWindowBase()
	: _listLitem(0L)
	, _strEmpty(0L)
	, _fileidSubMenu(0)
	, _paramGameMenuCreate(0)
	, _fontRef(0L)
	, _texRef(0L)
	, _lwmode(LWMODE_NULL)
	, _hpnlHbar(0L)
	, _menuList(0L)
	, _menuSub(0L)
	, _msprmList(0L)
	, _msprmSub(0L)
	, _isDone(false)
	, _isDoneReserved(false)
	, _isSelfClose(false)
	, _idxSelected(-1)
	, _idxSortSelected(-1)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ListWindowBase::~ListWindowBase()
{
	ASSERT(_listLitem == 0L);
	ASSERT(_strEmpty == 0L);
	ASSERT(_hpnlHbar == 0L);
	ASSERT(_menuList == 0L);
	ASSERT(_menuSub == 0L);
	ASSERT(_msprmList == 0L);
	ASSERT(_msprmSub == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ListWindowBase::create(Font* fontRef, Texture* texRef, const VcString* titleDefault)
{
	// ウインドウの作成
	RectF32 rect(0, Game::getGame()->getLogicalHeight() - THIS_H, Game::getGame()->getLogicalWidth(), THIS_H);
	if(!Window::create(&rect))
	{
		return false;
	}

	// リストアイテムリストの作成
	_listLitem = new List<Litem*>(true);

	// 空アイテム時の文字列作成
	_strEmpty = new VcString();

	// ヘッダーパネル作成
	HeaderPanel* hpnlHbar = new HeaderPanel();
	if(!hpnlHbar->create(fontRef, texRef, titleDefault, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_BACK), 0L, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU), 0))
	{
		delete hpnlHbar;
		ASSERT(false);
		return false;
	}
	_hpnlHbar = hpnlHbar;

	// デフォルトのモード設定
	if(_lwmode == LWMODE_NULL)	{	setMode(LWMODE_SHOW, TypeUtils::getEmptyString());	}

	// 値の保存
	_fontRef = fontRef;
	_texRef = texRef;
	
	// その他の変数初期化
	_strEmpty->set("-");

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ListWindowBase::destroy()
{
	// メニュー破棄
	destroyMenu();

	// ヘッダーパネル削除
	if(_hpnlHbar != 0L)
	{
		_hpnlHbar->destroy();
		delete _hpnlHbar;
		_hpnlHbar = 0L;
	}

	// 空アイテム時の文字列の削除
	delete _strEmpty;
	_strEmpty = 0L;

	// リストアイテムリストの削除
	delete _listLitem;
	_listLitem = 0L;

	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	メニュー作成
**//*---------------------------------------------------------------------*/
bool ListWindowBase::createMenu()
{
	MenuTreeNode** mtnarrParent = 0L;
	MenuTreeNode** mtnarrChild = 0L;
	Menu* menu = 0L;
	MenuFuncTable* mftbl = 0L;
	MenuPanelFactory* pfact = 0L;
	GameMenuCreateParam* mcprmList = 0L;
	GameMenuCreateParam* mcprmSub = 0L;
	GameMenuShowParam* msprmList = 0L;
	GameMenuShowParam* msprmSub = 0L;
	Panel* panelFunc;
	int cnt;
	
	// アイテムの数を得る
	s32 numItem = _listLitem->getCount();

	// メニュー要素の作成
	mtnarrParent = new MenuTreeNode*[numItem];
	mtnarrChild = new MenuTreeNode*[numItem];
	if((mtnarrParent == 0L) || (mtnarrChild == 0L))	{	goto FAILED;	}
	cnt = 0;
	for(ListIterator<Litem*> it = _listLitem->iterator(); it.has(); it.next())
	{
		const Litem* litem = it.object();

		mtnarrParent[cnt] = new MenuTreeNode();

		// アイテムの名称を得る
		const VcString* name;
		if(litem->_name->getLength() > 0)
		{
			name = litem->_name;
		}
		else
		{
			name = _strEmpty;
		}

		// ノード作成
		if(!mtnarrParent[cnt]->create(name, false, false, false, 0, reinterpret_cast<u64>(litem->_obj)))
		{
			goto FAILED;
		}
		mtnarrParent[cnt]->setGeneralParam(reinterpret_cast<u64>(litem->_obj));

		// ノードの有効・無効化
		///mtnarrParent[cnt]->setDisabled(TFW_IS_FLAG(litem->_liflags, Litem::LIF_NOEDIT));

		// 子ノードに値を設定
		mtnarrChild[cnt] = new MenuTreeNode();
		VcString valu = VcString::format(PARAM_LIST"%d", cnt);
		if(!mtnarrChild[cnt]->create(&valu, true, false, false, 0, 0))
		{
			goto FAILED;
		}
		mtnarrChild[cnt]->setValue(&valu);

		cnt++;
	}

	// 機能テーブル
	mftbl = new MenuFuncTable_ListBase();
	if(mftbl == 0L)									{	goto FAILED;	}

	// メニューパネルファクトリ
	pfact = new GameMenuPanelFactory();
	if(pfact == 0L)									{	goto FAILED;	}

	// リストメニュー作成
	{
		// 作成パラメータ
		mcprmList = new GameMenuCreateParam();
		if(mcprmList == 0L)								{	goto FAILED;	}
		mcprmList->_case = (GameMenuCreateParam::Case)_paramGameMenuCreate;
		mcprmList->_owner = this;

		// 表示パラメータ
		msprmList = new GameMenuShowParam();
		if(msprmList == 0L)								{	goto FAILED;	}
		msprmList->_xBase = 0;
		msprmList->_yBase = _hpnlHbar->getRectangle()->y() + _hpnlHbar->getRectangle()->h();
		msprmList->_wMax = Game::getGame()->getLogicalWidth();
		msprmList->_hMax = Game::getGame()->getLogicalHeight() - msprmList->_yBase;
		msprmList->_ownerRef = this;

		// メニュー作成
		_menuList = new Menu();
		if(_menuList == 0L)								{	goto FAILED;	}
		if(!_menuList->create(TypeUtils::getEmptyString(), mtnarrParent, mtnarrChild, numItem, mftbl, pfact, mcprmList))
		{
			delete _menuList;
			_menuList = 0L;
			goto FAILED;
		}
		_msprmList = msprmList;
	}

	// サブメニュー作成
	if(_fileidSubMenu != 0)
	{
		// 作成パラメータ
		GameMenuCreateParam* mcprmSub = new GameMenuCreateParam();
		if(mcprmSub == 0L)								{	goto FAILED;	}
		mcprmSub->_case = GameMenuCreateParam::CASE_INGAME_SUB;
		mcprmSub->_owner = this;

		// 表示パラメータ
		msprmSub = new GameMenuShowParam();
		if(msprmSub == 0L)								{	goto FAILED;	}
		msprmSub->_xBase = Game::getGame()->getLogicalWidth() - 144 - 12;
		msprmSub->_yBase = 12;
		msprmSub->_wMax = Game::getGame()->getLogicalWidth() - msprmSub->_xBase;
		msprmSub->_hMax = Game::getGame()->getLogicalHeight() - msprmSub->_yBase;
		msprmSub->_ownerRef = this;

		_menuSub = new Menu();
		if(!_menuSub->create(_fileidSubMenu, mftbl, pfact, mcprmSub))
		{	
			delete _menuSub;
			_menuSub = 0L;
			ASSERT(false);
			return false;
		}
		_msprmSub = msprmSub;
	}
	// ヘッダーパネルの機能ボタンを有効・無効化
	panelFunc = _hpnlHbar->funcButtonPanel();
	if(panelFunc != 0L)	{	panelFunc->setEnable(_fileidSubMenu != 0);	}	// サブメニューがあるときのみ有効化
	
	return true;

FAILED:
	ASSERT(false);
	if(msprmList != 0L)		{	delete msprmList;		}
	if(msprmSub != 0L)		{	delete msprmSub;		}
	if(mcprmList != 0L)		{	delete mcprmList;		}
	if(mcprmSub != 0L)		{	delete mcprmSub;		}
	if(pfact != 0L)			{	delete pfact;			}
	if(mftbl != 0L)			{	delete mftbl;			}
	if(mtnarrChild != 0L)
	{
		for(s32 i = 0; i < numItem; i++)
		{
			delete mtnarrChild[i];
		}
		delete[] mtnarrChild;
	}
	if(mtnarrParent != 0L)
	{
		for(s32 i = 0; i < numItem; i++)
		{
			delete mtnarrParent[i];
		}
		delete[] mtnarrParent;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	メニュー破棄
**//*---------------------------------------------------------------------*/
void ListWindowBase::destroyMenu()
{
	// サブメニュー削除
	if(_menuSub != 0L)
	{
		GameMenuCreateParam* mcprm = (GameMenuCreateParam*)_menuSub->getCreateParam();
		_menuSub->destroy();
		delete _menuSub;
		_menuSub = 0L;
		delete mcprm;
		delete _msprmSub;
		_msprmSub = 0L;
	}

	// リストメニュー削除
	if(_menuList != 0L)
	{
		MenuFuncTable* mftbl = _menuList->getFuncTable();
		MenuPanelFactory* pfact = _menuList->getPanelFactory();
		GameMenuCreateParam* mcprm = (GameMenuCreateParam*)_menuList->getCreateParam();
		_menuList->destroy();
		delete _menuList;
		_menuList = 0L;
		delete mcprm;
		delete pfact;
		delete mftbl;
		delete _msprmList;
		_msprmList = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ListWindowBase::execContents(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* ui = gec->getTouchPanel();

	// 終了予約がある場合は終了して抜ける
	if(_isDoneReserved)
	{
		if(res != 0L)	{	res->setDone();	}
		_isDone = true;
		return;
	}

	// YES / NO メッセージウインドウ制御
	GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
	if((wndSysMsg != 0L) && wndSysMsg->isShowWindow())
	{
		wndSysMsg->exec(res, ec);
		const GameSysMsgWindow::Result* res = wndSysMsg->getResult();
		if(res->isClickYesButton())
		{
			// モードによる処理
			switch(_lwmode)
			{
			case LWMODE_CLONE:		// 複製モード
				doClone();
				break;
			case LWMODE_DELETE:		// 削除モード
				doDelete();
				break;
			case LWMODE_EDIT:		// 編集モード
				doEdit();
				break;
			}
		}
		if(res->isClickYesButton() || res->isClickNoButton())
		{
			// ウインドウを消す
			wndSysMsg->showWindow(false);	
			// 一般選択解除
			setSelectedIndex(-1);
			// 見るモードに強制移行
			setMode(LWMODE_SHOW, TypeUtils::getEmptyString());
		}
		return;
	}

	// サブメニュー制御
	if((_menuSub != 0L) && _menuSub->isShow())
	{
		ExecRes resMenu;
		_menuSub->exec(&resMenu, ec);
		return;
	}

	// ヘッダーパネル制御
	if(_hpnlHbar != 0L)
	{
		_hpnlHbar->exec(res, ec);
		const HeaderPanel::Res* cres = _hpnlHbar->getResult();
		if(cres->isClickLeftButton())
		{
			// 左ボタンで終了
			if(res != 0L)	{	res->setDone();	}
			_isDone = true;

			ui->setReacted(true);	// 反応済み
		}
		else if(cres->isClickFuncButton())
		{
			// サブメニューがある場合は表示
			if((_menuSub != 0L) && !_menuSub->isShow())	{	_menuSub->showMenu(_msprmSub);	}

			ui->setReacted(true);	// 反応済み
		}
		else if(cres->isTouchBody() || cres->isClickBody())
		{
			ui->setReacted(true);	// 反応済み
		}
		_hpnlHbar->resetResult();
	}

	// リストメニュー制御
	if(_menuList != 0L)
	{
		ExecRes resMenu;
		_menuList->exec(&resMenu, ec);
		if(resMenu.isDone())
		{
			MenuTreeNode* mtnode = _menuList->getCurrentMenuNode();
			if(mtnode != 0L)
			{
				int index = StringUtils::toInteger(mtnode->getName());
				TRACE("%d\n", index);
			}

			// 決定終了
			if(res != 0L)	{	res->setDone();	}
			_isDone = true;
		}
	}

	// 自ら閉じる
	if(_isDone && _isSelfClose)
	{
		showWindow(false);
	}

	// 応答を独占する
	ui->setReacted(true);
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void ListWindowBase::draw(const RenderCtx* rc)
{
	if(!isShowWindow())	{	return;	}

	Renderer* rdr = rc->getRenderer();
	const RectF32* rectBase = getRectangle();

	// 背景描画
	rdr->setSolidColor(0, 0, 0, 63);
	rdr->bindTexture(0L);
	RendererUtils::draw2dRect(rdr, (f32)rectBase->x(), (f32)rectBase->y(), (f32)rectBase->w(), (f32)rectBase->h());

	// リストメニュー描画
	if(_menuList != 0L)	{	_menuList->draw(rc);	}

	// ヘッダーパネル描画
	if(_hpnlHbar != 0L)	{	_hpnlHbar->draw(rc);	}

	// サブメニュー描画
	if(_menuSub != 0L)	{	_menuSub->draw(rc);		}

	Window::draw(rc);
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void ListWindowBase::notifyChangeLanguage()
{
	Window::notifyChangeLanguage();

	if(_hpnlHbar != 0L)
	{
		_hpnlHbar->changeGuiString(
			0L,
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_BACK),
			0L,
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_MENU)	);
	}
}

/*---------------------------------------------------------------------*//**
	メニューノードクリック通知
**//*---------------------------------------------------------------------*/
bool ListWindowBase::onClickMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	// 汎用値解析
	const VcString* valu = mtnodeCall->getValue();
	if(valu != 0L)
	{
		CcString prefixList(PARAM_LIST);
		s32 idxPrf = valu->find(&prefixList, 0);
		if(idxPrf != -1)	// リストがクリックされた
		{
			// 選択インデックスを得る
			VcString strIndex = valu->substring(idxPrf + prefixList.getLength());
			_idxSelected = StringUtils::toInteger(&strIndex);	// 選択する
			_res.setSelectedIndex(_idxSelected);				// 結果値の設定

			// モードによる処理
			switch(_lwmode)
			{
			case LWMODE_USE:		// 使用モード
				// 終了予約する
				_isDoneReserved = true;
				break;
			case LWMODE_SORT:		// ソートモード
				if(_idxSortSelected == -1)	// 非選択→選択時
				{
					selectSortFirst();
				}
				else	// 選択→並べ替え時
				{
					doSort(mtnodeParent);
				}
				break;
			case LWMODE_CLONE:		// 複製モード
				confirmClone();
				break;
			case LWMODE_DELETE:		// 削除モード
				confirmDelete();
				break;
			case LWMODE_EDIT:		// 編集モード
				confirmEdit();
				break;
			}

			// 終了
			return true;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	範囲外クリック通知
**//*---------------------------------------------------------------------*/
void ListWindowBase::onClickOutside(const ExecCtx* ec, void* objCreateParam, void* objShowParam)
{
	if(_idxSortSelected != -1)
	{
		cancelSort(-1);
	}

	if(_idxSelected != -1)
	{
		setSelectedIndex(-1);
	}
}

/*---------------------------------------------------------------------*//**
	リストの状態を設定
**//*---------------------------------------------------------------------*/
void ListWindowBase::setListState()
{
	MenuTreeNode* mtnode = _menuList->getCurrentMenuNode(); ASSERT(mtnode != 0L);
	MenuWindow* mwnd = (mtnode != 0L) ? (MenuWindow*)mtnode->getPanel() : 0L; ASSERT(mwnd != 0L);
	if(mwnd == 0L)	{	return;	}

	int cnt = 1;	// 0 はタイトル
	for(ListIterator<Litem*> it = _listLitem->iterator(); it.has(); it.next())
	{
		const Litem* litem = it.object();

		mwnd->setElementUnused(cnt, TFW_IS_FLAG(litem->_liflags, Litem::LIF_NOEDIT));

		cnt++;
	}

	// メニューウインドウの使用モード設定
	mwnd->setForUse(_lwmode == LWMODE_USE);
}

/*---------------------------------------------------------------------*//**
	ソートの１番目の要素選択
**//*---------------------------------------------------------------------*/
void ListWindowBase::selectSortFirst()
{
	if(_idxSelected == -1)	{	return;	}

	_idxSortSelected = _idxSelected;

	// 少し浮いた表現になるように矩形を設定
	MenuTreeNode* mtnode = _menuList->getCurrentMenuNode(); ASSERT(mtnode != 0L);
	MenuWindow* mwnd = (mtnode != 0L) ? (MenuWindow*)mtnode->getPanel() : 0L; ASSERT(mwnd != 0L);
	RectF32 rectOfs(5, -5, 0, 0);
	mwnd->setElementRect(
		_idxSortSelected + 1,	// 0 は TF_TITLE なので +1 する
		0L,						// 変更無し
		&rectOfs,				// 少し浮かせる位置に
		0L	);					// 現在のアニメーション位置から
}

/*---------------------------------------------------------------------*//**
	ソートの実行
**//*---------------------------------------------------------------------*/
void ListWindowBase::doSort(MenuTreeNode* mtnodeParent)
{
	MenuTreeNode* mtnode = _menuList->getCurrentMenuNode(); ASSERT(mtnode != 0L);
	MenuWindow* mwnd = (mtnode != 0L) ? (MenuWindow*)mtnode->getPanel() : 0L; ASSERT(mwnd != 0L);
	s32 index2 = mwnd->getElementContentIndexByNode(mtnodeParent);
	if((index2 != -1) && (_idxSortSelected != index2))
	{
		// 置換処理
		if(onReplaceLitem(_idxSortSelected, index2))
		{
			// リストメニューの更新
			updateLitem(_idxSortSelected);
			updateLitem(index2);

			// 戻す
			cancelSort(index2);
		}
	}
}

/*---------------------------------------------------------------------*//**
	ソートを戻す
**//*---------------------------------------------------------------------*/
void ListWindowBase::cancelSort(s32 idxSortSelected2)
{
	if(_idxSortSelected == -1)	{	return;	}

	MenuTreeNode* mtnode = _menuList->getCurrentMenuNode(); ASSERT(mtnode != 0L);
	MenuWindow* mwnd = (mtnode != 0L) ? (MenuWindow*)mtnode->getPanel() : 0L; ASSERT(mwnd != 0L);

	// 矩形を戻す
	RectF32 rectOfs(0, 0, 0, 0);
	RectF32 rectOfsAnim;
	if(idxSortSelected2 != -1)
	{
		// 入れ替えアニメーションをさせる
		RectF32 rect1, rect2;
		RectF32 rectA1, rectA2;
		mwnd->getElementRect(_idxSortSelected + 1, &rect1, 0L, &rectA1);
		mwnd->getElementRect(idxSortSelected2 + 1, &rect2, 0L, &rectA2);
		mwnd->setElementRect(
			_idxSortSelected + 1,	// 0 は TF_TITLE なので +1 する
			0L,						// 変更無し
			&rectOfs,				// 標準位置に
			RectF32::setout(
				&rectOfsAnim,
				rect2.x() + rectA2.x() - rect1.x(),
				rect2.y() + rectA2.y() - rect1.y(),
				rect2.w() + rectA2.w() - rect1.w(),
				rect2.h() + rectA2.h() - rect1.h()) );
		mwnd->setElementRect(
			idxSortSelected2 + 1,	// 0 は TF_TITLE なので +1 する
			0L,						// 変更無し
			&rectOfs,				// 標準位置に
			RectF32::setout(
				&rectOfsAnim,
				rect1.x() + rectA1.x() - rect2.x(),
				rect1.y() + rectA1.y() - rect2.y(),
				rect1.w() + rectA1.w() - rect2.w(),
				rect1.h() + rectA1.h() - rect2.h()) );
		// 一般選択解除
		setSelectedIndex(-1);
	}
	else
	{
		mwnd->setElementRect(_idxSortSelected + 1, 0L, &rectOfs, 0L);	// 0 は TF_TITLE なので +1 する
	}

	// ソート選択解除
	_idxSortSelected = -1;
}

/*---------------------------------------------------------------------*//**
	複製確認
**//*---------------------------------------------------------------------*/
void ListWindowBase::confirmClone()
{
	// 派生クラスで複製チェックをする
	if(!onCloneCheck(_idxSelected))
	{
		return;
	}

	// YES / NO メッセージウインドウを表示する
	VcString msg;
	if(takeConfirmMessage(&msg, LWMODE_CLONE))
	{
		showYesNoMessageWindow(&msg);
	}
	else	// メッセージなしで実行
	{
		doClone();
	}
}

/*---------------------------------------------------------------------*//**
	複製実行
**//*---------------------------------------------------------------------*/
void ListWindowBase::doClone()
{
	if(_idxSelected != -1)
	{
		// 置換処理
		s32 indexShiftMax;
		if(onCloneLitem(&indexShiftMax, _idxSelected))
		{
			for(s32 i = indexShiftMax; i >= _idxSelected + 1; i--)
			{
				// リストメニューの更新
				updateLitem(i);

				// 複製アニメーションをさせる
				{
					MenuTreeNode* mtnode = _menuList->getCurrentMenuNode(); ASSERT(mtnode != 0L);
					MenuWindow* mwnd = (mtnode != 0L) ? (MenuWindow*)mtnode->getPanel() : 0L; ASSERT(mwnd != 0L);
					RectF32 rect1, rect2;
					RectF32 rectA1;
					mwnd->getElementRect(i - 1 + 1, &rect1, 0L, &rectA1);
					mwnd->getElementRect(i + 0 + 1, &rect2, 0L, 0L);
					RectF32 rectOfs(0, 0, 0, 0);
					RectF32 rectOfsAnim(
						rect1.x() + rectA1.x() - rect2.x(),
						rect1.y() + rectA1.y() - rect2.y(),
						rect1.w() + rectA1.w() - rect2.w(),
						rect1.h() + rectA1.h() - rect2.h());
					mwnd->setElementRect(
						i + 0 + 1,			// 0 は TF_TITLE なので +1 する
						0L,					// 変更無し
						&rectOfs,			// 標準位置に
						&rectOfsAnim );
				}
			}
		}
	}

	// 一般選択解除
	setSelectedIndex(-1);
	// 見るモードに強制移行
	setMode(LWMODE_SHOW, TypeUtils::getEmptyString());
}

/*---------------------------------------------------------------------*//**
	削除確認
**//*---------------------------------------------------------------------*/
void ListWindowBase::confirmDelete()
{
	// 派生クラスで削除チェックをする
	if(!onDeleteCheck(_idxSelected))
	{
		return;
	}

	// YES / NO メッセージウインドウを表示する
	VcString msg;
	if(takeConfirmMessage(&msg, LWMODE_DELETE))
	{
		showYesNoMessageWindow(&msg);
	}
	else	// メッセージなしで実行
	{
		doDelete();
	}
}

/*---------------------------------------------------------------------*//**
	削除実行
**//*---------------------------------------------------------------------*/
void ListWindowBase::doDelete()
{
	if(_idxSelected != -1)
	{
		// 置換処理
		if(onDeleteLitem(_idxSelected))
		{
			for(u32 i = _idxSelected; i < _listLitem->getCount(); i++)
			{
				// リストメニューの更新
				updateLitem(i);

				// 削除アニメーションをさせる
				if(i < _listLitem->getCount() - 1)
				{
					MenuTreeNode* mtnode = _menuList->getCurrentMenuNode(); ASSERT(mtnode != 0L);
					MenuWindow* mwnd = (mtnode != 0L) ? (MenuWindow*)mtnode->getPanel() : 0L; ASSERT(mwnd != 0L);
					RectF32 rect1, rect2;
					RectF32 rectA2;
					mwnd->getElementRect(i + 0 + 1, &rect1, 0L, 0L);
					mwnd->getElementRect(i + 1 + 1, &rect2, 0L, &rectA2);
					RectF32 rectOfs(0, 0, 0, 0);
					RectF32 rectOfsAnim(
						rect2.x() + rectA2.x() - rect1.x(),
						rect2.y() + rectA2.y() - rect1.y(),
						rect2.w() + rectA2.w() - rect1.w(),
						rect2.h() + rectA2.h() - rect1.h());
					mwnd->setElementRect(
						i + 0 + 1,				// 0 は TF_TITLE なので +1 する
						0L,						// 変更無し
						&rectOfs,				// 標準位置に
						&rectOfsAnim );
				}
			}
		}
	}

	// 一般選択解除
	setSelectedIndex(-1);
	// 見るモードに強制移行
	setMode(LWMODE_SHOW, TypeUtils::getEmptyString());
}

/*---------------------------------------------------------------------*//**
	編集確認
**//*---------------------------------------------------------------------*/
void ListWindowBase::confirmEdit()
{
	// 派生クラスで編集チェックをする
	if(!onEditCheck(_idxSelected))
	{
		return;
	}

	// YES / NO メッセージウインドウを表示する
	VcString msg;
	if(takeConfirmMessage(&msg, LWMODE_EDIT))
	{
		showYesNoMessageWindow(&msg);
	}
	else	// メッセージなしで実行
	{
		doEdit();
	}
}

/*---------------------------------------------------------------------*//**
	編集実行
**//*---------------------------------------------------------------------*/
void ListWindowBase::doEdit()
{
	if(_idxSelected != -1)
	{
		// 置換処理
		if(onEditLitem(_idxSelected))
		{
			// リストメニューの更新
			updateLitem(_idxSelected);
		}
	}

	// 一般選択解除
	setSelectedIndex(-1);
	// 見るモードに強制移行
	setMode(LWMODE_SHOW, TypeUtils::getEmptyString());
}

/*---------------------------------------------------------------------*//**
	YES / NO メッセージウインドウを表示する
**//*---------------------------------------------------------------------*/
void ListWindowBase::showYesNoMessageWindow(const VcString* msg)
{
	GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
	if((wndSysMsg != 0L) && !wndSysMsg->isShowWindow())
	{
		wndSysMsg->setKind(MessageWindow::KIND_YESNO);
		wndSysMsg->showMessage(msg, MSGWND_CTRL_HINT);
	}
}

/*---------------------------------------------------------------------*//**
	アイテム更新
**//*---------------------------------------------------------------------*/
void ListWindowBase::updateLitem(s32 idxLitem)
{
	MenuTreeNode* mtnode = _menuList->getCurrentMenuNode(); ASSERT(mtnode != 0L);
	MenuWindow* mwnd = (mtnode != 0L) ? (MenuWindow*)mtnode->getPanel() : 0L; ASSERT(mwnd != 0L);
	if(mwnd == 0L)		{	return;	}

	if(idxLitem < 0)	{	return;	}
	Litem* litem = _listLitem->getObject(idxLitem);
	if(litem == 0L)		{	return;	}
	onUpdateLitem(litem);

	mwnd->setElementContent(idxLitem + 1, litem->_name);
	mwnd->setElementParam(idxLitem + 1, reinterpret_cast<u64>(litem->_obj));
}

/*---------------------------------------------------------------------*//**
	表示状態の変更通知
**//*---------------------------------------------------------------------*/
void ListWindowBase::onShowWindow(bool isShow)
{
	if(isShow)
	{
		// メニュー作成
		if(!createMenu())
		{
			return;
		}

		// メニュー表示
		if(_menuList != 0L)
		{
			_menuList->showMenu(_msprmList);
			setListState();
		}

		// 結果値のクリア
		_isDone = false;			// 終了フラグをクリア
		_isSelfClose = false;
		if(_hpnlHbar != 0L)	{	_hpnlHbar->resetResult();	}
		_res.reset();
	}
	else
	{
		// メニューを閉じる
		if(_menuList != 0L)	{	_menuList->closeMenu();			}

		// メニュー破棄
		destroyMenu();

		// 値をクリア
		_isDoneReserved = false;
		_idxSelected = -1;
		_idxSortSelected = -1;
	}
}


//==========================================================================
// ListWindowBase::Litem メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ListWindowBase::Litem::Litem()
	: _name(new VcString())
	, _obj(0L)
	, _ridx(-1)
	, _liflags(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ListWindowBase::Litem::~Litem()
{
	delete _name;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void ListWindowBase::Litem::copy(const Litem* src)
{
	_name->set(src->_name);
	_obj = src->_obj;
	_liflags = src->_liflags;
}


//==========================================================================
// ListWindowBase::MenuFuncTable_ListBase メソッド

/*---------------------------------------------------------------------*//**
	表示通知
**//*---------------------------------------------------------------------*/
void ListWindowBase::MenuFuncTable_ListBase::onShow(Menu* menu, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam)
{
	GameMenuShowParam* sprm = (GameMenuShowParam*)objShowParam;	ASSERT(sprm != 0L);
	ListWindowBase* own = (ListWindowBase*)sprm->_ownerRef;	ASSERT(own != 0L);
	if(own->_menuSub == menu)
	{
		own->onSubMenuShow(menu, mtnodeShow, objCreateParam, objShowParam);
	}
}

/*---------------------------------------------------------------------*//**
	非表示通知
**//*---------------------------------------------------------------------*/
void ListWindowBase::MenuFuncTable_ListBase::onHide(Menu* menu, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	呼び出し通知
**//*---------------------------------------------------------------------*/
bool ListWindowBase::MenuFuncTable_ListBase::onCall(Menu* menu, const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	GameMenuShowParam* sprm = (GameMenuShowParam*)objShowParam;	ASSERT(sprm != 0L);
	ListWindowBase* own = (ListWindowBase*)sprm->_ownerRef;	ASSERT(own != 0L);
	if(own->_menuList == menu)
	{
		return own->onClickMenuNode(ec, mtnodeCall, mtnodeParent, objCreateParam, objShowParam);
	}
	else if(own->_menuSub == menu)
	{
		return own->onClickSubMenuNode(ec, mtnodeCall, mtnodeParent, objCreateParam, objShowParam);
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	範囲外のクリック通知
**//*---------------------------------------------------------------------*/
void ListWindowBase::MenuFuncTable_ListBase::onClickOutside(Menu* menu, const ExecCtx* ec, void* objCreateParam, void* objShowParam)
{
	GameMenuShowParam* sprm = (GameMenuShowParam*)objShowParam;	ASSERT(sprm != 0L);
	ListWindowBase* own = (ListWindowBase*)sprm->_ownerRef;	ASSERT(own != 0L);
	return own->onClickOutside(ec, objCreateParam, objShowParam);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
