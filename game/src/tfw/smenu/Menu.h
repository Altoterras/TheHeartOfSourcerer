/***********************************************************************//**
 *  Menu.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SMENU_MENU_H_
#define _TFW_SMENU_MENU_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "Base.h"

#include "../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Tree;
template<class TYPE> class TreeNode;
class XmlNode;
class CStringBase;
class FileBase;
class PanelBase;

SMENU_BEGIN_NS

class MenuFuncTable;
class MenuPanelFactory;
class MenuTreeNode;

/*---------------------------------------------------------------------*//**
 *	メニュー
 *	
**//*---------------------------------------------------------------------*/
class Menu : public FrameExecDraw
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 表示中かどうかを得る
	inline bool isShow() const { return _isShow; }
	// 終了済みかどうかを得る
	inline bool isDone() const { return _isDone; }
	// ファンクションテーブルを取得する
	inline MenuFuncTable* getFuncTable() const	{ return _functblRef; }
	// パネルファクトリーを取得する
	inline MenuPanelFactory* getPanelFactory() const { return _pnlfctryRef; }
	// 作成パラメータへのポインタを取得する
	inline void* getCreateParam() const { return _objCreateParamRef; }
	// 表示パラメータへのポインタを取得する
	inline void* getShowParam() const { return _objShowParamRef; }
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// 現在のノードを取得
	MenuTreeNode* getCurrentMenuNode() const;
	// ルートノード取得
	MenuTreeNode* getMenuTreeRootNode() const;
	// 子ノード取得
	MenuTreeNode* getMenuTreeNodeChild(MenuTreeNode* mtnode) const;
	// 兄弟ノード取得
	MenuTreeNode* getMenuTreeNodeSibling(MenuTreeNode* mtnode) const;
	// ノード検索
	MenuTreeNode* findMenuTreeNode(int id) const;
	MenuTreeNode* findMenuTreeNode(const char* name) const;
	// ノード追加
	bool addMenuTreeNode(MenuTreeNode* mtnodePos, MenuTreeNode* mtnodeAdd, bool isChild);
	// メニューを表示する
	void showMenu(void* objShowParam);
	// メニューを閉じる
	void closeMenu();
	// 最上面のノードを閉じる
	void closeTopNode();
	// ノードフラグの設定
	void setNodeGeneralFlag(int id, int flag, bool isOn);
	// ノードフラグの設定（ダイレクト設定）
	void setNodeGeneralFlags(int id, int flag);
	
private:
	// ノード取得
	MenuTreeNode* findMenuTreeNode(TreeNode<MenuTreeNode>* tnode, int id, const char* name) const;
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	// コンストラクタ
	Menu();
	// デストラクタ
	virtual ~Menu();
	// XMLから作成
	bool create(const CStringBase* filepathXml, MenuFuncTable* functblRef, MenuPanelFactory* pnlfctryRef, void* objCreateParam);
	bool create(u32 fileidXml, MenuFuncTable* functblRef, MenuPanelFactory* pnlfctryRef, void* objCreateParam);
	// ノードをプログラムで作成
	bool create(const CStringBase* name, MenuTreeNode** mtnodeParentArrayEntr, MenuTreeNode** mtnodeChildArrayEntr, int numArray, MenuFuncTable* functblRef, MenuPanelFactory* pnlfctryRef, void* objCreateParam);
	// 破棄
	void destroy();
	// フレーム制御
	virtual void exec(ExecRes* res, const ExecCtx* ec);
	// 描画
	virtual void draw(const RenderCtx* rc);

private:
	// XMLから作成
	bool createFromXml(FileBase* fileXml, MenuFuncTable* functblRef, MenuPanelFactory* pnlfctryRef, void* objCreateParam);
	// ツリーノードの追加
	void addTreeNode(TreeNode<MenuTreeNode>* tnodeDst, bool isDiveChild, const XmlNode* xnodeSrc);
	// ツリーの作成
	void makeTree(TreeNode<MenuTreeNode>* tnodeDst, bool isDiveChild, const XmlNode* xnodeSrc);
	// パネルレンダー
	void diveDrawPanel(const RenderCtx* rc, TreeNode<MenuTreeNode>* tnode) const;
	// ツリーノードの検索
	TreeNode<MenuTreeNode>* findTreeNodeFromMenuTreeNode(const MenuTreeNode* mtnode, TreeNode<MenuTreeNode>* tnodeCurDive) const;

	//======================================================================
	// メンバ変数
private:
	Tree<MenuTreeNode>*		_tree;
	TreeNode<MenuTreeNode>*	_tnodeCur;
	MenuFuncTable*			_functblRef;
	MenuPanelFactory*		_pnlfctryRef;
	void*					_objCreateParamRef;
	void*					_objShowParamRef;
	
	bool					_isShow;
	bool					_isDone;
	int						_depthCur;
};

SMENU_END_NS
TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_SMENU_MENU_H_
