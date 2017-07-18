/***********************************************************************//**
 *	EsItNode.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/27.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_IT_NODE_H_
#define _ES_ES_IT_NODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsAtomList.h"
#include "EsFunctionBox.h"
#include "EsOpcode.h"
#include "EsUpvarInfo.h"
#include "EsToken.h"
#include "EsValue.h"

#include <new>

////////////////////////////////////////////////////////////////////////////
// 定数

ES_BEGIN_NS

enum EsNodeType
{
	NT_NULLARY,
	NT_UNARY,
	NT_BINARY,
	NT_TERNARY,
	NT_LIST,
	NT_NAME,
	NT_NAMESET,
	NT_DOUBLE,
};

ES_END_NS

#define DEL_TREE_CHILD	0

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsAtomList;
class EsDefItNode;

/*---------------------------------------------------------------------*//**
 *	命令ツリーノード
 *
**//*---------------------------------------------------------------------*/
class EsItNode
{
	//======================================================================
	// 定数
public:
	// ノードフラグ ； nodeFlag() でアクセス
	static const u8		NF_NEW			= 0x01;

	// 解析フラグ ； parseFlag() でアクセス
	static const u8		PF_POPVAR		= 0x01;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ノードタイプを取得する
	inline EsNodeType			getNodeType() const					{	return (EsNodeType)_nt;		}
	// トークンタイプを取得・設定する
	inline EsTokenType			getTokenType() const				{	return (EsTokenType)_tt;	}
	inline void					setTokenType(EsTokenType tt)		{	_tt = tt;					}
	// 演算子タイプを取得・設定する
	inline EsOpcodeType			getOpcodeType() const				{	return (EsOpcodeType)_ot;	}
	inline void					setOpcodeType(EsOpcodeType ot)		{	_ot = ot;					}
	// ノードフラグを得る
	inline u8*					nodeFlag()							{	return &_nodeflag;			}
	// 解析フラグを得る
	inline u8*					parseFlag()							{	return &_parseflag;			}
	// ソース位置を取得・設定する
	inline const EsTokenPos*	getPos() const						{	return &_pos;				}
	inline EsTokenPos*			pos()								{	return &_pos;				}
	inline void					setPos(const EsTokenPos* pos)		{	_pos = *pos;				}
	// バイトコードオフセット位置を取得・設定する
	inline s32					getBytecodeOffset() const			{	return _bcofs;				}
	inline void					setBytecodeOffset(s32 bcofs)		{	_bcofs = bcofs;				}
	// 次のノードを取得・設定する
	inline EsItNode*			getNextNode() const					{	return _nodeNext;			}
	inline void					setNextNode(EsItNode* node)			{	_nodeNext = node;			}
	// リンクノードを取得・設定する
	inline EsItNode*			getLinkNode() const					{	return _nodeLink;			}
	inline void					setLinkNode(EsItNode* node)			{	_nodeLink = node;			}
	// 子ノードを取得・設定する
	virtual EsItNode*			getChildNode() const				{	return 0;					}
	virtual void				setChildNode(EsItNode* node)		{								}
	// ノードは定義ノードである
	virtual bool				isDefNode() const					{	return false;				}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	const EsItNode*				getLastNextNode() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsItNode(EsNodeType nt)
		: _nt(nt)
		, _tt(TKN_NULL)
		, _ot(OP_NULL)
		, _bcofs(0)
		, _nodeflag(0)
		, _parseflag(0)
		, _nodeNext(0L)
		, _nodeLink(0L)
	{
	}
	virtual ~EsItNode()
	{
		#if DEL_TREE_CHILD
			delete _nodeNext;
		#endif
	}
	virtual void copy(const EsItNode* src)
	{
		_nt = src->_nt;
		_tt = src->_tt;
		_ot = src->_ot;
		_pos.copy(&src->_pos);
		_bcofs = src->_bcofs;
		_nodeNext = src->_nodeNext;
		_nodeLink = src->_nodeLink;
		_nodeflag = src->_nodeflag;
		_parseflag = src->_parseflag;
	}

