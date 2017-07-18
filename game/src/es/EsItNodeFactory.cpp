/***********************************************************************//**
 *	EsItNodeFactory.cpp
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
#include "EsItNodeFactory.h"

// Friends
#include "EsContext.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define USE_PLACEMENT_NEW	0

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsItNodeFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsItNodeFactory::EsItNodeFactory()
	: _sizeNode(0)
	, _bufPool(0L)
	, _sizePoolBuf(0)
	, _idxCur(0)
	, _numNode(0)
{
	if(_sizeNode < (s32)sizeof(EsItNode)) { _sizeNode = (s32)sizeof(EsItNode); }
	if(_sizeNode < (s32)sizeof(EsUnaryItNode)) { _sizeNode = (s32)sizeof(EsUnaryItNode); }
	if(_sizeNode < (s32)sizeof(EsBinaryItNode)) { _sizeNode = (s32)sizeof(EsBinaryItNode); }
	if(_sizeNode < (s32)sizeof(EsTernaryItNode)) { _sizeNode = (s32)sizeof(EsTernaryItNode); }
	if(_sizeNode < (s32)sizeof(EsListItNode)) { _sizeNode = (s32)sizeof(EsListItNode); }
	if(_sizeNode < (s32)sizeof(EsNameItNode)) { _sizeNode = (s32)sizeof(EsNameItNode); }
	if(_sizeNode < (s32)sizeof(EsNameSetItNode)) { _sizeNode = (s32)sizeof(EsNameSetItNode); }
	if(_sizeNode < (s32)sizeof(EsDoubleItNode)) { _sizeNode = (s32)sizeof(EsDoubleItNode); }
#if defined(_DEBUG)
	TRACE("{EsItNodeFactory::EsItNodeFactory} _sizeNode=%d\n", _sizeNode);
	TRACE("    sizeof(EsItNode)=%d\n", sizeof(EsItNode));
	TRACE("    sizeof(EsUnaryItNode)=%d\n", sizeof(EsUnaryItNode));
	TRACE("    sizeof(EsBinaryItNode)=%d\n", sizeof(EsBinaryItNode));
	TRACE("    sizeof(EsTernaryItNode)=%d\n", sizeof(EsTernaryItNode));
	TRACE("    sizeof(EsListItNode)=%d\n", sizeof(EsListItNode));
	TRACE("    sizeof(EsNameItNode)=%d\n", sizeof(EsNameItNode));
	TRACE("    sizeof(EsNameSetItNode)=%d\n", sizeof(EsNameSetItNode));
	TRACE("    sizeof(EsDoubleItNode)=%d\n", sizeof(EsDoubleItNode));
#endif
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsItNodeFactory::~EsItNodeFactory()
{
	#if USE_PLACEMENT_NEW
		delete[] _bufPool;
	#endif
}

/*---------------------------------------------------------------------*//**
	バッファを得る
**//*---------------------------------------------------------------------*/
s8* EsItNodeFactory::getFreeBuffer()
{
	// まだバッファが無い場合は作成する
	if(_bufPool == 0L)
	{
		s32 numNode = 512;
		s32 sizePoolBuf = numNode * _sizeNode;
		s8* bufPool = new s8[sizePoolBuf];
		if(bufPool == 0L)	{	ASSERT(false);	return 0L;	}	// 失敗
		::memset(bufPool, 0, sizeof(s8) * sizePoolBuf);
		_numNode = numNode;
		_sizePoolBuf = sizePoolBuf;
		_bufPool = bufPool;
	}

	s8* bufNode = 0L;

	// 空きを探す
	while(_idxCur < _numNode)
	{
		bufNode = _bufPool + (_idxCur * _sizeNode);
		EsItNode* node = (EsItNode*)bufNode;
		if(*node->nodeFlag() == 0)		// F_NEW や F_USED は使えない
		{
			break;	// 使える
		}
		_idxCur++;
	}

	// もう１回チャレンジ
	if(bufNode == 0L)
	{
		_idxCur = 0;
		while(_idxCur < _numNode)
		{
			bufNode = _bufPool + (_idxCur * _sizeNode);
			EsItNode* node = (EsItNode*)bufNode;
			if(*node->nodeFlag() == 0)		// F_NEW や F_USED は使えない
			{
				break;	// 使える
			}
			_idxCur++;
		}
	}

	if(bufNode == 0L)
	{
		// 失敗
		return 0L;
	}

	// 使用バッファをクリアして返す
	::memset(bufNode, 0, _sizeNode);
	return bufNode;
}

/*---------------------------------------------------------------------*//**
	ノードを作成する
**//*---------------------------------------------------------------------*/
EsItNode* EsItNodeFactory::make(EsContext* ctx, EsNodeType nt)
{
	#if USE_PLACEMENT_NEW
		// バッファを得る
		s8* bufNode = getFreeBuffer();
		if(bufNode == 0L)	{	return 0L;	}

		// placement new 使用のため、メモリリーク検知用の new 定義を一度破棄する
		#pragma push_macro("new")
		#undef new
		#define NEW new(bufNode)
	#else
		#define NEW new
	#endif

	// placement new を使用した作成
	EsItNode* node = 0L;
	switch(nt)
	{
	case NT_NULLARY:	ASSERT(sizeof(EsItNode) <= _sizeNode);			node = NEW EsItNode(NT_NULLARY);	break;
	case NT_UNARY:		ASSERT(sizeof(EsUnaryItNode) <= _sizeNode);		node = NEW EsUnaryItNode();			break;
	case NT_BINARY:		ASSERT(sizeof(EsBinaryItNode) <= _sizeNode);	node = NEW EsBinaryItNode();		break;
	case NT_TERNARY:	ASSERT(sizeof(EsTernaryItNode) <= _sizeNode);	node = NEW EsTernaryItNode();		break;
	case NT_LIST:		ASSERT(sizeof(EsListItNode) <= _sizeNode);		node = NEW EsListItNode();			break;
	case NT_NAME:		ASSERT(sizeof(EsNameItNode) <= _sizeNode);		node = NEW EsNameItNode();			break;
	case NT_NAMESET:	ASSERT(sizeof(EsNameSetItNode) <= _sizeNode);	node = NEW EsNameSetItNode();		break;
	case NT_DOUBLE:		ASSERT(sizeof(EsDoubleItNode) <= _sizeNode);	node = NEW EsDoubleItNode();		break;
	default:			break;
	}

	#if USE_PLACEMENT_NEW
		// placement new 使用のため、の new マクロを戻す
		#pragma pop_macro("new")
	#endif

	// 新規フラグ設定
	TFW_SET_FLAG(*node->nodeFlag(), EsItNode::NF_NEW, true);

	#if !DEL_TREE_CHILD
		// 削除リストに追加する
		ctx->addDeleteCollectionNode(node);
	#endif

	return node;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
