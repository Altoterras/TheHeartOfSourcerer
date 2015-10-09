/***********************************************************************//**
 *  Tree.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/04/13.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COLLECTION_TREE_H_
#define _TFW_COLLECTION_TREE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class TreeIterator;

/*---------------------------------------------------------------------*//**
 * ツリー要素
 * 	（オブジェクトを内包するタイプ）
 * 
**//*---------------------------------------------------------------------*/
template<class TYPE> class TreeNode
{
	//======================================================================
	// 変数
private:

	TreeNode*	_tnodeParent;
	TreeNode*	_tnodeChild;
	TreeNode*	_tnodeSibling;
	TYPE*		_obj;
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	/**
	 * オブジェクト取得
	 * 
	 */
	const TYPE* getObject() const		{	return _obj;			}
	TYPE* object() const				{	return _obj;			}

	/**
	 * 親要素取得
	 * 
	 */
	const TreeNode* getParent() const	{	return _tnodeParent;	}
	TreeNode* parent() const			{	return _tnodeParent;	}
	
	/**
	 * 子要素取得
	 * 
	 */
	const TreeNode* getChild() const	{	return _tnodeChild;		}
	TreeNode* child() const				{	return _tnodeChild;		}
	
	/**
	 * 兄弟要素取得
	 * 
	 */
	const TreeNode* getSibling() const	{	return _tnodeSibling;	}
	TreeNode* sibling() const			{	return _tnodeSibling;	}

	/**
	 * オブジェクト設定
	 * 
	 */
	void setObject(TYPE* obj)	{	_obj = obj;	}

	/**
	 * 親要素設定
	 * 
	 */
	void setParent(TreeNode* node)		{	_tnodeParent = node;	}
	
	/**
	 * 子要素設定
	 * 
	 */
	void setChild(TreeNode* node)		{	_tnodeChild = node;	}
	
	/**
	 * 兄弟要素設定
	 * 
	 */
	void setSibling(TreeNode* node)	{	_tnodeSibling = node;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	/**
	 * コンストラクタ
	 * 
	 */
	TreeNode() :
		_tnodeParent(0L),
		_tnodeChild(0L),
		_tnodeSibling(0L),
		_obj(0L)
	{
	}
	
	/**
	 * 子要素追加
	 * 
	 */
	TreeNode* addChild(TYPE* obj)
	{
		TreeNode* tnodeNew = addChild(new TreeNode());
		tnodeNew->_obj = obj;
		return tnodeNew;
	}
	
	/**
	 * 兄弟要素追加
	 * 
	 */
	TreeNode* addSibling(TYPE* obj)
	{
		TreeNode* tnodeNew = addSibling(new TreeNode());
		tnodeNew->_obj = obj;
		return tnodeNew;
	}

	/**
	 * 子要素追加
	 * 
	 */
	TreeNode* addChild(TreeNode* tnodeNew)
	{
		TreeNode* tnodeWk = tnodeNew;
		while(tnodeWk != 0L)
		{
			tnodeWk->_tnodeParent = this;
			tnodeWk = tnodeWk->sibling();
		}
		
		if(_tnodeChild == 0L)
		{
			_tnodeChild = tnodeNew;
		}
		else
		{
			TreeNode* nodeTail = _tnodeChild;
			while(nodeTail->_tnodeSibling != 0L)
			{
				nodeTail = nodeTail->_tnodeSibling;
			}
			nodeTail->_tnodeSibling = tnodeNew;
		}
		
		return tnodeNew;
	}

	/**
	 * 兄弟要素追加
	 * 
	 */
	TreeNode* addSibling(TreeNode* tnodeNew)
	{
		tnodeNew->_tnodeParent = _tnodeParent;
		
		TreeNode* nodeTail = this;
		while(nodeTail->_tnodeSibling != 0L)
		{
			nodeTail = nodeTail->_tnodeSibling;
		}
		nodeTail->_tnodeSibling = tnodeNew;
		
		return tnodeNew;
	}
};

/**
 * ツリー
 * 
 */
template<class TYPE> class Tree
{
	//======================================================================
	// メンバ変数
private:

	TreeNode<TYPE>*	_tnodeRoot;			// ルートは必ず存在する
	bool			_isAutoObjDelete;
	
	//======================================================================
	// アクセサ
public:
	
	// ルート要素取得
	TreeNode<TYPE>* getRoot()	{	return _tnodeRoot;	}

	//======================================================================
	// メソッド
public:
	
	/**
	 * コンストラクタ
	 * 
	 */
	Tree() : _tnodeRoot(0L), _isAutoObjDelete(false)
	{
	}
	
	/**
	 * コンストラクタ
	 * 
	 * @param		isAutoObjDelete	自動削除をするかどうか
	 */
	explicit Tree(bool isAutoObjDelete) : _tnodeRoot(0L), _isAutoObjDelete(isAutoObjDelete)
	{
	}

	/**
	 * デストラクタ
	 * 
	 */
	~Tree()
	{
		removeAll();
	}

	/**
	 * 自動オブジェクト削除フラグの設定
	 *
	 */
	void setAutoObjectDelete(bool isAuto)
	{
		_isAutoObjDelete = true;
	}

	/**
	 * ルートノードの追加
	 *
	 */
	TreeNode<TYPE>* addRootNode()
	{
		_tnodeRoot = new TreeNode<TYPE>();
		return _tnodeRoot;
	}

	/**
	 * ルートノードの追加
	 *
	 */
	TreeNode<TYPE>* addRootNode(TreeNode<TYPE>* node)
	{
		_tnodeRoot = node;
		return _tnodeRoot;
	}
	
	/**
	 * 子要素追加
	 * 
	 */
	TreeNode<TYPE>* addChild(TreeNode<TYPE>* nodeTrg, TYPE* obj)
	{
		return nodeTrg->addChild(obj);
	}
	
	/**
	 * 兄弟要素追加
	 * 
	 */
	TreeNode<TYPE>* addSibling(TreeNode<TYPE>* nodeTrg, TYPE* obj)
	{
		return nodeTrg->addSibling(obj);
	}
	
	/**
	 * ノード削除
	 *
	 */
	void remove(TreeNode<TYPE>* nodeTrg)
	{
		if(nodeTrg == 0L)		{	return;	}
		
		// 兄弟間の位置替え
		if(nodeTrg->getParent() != 0L)	// ルートノード以外
		{
			// 兄弟のどの位置かを取得
			TreeNode<TYPE>* nodePrevSibling = 0L;
			TreeNode<TYPE>* nodeCur = nodeTrg->getParent()->getChild();
			while(nodeCur != nodeTrg)
			{
				nodePrevSibling = nodeCur;
				nodeCur = nodeCur->getSibling();
			}

			// ポインタ付け替え
			if(nodePrevSibling == 0L)		// 最初の子の場合
			{
				nodeTrg->getParent()->setChild(nodeCur->getSibling());
			}
			else							// 途中の子の場合
			{
				nodePrevSibling->setSibling(nodeCur->getSibling());
			}
		}

		// 子以下を全て削除
		removeDive(nodeTrg->getChild());
		
		// 削除
		if(_isAutoObjDelete)	{	delete nodeTrg->getObject();	}
		delete nodeTrg;
	}
	
	/**
	 * 指定ノード以下を全て削除
	 *
	 */
	void removeDive(TreeNode<TYPE>* nodeTrg)
	{
		if(nodeTrg == 0L)		{	return;	}

		if(nodeTrg->child() != 0L)
		{
			removeDive(nodeTrg->child());
		}
		if(nodeTrg->sibling() != 0L)
		{
			removeDive(nodeTrg->sibling());
		}
		
		// 削除
		if(_isAutoObjDelete)	{	delete nodeTrg->getObject();	}
		delete nodeTrg;
	}
	
	/**
	 * 全てのノードを削除
	 *
	 */
	void removeAll()
	{
		removeDive(_tnodeRoot);
	}
	
	/**
	 * 正当性検証
	 * 
	 */
	void validation()
	{
		validationDive(_tnodeRoot);
	}

	/**
	 * イテレータの取得
	 *
	 */
	TreeIterator<TYPE> iterator() const
	{
		TreeIterator<TYPE> it(_tnodeRoot);
		return it;
	}

private:
	/**
	 * 正当性検証再帰処理サブ関数
	 * 
	 */
	void validationDive(TreeNode<TYPE>* node)
	{
		TreeNode<TYPE>* nodeChild	= node->getChild();
		TreeNode<TYPE>* nodeSibling	= node->getSibling();

		// 親の確認
		if((_tnodeRoot != node) && (node->GetParent() == 0L))
		{
			TRACE("tree error! - 01\n");
		}
		if(nodeChild != 0L)
		{
			if(nodeChild->getParent() != node)
			{
				TRACE("tree error! - 02\n");
			}
		}
		if(nodeSibling != 0L)
		{
			if(nodeSibling->getParent() != node->getParent())
			{
				TRACE("tree error! - 03\n");
			}
		}

		// 再帰処理
		if(nodeChild != 0L)
		{
			validationDive(nodeChild);
		}
		if(nodeSibling != 0L)
		{
			validationDive(nodeSibling);
		}
	}
};

/*---------------------------------------------------------------------*//**
	ツリーイテレータ クラス

**//*---------------------------------------------------------------------*/
template<class TYPE> class TreeIterator 
{
private:
	/*-----------------------------------------------------------------*//**
		ツリーイテレータ 走査ステイト クラス
			再起処理と同等の実行を実現する状態保持クラス

	**//*-----------------------------------------------------------------*/
	class DiveState
	{
	public:
		enum State { SELF, CHLD, SIBL };
		TreeNode<TYPE>*		_nodePtr;
		DiveState*			_dstatPrev;
		int					_state;
		DiveState() : _nodePtr(0L), _dstatPrev(0L), _state(SELF) {}
		DiveState(const DiveState& ds) : _nodePtr(0L), _dstatPrev(ds._dstatPrev), _state(ds._state) {}
	};

private:
	TreeNode<TYPE>*		_nodeCur;
	DiveState*			_dstatCur;
	int					_depth;

public:
	TreeIterator() : _nodeCur(0L), _dstatCur(0L), _depth(0)							{}
	TreeIterator(TreeNode<TYPE>* node) : _nodeCur(node), _dstatCur(0L), _depth(0)	{}
	TreeIterator(const TreeIterator& tit) : _nodeCur(0L), _dstatCur(0L), _depth(0)	{	copy(&tit);	}

	~TreeIterator()
	{
		cleanup();
	}

	TreeIterator& copy(const TreeIterator* tit)
	{
		_nodeCur = tit->_nodeCur;

		DiveState* dstatWkSrc = tit->_dstatCur;
		DiveState** dstatWkDst = &(this->_dstatCur);
		while(dstatWkSrc != 0L)
		{
			*dstatWkDst = new DiveState(*dstatWkSrc);
			dstatWkSrc = dstatWkSrc->_dstatPrev;
			dstatWkDst = &(*dstatWkDst)->_dstatPrev;
		}

		return *this;
	}

	void cleanup()
	{
		DiveState* dstatWk1 = _dstatCur;
		while(dstatWk1 != 0L)
		{
			DiveState* dstatWk2 = dstatWk1;
			dstatWk1 = dstatWk1->_dstatPrev;
			delete dstatWk2;
		}
		_dstatCur = 0L;
		_nodeCur = 0L;
		_depth = 0;
	}

	bool equals(const TreeIterator& tit)		{	return _nodeCur == tit._nodeCur;					}
	TreeNode<TYPE>* node() const				{	return _nodeCur;									}
	TYPE* object() const						{	return _nodeCur->object();							}
	bool has() const							{	return _nodeCur != 0L;								}
	int getDepth() const						{	return _depth;										}

	TYPE* next()
	{
		TYPE* obj = _nodeCur->object();

		// 最初の実行
		if(_dstatCur == 0L)
		{
			_dstatCur = new DiveState();
			_dstatCur->_nodePtr = _nodeCur;
		}

		while(true)
		{
			// 次のノードへ
			switch(_dstatCur->_state)
			{
			case DiveState::SELF:	// self の次
				{
					TreeNode<TYPE>* nodeNext = _dstatCur->_nodePtr->child();
					if(nodeNext != 0L)
					{
						// 子へ
						_nodeCur = nodeNext;
						_dstatCur->_state = DiveState::CHLD;
						DiveState* dstatChild = new DiveState();
						dstatChild->_nodePtr = _nodeCur;
						dstatChild->_dstatPrev = _dstatCur;
						_dstatCur = dstatChild;
						_depth++;
						return obj;
					}
				}
				// fall through
			case DiveState::CHLD:	// child の次
				{
					TreeNode<TYPE>* nodeNext = _dstatCur->_nodePtr->sibling();
					if(nodeNext != 0L)
					{
						// 兄弟へ
						_nodeCur = nodeNext;
						_dstatCur->_nodePtr = nodeNext;
						_dstatCur->_state = DiveState::SIBL;
						DiveState* dstatSibling = new DiveState();
						dstatSibling->_nodePtr = nodeNext;
						dstatSibling->_dstatPrev = _dstatCur;
						_dstatCur = dstatSibling;
						return obj;
					}
				}
				// fall through
			case DiveState::SIBL:	// sibling の次
				{
					// 呼び出し元へ
					DiveState* dstatChild = _dstatCur;
					_dstatCur = _dstatCur->_dstatPrev;
					delete dstatChild;
					_depth--;

					if(_dstatCur == 0L)	// ルート
					{
						_nodeCur = 0L;
						return obj;
					}
				}
				break;
			}
		}

		return obj;
	}
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_COLLECTION_TREE_H_