	//======================================================================
	// 変数
private:
#if defined(_DEBUG)
	EsNodeType			_nt;
	EsTokenType			_tt;
	EsOpcodeType		_ot;
#else
	u8					_nt;
	u8					_tt;
	u8					_ot;
#endif
	EsTokenPos			_pos;			// ⇒ pn_pos
	s32					_bcofs;			// ⇒ pn_offset
	EsItNode*			_nodeNext;		// ⇒ pn_next
	EsItNode*			_nodeLink;		// ⇒ dn_uses
	u8					_nodeflag;
	u8					_parseflag;
};

/*---------------------------------------------------------------------*//**
 *	単項命令ノード
 *
**//*---------------------------------------------------------------------*/
class EsUnaryItNode : public EsItNode
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 子ノードを取得・設定する
	virtual EsItNode*		getChildNode() const				{	return _nodeChild;			}
	virtual void			setChildNode(EsItNode* node)		{	_nodeChild = node;			}
	// 非表示フラグを取得・設定する
	inline bool				isHidden() const					{	return _hidden;				}
	inline void				setHidden(bool isHidden)			{	_hidden = isHidden;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsUnaryItNode()
		: EsItNode(NT_UNARY)
		, _nodeChild(0)
		, _hidden(false)
	{
	}
	virtual ~EsUnaryItNode()
	{
		#if DEL_TREE_CHILD
			delete _nodeChild;
		#endif
	}

	//======================================================================
	// 変数
private:
	EsItNode*			_nodeChild;
	bool				_hidden;
};

