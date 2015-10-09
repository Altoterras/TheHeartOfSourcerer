/***********************************************************************//**
 *  SgNodeChain.cpp
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
#include "SgNodeChain.h"

// Friends
#include "../SgManager.h"
#include "../../../collection/List.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SgNodeChain アクセサ

/*---------------------------------------------------------------------*//**
	子ノード数を取得
**//*---------------------------------------------------------------------*/
s32 SgNodeChain::getChildNodeNum() const
{
	if(_listChild == 0L)	{	return 0L;	}
	return  _listChild->getCount();
}

/*---------------------------------------------------------------------*//**
	子ノードを取得
**//*---------------------------------------------------------------------*/
SgNode* SgNodeChain::getChildNode(int index) const
{
	if(_listChild == 0L)	{	return 0L;	}
	return _listChild->getObject(index);
}

/*---------------------------------------------------------------------*//**
	ノード ID から子ノードを取得
**//*---------------------------------------------------------------------*/
SgNode* SgNodeChain::getChildNodeFromId(u16 sgnodeid) const
{
	for(ListIterator<SgNode*> it = _listChild->iterator(); it.has(); it.next())
	{
		SgNode* sgnode = it.object();
		if(sgnode->getId() == sgnodeid)
		{
			return sgnode;
		}

		if(sgnode->getType() == SGNTYPE_CHAIN)
		{
			sgnode = ((SgNodeChain*)sgnode)->getChildNodeFromId(sgnodeid);
			if(sgnode != 0L)
			{
				return sgnode;
			}
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	リストのイテレータを得る
**//*---------------------------------------------------------------------*/
bool SgNodeChain::getIterator(ListIterator<SgNode*>* it) const
{
	if(_listChild == 0L)	{	return false;	}
	*it = _listChild->iterator();
	return true;
}

//==========================================================================
// SgNodeChain メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SgNodeChain::SgNodeChain() :
	_listParent(0L),
	_listChild(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SgNodeChain::~SgNodeChain()
{
	ASSERT(_listParent == 0L);
	ASSERT(_listChild == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SgNodeChain::create()
{
	if(!SgNode::create(SGNTYPE_CHAIN))
	{
		return false;
	}
	setDestroyWhenNodeDestruction(true);

	_listParent = new List<SgNode*>();
	_listChild = new List<SgNode*>();

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SgNodeChain::destroy()
{
	if(_listParent != 0L)
	{
		delete _listParent;
		_listParent = 0L;
	}

	if(_listChild != 0L)
	{
		ListIterator<SgNode*> it = _listChild->iterator();
		while(it.has())
		{
			SgNode* node = it.object();
			it.next();

			// リストから削除する
			removeChildNode(node);

			if(node->isDestroyWhenNodeDestruction())
			{
				node->destroy();
				delete node;
			}
		}
		delete _listChild;
		_listChild = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	子ノードを追加
**//*---------------------------------------------------------------------*/
bool SgNodeChain::addChildNode(SgNode* sgnode)
{
	ASSERT(this != sgnode);

	// 子ノード追加
	if(_listChild == 0L)	{	return false;	}
	ListNode<SgNode*>* lnode = _listChild->addTail(sgnode);
	if(lnode == 0L)			{	return false;	}

	// シーングラフ配置のフラグを立てる
	sgnode->setOnSg(true);

	// 子がチェインノードの場合、親として自身を設定する
	if(sgnode->getType() == SGNTYPE_CHAIN)
	{
		((SgNodeChain*)sgnode)->addParentNode(this);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	子ノードを削除
**//*---------------------------------------------------------------------*/
bool SgNodeChain::removeChildNode(SgNode* sgnode)
{
	ASSERT(this != sgnode);

	// ノード検索
	if(_listChild == 0L)	{	return false;	}
	ListNode<SgNode*>* lnode = _listChild->findNode(sgnode);
	if(lnode == 0L)			{	return false;	}

	// 子ノードリストから削除
	_listChild->removeNode(lnode);

	// シーングラフ配置のフラグを下ろす
	sgnode->setOnSg(false);

	return true;
}

/*---------------------------------------------------------------------*//**
	子ノードを ID で削除
**//*---------------------------------------------------------------------*/
bool SgNodeChain::removeChildNode(u16 sgnodeid)
{
	// 子ノードから削除
	if(_listChild == 0L)	{	return false;	}
	bool isDelete = false;
	ListIterator<SgNode*> it = _listChild->iterator();
	while(it.has())
	{
		SgNode* sgnode = it.object();
		if(sgnode->getId() == sgnodeid)
		{
			// 子ノードリストから削除
			sgnode->setOnSg(false);	// シーングラフ配置のフラグを下ろす
			isDelete = true;
			ListIterator<SgNode*> itDelete = it;
			it.next();	// イテレータを次に進めておく
			_listChild->removeNode(itDelete.node());	// ノード削除
		}
		else
		{
			it.next();
		}
	}
	return isDelete;
}

/*---------------------------------------------------------------------*//**
	親ノードを追加
**//*---------------------------------------------------------------------*/
bool SgNodeChain::addParentNode(SgNode* sgnode)
{
	ASSERT(this != sgnode);

	if(_listParent == 0L)	{	return false;	}
	ListNode<SgNode*>* lnode = _listParent->addTail(sgnode);
	if(lnode == 0L)			{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SgNodeChain::exec(ExecRes* res, const ExecCtx* ec)
{
	for(ListIterator<SgNode*> it = _listChild->iterator(); it.has(); it.next())
	{
		it.object()->exec(res, ec);
	}
}

/*---------------------------------------------------------------------*//**
	描画前処理
**//*---------------------------------------------------------------------*/
void SgNodeChain::previseImplements(const RenderCtx* rc)
{
	for(ListIterator<SgNode*> it = _listChild->iterator(); it.has(); it.next())
	{
		it.object()->previse(rc);
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void SgNodeChain::renderImplements(const RenderCtx* rc)
{
	for(ListIterator<SgNode*> it = _listChild->iterator(); it.has(); it.next())
	{
		it.object()->render(rc);
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS