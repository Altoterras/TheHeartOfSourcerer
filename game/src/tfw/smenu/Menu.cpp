/*
 *  Menu.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
 */

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Menu.h"

// Friends
#include "MenuExecRes.h"
#include "MenuFuncTable.h"
#include "MenuPanelReceptor.h"
#include "MenuPanelFactory.h"
#include "MenuTreeNode.h"

// External
#include "../file/File.h"
#include "../file/IdFile.h"
#include "../string/VcString.h"
#include "../string/StringUtils.h"
#include "../lib/XmlParser.h"
#include "../collection/Tree.h"

// Library
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS
SMENU_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラスメソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	現在のノード取得
**//*---------------------------------------------------------------------*/
MenuTreeNode* Menu::getCurrentMenuNode() const
{
	if(_tnodeCur == 0L)	{	return 0L;	}
	return _tnodeCur->object();
}

/*---------------------------------------------------------------------*//**
	ルートノード取得
**//*---------------------------------------------------------------------*/
MenuTreeNode* Menu::getMenuTreeRootNode() const
{
	TreeNode<MenuTreeNode>* tnode = _tree->getRoot();
	if(tnode == 0L)	{	return 0L;	}
	tnode = tnode->child();
	if(tnode == 0L)	{	return 0L;	}
	return tnode->object();
}