/*---------------------------------------------------------------------*//**
 *	二項命令ノード
 *
**//*---------------------------------------------------------------------*/
class EsBinaryItNode : public EsItNode
{
	//======================================================================
	// 定数
public:
	static const u8 ITERF_ENUMERATE		= 0x1;	// for in ループの隠しデフォルトイテレータ ⇒ JSITER_ENUMERATE
	static const u8 ITERF_FOREACH		= 0x2;	// foreach ⇒ JSITER_FOREACH
	static const u8 ITERF_KEYVALUE		= 0x4;	// ⇒ JSITER_KEYVALUE

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 子ノードを取得・設定する
	virtual EsItNode*		getChildNode() const				{	return getLeftNode();		}
	virtual void			setChildNode(EsItNode* node)		{	setLeftNode(node);			}
	// 左ノードを取得・設定する
	inline EsItNode*		getLeftNode() const					{	return _nodeLeft;			}
	inline void				setLeftNode(EsItNode* node)			{	_nodeLeft = node;			}
	// 右ノードを取得・設定する
	inline EsItNode*		getRightNode() const				{	return _nodeRight;			}
	inline void				setRightNode(EsItNode* node)		{	_nodeRight = node;			}
	// swtich case 用の値を取得・設定する
	inline const EsValue*	getSwitchCaseValue() const			{	return &_valSwitchCase;		}
	inline void				setSwitchCaseValue(EsValue* val)	{	_valSwitchCase.copy(val);	}
	// フラグを取得・設定する
	inline u32				getIterFlag() const					{	return _iterflg;			}
	inline void				setIterFlag(u32 flag)				{	_iterflg = flag;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsBinaryItNode()
		: EsItNode(NT_BINARY)
		, _nodeLeft(0L)
		, _nodeRight(0L)
		, _iterflg(0)
	{
	}
	EsBinaryItNode(EsNodeType nt)
		: EsItNode(nt)
		, _nodeLeft(0L)
		, _nodeRight(0L)
		, _iterflg(0)
	{
	}
	virtual ~EsBinaryItNode()
	{
		#if DEL_TREE_CHILD
			delete _nodeLeft;
			delete _nodeRight;
		#endif
	}
	virtual void copy(const EsBinaryItNode* src)
	{
		EsItNode::copy(src);
		_nodeLeft = src->_nodeLeft;
		_nodeRight = src->_nodeRight;
		_iterflg = src->_iterflg;
	}

	//======================================================================
	// 変数
private:
	EsItNode*			_nodeLeft;
	EsItNode*			_nodeRight;
	EsValue				_valSwitchCase;
	u32					_iterflg;
};

/*---------------------------------------------------------------------*//**
 *	三項命令ノード
 *
**//*---------------------------------------------------------------------*/
class EsTernaryItNode : public EsItNode
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 子ノードを取得・設定する
	virtual EsItNode*		getChildNode() const				{	return getChildNode1();		}
	virtual void			setChildNode(EsItNode* node)		{	setChildNode1(node);		}
	inline EsItNode*		getChildNode1() const				{	return _nodeChild1;			}
	inline void				setChildNode1(EsItNode* node)		{	_nodeChild1 = node;			}
	inline EsItNode*		getChildNode2() const				{	return _nodeChild2;			}
	inline void				setChildNode2(EsItNode* node	)	{	_nodeChild2 = node;			}
	inline EsItNode*		getChildNode3() const				{	return _nodeChild3;			}
	inline void				setChildNode3(EsItNode* node)		{	_nodeChild3 = node;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsTernaryItNode()
		: EsItNode(NT_TERNARY)
		, _nodeChild1(0L)
		, _nodeChild2(0L)
		, _nodeChild3(0L)
	{
	}
	virtual ~EsTernaryItNode()
	{
		#if DEL_TREE_CHILD
			delete _nodeChild1;
			delete _nodeChild2;
			delete _nodeChild3;
		#endif
	}

	//======================================================================
	// 変数
private:
	EsItNode*			_nodeChild1;
	EsItNode*			_nodeChild2;
	EsItNode*			_nodeChild3;
};

/*---------------------------------------------------------------------*//**
 *	名称ノード
 *
**//*---------------------------------------------------------------------*/
class EsNameItNode : public EsBinaryItNode
{
	//======================================================================
	// 定数
public:
	// 定義フラグ ； defFlag() でアクセス
	static const u32	DF_PLACEHOLDER		= 0x00000001;	// 置き換え用の代替語彙ノード ⇒ PND_PLACEHOLDER
	static const u32	DF_FUNCARG			= 0x00000002;	// 関数の引数 ⇒ PND_FUNARG, isFunArg()
	static const u32	DF_ASSIGNED			= 0x00000004;	// 代入 ⇒ PND_ASSIGNED, isAssigned()
	static const u32	DF_CONST			= 0x00000008;	// 定数 ⇒ PND_CONST
	static const u32	DF_LET				= 0x00000010;	// let 文 ⇒ PND_LET
	static const u32	DF_TOPLEVEL			= 0x00000020;	// トップレベルのボディやプログラムにある名称（関数や変数） ⇒ PND_TOPLEVEL
	static const u32	DF_BLOCKCHILD		= 0x00000040;	// ブロック直下の名称 ⇒ PND_BLOCKCHILD
	static const u32	DF_INITED			= 0x00000080;	// 初期済 ⇒ PND_INITIALIZED
	static const u32	DF_GVAR				= 0x00000100;	// グローバル変数 ⇒ PND_GVAR
	static const u32	DF_BOUND_LG			= 0x00001000;	// スタック、もしくはグローバルスロットに配置する ⇒ PND_BOUND
	static const u32	DF_EXTYPE_DEFINE	= 0x00010000;	// 定義ノード ⇒ pn_defn
	static const u32	DF_EXTYPE_FUNC		= 0x00020000;	// 関数ノード

