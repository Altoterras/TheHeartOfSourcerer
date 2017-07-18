/***********************************************************************//**
 *	EsItNode.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/27.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsItNode.h"

// Friends
#include "EsFunction.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsItNode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	Next ノードの最終ノードを得る
**//*---------------------------------------------------------------------*/
const EsItNode* EsItNode::getLastNextNode() const
{
	const EsItNode* nodePrev = 0L;
	const EsItNode* nodeWk = this;
	while(nodeWk->_nodeNext != 0L)
	{
		nodePrev = nodeWk;
		nodeWk = nodeWk->_nodeNext;
	}
	return nodePrev;
}

//==========================================================================
// EsNameItNode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	定義種別を得る
**//*---------------------------------------------------------------------*/
EsNameItNode::DefKind EsNameItNode::getDefKind() const
{
	ASSERT(TFW_IS_FLAG(_defflag, DF_EXTYPE_DEFINE));
	if(getTokenType() == TKN_FUNCTION)	{	return DK_FUNC;		}
	if(getOpcodeType() == OP_NOP)		{	return DK_NULL;		}
	if(getOpcodeType() == OP_GETARG)	{	return DK_ARG;		}
	if(TFW_IS_FLAG(_defflag, DF_CONST))	{	return DK_CONST;	}
	if(TFW_IS_FLAG(_defflag, DF_LET))	{	return DK_LET;		}
	return DK_VAR;
}

/*---------------------------------------------------------------------*//**
	関数ボックスを設定する
**//*---------------------------------------------------------------------*/
void EsNameItNode::setFunctionBox(EsFunctionBox* funcbox)
{
	_objbox = funcbox;

	// 同時にアトムも設定する
	if(funcbox != 0L)
	{
		const EsFunction* func = funcbox->getFunction();
		_atom = func->getAtom();
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	関数の引数として設定する ⇒ setFunArg
**//*---------------------------------------------------------------------*/
void EsNameItNode::setFuncArg()
{
	_defflag |= DF_FUNCARG;	// 引数フラグを立てる

	ASSERT(!(TFW_IS_FLAG(_defflag, DF_EXTYPE_DEFINE) && isHasLex()));	// 自身が語彙定義か、語彙ノードを持つかのどちらかのはずである
	if(isHasLex())	// 語彙ノードを持つ場合
	{
		_nodeLexDef->_defflag |= DF_FUNCARG;	// 語彙ノードにも引数フラグを立てる
	}
}

/*---------------------------------------------------------------------*//**
	最終的な定義ノードを得る ⇒ resolve
**//*---------------------------------------------------------------------*/
EsNameItNode* EsNameItNode::findFinalDefNode()
{
	EsItNode* node = this;
	while(!node->isDefNode())
	{
		if(node->getTokenType() == TKN_ASSIGN)
		{
			ASSERT(node->getNodeType() == NT_BINARY);
			node = ((EsBinaryItNode*)node)->getLeftNode();
		}
		ASSERT(node->getNodeType() == NT_NAME);
		node = ((EsNameItNode*)node)->getLexDefNode();
	}
	ASSERT(node->getNodeType() == NT_NAME);
	return (EsNameItNode*)node;
}

/*---------------------------------------------------------------------*//**
	空きの変数かどうかを得る ⇒ isFreeVar
**//*---------------------------------------------------------------------*/
bool EsNameItNode::isFreeVar() const
{
	ASSERT(TFW_IS_FLAG(_defflag, DF_EXTYPE_DEFINE));
	return _upvari.isFree() || TFW_IS_FLAG(_defflag, DF_GVAR);
}

/*---------------------------------------------------------------------*//**
	バインドされる定義かどうかを得る ⇒ isBindingForm
**//*---------------------------------------------------------------------*/
bool EsNameItNode::isBindingDefinition() const
{
	DefKind dkind = getDefKind();
	return (dkind == EsNameItNode::DK_VAR) || (dkind == EsNameItNode::DK_CONST) || (dkind == EsNameItNode::DK_LET);
}

//==========================================================================
// EsListItNode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	リストを初期化する ⇒ initList
**//*---------------------------------------------------------------------*/
void EsListItNode::resetList(EsItNode* nodeHead)
{
	_nodeHead = nodeHead;
	_cntNode = 1;

	_blockid = 0;
	_lflag = 0;
}

/*---------------------------------------------------------------------*//**
	リストにノードを追加する ⇒ append
**//*---------------------------------------------------------------------*/
u32 EsListItNode::addListNode(EsItNode* node)
{
	if(_nodeHead == 0L)
	{
		_nodeHead = node;
		_cntNode = 1;
	}
	else
	{
		EsItNode* nodeWk = _nodeHead;
		while(nodeWk->getNextNode() != 0)
		{
			nodeWk = nodeWk->getNextNode();
		}
		ASSERT(nodeWk->getNextNode() == 0L);
		nodeWk->setNextNode(node);
		_cntNode++;
	}
	return _cntNode;
}

/*---------------------------------------------------------------------*//**
	リストの最終ノードを得る ⇒ JSParseNode *last() const
**//*---------------------------------------------------------------------*/
EsItNode* EsListItNode::getLastNode() const
{
	if(_nodeHead == 0L)
	{
		return 0L;
	}
	EsItNode* nodeWk = _nodeHead;
	while(nodeWk->getNextNode() != 0)
	{
		nodeWk = nodeWk->getNextNode();
	}
	return nodeWk;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