/*---------------------------------------------------------------------*//**
	子ノード取得
**//*---------------------------------------------------------------------*/
MenuTreeNode* Menu::getMenuTreeNodeChild(MenuTreeNode* mtnode) const
{
	for(TreeIterator<MenuTreeNode> it = _tree->iterator(); it.has(); it.next())
	{
		if(it.object() == mtnode)
		{
			TreeNode<MenuTreeNode>* tnodeChild = it.node()->child();
			return (tnodeChild != 0L) ? tnodeChild->object() : 0L;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	兄弟ノード取得
**//*---------------------------------------------------------------------*/
MenuTreeNode* Menu::getMenuTreeNodeSibling(MenuTreeNode* mtnode) const
{
	for(TreeIterator<MenuTreeNode> it = _tree->iterator(); it.has(); it.next())
	{
		if(it.object() == mtnode)
		{
			TreeNode<MenuTreeNode>* tnodeSibling = it.node()->sibling();
			return (tnodeSibling != 0L) ? tnodeSibling->object() : 0L;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	ノード取得
**//*---------------------------------------------------------------------*/
MenuTreeNode* Menu::findMenuTreeNode(int id) const
{
	MenuTreeNode* mtnode = findMenuTreeNode(_tree->getRoot(), id, 0L);
	return mtnode;
}

/*---------------------------------------------------------------------*//**
	ノード取得
**//*---------------------------------------------------------------------*/
MenuTreeNode* Menu::findMenuTreeNode(const char* name) const
{
	MenuTreeNode* mtnode = findMenuTreeNode(_tree->getRoot(), 0, name);
	return mtnode;
}

/*---------------------------------------------------------------------*//**
	ノード追加
**//*---------------------------------------------------------------------*/
bool Menu::addMenuTreeNode(MenuTreeNode* mtnodePos, MenuTreeNode* mtnodeAdd, bool isChild)
{
	TreeNode<MenuTreeNode>* tnodePos = findTreeNodeFromMenuTreeNode(mtnodePos, _tree->getRoot());
	if(tnodePos == 0L)	{	return false;	}

	if(isChild)
	{
		tnodePos->addChild(mtnodeAdd);
	}
	else
	{
		tnodePos->addSibling(mtnodeAdd);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	メニューを表示する
**//*---------------------------------------------------------------------*/
void Menu::showMenu(void* objShowParam)
{
	// 初期化
	_tnodeCur = _tree->getRoot()->child();	// 初期のカーソル位置を設定する
	_depthCur = 0;
	_isShow = true;
	_isDone = false;
	
	// パラメータオブジェクトを保存する
	_objShowParamRef = objShowParam;

	// ノードを作成する
	MenuTreeNode* mtnode = _tnodeCur->object();
	ASSERT(mtnode != 0L);
	PanelBase* panel = _pnlfctryRef->makePanel(_tnodeCur, _depthCur, false, _objShowParamRef, _objCreateParamRef);
	ASSERT(panel != 0L);
	mtnode->panelReceptor()->create(panel);
	_functblRef->onShow(this, mtnode, _objCreateParamRef, _objShowParamRef);
}

/*---------------------------------------------------------------------*//**
	メニューを閉じる
**//*---------------------------------------------------------------------*/
void Menu::closeMenu()
{
	_isShow = false;
	
	TreeNode<MenuTreeNode>* tnode = _tnodeCur;
	// ノードを破棄する
	while(tnode != 0L)
	{
		MenuTreeNode* mtnode = tnode->object();
		if(mtnode != 0L)
		{
			ASSERT(mtnode->panelReceptor() != 0L);
			mtnode->panelReceptor()->destroy();
		}
		tnode = tnode->parent();
	}
	_functblRef->onHide(this, _objCreateParamRef, _objShowParamRef);
}

/*---------------------------------------------------------------------*//**
	最上面のノードを閉じる
**//*---------------------------------------------------------------------*/
void Menu::closeTopNode()
{
	if(_depthCur <= 0)
	{
		// メニューを閉じる
		closeMenu();
		_isDone = true;
	}
	else
	{
		// パネルを破棄する
		MenuTreeNode* mtnodeChild = _tnodeCur->object();
		ASSERT(mtnodeChild != 0L);
		ASSERT(mtnodeChild->panelReceptor() != 0L);
		mtnodeChild->panelReceptor()->destroy();
			
		_tnodeCur = _tnodeCur->parent();
		_depthCur--;
	}
}

/*---------------------------------------------------------------------*//**
	ノードの有効化・無効化
**//*---------------------------------------------------------------------*/
void Menu::setNodeGeneralFlag(int id, int flag, bool isOn)
{
	MenuTreeNode* mtnode = findMenuTreeNode(id);
	if(mtnode == 0L)	{	return;	}
	
	int flags = mtnode->getGeneralFlags();
	if(isOn)	{	flags |= flag;	}
	else		{	flags &= ~flag;	}
	mtnode->setGeneralFlags(flags);
}

/*---------------------------------------------------------------------*//**
	ノードの有効化・無効化（ダイレクト設定）
**//*---------------------------------------------------------------------*/
void Menu::setNodeGeneralFlags(int id, int flag)
{
	MenuTreeNode* mtnode = findMenuTreeNode(id);
	if(mtnode == 0L)	{	return;	}
	
	int flags = mtnode->getGeneralFlags();
	flags = flag;
	mtnode->setGeneralFlags(flags);
}

/*---------------------------------------------------------------------*//**
	ノード取得（再帰）
**//*---------------------------------------------------------------------*/
MenuTreeNode* Menu::findMenuTreeNode(TreeNode<MenuTreeNode>* tnode, int id, const char* name) const
{
	MenuTreeNode* mtnode = tnode->object();

	if(mtnode != 0L)
	{
		// 条件判定
		if(id > 0)
		{
			if(id == mtnode->getId())
			{
				return mtnode;
			}
		}
		if(name != 0L)
		{
			if(::strcmp(name, (const char*)mtnode->getName()) == 0)
			{
				return mtnode;
			}
		}
	}
	
	// 再帰処理
	if(tnode->getChild() != 0L)
	{
		MenuTreeNode* mtnodeRet = findMenuTreeNode(tnode->child(), id, name);
		if(mtnodeRet != 0L)	{	return mtnodeRet;	}
	}
	if(tnode->getSibling() != 0L)
	{
		MenuTreeNode* mtnodeRet = findMenuTreeNode(tnode->sibling(), id, name);
		if(mtnodeRet != 0L)	{	return mtnodeRet;	}
	}
	
	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Menu::Menu()
	: _tree(0L)
	, _tnodeCur(0L)
	, _functblRef(0L)
	, _pnlfctryRef(0L)
	, _objCreateParamRef(0L)
	, _objShowParamRef(0L)
	, _isShow(false)
	, _depthCur(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Menu::~Menu()
{
	ASSERT(_tree == 0L);
}

/*---------------------------------------------------------------------*//**
	XMLから作成
**//*---------------------------------------------------------------------*/
bool Menu::create(const CStringBase* filepathXml, MenuFuncTable* functblRef, MenuPanelFactory* pnlfctryRef, void* objCreateParam)
{
	TRACE(VcString::format("{Menu.Create} read : filepath=%s\n", filepathXml));

	// ファイルを読む
	File file(filepathXml, File::MODE_READ);
	if(!file.isOpened())	{	ASSERT(false);	return false;	}

	// XML から作成
	bool isSucceeded = createFromXml(&file, functblRef, pnlfctryRef, objCreateParam);

	// ファイルを閉じる
	file.close();

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	XMLから作成
**//*---------------------------------------------------------------------*/
bool Menu::create(u32 fileidXml, MenuFuncTable* functblRef, MenuPanelFactory* pnlfctryRef, void* objCreateParam)
{
	TRACE(VcString::format("{Menu.Create} read : fileid=%d\n", fileidXml));

	// ファイルを読む
	IdFile file(fileidXml);
	if(!file.isOpened())	{	ASSERT(false);	return false;	}

	// XML から作成
	bool isSucceeded = createFromXml(&file, functblRef, pnlfctryRef, objCreateParam);

	// ファイルを閉じる
	file.close();

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	XMLから作成
**//*---------------------------------------------------------------------*/
#if 0	// デバッグ出力をするかどうか
	#define CMXML_TRACE	TRACE
#else
	#define CMXML_TRACE	//
#endif
bool Menu::createFromXml(FileBase* fileXml, MenuFuncTable* functblRef, MenuPanelFactory* pnlfctryRef, void* objCreateParam)
{
	// XML ファイルをバッファに読み込む
	VcString bufFile;
	while(true)
	{
		const int SIZE_BUF = 100 * 1024;
		char buf[SIZE_BUF];
		int sizeRead = fileXml->read(buf, SIZE_BUF);
		bufFile.add(buf, sizeRead);
		if(sizeRead < SIZE_BUF)	{	break;	}
	}
	CMXML_TRACE(VcString::format("{Menu::createFromXml} menu xml : size=%d\n", bufFile.getLength()));
	
	// XML を解析する
	XmlParser xmlparser;
	xmlparser.parseXmlDocument(&bufFile);
	CMXML_TRACE("{Menu::createFromXml} XmlParser::parseXmlDocument end.\n");
	
	// ツリーを作成する
	_tree = new Tree<MenuTreeNode>(true);
	addTreeNode(_tree->addRootNode(), true, xmlparser.getRootNode());

	#if defined(_DEBUG)
		TRACE("{Menu::createFromXml} menu hierarchy\n");
		for(TreeIterator<MenuTreeNode> it = _tree->iterator(); it.has(); it.next())
		{
			if(it.object() != 0L)	{	for(int i = 0; i < it.getDepth(); i++) { CMXML_TRACE(" "); }	CMXML_TRACE( *it.object()->getName() + "\n" );	}
		}
	#endif

	// ファンクションテーブルを保存する
	_functblRef = functblRef;
	// パネルファクトリを保存する
	_pnlfctryRef = pnlfctryRef;
	// パラメータオブジェクトを保存する
	_objCreateParamRef = objCreateParam;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	ツリーノードの追加

	@retval	true				成功
	@retval	false				失敗
**//*---------------------------------------------------------------------*/
void Menu::addTreeNode(TreeNode<MenuTreeNode>* tnodeDst, bool isDiveChild, const XmlNode* xnodeSrc)
{
	// タグの場合は下にぶらさげる．PCDATA の場合は親に与える
	if(xnodeSrc->getType() == XmlParser::XMLNODETYPE_TAG)		// ▼ 通常タグの場合
	{
		MenuTreeNode* mtnode = new MenuTreeNode();
		mtnode->create(xnodeSrc);
		
		if(isDiveChild)	// 素直に子として呼ばれたか、兄弟として呼ばれたが呼び元の兄弟が非通常タグであったため子がまだ追加れていない場合
		{
			TreeNode<MenuTreeNode>* tnodeDstChild = tnodeDst->addChild(mtnode);
			makeTree(tnodeDstChild, false, xnodeSrc);	// もぐる
		}
		else
		{
			TreeNode<MenuTreeNode>* tnodeDstSibling = tnodeDst->addSibling(mtnode);
			makeTree(tnodeDstSibling, false, xnodeSrc);	// もぐる
		}
	}
	else													// ▼ PCDATA（非タグ）の場合
	{
		MenuTreeNode* mtnode = tnodeDst->object();
		if(mtnode != 0L)
		{
			VcString name(xnodeSrc->getName());
			StringUtils::trim(&name);
			mtnode->setName(&name);
		}
		
		makeTree(tnodeDst, isDiveChild, xnodeSrc);			// もぐる
	}
}

/*---------------------------------------------------------------------*//**
	ツリーの作成

	@retval	true				成功
	@retval	false				失敗
**//*---------------------------------------------------------------------*/
void Menu::makeTree(TreeNode<MenuTreeNode>* tnodeDst, bool isDiveChild, const XmlNode* xnodeSrc)
{
	const XmlNode* xnodeSrcChild = xnodeSrc->getChildNode();
	if(xnodeSrcChild != 0L)
	{
		addTreeNode(tnodeDst, true, xnodeSrcChild);
	}
	
	const XmlNode* xnodeSrcSibling = xnodeSrc->getSiblingNode();
	if(xnodeSrcSibling != 0L)
	{
		addTreeNode(tnodeDst, isDiveChild, xnodeSrcSibling);
	}
}

/*---------------------------------------------------------------------*//**
	ノード配列から作成
		単純な Function 集合メニューを作成する
	
			○ルートノード
			┣●親ノード
			┃┗●子ノード（Fuction）
			┣●親ノード
			┃┗●子ノード（Fuction）
			・
			・
			・
			┗●親ノード
				┗●子ノード（Fuction）
			↑このような形

		また、
		mtnodeParentArrayEntr = NULL, mtnodeChildArrayEntr = NULL,
		numArray = 0 でこの create 実行した後、
		getMenuTreeRootNode でルートノードを得て、
		addMenuTreeNode で要素となるノードを追加する方法もある

	@param	name					名前
	@param	mtnodeParentArrayEntr	親ノード配列（メモリの解放処理は Menu クラス内で行う）
	@param	mtnodeChildArrayEntr	子ノード配列（メモリの解放処理は Menu クラス内で行う）
									# 親ノード配列と同配列数である必要がある
									# 全ての子は Function ノードである必要がある
	@param	numArray				配列長
	@param	functblRef				ファンクションテーブルデータ 
	@param	objParam				パラメータオブジェクト
	@retval	true					成功
	@retval	false					失敗
**//*---------------------------------------------------------------------*/
bool Menu::create(const CStringBase* name, MenuTreeNode** mtnodeParentArrayEntr, MenuTreeNode** mtnodeChildArrayEntr, int numArray, MenuFuncTable* functblRef, MenuPanelFactory* pnlfctryRef, void* objCreateParam)
{
	// ツリーを作成する
	_tree = new Tree<MenuTreeNode>(true);
	
	// 表示対象においての根ノードを作る
	MenuTreeNode* mtnodeRoot = new MenuTreeNode();
	if(!mtnodeRoot->create(name, false, false, false, 0, 0))
	{
		return false;
	}
	
	// ノードを作成する
	TreeNode<MenuTreeNode>* tnode = _tree->addRootNode();
	tnode = tnode->addChild(mtnodeRoot);
	for(int i = 0; i < numArray; i++)
	{
		// 表示される親ノードの追加
		if(i == 0)
		{
			tnode = tnode->addChild(mtnodeParentArrayEntr[i]);
		}
		else
		{
			tnode = tnode->addSibling(mtnodeParentArrayEntr[i]);
		}
		
		// Fuction を提供する子ノードを追加
		tnode->addChild(mtnodeChildArrayEntr[i]);
	}
	
	// ファンクションテーブルを保存する
	_functblRef = functblRef;
	// パネルファクトリを保存する
	_pnlfctryRef = pnlfctryRef;
	// パラメータオブジェクトを保存する
	_objCreateParamRef = objCreateParam;

	// メモリ管理を委譲された配列を削除
	delete[] mtnodeParentArrayEntr;
	delete[] mtnodeChildArrayEntr;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Menu::destroy()
{
	if(_isShow)
	{
		closeMenu();
	}

	// ツリー削除
	if(_tree != 0L)
	{
		for(TreeIterator<MenuTreeNode> it = _tree->iterator(); it.has(); it.next())
		{
			MenuTreeNode* mtnode = it.object();
			if(mtnode != 0L)	// ルートは NULL
			{
				mtnode->destroy();
			}
		}
		delete _tree;
		_tree = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Menu::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!_isShow)	{	return;		}
	
	// 現在のパネルに制御をまわす
	MenuTreeNode* mtnodeCur = _tnodeCur->object();
	ASSERT(mtnodeCur != 0L);
	MenuExecRes resPanel;
	mtnodeCur->panelReceptor()->exec(&resPanel, ec);
	switch(*resPanel.returnValue())
	{
	case MenuExecRes::ON_FUNC:
		{
			MenuTreeNode* mtnodeTrg = (*resPanel.targetNode())->object();
			TreeNode<MenuTreeNode>* tnodeChild = (*resPanel.targetNode())->child();
			ASSERT(tnodeChild != 0L);
			MenuTreeNode* mtnodeChild = tnodeChild->object();
			ASSERT(mtnodeChild != 0L);
			
			// 関数コール
			if(!mtnodeTrg->isDisabled())	// ただし、無効化中以外
			{
				if(_functblRef->onCall(this, ec, mtnodeChild, mtnodeTrg, _objCreateParamRef, _objShowParamRef))
				{
					// 反応があった場合
					if(mtnodeChild->isMenuClose())
					{
						// メニューを閉じる
						closeMenu();
						if(res != 0L)	{	res->setDone();	}
						_isDone = true;
					}
				}
			}
		}
		break;
	case MenuExecRes::ON_DIVE:
		{
			// 下の階層に潜る
			ASSERT(*resPanel.targetNode() != 0L);
			MenuTreeNode* mtnodeTrg = (*resPanel.targetNode())->object();
			if(!mtnodeTrg->isDisabled())	// ただし、無効化中以外
			{
				_tnodeCur = *resPanel.targetNode();
				_depthCur++;

				// パネルを作成する
				MenuTreeNode* mtnodeChild = _tnodeCur->object();
				ASSERT(mtnodeChild != 0L);
				PanelBase* panel = _pnlfctryRef->makePanel(_tnodeCur, _depthCur, false, _objShowParamRef, _objCreateParamRef);
				ASSERT(panel != 0L);
				mtnodeChild->panelReceptor()->create(panel);
			}
		}
		break;
	case MenuExecRes::ON_CANCEL:
		// 階層を上がる
		closeTopNode();
		if(_isDone)	// 最上位ノードだった場合
		{
			if(res != 0L)	{	res->setDone();	}
		}
		break;
	case MenuExecRes::ON_CSTMCHLD:
		{
			// 下の階層に潜る
			ASSERT(*resPanel.targetNode() != 0L);
			_tnodeCur = *resPanel.targetNode();
			_depthCur++;

			// 子の付け替え
			MenuTreeNode* mtnodeChild = _tnodeCur->object();
			VcString name = *mtnodeChild->getName();
			mtnodeChild->destroy();									// 一度破棄
			mtnodeChild->create(&name, false, false, false, 0, 0);	// 再生成

			// パネル作成
			PanelBase* panel = _pnlfctryRef->makePanel(_tnodeCur, _depthCur, true, _objShowParamRef, _objCreateParamRef);
			ASSERT(panel != 0L);
			mtnodeChild->panelReceptor()->create(panel);
		}
		break;
	case MenuExecRes::ON_CLICKOUTSIDE:
		{
			_functblRef->onClickOutside(this, ec, _objCreateParamRef, _objShowParamRef);
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void Menu::draw(const RenderCtx* rc)
{
	//if(!m_isShow)	{	return;		}
	
	diveDrawPanel(rc, _tree->getRoot()->child());
}

/*---------------------------------------------------------------------*//**
	パネル描画
**//*---------------------------------------------------------------------*/
void Menu::diveDrawPanel(const RenderCtx* rc, TreeNode<MenuTreeNode>* tnode) const
{
	if(tnode == 0L)	{	return;		}
	
	MenuTreeNode* mtnode = tnode->object();
	ASSERT(mtnode != 0L);
	if(mtnode->getPanelReceptor()->isAvailable())
	{
		mtnode->panelReceptor()->draw(rc, mtnode == _tnodeCur->getObject());
	}
	
	if((tnode->getChild() != 0L) && mtnode->getPanelReceptor()->isAvailable())
	{
		diveDrawPanel(rc, tnode->child());
	}
	if(tnode->getSibling() != 0L)
	{
		diveDrawPanel(rc, tnode->sibling());
	}
}

/*---------------------------------------------------------------------*//**
	ツリーノードの検索
**//*---------------------------------------------------------------------*/
TreeNode<MenuTreeNode>* Menu::findTreeNodeFromMenuTreeNode(const MenuTreeNode* mtnode, TreeNode<MenuTreeNode>* tnodeCurDive) const
{
	if(tnodeCurDive->getObject() == mtnode)
	{
		return tnodeCurDive;
	}
	
	// 再帰処理
	if(tnodeCurDive->getChild() != 0L)
	{
		TreeNode<MenuTreeNode>* tnodeRet = findTreeNodeFromMenuTreeNode(mtnode, tnodeCurDive->child());
		if(tnodeRet != 0L)	{	return tnodeRet;	}
	}
	if(tnodeCurDive->getSibling() != 0L)
	{
		TreeNode<MenuTreeNode>* tnodeRet = findTreeNodeFromMenuTreeNode(mtnode, tnodeCurDive->sibling());
		if(tnodeRet != 0L)	{	return tnodeRet;	}
	}
	
	return 0L;
}

////////////////////////////////////////////////////////////////////////////

SMENU_END_NS
TFW_END_NS