	// 定義種別 ⇒ Kind
	enum DefKind
	{
		DK_NULL,		// ⇒ UNKNOWN
		DK_VAR,
		DK_CONST,
		DK_LET,
		DK_FUNC,
		DK_ARG,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// アトムを取得・設定する
	inline const EsAtom* getAtom() const { return _atom; }
	inline void setAtom(const EsAtom* atom) { _atom = atom; }
	// 式ノードを取得・設定する
	inline EsItNode* getExpressionNode() const { return isHasLex() ? 0L : _nodeExpr; }		// ⇒ expr(), maybeExpr()
	inline void setExpressionNode(EsItNode* node) { _nodeExpr = node; }
	// 語彙定義ノードを持っているかどうかを取得する
	inline bool isHasLex() const { return _nodeLexDef != 0L; }	// ⇒ pn_used
	// 語彙定義ノードを取得・設定する
	inline EsNameItNode* getLexDefNode() const { return isHasLex() ? _nodeLexDef : 0L; }	// ⇒ lexdef(), maybeLexDef()
	inline void setLexDefNode(EsNameItNode* nodeLexDef) { _nodeLexDef = nodeLexDef; }
	// 定義フラグを得る
	inline u32 getDefFlag() const { return _defflag; }
	inline u32* defFlag() { return &_defflag; }
	// 上域変数情報を取得する
	inline const EsUpvarInfo* getUpvarInfo() const { return &_upvari; }
	inline EsUpvarInfo* upvarInfo() { return &_upvari; }
	// ブロック ID を取得・設定する
	inline u32 getBlockId() const { return _blockid; }
	inline void setBlockId(u32 blockid) { _blockid = blockid; }
	// 関数インデックスを取得・設定する
	inline u32 getFunctionIndex() const { return _indexFunc; }
	inline void setFunctionIndex(u32 funcidx) { _indexFunc = funcidx; }
	// 定義種別を取得する
	DefKind getDefKind() const;
	// 関数ボックスを取得・設定する
	inline EsFunctionBox* getFunctionBox() const { return (EsFunctionBox*)_objbox; }
	void setFunctionBox(EsFunctionBox* funcbox);
	// オブジェクト ボックスを取得・設定する
	inline EsObjectBox* getObjectBox() const { return _objbox; }
	inline void setObjectBox(EsObjectBox* objbox) { _objbox = objbox; }
	// ノードは定義ノードである
	virtual bool isDefNode() const { return TFW_IS_FLAG(_defflag, DF_EXTYPE_DEFINE); }

	// Up Var スキップレベル／スロットインデックスを設定する ⇒ pn->pn_cookie = MAKE_UPVAR_COOKIE(tc->staticLevel, index);
	inline void setUpvarInfo(u16 skip, u16 slot) { _upvari.setSkipDepth(skip), _upvari.setSlotIndex(slot); }
	inline void setUpvarInfo(const EsUpvarInfo* upvari) { _upvari = *upvari; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setFuncArg();
	EsNameItNode* findFinalDefNode();
	bool isFreeVar() const;
	bool isBindingDefinition() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsNameItNode()
		: EsBinaryItNode(NT_NAME)
		, _atom(0L)
		, _nodeExpr(0L)
		, _nodeLexDef(0L)
		, _defflag(0)
		, _blockid(0)
		, _indexFunc(0)
		, _objbox(0L)
	{
		_upvari.setFree();
	}
	virtual ~EsNameItNode()
	{
		#if DEL_TREE_CHILD
			delete _nodeExpr;
		#endif
	}
	virtual void copy(const EsNameItNode* src)
	{
		EsBinaryItNode::copy(src);
		_atom = src->_atom;
		_nodeExpr = src->_nodeExpr;
		_nodeLexDef = src->_nodeLexDef;
		_defflag = src->_defflag;
		_blockid = src->_blockid;
		_indexFunc = src->_indexFunc;
		_objbox = src->_objbox;
	}

	//======================================================================
	// 変数
private:
	const EsAtom*		_atom;			// 名称アトム
	EsItNode*			_nodeExpr;		// 式ノード ⇒ expr, pn_expr, pn_body
	EsNameItNode*		_nodeLexDef;	// 語彙（Lexical）定義ノード ⇒ lexdef
	u32					_defflag;		// 定義フラグ
	u32					_blockid;		// ブロック ID ⇒ blockid
	u32					_indexFunc;		// 関数インデックス ⇒ pn_index
	EsUpvarInfo			_upvari;		// 上域変数情報 ⇒ cookie, pn_cookie
	EsObjectBox*		_objbox;		// オブジェクト ボックス ⇒ objbox, funbox
};

/*---------------------------------------------------------------------*//**
 *	名称セットノード
 *
**//*---------------------------------------------------------------------*/
class EsNameSetItNode : public EsItNode
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 名称セットを取得する
	inline const EsAtomList*	getNamesSet() const				{	return _alistNames;		}
	inline EsAtomList*			namesSet()						{	return _alistNames;		}
	// 子ノードツリーを取得・設定する
	inline EsItNode*			getSubTree() const				{	return _nodeTree;			}
	inline void					setSubTree(EsItNode* node)		{	_nodeTree = node;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsNameSetItNode()
		: EsItNode(NT_NAMESET)
		, _alistNames(new EsAtomList())
		, _nodeTree(0L)
	{
	}
	virtual ~EsNameSetItNode()
	{
		delete _alistNames;
		#if DEL_TREE_CHILD
			delete _nodeTree;
		#endif
	}

	//======================================================================
	// 変数
private:
	EsAtomList*			_alistNames;	// 語彙リスト ⇒ names
	EsItNode*			_nodeTree;		// 子ノードツリー ⇒ tree
};

/*---------------------------------------------------------------------*//**
 *	ダブル値ノード
 *
**//*---------------------------------------------------------------------*/
class EsDoubleItNode : public EsItNode
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ダブル値を取得・設定する
	inline f64				getDouble() const					{	return _dval;				}
	inline void				setDouble(f64 dval)					{	_dval = dval;				}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsDoubleItNode()
		: EsItNode(NT_DOUBLE)
		, _dval(0.0)
	{
	}

	//======================================================================
	// 変数
private:
	f64					_dval;	// ⇒ pn_dval, dval
};

/*---------------------------------------------------------------------*//**
 *	リストノード
 *
**//*---------------------------------------------------------------------*/
class EsListItNode : public EsItNode
{
	//======================================================================
	// 定数
public:
	// リストフラグ ； listFlag() でアクセス ⇒ PNX_*
	static const u16	LF_GROUPINIT	= 0x0040;		// var [a, b] = [c, d] による変数グループ初期化 ⇒ PNX_GROUPINIT
	static const u16	LF_FORINVAR		= 0x0008;		// for(var xxx = yyy in zzz) による変数宣言 ⇒ PNX_FORINVAR
	static const u16	LF_FUNCDEF		= 0x0100;		// 関数定義 ⇒ PNX_FUNCDEFS
	static const u16	LF_NEED_BRACES	= 0x0080;		// 大括弧 {} が必要 ⇒ PNX_NEEDBRACES
	static const u16	LF_END_CAMMNA	= 0x0010;		// 配列は末尾にカンマ , がある ⇒ PNX_ENDCOMMA
	static const u16	LF_HAS_HOLES	= 0x0400;		// 配列初期化に空がある ⇒ PNX_HOLEY
	static const u16	LF_POPVAR		= 0x0004;		// 変数は最終結果からポップしてくる ⇒ PNX_POPVAR

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 先頭ノードを取得する
	inline EsItNode*		getHeadNode() const					{	return _nodeHead;			}
	// ノード数を取得する
	inline u32				getNodeCount() const				{	return _cntNode;			}
	// ブロック ID を取得・設定する
	inline u32				getBlockId() const					{	return _blockid;			}
	inline void				setBlockId(u32 blockid)				{	_blockid = blockid;			}
	// リストフラグを得る
	inline u16*				listFlag()							{	return &_lflag;				}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void resetList(EsItNode* nodeHead);
	u32 addListNode(EsItNode* node);
	EsItNode* getLastNode() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsListItNode()
		: EsItNode(NT_LIST)
		, _nodeHead(0L)
		, _cntNode(0)
		, _lflag(0)
	{
	}
	virtual ~EsListItNode()
	{
		#if DEL_TREE_CHILD
			delete _nodeHead;
		#endif
	}

	//======================================================================
	// 変数
private:
	EsItNode*			_nodeHead;
	u32					_cntNode;		// ノード数 ⇒ pn_count, pn_u.list.count
	u32					_blockid;		// ブロック ID ⇒ blockid
	u16					_lflag;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_IT_NODE_H_
