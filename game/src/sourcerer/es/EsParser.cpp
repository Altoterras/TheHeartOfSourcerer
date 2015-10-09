/***********************************************************************//**
 *	EsParser.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsParser.h"

// Friends
#include "EsArray.h"
#include "EsAtomList.h"
#include "EsBytecodeBuilder.h"
#include "EsContext.h"
#include "EsSysClasses.h"
#include "EsFunction.h"
#include "EsFunctionBox.h"
#include "EsInterlang.h"
#include "EsItNodeFactory.h"
#include "EsKeyword.h"
#include "EsKeywordSet.h"
#include "EsObjectBox.h"
#include "EsScope.h"
#include "EsScopeProperty.h"
#include "EsStatement.h"
#include "EsTokenParser.h"
#include "EsValue.h"

// External
#include "../../tfw/collection/List.h"
#include "../../tfw/lib/TypeUtils.h"
#include "../../tfw/string/VcStringArray.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define OPT_STRICT	0		// ⇒ JS_HAS_STRICT_OPTION(cx)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// 静的定数

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsParser::EsParser()
	: _ctxRef(0L)
	, _tknprs(0L)
	, _bcbCur(0L)
	, _cntFunc(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsParser::~EsParser()
{
}

/*---------------------------------------------------------------------*//**
	解析
**//*---------------------------------------------------------------------*/
EsInterlang* EsParser::parse(EsContext* ctxRef, const VcString* strScript, u32 bflags)
{
	_ctxRef = ctxRef;

	// トークンパーサーの作成
	EsTokenParser tknprs(ctxRef, strScript, TFW_IS_FLAG(_ctxRef->getBehaviorModeFlags(), ES_MF_PARSE_STRING_SIGN));
	_tknprs = &tknprs;

	// バイトコードビルダーの作成
	EsBytecodeBuilder bcb(_ctxRef);
	bcb.setExInfoAdd(TFW_IS_FLAG(_ctxRef->getBehaviorModeFlags(), ES_MF_ADD_DEBUG_INFO));
	bcb.setScopeChainObject(_ctxRef->getGlobalObject());
	TFW_SET_FLAG(*bcb.buildFlag(), bflags, true);
	_bcbCur = &bcb;

	// 解析処理
	bool isSucceeded = parseMain();

	// 中間言語インスタンスに移す
	EsInterlang* il = 0L;
	if(isSucceeded)
	{
		il = _ctxRef->newInterlang();
		if(!il->create(_bcbCur))		{	isSucceeded = false;	}
	}

	// 不要な解析用オブジェクトの削除
	_ctxRef->clearDeleteCollectionNodes();		// ノード削除リストを全削除
	_ctxRef->clearDeleteCollectionUnits(true);

	_tknprs = 0L;
	_bcbCur = 0L;

	if(!isSucceeded)
	{
		delete il;
		return 0L;
	}
	return il;
}

/*---------------------------------------------------------------------*//**
	関数の , セパレータにより列挙された仮引数を解析する
	⇒ Function の js_InitTokenStream ～ js_CloseTokenStream 部
**//*---------------------------------------------------------------------*/
bool EsParser::parseFunctionParameters(EsContext* ctxRef, EsFunction* func, const VcString* strScript)
{
	_ctxRef = ctxRef;

	// トークンパーサーの作成
	EsTokenParser tknprs(ctxRef, strScript, TFW_IS_FLAG(_ctxRef->getBehaviorModeFlags(), ES_MF_PARSE_STRING_SIGN));
	_tknprs = &tknprs;

	// バイトコードビルダーの作成
	EsBytecodeBuilder bcb(_ctxRef);
	bcb.setExInfoAdd(TFW_IS_FLAG(_ctxRef->getBehaviorModeFlags(), ES_MF_ADD_DEBUG_INFO));
	_bcbCur = &bcb;

	// ※※※ 未実装 ※※※ ⇒ js_InitTokenStream

	// , セパレータによる仮引数定義ループ
	bool isFormal = false;
	EsTokenType tt = _tknprs->nextToken();
	if(tt != TKN_EOF)
	{
		while(true)
		{
			if(tt != TKN_NAME)
			{
				goto END;
			}

			const EsAtom* atom = _tknprs->getCurToken()->getAtomValue();

			// 既に変数定義がないかチェック
			if(func->findLocalVar(0L, 0L, atom))	// ⇒ if (js_LookupLocal(cx, fun, atom, NULL) != JSLOCAL_NONE)
			{
				addCompileError(EsError::ID_DUPLICATE_PARAMETER, 0L);

				if(atom->getString() == 0L)
				{
					goto END;
				}
			}
			// 仮引数定義
			if(!func->addLocalVar(atom, EsFunction::LVK_ARG))	// ⇒ if (!js_AddLocal(cx, fun, atom, JSLOCAL_ARG))
			{
				goto END;
			}

			// , セパレーターをスキップ
			tt = _tknprs->nextToken();
			if(tt == TKN_EOF)
			{
				break;
			}
			if(tt != TKN_COMMA)
			{
				goto END;
			}

			tt = _tknprs->nextToken();
		}
	}

	isFormal = true;

END:
	if(!isFormal && TFW_IS_FLAG(_tknprs->getParseFlag(), EsTokenParser::PF_ERROR))	// ⇒ if (state == BAD_FORMAL && !(ts.flags & TSF_ERROR))
	{
		addCompileError(EsError::ID_BAD_FORMAL, 0L);
	}

	return isFormal;
}

/*---------------------------------------------------------------------*//**
	関数本体部を解析する ⇒ compileFunctionBody
**//*---------------------------------------------------------------------*/
bool EsParser::parseFunctionBody(EsContext* ctxRef, EsFunction* func, const VcString* strScript)
{
	_ctxRef = ctxRef;

	// トークンパーサーの作成
	EsTokenParser tknprs(ctxRef, strScript, TFW_IS_FLAG(_ctxRef->getBehaviorModeFlags(), ES_MF_PARSE_STRING_SIGN));
	_tknprs = &tknprs;

	// バイトコードビルダーの作成
	EsBytecodeBuilder bcb(_ctxRef);
	bcb.setExInfoAdd(TFW_IS_FLAG(_ctxRef->getBehaviorModeFlags(), ES_MF_ADD_DEBUG_INFO));
	_bcbCur = &bcb;
	TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION, true);	// ⇒ funcg.flags |= TCF_IN_FUNCTION;
	_bcbCur->setCurrentFunction(func);	// ⇒ funcg.fun = fun;
	_bcbCur->makeBlockId();				// ⇒ GenerateBlockId(&funcg, funcg.bodyid)

	// 関数ノードの作成
	_tknprs->curToken()->setType(TKN_NAME);
	EsNameItNode* nodeFunc = makeFuncNode(_tknprs->getCurToken()->getPos());
	nodeFunc->setExpressionNode(0L);	// ⇒ fn->pn_body = NULL;
	nodeFunc->upvarInfo()->setFree();	// ⇒ fn->pn_cookie = FREE_UPVAR_COOKIE;
	EsAtomList alistArgs;
	func->enumLocalAtoms(&alistArgs, EsFunction::LVK_ARG);
	if(alistArgs.getCount() == 0)
	{
		nodeFunc = 0L;
	}
	else
	{
		for(int i = 0; i < alistArgs.getCount(); i++)
		{
			const EsAtom* atomArg = alistArgs.getAtom(i);
			if(!defineArg(nodeFunc, atomArg, i))
			{
				nodeFunc = 0L;
				break;
			}
		}
	}

	// 本体の解析
	_tknprs->curToken()->setType(TKN_LC);
	EsItNode* nodeBody = 0L;
	if(nodeFunc != 0L)
	{
		nodeBody = parseFunctionBody();
		if(nodeBody != 0L)
		{
			do
			{
				if(!_tknprs->matchToken(TKN_EOF))	// ⇒ if (!js_MatchToken(cx, &jsc.tokenStream, TOK_EOF))
				{
					addCompileError(EsError::ID_SYNTAX_ERROR, 0L);
					nodeBody = 0L;
					break;
				}
				
				// ※※※ 実装保留 ※※※ 定数を束ねる処理 ⇒ js_FoldConstants(cx, pn, &funcg)

				if(_bcbCur->getFunctionBoxTree() != 0L)
				{
					u32 bflagsTmp = _bcbCur->getBuildFlag();
					_bcbCur->getFunctionBoxTree()->postprocFunction(&bflagsTmp);
				}

				EsItNode* nodeBodyRoot = nodeFunc->getExpressionNode();
				if(nodeBodyRoot != 0L)	// ⇒ if (fn->pn_body)
				{
					ASSERT(nodeBodyRoot->getTokenType() == TKN_ARGSBODY);
					ASSERT(nodeBodyRoot->getNodeType() == NT_LIST);
					((EsListItNode*)nodeBodyRoot)->addListNode(nodeBody);
					nodeBodyRoot->setPos(nodeBody->getPos());
					nodeBody = nodeBodyRoot;
				}

				if(!_bcbCur->buildFunctionBody(nodeBody))	// ⇒ if(!js_EmitFunctionScript(cx, &funcg, pn))
				{
					nodeBody = 0L;
					break;
				}

			}
			while(false);
		}
	}

	return nodeBody != 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンパイルエラーを追加する
**//*---------------------------------------------------------------------*/
void EsParser::addCompileError(EsError::Id id, VcStringArray* saParamDlg)
{
	_ctxRef->getError()->addCompileError(id, _tknprs->getCurScriptPos(), saParamDlg);
}

/*---------------------------------------------------------------------*//**
	単項演算子ノードを作成する
**//*---------------------------------------------------------------------*/
EsUnaryItNode* EsParser::makeUnaryNode(EsTokenType tt, EsOpcodeType ot, const EsTokenPos* pos, EsItNode* nodeChild)
{
	EsUnaryItNode* node = (EsUnaryItNode*)_ctxRef->getItNodeFactory()->make(_ctxRef, NT_UNARY);
	node->setTokenType(tt);
	node->setOpcodeType(ot);
	node->setPos(pos);
	node->setChildNode(nodeChild);
	return node;
}

/*---------------------------------------------------------------------*//**
	単項演算子ノードを作成する
**//*---------------------------------------------------------------------*/
EsBinaryItNode* EsParser::makeBinaryNode(EsTokenType tt, EsOpcodeType ot, const EsTokenPos* pos, EsItNode* nodeLeft, EsItNode* nodeRight)
{
	EsTokenPos posBin(*pos);
	if(nodeRight != 0L)	{	posBin.copyEnd(nodeRight->getPos());	}
	EsBinaryItNode* node = (EsBinaryItNode*)_ctxRef->getItNodeFactory()->make(_ctxRef, NT_BINARY);
	node->setTokenType(tt);
	node->setOpcodeType(ot);
	node->setPos(&posBin);
	node->setLeftNode(nodeLeft);
	node->setRightNode(nodeRight);
	return node;
}

/*---------------------------------------------------------------------*//**
	三項演算子ノードを作成する
**//*---------------------------------------------------------------------*/
EsTernaryItNode* EsParser::makeTernaryNode(EsTokenType tt, EsOpcodeType ot, const EsTokenPos* pos, EsItNode* node1, EsItNode* node2, EsItNode* node3)
{
	EsTokenPos posTer(*pos);
	if(node3 != 0L)			{ posTer.copyEnd(node3->getPos());	}
	else if(node2 != 0L)	{ posTer.copyEnd(node2->getPos());	}
	EsTernaryItNode* node = (EsTernaryItNode*)_ctxRef->getItNodeFactory()->make(_ctxRef, NT_TERNARY);
	node->setTokenType(tt);
	node->setOpcodeType(ot);
	node->setPos(&posTer);
	node->setChildNode1(node1);
	node->setChildNode2(node2);
	node->setChildNode3(node3);
	return node;
}

/*---------------------------------------------------------------------*//**
	NAME ノードを作成する ⇒ NewNameNode
**//*---------------------------------------------------------------------*/
EsNameItNode* EsParser::makeNameNode(EsOpcodeType ot, const EsTokenPos* pos, const EsAtom* atom, EsItNode* nodeExpr)
{
	EsNameItNode* node = (EsNameItNode*)_ctxRef->getItNodeFactory()->make(_ctxRef, NT_NAME);
	node->setTokenType(TKN_NAME);
	node->setOpcodeType(ot);
	node->setPos(pos);
	node->setAtom(atom);
	node->setExpressionNode(nodeExpr);

	if(_bcbCur->isTopLevelStatement())
	{
		TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_TOPLEVEL, true);
	}

	// ブロック内の子かどうかを判定し、フラグを設定する
	const EsStatement* stmtTop = _bcbCur->topEnclosingStatement();
	if((stmtTop != 0L) && (stmtTop->getType() == EsStatement::STMTT_BLOCK))
	{
		TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BLOCKCHILD, true);
	}

	node->setBlockId(_bcbCur->getCurrentBlockId());

	return node;
}

/*---------------------------------------------------------------------*//**
	置き換え用の代替語彙ノードを作成する ⇒ MakePlaceholder
**//*---------------------------------------------------------------------*/
EsNameItNode* EsParser::makePlaceholder(const EsAtom* atom)
{
	// 語彙ノードを作成する
	EsNameItNode* nodeDefNam = makeNameNode(OP_NOP, _tknprs->getCurToken()->getPos(), atom, 0L);
	if(nodeDefNam == 0L)	{	return 0L;	}

	// 代替用のためノードツリーには参加しない．そのためコンテキストの削除リストに手動追加する
	_ctxRef->addLeakNode(nodeDefNam);

	// 未解決アトムリストに追加する
	EsAtomList* alistUn = _bcbCur->getUnresolvedNameAtomList();
	EsAtomList::EsAtomListEntity* ale = alistUn->add(atom);
	if(ale == 0L)		{	return 0L;	}
	ale->setDefName(nodeDefNam);

	TFW_SET_FLAG(*nodeDefNam->defFlag(), EsNameItNode::DF_PLACEHOLDER | EsNameItNode::DF_EXTYPE_DEFINE, true);	// 代替語彙フラグを立てる

	return nodeDefNam;
}

/*---------------------------------------------------------------------*//**
	リストノードを作成する
**//*---------------------------------------------------------------------*/
EsListItNode* EsParser::makeListNode(EsTokenType tt, EsOpcodeType ot, const EsTokenPos* pos, EsItNode* nodeListHead)
{
	EsListItNode* node = (EsListItNode*)_ctxRef->getItNodeFactory()->make(_ctxRef, NT_LIST);
	node->setTokenType(tt);
	node->setOpcodeType(ot);
	node->setPos(pos);
	if(nodeListHead != 0L)	{	node->addListNode(nodeListHead);	}
	return node;
}

/*---------------------------------------------------------------------*//**
	代入演算子ノードを作成する ⇒ MakeAssignment
**//*---------------------------------------------------------------------*/
EsNameItNode* EsParser::makeAssignmentNode(EsNameItNode* nodeName, EsItNode* nodeRight)
{
	EsNameItNode* nodeLeft = (EsNameItNode*)_ctxRef->getItNodeFactory()->make(_ctxRef, NT_NAME);
	nodeLeft->copy(nodeName);

	if(nodeName->isHasLex())	// ⇒ if (pn->pn_used) {
	{
		EsNameItNode* nodeDef = nodeName->getLexDefNode();

		EsItNode* nodeWkCur = nodeDef->getLinkNode();	// ⇒ JSParseNode **pnup = &dn->dn_uses;
		EsItNode* nodeWkPrev = nodeWkCur;
		while(nodeWkCur != nodeName)
		{
			nodeWkPrev = nodeWkCur;
			nodeWkCur = nodeWkCur->getLinkNode();			// ⇒ &(*pnup)->pn_link;
		}
		nodeWkPrev->setLinkNode(nodeLeft);				// ⇒ *pnup = lhs;
		nodeLeft->setLinkNode(nodeName->getLinkNode());	// ⇒ lhs->pn_link = pn->pn_link;
		nodeName->setLinkNode(0L);						// ⇒ pn->pn_link = NULL;
	}

	nodeName->setTokenType(TKN_ASSIGN);
	nodeName->setOpcodeType(OP_NOP);
	nodeName->setLeftNode(nodeLeft);
	nodeName->setRightNode(nodeRight);

	return nodeLeft;
}

/*---------------------------------------------------------------------*//**
	ダブル値ノードを作成する
**//*---------------------------------------------------------------------*/
EsDoubleItNode* EsParser::makeDoubleNode(const EsTokenPos* pos, f64 dval)
{
	EsDoubleItNode* node = (EsDoubleItNode*)_ctxRef->getItNodeFactory()->make(_ctxRef, NT_DOUBLE);
	node->setTokenType(TKN_NUMBER);
	node->setOpcodeType(OP_DOUBLE);
	node->setPos(pos);
	node->setDouble(dval);
	return node;
}

/*---------------------------------------------------------------------*//**
	文字列値ノードを作成する
**//*---------------------------------------------------------------------*/
EsNameItNode* EsParser::makeStringNode(EsTokenType tt, EsOpcodeType ot, const EsTokenPos* pos, const EsAtom* atom)
{
	EsNameItNode* node = (EsNameItNode*)_ctxRef->getItNodeFactory()->make(_ctxRef, NT_NAME);	// ⇒ pn = NewParseNode(PN_NULLARY, tc);
	if(node == 0L)	{	return 0L;	}
	node->setTokenType(tt);
	node->setAtom(atom);
	node->setOpcodeType(ot);
	node->setPos(pos);
	return node;
}

/*---------------------------------------------------------------------*//**
	関数ノードを作成する
**//*---------------------------------------------------------------------*/
EsNameItNode* EsParser::makeFuncNode(const EsTokenPos* pos)
{
	EsNameItNode* node = (EsNameItNode*)_ctxRef->getItNodeFactory()->make(_ctxRef, NT_NAME);
	node->setTokenType(_tknprs->getCurToken()->getType());
	TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_EXTYPE_FUNC, true);
	node->setPos(pos);
	return node;
}

/*---------------------------------------------------------------------*//**
	空のノードを作成する
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::makeNullaryNode()
{
	EsItNode* node = _ctxRef->getItNodeFactory()->make(_ctxRef, NT_NULLARY);
	if(node == 0L)	{	return 0L;	}
	node->setTokenType(_tknprs->getCurToken()->getType());
	node->setOpcodeType(_tknprs->getCurToken()->getOpcodeTypeValue());
	node->setPos(_tknprs->getCurToken()->getPos());
	return node;
}

/*---------------------------------------------------------------------*//**
	セミコロントークンで終行するか確認 ⇒ MatchOrInsertSemicolon
**//*---------------------------------------------------------------------*/
bool EsParser::matchOrInsertSemicolonToken()
{
	EsTokenType tt = _tknprs->peekTokenSameLine();
	switch(tt)
	{
	case TKN_ERROR:
		return false;
	case TKN_SEMI:
	case TKN_RC:
	case TKN_EOL:
	case TKN_EOF:
		break;
	default:
		addCompileError(EsError::ID_STMT_AFTER_SEMICOLON, 0L);
		return false;
	}

	_tknprs->matchToken(TKN_SEMI);	// セミコロンの場合はポインタを送る

	return true;
}

/*---------------------------------------------------------------------*//**
	必ず一致しなければならないトークンの確認
		一致しない場合はエラー
**//*---------------------------------------------------------------------*/
bool EsParser::mustMatchToken(EsTokenType tt, EsError::Id errid)
{
	if(_tknprs->nextToken() != tt)
	{
		ESCASSERT(false);
		addCompileError(errid, 0L);
		return false;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	ラベル取得
**//*---------------------------------------------------------------------*/
bool EsParser::matchLabel(const EsAtom** atom)
{
	EsTokenType tt = _tknprs->peekTokenSameLine();
	switch(tt)
	{
	case TKN_ERROR:
		return false;
	case TKN_NAME:
		_tknprs->nextToken();
		*atom = _tknprs->getCurToken()->getAtomValue();
		return true;
	default:
		*atom = 0L;
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	左辺値のフラグをセットする ⇒ NoteLValue
**//*---------------------------------------------------------------------*/
void EsParser::setLeftValueFlags(EsItNode* node, u32 defflag)
{
	if(node->getNodeType() != NT_NAME)
	{
		ASSERT(false);
		return;
	}

	EsNameItNode* nodeNam = (EsNameItNode*)node;
	if(nodeNam->isHasLex())	// ⇒ if (pn->pn_used) {
	{
		EsNameItNode* nodeDef = nodeNam->getLexDefNode(); ASSERT(nodeDef != 0L);

		if(	!TFW_IS_FLAG(nodeDef->getDefFlag(), EsNameItNode::DF_INITED | EsNameItNode::DF_CONST | EsNameItNode::DF_PLACEHOLDER) &&
			TFW_IS_FLAG(nodeDef->getDefFlag(), EsNameItNode::DF_BLOCKCHILD) &&
			TFW_IS_FLAG(nodeNam->getDefFlag(), EsNameItNode::DF_BLOCKCHILD) &&
			(nodeDef->getBlockId() == nodeNam->getBlockId()) &&
			(nodeDef->getPos()->getEnd()->le(nodeDef->getPos()->getBegin())) &&
			(nodeDef->getLinkNode() == nodeNam)	)
		{
			defflag |= EsNameItNode::DF_INITED;
		}

		TFW_SET_FLAG(*nodeDef->defFlag(), defflag, true);

		u32 depthDef = nodeDef->getUpvarInfo()->getSkipDepth();	// ⇒ dn->frameLevel()
		if(depthDef != _bcbCur->getNestDepth())				// ⇒ if (dn->frameLevel() != tc->staticLevel)
		{
			TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_FUNC_SETS_OUTER_NAME, true);
		}
	}

	TFW_SET_FLAG(*nodeNam->defFlag(), defflag, true);

	// arguments プロパティの場合
	if(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ARGUMENTS)->getAtom() == nodeNam->getAtom())	// ⇒ if (pn->pn_atom == cx->runtime->atomState.argumentsAtom)
	{
		TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ, true);
	}
}

/*---------------------------------------------------------------------*//**
	左辺値としての子ノードを設定・取得する
	⇒ SetLvalKid
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::setLeftValueChild(EsItNode* nodeParent, EsItNode* nodeChild)
{
	while(nodeChild->getTokenType() == TKN_RP)
	{
		nodeChild = nodeChild->getChildNode();
		if(nodeChild == 0L)
		{
			ASSERT(false);
			addCompileError(EsError::ID_BAD_MEMORY_ACCESS, 0L);
			return 0L;
		}
	}

	EsTokenType tt = nodeChild->getTokenType();
	if(	(tt != TKN_NAME)
		&& (tt != TKN_DOT)
		&& (tt != TKN_LB)	)
	{
		addCompileError(EsError::ID_BAD_OPERAND, 0L);
		return 0L;
	}

	nodeParent->setChildNode(nodeChild);
	return nodeChild;
}

/*---------------------------------------------------------------------*//**
	加算（++）・減算（--）ノードの命令コードを得る
	@param	isPreOrder	++i:true / i++:false
**//*---------------------------------------------------------------------*/
EsOpcodeType EsParser::getIncrementOpcodeType(EsTokenType tt, EsItNode* nodeChild, bool isPreOrder)
{
	EsOpcodeType ot = OP_NULL;

	switch(nodeChild->getTokenType())
	{
	case TKN_NAME:
		if(tt == TKN_INC)	{	ot = isPreOrder ? OP_INCNAME : OP_NAMEINC;	}
		else				{	ot = isPreOrder ? OP_DECNAME : OP_NAMEDEC;	}
		break;
	case TKN_DOT:
		if(tt == TKN_INC)	{	ot = isPreOrder ? OP_INCPROP : OP_PROPINC;	}
		else				{	ot = isPreOrder ? OP_DECPROP : OP_PROPDEC;	}
		break;
	case TKN_LB:
		if(tt == TKN_INC)	{	ot = isPreOrder ? OP_INCELEM : OP_ELEMINC;	}
		else				{	ot = isPreOrder ? OP_DECELEM : OP_ELEMDEC;	}
		break;
	default:
		ESCASSERT(false);
		ot = OP_NOP;
		break;
	}

	return ot;
}

/*---------------------------------------------------------------------*//**
	加算（++）・減算（--）子ノードを設定する ⇒ SetIncOpKid

	@param	isPreOrder	++i:true / i++:false
**//*---------------------------------------------------------------------*/
bool EsParser::setIncrementChild(EsItNode* nodeParent, EsItNode* nodeChild, bool isPreOrder)
{
	nodeChild = setLeftValueChild(nodeParent, nodeChild);
	if(nodeChild == 0L)	{	return false;	}

	nodeParent->setOpcodeType( getIncrementOpcodeType(nodeParent->getTokenType(), nodeChild, isPreOrder) );
	return true;
}

/*---------------------------------------------------------------------*//**
	変数（または定数）データを設定する ⇒ BindVarOrConst
**//*---------------------------------------------------------------------*/
bool EsParser::bindVarOrConstData(EsNameItNode* node, EsOpcodeType otBind, bool* isFreshBind)
{
	const EsAtom* atom = node->getAtom();
	EsAtomList::EsAtomListEntity* ale;

	EsStatement* stmt = 0L;
	_bcbCur->findLexical(&stmt, 0L, atom, 0L); // ⇒ js_LexicalLookup

	if((stmt != 0L) && (stmt->getType() == EsStatement::STMTT_WITH))
	{
		node->setOpcodeType(OP_NAME);
		*isFreshBind = false;	// ⇒ data->fresh = false;
		return true;
	}

	EsAtomList* alistDecl = _bcbCur->getDeclaredNameAtomList();
	ale = alistDecl->find(atom);	// ⇒ JSAtomListElement *ale = tc->decls.lookup(atom);

	if((stmt != 0L) || (ale != 0L))
	{
		EsNameItNode* nodeDef = (ale != 0L) ? ale->getDefName() : 0L;		// ⇒ JSDefinition *dn = ale ? ALE_DEFN(ale) : NULL;
		EsNameItNode::DefKind defkind = (nodeDef != 0L) ? nodeDef->getDefKind() : EsNameItNode::DK_VAR;	// ⇒ JSDefinition::Kind dn_kind = dn ? dn->kind() : JSDefinition::VAR;

		if(defkind == EsNameItNode::DK_ARG)	// 既存の定義が引数の場合
		{
			const VcString* name = atom->getString();
			if(name == 0L)	{	return false;	}

			VcStringArray sarr(name);
			if(otBind == OP_DEFCONST)
			{
				addCompileError(EsError::ID_REDECLARED_PARAM, &sarr);
			}
			else
			{
				addCompileError(EsError::ID_VAR_HIDES_ARG, &sarr);
			}
			return false;
		}
		else	// 既存の定義が引数以外の場合
		{
			// エラー判定
			bool isError =
				(otBind == OP_DEFCONST) ||
				(defkind == EsNameItNode::DK_CONST) ||
				((defkind == EsNameItNode::DK_LET) && (stmt->getType() != EsStatement::STMTT_CATCH));
			if(!isError && (defkind == EsNameItNode::DK_LET))
			{
				// ⇒ OuterLet
				bool isOuterLet = false;
				EsStatement* stmtWk = stmt;
				while(stmtWk->getDownScopeStatement() != 0L)	// ⇒　while (stmt->downScope) {
				{
					_bcbCur->findLexical(&stmtWk, 0L, atom, stmtWk->getDownScopeStatement()); // ⇒ stmt = js_LexicalLookup(tc, atom, NULL, stmt->downScope);
					if(stmtWk == 0L)
					{
						break;
					}
					if(stmtWk->getType() == EsStatement::STMTT_BLOCK)
					{
						isOuterLet = true;
						break;
					}
				}

				if(isOuterLet)
				{
					isError = true;
				}
			}

			// 再定義時のエラー
			if(OPT_STRICT ? ((otBind != OP_DEFVAR) || (defkind != EsNameItNode::DK_VAR)) : isError)
			{
				const VcString* name = atom->getString();	// ⇒ name = js_AtomToPrintableString(cx, atom);
				if(name == 0L)	{	return false;	}

				VcStringArray sarr(name);
				addCompileError(EsError::ID_REDECLARED_VAR, &sarr);
				return false;
			}
		}
	}

	if(ale == 0L)
	{
		if(!defineDeclaredName(node, atom, false))
		{
			return false;
		}
	}
	else
	{
		EsNameItNode* nodeDefNam = ale->getDefName();	// ⇒ JSDefinition *dn = ALE_DEFN(ale);

		*isFreshBind = false;	// ⇒ data->fresh = false;

		if(!node->isHasLex())	// ⇒ if (!pn->pn_used) {
		{
			EsNameItNode* nodeWk = node;
			if(TFW_IS_FLAG(node->getDefFlag(), EsNameItNode::DF_EXTYPE_DEFINE))	// ⇒ if (pn->pn_defn)
			{
				nodeWk = makeNameNode(OP_NOP, _tknprs->getCurToken()->getPos(), atom, 0L); // ⇒ pnu = NewNameNode(cx, TS(tc->compiler), atom, tc);
				if(nodeWk == 0L)
				{
					return false;
				}
			}

			ASSERT(nodeDefNam != 0L);
			setLexDefNodeLink(nodeWk, nodeDefNam);	// ⇒ LinkUseToDef(pnu, dn, tc);
			nodeWk->setOpcodeType(OP_NAME);
		}

		ASSERT(nodeDefNam != 0L);
		while(nodeDefNam->getDefKind() == EsNameItNode::DK_LET)
		{
			while(ale != 0L)
			{
				// 次のエントリーを得る（遅さが気になったら改善）
				u32 index = alistDecl->getListIndex(ale);
				ale = alistDecl->getEntity(index + 1);
				if(ale->getAtom() == atom)	{	break;	}
			}
			if(ale == 0L)	{	break;	}
			nodeDefNam = ale->getDefName();
		}

		if(ale != 0L)
		{
			return true;
		}

		if(!TFW_IS_FLAG(node->getDefFlag(), EsNameItNode::DF_EXTYPE_DEFINE))	// ⇒ if (!pn->pn_defn)
		{
			// 未解決語彙リストから探す
			EsAtomList* alistUn = _bcbCur->getUnresolvedNameAtomList();
			ale = alistUn->find(atom);	// ⇒ ale = tc->lexdeps.rawLookup(atom, hep);
			if(ale != 0L)
			{
				node = ale->getDefName();
				alistUn->remove(ale);	// ⇒ tc->lexdeps.rawRemove(tc->compiler, ale, hep);
			}
			else
			{
				EsNameItNode* nodeWk = makeNameNode(OP_NOP, node->getPos(), atom, 0L);	// ⇒ JSParseNode *pn2 = NewNameNode(cx, TS(tc->compiler), atom, tc);
				if(nodeWk == 0L)
				{
					return false;
				}
				nodeWk->setTokenType(TKN_NAME);
				node = nodeWk;
			}
			node->setOpcodeType(OP_NAME);
		}

		ale = alistDecl->add(atom, EsAtomList::AH_TAIL);	// ⇒ ale = tc->decls.add(tc->compiler, atom, JSAtomList::HOIST);
		if(ale == 0L)
		{
			return false;
		}
		ale->setDefName(node);	// ⇒ ALE_SET_DEFN(ale, pn);
		TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_EXTYPE_DEFINE, true);	// ⇒ pn->pn_defn = true;
		TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_PLACEHOLDER, false);	// 仮実装フラグを下ろす ⇒ pn->pn_dflags &= ~PND_PLACEHOLDER;
	}

	if(otBind == OP_DEFCONST)
	{
		TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_CONST, true);
	}

	if(!TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION))
	{
		node->setOpcodeType(OP_NAME);

		if(_bcbCur->getCallerStackFrame() == 0L)	// ⇒ if ((tc->flags & TCF_COMPILING) && !tc->compiler->callerFrame) {
		{
			// グローバルリテラルに追加
			ale = _bcbCur->literalAtomList()->add(node->getAtom());	// ⇒ ale = cg->atomList.add(tc->compiler, atom);
			if(ale == 0L)	{	return false;	}

			// 最大大域変数数を増やす
			s32 slotidx = ale->getIndex(); // ⇒ uintN slot = ALE_INDEX(ale);
			if((slotidx + 1) >= _bcbCur->getGlobalVarsNum())
			{
				_bcbCur->setGlobalVarsNum(slotidx + 1);	// ⇒ cg->ngvars = (uint16)(slot + 1);
			}

			node->setOpcodeType(OP_GETGVAR);
			node->setUpvarInfo(0, slotidx);
			TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_GVAR | EsNameItNode::DF_BOUND_LG, true);
		}

		return true;
	}

	// arguments プロパティの場合
	if(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ARGUMENTS)->getAtom() == atom)	// ⇒ if (atom == cx->runtime->atomState.argumentsAtom) {
	{
		node->setOpcodeType(OP_ARGUMENTS);
		TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG, true);
		return true;
	}

	EsFunction* func = _bcbCur->getCurrentFunction();	ASSERT(func != 0L);
	EsFunction::LocalVarKind lvk;
	if(!func->findLocalVar(&lvk, 0L, atom))	// ⇒ JSLocalKind localKind = js_LookupLocal(cx, tc->fun, atom, NULL);
	{
		lvk = (otBind == OP_DEFCONST) ? EsFunction::LVK_CONST : EsFunction::LVK_VAR;

		u32 index = func->getLocalVarNum(EsFunction::LVK_VAR);	// ⇒ uintN index = tc->fun->u.i.nvars;
		if(!func->addLocalVar(atom, lvk))	// ⇒ if (!BindLocalVariable(cx, tc->fun, atom, localKind))
		{
			return false;
		}
		node->setOpcodeType(OP_GETLOCAL);
		node->setUpvarInfo(_bcbCur->getNestDepth(), (u16)index);
		TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG, true);
		return true;
	}

	if(lvk == EsFunction::LVK_ARG)
	{
		ASSERT((ale != 0L) && (ale->getDefName()->getDefKind() == EsNameItNode::DK_ARG));
	}
	else
	{
		ASSERT((lvk == EsFunction::LVK_VAR) || (lvk == EsFunction::LVK_CONST));
	}

	node->setOpcodeType(OP_NAME);

	return true;
}

/*---------------------------------------------------------------------*//**
	let データを設定する ⇒ BindLet
**//*---------------------------------------------------------------------*/
bool EsParser::bindLetData(EsNameItNode* node, EsOpcodeType otBind, bool* isFreshBind)
{
	EsObject* objBlock = _bcbCur->getBlockChainObject();

	const EsAtom* atom = node->getAtom();
	EsAtomList* alistDecl = _bcbCur->getDeclaredNameAtomList();
	EsAtomList::EsAtomListEntity* ale = alistDecl->find(atom);	// ⇒ ale = tc->decls.lookup(atom);

	if((ale != 0L) && ale->getDefName()->getBlockId() == _bcbCur->getCurrentBlockId())
	{
		const VcString* name = atom->getString();	// ⇒ const char *name = js_AtomToPrintableString(cx, atom);
		if(name != 0L)
		{
			VcStringArray sarr(name);
			addCompileError(EsError::ID_REDECLARED_VAR, &sarr);
		}
		ESCASSERT(false);
		return false;
	}

	// スコープのエントリー数チェック
	s32 entrynum = objBlock->getScope()->getScopePropertyNum();
	if(entrynum > 0xffff)
	{
		addCompileError(EsError::ID_TOO_MANY_SCORE_ENTRIES, 0L);
		ESCASSERT(false);
		return false;
	}

	// 変数定義
	if(!defineDeclaredName(node, atom, true))
	{
		return false;
	}
	
	// ノードの属性を変更する
	node->setOpcodeType(OP_GETLOCAL);
	node->setUpvarInfo((u16)_bcbCur->getNestDepth(), entrynum);
	TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_LET | EsNameItNode::DF_BOUND_LG, true);

	// ブロックオブジェクトにプロパティとして変数を定義する
	EsValue valAtom(atom);
	EsValue valVoid;
	EsScopeProperty* sprop = objBlock->definePropertyNative(_ctxRef, &valAtom, &valVoid, 0L, 0L, EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED, EsScopeProperty::OPTF_HAS_SHORTID, (s16)entrynum, false);
	if(sprop == 0L)
	{
		return false;
	}

	// スロットインデックスの算出と、スロットへのノードの保存
	u32 slotidx = objBlock->getClass()->getFreeSlotIndex() + entrynum;	// ⇒ uintN slot = JSSLOT_FREE(&js_BlockClass) + n;
	if(slotidx >= (u32)objBlock->getSlots()->getNum())
	{
		if(!objBlock->slots()->recreate(slotidx + 1))
		{
			return false;
		}
	}
	objBlock->getScope()->setFreeSlotIndex(slotidx + 1);
	objBlock->slots()->value(slotidx)->setPrivateData(node);

	return true;
}

/*---------------------------------------------------------------------*//**
	let データを再設定する ⇒ RebindLets
**//*---------------------------------------------------------------------*/
bool EsParser::rebindLetData(EsItNode* node)
{
	if(node == 0L)	{	return false;	}

	switch(node->getNodeType())
	{
	case NT_NAME:
		{
			EsNameItNode* nodeName = (EsNameItNode*)node;
			rebindLetData(nodeName->getExpressionNode());

			if(TFW_IS_FLAG(nodeName->getDefFlag(), EsNameItNode::DF_EXTYPE_DEFINE))	// ⇒ if (pn->pn_defn)
			{
				ASSERT(nodeName->getBlockId() > _bcbCur->getBodyBlockId());
			}
			else if(nodeName->isHasLex())
			{
				EsNameItNode* nodeLex = nodeName->getLexDefNode();
				if(nodeLex->getBlockId() == _bcbCur->getBodyBlockId())
				{
					unsetLexDefNodeLink(nodeName);	// ⇒ ForgetUse(pn);

					// 宣言済み語彙ノードがある場合はそれをリンクさせて抜ける
					EsAtomList* alistDecl = _bcbCur->getDeclaredNameAtomList();
					EsAtomList::EsAtomListEntity* ale = alistDecl->find(nodeName->getAtom());	// ⇒ JSAtomListElement *ale = tc->decls.lookup(pn->pn_atom);
					if(ale != 0L)
					{
						while(true)
						{
							ale = alistDecl->getEntity(ale->getIndex() + 1);	// ⇒ ale = ALE_NEXT(ale)
							if(ale == 0L)	{	break;	}
							if(ale->getAtom() == nodeName->getAtom())
							{
								setLexDefNodeLink(nodeName, ale->getDefName());	// ⇒ LinkUseToDef(pn, ALE_DEFN(ale), tc);
								return true;
							}

						}
					}

					// 未解決語彙リストから探す
					EsNameItNode* nodeDefNam = 0L;
					EsAtomList* alistUn = _bcbCur->getUnresolvedNameAtomList();
					ale = alistUn->find(nodeName->getAtom());	// ⇒ ale = tc->lexdeps.lookup(pn->pn_atom);
					if(ale != 0L)
					{
						nodeDefNam = ale->getDefName();
					}
					else
					{
						// 代替語彙ノードを作成する
						nodeDefNam = makePlaceholder(nodeName->getAtom());
						if(nodeDefNam == 0L)	{	return false;	}
						TFW_SET_FLAG(*nodeDefNam->defFlag(), EsNameItNode::DF_FUNCARG, true);
					}
					ASSERT(nodeDefNam != 0L);
					setLexDefNodeLink(nodeName, nodeDefNam);	// ⇒ LinkUseToDef(pn, ALE_DEFN(ale), tc);
				}
			}
		}
		break;
	case NT_UNARY:
		rebindLetData(node->getChildNode());
		break;
	case NT_BINARY:
		rebindLetData(((EsBinaryItNode*)node)->getLeftNode());
		rebindLetData(((EsBinaryItNode*)node)->getRightNode());
		break;
	case NT_TERNARY:
		rebindLetData(((EsTernaryItNode*)node)->getChildNode1());
		rebindLetData(((EsTernaryItNode*)node)->getChildNode2());
		rebindLetData(((EsTernaryItNode*)node)->getChildNode3());
		break;
	case NT_LIST:
		for(EsItNode* nodeWk = ((EsListItNode*)node)->getHeadNode(); nodeWk != 0L; nodeWk = nodeWk->getNextNode())
		{
			rebindLetData(nodeWk);
		}
		break;
	case NT_NAMESET:
		rebindLetData(((EsNameSetItNode*)node)->getSubTree());
		break;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	語彙ノードをリンクする ⇒ LinkUseToDef
**//*---------------------------------------------------------------------*/
void EsParser::setLexDefNodeLink(EsNameItNode* node, EsNameItNode* nodeDef)
{
	ASSERT(node->getLinkNode() == 0L);
	ASSERT(nodeDef->getLinkNode() != node);
	node->setLinkNode(nodeDef->getLinkNode());
	nodeDef->setLinkNode(node);
	node->setLexDefNode(nodeDef);
	node->setExpressionNode(0L);
}

/*---------------------------------------------------------------------*//**
	語彙ノードをリンクを解除する ⇒ ForgetUse
**//*---------------------------------------------------------------------*/
void EsParser::unsetLexDefNodeLink(EsNameItNode* node)
{
	if(!node->isHasLex())	
	{
		ASSERT(!node->isDefNode());
		return;
	}

	EsNameItNode* nodeDef = node->getLexDefNode();

	EsItNode* nodeWkCur = nodeDef->getLinkNode();	// ⇒ JSParseNode **pnup = &pn->lexdef()->dn_uses;
	EsItNode* nodeWkPrev = nodeWkCur;
	while(nodeWkCur != node)
	{
		nodeWkPrev = nodeWkCur;
		nodeWkCur = nodeWkCur->getLinkNode();			// ⇒ pnup = &pnu->pn_link;
	}
	nodeWkPrev->setLinkNode(node->getLinkNode());	// ⇒ *pnup = pn->pn_link;

	// 語彙ノードではなくす ⇒ pn->pn_used = false;
	node->setLexDefNode(0L);
	node->setExpressionNode(nodeDef);
}

/*---------------------------------------------------------------------*//**
	ノードを SETCALL に設定する ⇒ MakeSetCall
**//*---------------------------------------------------------------------*/
bool EsParser::setSetCall(EsItNode* node)
{
	ASSERT((node->getOpcodeType() == OP_CALL) || (node->getOpcodeType() == OP_EVAL) || (node->getOpcodeType() == OP_APPLY));

	ASSERT(node->getNodeType() == NT_LIST);
	EsListItNode* nodeList = (EsListItNode*)node;

	EsItNode* nodeWk = nodeList->getHeadNode();
	if(nodeWk->getTokenType() == TKN_FUNCTION)
	{
		EsFunctionBox* funcbox = ((EsNameItNode*)nodeWk)->getFunctionBox();
		if(TFW_IS_FLAG(funcbox->getBuildFlag(), EsBytecodeBuilder::BF_GENEXP_LAMBDA))
		{
			return false;
		}
	}
	node->setOpcodeType(OP_SETCALL);
	return true;
}

/*---------------------------------------------------------------------*//**
	名前を定義する（変数、定数、関数名など）を定義する ⇒ Define
**//*---------------------------------------------------------------------*/
bool EsParser::defineDeclaredName(EsNameItNode* node, const EsAtom* atom, bool isLet)
{
	ASSERT(!node->isHasLex());

	EsAtomList* alist = 0L;
	EsAtomList::EsAtomListEntity* ale = 0L;

	if(isLet)
	{
		// 宣言済みリストから探す ⇒ ale = (list = &tc->decls)->rawLookup(atom, hep);
		alist = _bcbCur->getDeclaredNameAtomList();
		ale = alist->find(atom);
	}
	if(ale == 0L)
	{
		// 未解決リストから探す ⇒ ale = (list = &tc->lexdeps)->rawLookup(atom, hep);
		alist = _bcbCur->getUnresolvedNameAtomList();
		ale = alist->find(atom);
	}

	if(ale != 0L)	// 既に存在する場合
	{
		EsNameItNode* nodeDef = ale->getDefName();	// ⇒ ALE_DEFN(ale);
		ASSERT(nodeDef != 0L);
		if(nodeDef != node)	// 今回定義するノードと違う名前ノードの場合
		{
			EsItNode* nodeUpCur = nodeDef->getLinkNode();
			EsItNode* nodeUpPrev = nodeUpCur;
			u32 blockidStart = isLet ? node->getBlockId() : _bcbCur->getBodyBlockId();	// ⇒ uintN start = let ? pn->pn_blockid : tc->bodyid;
			while(nodeUpCur != 0L)
			{
				ASSERT(nodeUpCur->getNodeType() == NT_NAME);
				{
					EsNameItNode* nodeNameUp = (EsNameItNode*)nodeUpCur;
					if(nodeNameUp->getBlockId() < blockidStart)
					{
						break;
					}
					ASSERT(nodeNameUp->isHasLex());
					nodeNameUp->setLexDefNode(node);	// ⇒ pnu->pn_lexdef = (JSDefinition *) pn;
					*node->defFlag() |= *nodeNameUp->defFlag() & (EsNameItNode::DF_ASSIGNED | EsNameItNode::DF_FUNCARG);
				}

				nodeUpPrev = nodeUpCur;
				nodeUpCur = nodeUpCur->getLinkNode();
			}

			if(nodeUpCur != nodeDef->getLinkNode())	// ⇒ if (pnu != dn->dn_uses)
			{
				ASSERT(nodeUpPrev != 0L);
				nodeUpPrev->setLinkNode(node->getLinkNode());
				node->setLinkNode(nodeDef->getLinkNode());	// ⇒ pn->dn_uses = dn->dn_uses;
				nodeDef->setLinkNode(nodeUpCur);

				if(((nodeUpCur == 0L) || (((EsNameItNode*)nodeUpCur)->getBlockId() < _bcbCur->getBodyBlockId())) && (alist != _bcbCur->getDeclaredNameAtomList()))
				{
					alist->remove(ale);
				}
			}
		}
	}

	// 宣言済みリストに追加する
	ale = _bcbCur->getDeclaredNameAtomList()->add(atom, isLet ? EsAtomList::AH_HEAD : EsAtomList::AH_UNIQUE);
	if(ale == 0L)
	{
		ESCASSERT(false);
		return false;
	}

	ale->setDefName(node);	// ⇒ ALE_SET_DEFN(ale, pn);
	TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_EXTYPE_DEFINE, true);
	TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_PLACEHOLDER, false);	// 仮実装フラグを下ろす ⇒ pn->pn_dflags &= ~PND_PLACEHOLDER;
	_ctxRef->removeLeakNode(node);	// ツリーに参加したため、コンテキストの削除リストから手動除外する

	return true;
}

/*---------------------------------------------------------------------*//**
	関数の引数を定義する ⇒ DefineArg
**//*---------------------------------------------------------------------*/
bool EsParser::defineArg(EsNameItNode* nodeFunc, const EsAtom* atom, s32 indexArg)
{
	EsNameItNode* nodeArg = makeNameNode(OP_GETARG, _tknprs->getCurToken()->getPos(), _tknprs->getCurToken()->getAtomValue(), 0L);
	if(nodeArg == 0L)
	{
		return false;
	}
	nodeArg->setUpvarInfo((u16)_bcbCur->getNestDepth(), indexArg);	// ⇒ argpn->pn_cookie = MAKE_UPVAR_COOKIE(tc->staticLevel, i);
	TFW_SET_FLAG(*nodeArg->defFlag(), EsNameItNode::DF_INITED | EsNameItNode::DF_BOUND_LG, true);

	// 定義
	if(!defineDeclaredName(nodeArg, atom, false))
	{
		return false;
	}

	// リストノードを作成
	EsItNode* nodeBody = nodeFunc->getExpressionNode();
	if(nodeBody == 0L)
	{
		nodeBody = makeListNode(TKN_ARGSBODY, OP_NOP, _tknprs->getCurToken()->getPos(), 0L);
		if(nodeBody == 0L)
		{
			return false;
		}
		nodeFunc->setExpressionNode(nodeBody);
	}
	ASSERT(nodeBody->getNodeType() == NT_LIST);
	((EsListItNode*)nodeBody)->addListNode(nodeArg);	// ⇒ argsbody->append(argpn);

	return true;
}

/*---------------------------------------------------------------------*//**
	語彙ノードの中から定義ノードを作成する ⇒ MakeDefIntoUse
**//*---------------------------------------------------------------------*/
bool EsParser::defineFuncIntoLexDefNode(EsNameItNode* nodeDef, EsNameItNode* nodeFuncName, const EsAtom* atom)
{
	// 定義が var, const, let の場合は初期化子を持っている
	// 代入 = （AssignmentNode） ノードに変える
	if(nodeDef->isBindingDefinition())
	{
		EsItNode* nodeRightSide = nodeDef->getExpressionNode();
		if(nodeRightSide != 0L)
		{
			EsNameItNode* nodeLeftSide = makeAssignmentNode(nodeDef, nodeRightSide);
			if(nodeLeftSide == 0L)	{	return false;	}
			nodeDef = nodeLeftSide;
		}
	}
	else if(nodeDef->getDefKind() == EsNameItNode::DK_FUNC)
	{
		ASSERT(TFW_IS_FLAG(nodeDef->getDefFlag(), EsNameItNode::DF_TOPLEVEL));
		ASSERT(nodeDef->getOpcodeType() == OP_NOP);
		nodeDef->setTokenType(TKN_NAME);
		nodeDef->setAtom(atom);
	}
	ASSERT(nodeDef->getTokenType() == TKN_NAME);
	ASSERT(nodeDef->getAtom() == atom);

	for(EsItNode* nodeWk = nodeDef->getLinkNode(); nodeWk != 0L; nodeWk = nodeWk->getLinkNode())
	{
		ASSERT(nodeWk->getNodeType() == NT_NAME);
		if(nodeWk->getNodeType() == NT_NAME)
		{
			EsNameItNode* nodeWkName = (EsNameItNode*)nodeWk;
			ASSERT(nodeWkName->isHasLex());
			ASSERT(!TFW_IS_FLAG(nodeWkName->getDefFlag(), EsNameItNode::DF_EXTYPE_DEFINE));
			nodeWkName->setLexDefNode(nodeFuncName);
			*nodeFuncName->defFlag() |= nodeWkName->getDefFlag() & (EsNameItNode::DF_ASSIGNED | EsNameItNode::DF_FUNCARG);
		}
	}
	*nodeFuncName->defFlag() |= nodeDef->getDefFlag() & (EsNameItNode::DF_ASSIGNED | EsNameItNode::DF_FUNCARG);
	nodeFuncName->setLinkNode(nodeDef);	// ⇒ pn->dn_uses = dn;

	TFW_SET_FLAG(*nodeDef->defFlag(), EsNameItNode::DF_EXTYPE_DEFINE, false);	// ⇒ dn->pn_defn = false;
	nodeDef->setLexDefNode(nodeFuncName);
	nodeDef->upvarInfo()->setFree();											// ⇒ dn->pn_cookie = FREE_UPVAR_COOKIE;
	TFW_SET_FLAG(*nodeDef->defFlag(), EsNameItNode::DF_BOUND_LG, false);		// ⇒ dn->pn_dflags &= ~PND_BOUND;

	return true;
}

/*---------------------------------------------------------------------*//**
	関数定義に入る ⇒ EnterFunction
**//*---------------------------------------------------------------------*/
EsFunctionBox* EsParser::enterFunction(EsNameItNode* nodeFunc, EsBytecodeBuilder* bcbFunc, const EsAtom* atomFunc, bool isLambda)
{
	EsFunction* func = makeFunction(atomFunc, isLambda);
	if(func == 0L)
	{
		return 0L;
	}
	EsFunctionBox* funcbox = makeFunctionBox(func, nodeFunc);
	if(funcbox == 0L)
	{
		return 0L;
	}

	bcbFunc->inheritForChildBlockParseStart(_bcbCur);
	bcbFunc->setCurrentFunction(func);
	bcbFunc->setCurrentFunctionBox(funcbox);

	return funcbox;
}

/*---------------------------------------------------------------------*//**
	関数定義を抜ける ⇒ LeaveFunction
**//*---------------------------------------------------------------------*/
bool EsParser::leaveFunction(EsNameItNode* nodeFunc, EsBytecodeBuilder* bcbFunc, const EsAtom* atomFunc, bool isLambda)
{
	// 各種フラグ設定
	_bcbCur->inheritForChildBlockParseEnd(bcbFunc);
	// バイトコードビルダーの生成フラグを継承する
	*nodeFunc->getFunctionBox()->buildFlag() |= bcbFunc->getBuildFlag() & EsBytecodeBuilder::BF_FUNC_FLAGS;
	// 定義による初期化済フラグを設定する
	TFW_SET_FLAG(*nodeFunc->defFlag(), EsNameItNode::DF_INITED, true);
	// ブロック内の子かどうかを判定し、フラグを設定する
	const EsStatement* stmtTop = _bcbCur->topEnclosingStatement();
	if((stmtTop != 0L) && (stmtTop->getType() == EsStatement::STMTT_BLOCK))
	{
		TFW_SET_FLAG(*nodeFunc->defFlag(), EsNameItNode::DF_BLOCKCHILD, true);
	}

	// 関数内の未解決語彙の処理
	EsAtomList* alistLexdeps = bcbFunc->getUnresolvedNameAtomList();
	if(alistLexdeps->getCount() > 0)
	{
		bool isExistCallee = false;

		for(s32 i = 0; i < alistLexdeps->getCount(); i++)
		{
			EsAtomList::EsAtomListEntity* ale = alistLexdeps->getEntity(i);
			const EsAtom* atom = ale->getAtom();
			EsNameItNode* nodeDefNam = ale->getDefName();	// ⇒ ALE_DEFN(ale);
			ASSERT(TFW_IS_FLAG(nodeDefNam->getDefFlag(), EsNameItNode::DF_PLACEHOLDER));

			if((atom == atomFunc) && isLambda)
			{
				nodeDefNam->setOpcodeType(OP_CALLEE);
				nodeDefNam->setUpvarInfo((u16)bcbFunc->getNestDepth(), EsUpvarInfo::SLOTIDX_CALEE_UPVAR);	// MAKE_UPVAR_COOKIE(funtc->staticLevel, CALLEE_UPVAR_SLOT);
				TFW_SET_FLAG(*nodeDefNam->defFlag(), EsNameItNode::DF_BOUND_LG, true);

				// 関数は（パラメータ名を除いた）引数を使用する
				if(TFW_IS_FLAG(nodeDefNam->getDefFlag(), EsNameItNode::DF_FUNCARG))
				{
					*nodeFunc->getFunctionBox()->buildFlag() |= bcbFunc->getBuildFlag() & EsBytecodeBuilder::BF_FUNC_USES_ARGUMENTS;
				}

				isExistCallee = true;
				continue;
			}

			if(TFW_IS_FLAG(*nodeFunc->getFunctionBox()->buildFlag(), EsBytecodeBuilder::BF_FUNC_SETS_OUTER_NAME))	// ⇒ if (!(fn->pn_funbox->tcflags & TCF_FUN_SETS_OUTER_NAME)
			{
				if(TFW_IS_FLAG(*nodeDefNam->defFlag(), EsNameItNode::DF_ASSIGNED))
				{
					for(EsItNode* nodeWk = nodeDefNam->getLinkNode(); nodeWk != 0L; nodeWk = nodeWk->getLinkNode())
					{
						ASSERT(nodeWk->getNodeType() == NT_NAME);
						if(	TFW_IS_FLAG(*((EsNameItNode*)nodeWk)->defFlag(), EsNameItNode::DF_ASSIGNED) &&
							(((EsNameItNode*)nodeWk)->getBlockId() >= bcbFunc->getBodyBlockId()) )
						{
							TFW_SET_FLAG(*nodeFunc->getFunctionBox()->buildFlag(), EsBytecodeBuilder::BF_FUNC_SETS_OUTER_NAME, true);
						}
					}
				}
			}

			// 外ブロックツリーノードから語彙検索
			EsAtomList::EsAtomListEntity* aleOuter = _bcbCur->getDeclaredNameAtomList()->find(atom);
			if(aleOuter == 0L)
			{
				aleOuter = _bcbCur->getUnresolvedNameAtomList()->find(atom);
			}
			if(aleOuter != 0L)
			{
				EsNameItNode* nodeOuterDefNam = aleOuter->getDefName();
				if(nodeDefNam != nodeOuterDefNam)
				{
					EsItNode* nodeWkCur = nodeDefNam->getLinkNode();	// ⇒ JSParseNode **pnup = &dn->dn_uses;
					EsItNode* nodeWkPrev = nodeWkCur;
					while(nodeWkCur != 0L)
					{
						ASSERT(nodeWkCur->getNodeType() == NT_NAME);
						((EsNameItNode*)nodeWkCur)->setLexDefNode(nodeOuterDefNam);	// ⇒ pnu->pn_lexdef = outer_dn;

						nodeWkPrev = nodeWkCur;
						nodeWkCur = nodeWkCur->getLinkNode();	// ⇒ pnup = &pnu->pn_link;
					}

					// 外ブロックの語彙定義を使用するように設定を変える
					nodeWkPrev->setLinkNode(nodeOuterDefNam->getLinkNode());						// ⇒ *pnup = outer_dn->dn_uses;
					nodeOuterDefNam->setLinkNode(nodeDefNam);										// ⇒ outer_dn->dn_uses = dn;
					*nodeOuterDefNam->defFlag() |= nodeDefNam->getDefFlag() & ~EsNameItNode::DF_PLACEHOLDER;
					TFW_SET_FLAG(*nodeDefNam->defFlag(), EsNameItNode::DF_EXTYPE_DEFINE, false);	// ⇒ dn->pn_defn = false;
					nodeDefNam->setLexDefNode(nodeOuterDefNam);
				}
			}
			else
			{
				aleOuter = _bcbCur->getUnresolvedNameAtomList()->add(atom);
				if(aleOuter == 0L)
				{
					return false;
				}
				aleOuter->setDefName(nodeDefNam);
			}
		}

		// まだ残っている場合
		if((alistLexdeps->getCount() - (isExistCallee ? 1 : 0)) > 0)	// ⇒ if (funtc->lexdeps.count - foundCallee != 0)
		{
			// 既存の関数本体を名称セットノードの下にぶら下げる
			EsItNode* nodeBodySub = nodeFunc->getExpressionNode();
			EsNameSetItNode* nodeBodyNew = (EsNameSetItNode*)_ctxRef->getItNodeFactory()->make(_ctxRef, NT_NAMESET);
			nodeBodyNew->setTokenType(TKN_UPVARS);
			nodeBodyNew->setPos(nodeBodySub->getPos());
			if(isExistCallee)
			{
				alistLexdeps->remove(atomFunc);
			}
			nodeBodyNew->namesSet()->copy(alistLexdeps);
			nodeFunc->setExpressionNode(nodeBodyNew);
			nodeBodyNew->setSubTree(nodeBodySub);
		}

		alistLexdeps->removeAll();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	関数を作成する ⇒ JSCompiler::newFunction
**//*---------------------------------------------------------------------*/
EsFunction* EsParser::makeFunction(const EsAtom* atomFunc, bool isLambda)
{
	EsBytecodeBuilder* bcb = _bcbCur;
	while(bcb->getParent() != 0L)
	{
		bcb = bcb->getParent();
	}
	EsObject* objParent = TFW_IS_FLAG(bcb->getBuildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION) ? 0L : bcb->getScopeChainObject();

	EsFunction* func = _ctxRef->newFunction(atomFunc, 0L, objParent, isLambda);
	return func;
}

/*---------------------------------------------------------------------*//**
	関数ボックスを作成する ⇒ JSCompiler::newFunctionBox
**//*---------------------------------------------------------------------*/
EsFunctionBox* EsParser::makeFunctionBox(EsFunction* objFunc, EsNameItNode* nodeFunc)
{
	EsFunctionBox* funcbox = new EsFunctionBox();
	_ctxRef->registerCustomUnit(funcbox);	// 削除リストに追加

	// ループ内かどうか
	bool isInLoop = false;
	for(EsStatement* si = _bcbCur->topEnclosingStatement(); si != 0L; si = si->getDownEnclosingStatement())
	{
		if(si->isLoop())
		{
			isInLoop = true;
			break;
		}
	}

	// 基本設定
	funcbox->setup(
		nodeFunc,
		objFunc,
		_bcbCur->getCurrentFunctionBox(),
		_bcbCur->getFunctionBoxTree(),
		isInLoop,
		_bcbCur->getNestDepth(),
		_bcbCur->getBuildFlag()	);
	_bcbCur->setFunctionBoxTree(funcbox);

	_cntFunc++;

	return funcbox;
}

/*---------------------------------------------------------------------*//**
	関数ボックスを作成する ⇒ JSCompiler::newObjectBox
**//*---------------------------------------------------------------------*/
EsObjectBox* EsParser::makeObjectBox(EsObject* obj)
{
	EsObjectBox* objbox = new EsObjectBox();
	_ctxRef->registerCustomUnit(objbox);	// 削除リストに追加

	// 基本設定
	objbox->setup(
		obj,
		0L,
		0L	);

	return objbox;
}

/*---------------------------------------------------------------------*//**
	語彙スコープを追加する ⇒ PushLexicalScope
**//*---------------------------------------------------------------------*/
EsNameItNode* EsParser::pushLexicalScope(EsStatement* stmt)
{
	// ブロックオブジェクトを作成
	EsObject* objBlock = _ctxRef->newObjectWithGivenPrototype(EsSysClasses::getClass(EsSysClasses::CLASSTYPE_BLOCK), 0L, 0L);	// ⇒ obj = js_NewBlockObject(cx);
	if(objBlock == 0L)		{	return 0L;	}

	// ブロックオブジェクトボックスを作成
	EsObjectBox* objboxBlock = makeObjectBox(objBlock);
	if(objboxBlock == 0L)	{	return 0L;	}

	// バイトコードビルダに積む
	_bcbCur->pushBlockScope(stmt, objBlock, -1);
	u32 blockid = _bcbCur->makeBlockId();

	// 語彙スコープノードを作成する
	EsNameItNode* nodeLexScope = makeNameNode(OP_LEAVEBLOCK, _tknprs->getCurToken()->getPos(), _tknprs->getCurToken()->getAtomValue(), 0L);
	if(nodeLexScope == 0L)	{	return 0L;	}
	nodeLexScope->setTokenType(TKN_LEXICALSCOPE);
	nodeLexScope->setObjectBox(objboxBlock);
	nodeLexScope->upvarInfo()->setFree();		// ⇒ pn->pn_cookie = FREE_UPVAR_COOKIE;
	nodeLexScope->setBlockId(blockid);
	stmt->setBlockId(blockid);

	return nodeLexScope;
}

/*---------------------------------------------------------------------*//**
	arguments フラグを設定する ⇒ NoteArgumentsUse
**//*---------------------------------------------------------------------*/
void EsParser::setArgumentsFlags()
{
	ASSERT(TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION));
	TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_FUNC_USES_ARGUMENTS, true);
	if(_bcbCur->getCurrentFunctionBox() != 0L)
	{
		TFW_SET_FLAG(*_bcbCur->getCurrentFunctionBox()->getFuncNode()->defFlag(), EsNameItNode::DF_FUNCARG, true); 
	}
}

/*---------------------------------------------------------------------*//**
	直接適用されるラムダ式を調整 ⇒ CheckForImmediatelyAppliedLambda
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::checkImmediatelyAppliedLambda(EsItNode* node)
{
	while(node->getTokenType() == TKN_RP)
	{
		node = node->getChildNode();
	}
	if(node->getTokenType() == TKN_FUNCTION)
	{
		ASSERT(node->getNodeType() == NT_NAME);
		EsFunctionBox* funcbox = ((EsNameItNode*)node)->getFunctionBox();
		ASSERT(TFW_IS_FLAG(*funcbox->getFunction()->flags(), EsFunction::F_LAMBDA));
		if(!TFW_IS_FLAG(*funcbox->buildFlag(), EsBytecodeBuilder::BF_FUNC_USES_ARGUMENTS))
		{
			TFW_SET_FLAG(*((EsNameItNode*)node)->defFlag(), EsNameItNode::DF_FUNCARG, false);
		}
	}
	return node;
}

/*---------------------------------------------------------------------*//**
	ブロック ID がスコープ内科をチェックする ⇒ BlockIdInScope
**//*---------------------------------------------------------------------*/
bool EsParser::checkBlockIdInScope(u32 blockid)
{
	if(blockid > _bcbCur->getCurrentBlockId())
	{
		return false;
	}
	for(EsStatement* stmt = _bcbCur->topScopeStatement(); stmt != 0L; stmt = stmt->getDownScopeStatement())
	{
		if(stmt->getBlockId() == blockid)
		{
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	解析メイン ⇒ compileScript
**//*---------------------------------------------------------------------*/
bool EsParser::parseMain()
{
	s32 cntResolvExtFuncCode = 0;
	while(true)
	{
		EsTokenType tt = _tknprs->peekToken();
		if(tt == TKN_EOF)
		{
			// コンテキストに終了確認を行う
			if(_ctxRef->checkParseEnd())
			{
				break;
			}
			else
			{
				// 外部関数解決は２回までに制限
				cntResolvExtFuncCode++;
				if(cntResolvExtFuncCode >= 3)
				{
					return false;
				}

				// 追加スクリプトで解析再開
				_tknprs->restartExtendScript();
				continue;
			}
		}
		if(tt == TKN_ERROR)
		{
			return false;
		}

		// 命令文解析
		EsItNode* node = parseStatement();
		if(node == 0L)
		{
			return false;
		}

		// ※※※ 実装保留 ※※※ 定数を束ねる処理 ⇒ js_FoldConstants(cx, pn, &cg)

		// 関数解析の仕上げ処理
		if(_bcbCur->getFunctionBoxTree() != 0L)
		{
			u32 bflagsTmp = _bcbCur->getBuildFlag();
			_bcbCur->getFunctionBoxTree()->postprocFunction(&bflagsTmp);
			*_bcbCur->buildFlag() |= bflagsTmp;
		}

		// バイトコードを構築する ⇒ js_EmitTree
		if(!_bcbCur->buildFromTree(node))
		{
			return false;
		}

		_ctxRef->addLeakNode(node);	// ノード削除リストに追加する
	}

	// 最終調整処理
	if(!_bcbCur->finalize())
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	命令文解析 ⇒ Statement
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatement()
{
	EsTokenType tt = _tknprs->nextToken();

	switch(tt)
	{
	case TKN_FUNCTION:	return parseFunctionDefine(false);
	case TKN_IF:		return parseStatementSub_if();
	case TKN_SWITCH:	return parseStatementSub_switch();
	case TKN_WHILE:		return parseStatementSub_while();
	case TKN_DO:		return parseStatementSub_do();
	case TKN_FOR:		return parseStatementSub_for();
	case TKN_TRY:		/* ※※※ 実装保留 ※※※ */ return 0L;
	case TKN_THROW:		/* ※※※ 実装保留 ※※※ */ return 0L;
	case TKN_CATCH:		/* ※※※ 実装保留 ※※※ */ return 0L;
	case TKN_FINALLY:	/* ※※※ 実装保留 ※※※ */ return 0L;
	case TKN_BREAK:		return parseStatementSub_break();
	case TKN_CONTINUE:	return parseStatementSub_continue();
	case TKN_WITH:		return parseStatementSub_with();
	case TKN_VAR:		return parseStatementSub_var();
	case TKN_LET:		return parseStatementSub_let();
	case TKN_RETURN:	return parseStatementSub_return();
	case TKN_LC:		return parseStatementSub_lc();
	case TKN_EOL:		return parseStatementSub_semi();
	case TKN_SEMI:		return parseStatementSub_semi();
	case TKN_ERROR:		return 0L;
	default:			return parseStatementSub_other();
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - if 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_if()
{
	EsTokenType ttIf = _tknprs->getCurToken()->getType();
	EsOpcodeType otIf = _tknprs->getCurToken()->getOpcodeTypeValue();
	EsTokenPos posIf = *_tknprs->getCurToken()->getPos();

	// 条件文
	EsItNode* nodeCond = parseCondition();
	if(nodeCond == 0L)	{	return 0L;	}

	// 子ステートメント
	EsStatement stmt(EsStatement::STMTT_IF);
	_bcbCur->pushStatement(&stmt, -1);

	// 条件を満たしたときの文
	EsItNode* nodeChild = parseStatement();
	if(nodeChild == 0L)	{	return 0L;	}

	// else
	EsItNode* nodeElse = 0L;
	if(_tknprs->matchToken(TKN_ELSE))
	{
		stmt.setType(EsStatement::STMTT_ELSE);
		nodeElse = parseStatement();
		if(nodeElse == 0L)	{	return 0L;	}
		posIf.copyEnd(nodeElse->getPos());
	}
	else
	{
		posIf.copyEnd(nodeChild->getPos());
	}

	// ステートメントを戻す
	_bcbCur->popStatement();

	// If ノードを作成する
	EsTernaryItNode* nodeIf = makeTernaryNode(ttIf, otIf, &posIf, nodeCond, nodeChild, nodeElse);
	return nodeIf;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - switch 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_switch()
{
	EsTokenType ttSwitch = _tknprs->getCurToken()->getType();
	EsOpcodeType otSwitch = _tknprs->getCurToken()->getOpcodeTypeValue();
	const EsTokenPos* posSwitch = _tknprs->getCurToken()->getPos();
	
	// ( が来るはずである
	if(!mustMatchToken(TKN_LP, EsError::ID_PARENTHESIS_SWITCH_LEFT))		{	return 0L;	}

	// switch( x ) の x 部
	EsItNode* nodeDisc = parseExpression();
	if(nodeDisc == 0L)	{	return 0L;	}

	// ) が来るはずである
	if(!mustMatchToken(TKN_RP, EsError::ID_PARENTHESIS_SWITCH_RIGHT))	{	return 0L;	}
	// { が来るはずである
	if(!mustMatchToken(TKN_LC, EsError::ID_CURLY_SWITCH_LEFT))			{	return 0L;	}

	// 新しいステートメントを積む
	EsStatement stmt(EsStatement::STMTT_SWITCH);	// ⇒ js_PushStatement(tc, &stmtInfo, STMT_SWITCH, -1);
	_bcbCur->pushStatement(&stmt, -1);

	// case リストノードを作成する
	EsListItNode* nodeCaseList = makeListNode(_tknprs->getCurToken()->getType(), _tknprs->getCurToken()->getOpcodeTypeValue(), _tknprs->getCurToken()->getPos(), 0L);
	if(nodeCaseList == 0L)	{	return 0L;	}
	
	// ステートメントノード用にブロック ID を生成する ⇒ GenerateBlockIdForStmtNode
	ASSERT(_bcbCur->topEnclosingStatement() != 0L);
	ASSERT(_bcbCur->topEnclosingStatement()->isPerhapsScope());
	ASSERT((nodeCaseList->getTokenType() == TKN_LC) || (nodeCaseList->getTokenType() == TKN_LEXICALSCOPE));
	u32 blockid = _bcbCur->makeBlockId();
	_bcbCur->topEnclosingStatement()->setBlockId(blockid);
	nodeCaseList->setBlockId(blockid);

	// ブロックノードを設定
	EsItNode* nodeBlockBk = _bcbCur->getCurrentBlockNode();
	_bcbCur->setCurrentBlockNode(nodeCaseList);

	bool isFoundDefault = false;
	while(true)
	{
		EsTokenType tt = _tknprs->nextToken();
		if(tt == TKN_RC)	{	break;	}		// } まで

		// case もしくは default の解析
		EsBinaryItNode* nodeCase = 0L;
		switch(tt)
		{
		case TKN_DEFAULT:
			if(isFoundDefault)
			{
				addCompileError(EsError::ID_MULTIPLE_SWITCH_DEFAULT, 0L);
				return 0L;
			}
			isFoundDefault = true;
			// ↓そのまま処理
		case TKN_CASE:
			{
				EsTokenType ttCase = _tknprs->getCurToken()->getType();
				EsOpcodeType otCase = _tknprs->getCurToken()->getOpcodeTypeValue();
				const EsTokenPos* posCase = _tknprs->getCurToken()->getPos();
				EsItNode* nodeExpr = 0L;
				if(tt == TKN_CASE)
				{
					nodeExpr = parseExpression();	// 式を解析
					if(nodeExpr == 0L)	{	return 0L;	}
				}
				nodeCase = makeBinaryNode(ttCase, otCase, posCase, nodeExpr, 0L);
				if(nodeCase == 0L)	{	return 0L;	}
				nodeCaseList->addListNode(nodeCase);
			}
			break;
		case TKN_ERROR:
			return 0L;
		default:
			addCompileError(EsError::ID_BAD_SWITCH_SYNTAX, 0L);
			return 0L;
		}
		// : が来るはずである
		if(!mustMatchToken(TKN_COLON, EsError::ID_COLON_AFTER_CASE))	{	return 0L;	}
		
		// case の内容を解析する
		EsListItNode* nodeCaseBodyList = makeListNode(_tknprs->getCurToken()->getType(), _tknprs->getCurToken()->getOpcodeTypeValue(), _tknprs->getCurToken()->getPos(), 0L);
		if(nodeCaseBodyList == 0L)	{	return 0L;	}
		nodeCaseBodyList->setTokenType(TKN_LC);	// {
		/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, true);
		while(true)
		{
			tt = _tknprs->peekToken();
			if((tt == TKN_RC) || (tt == TKN_CASE) || (tt == TKN_DEFAULT))	{	break;	}		// }, case, default まで
			/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, false);
			if(tt == TKN_ERROR)	{	return 0L;	}

			// 文の解析
			EsItNode* nodeCaseBody = parseStatement();
			if(nodeCaseBody == 0L)	{	return 0L;	}

			// リストに追加
			nodeCaseBodyList->pos()->copyEnd(nodeCaseBody->getPos());
			nodeCaseBodyList->addListNode(nodeCaseBody);
			/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, true);
		}
		/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, false);

		// 位置の調整
		if(nodeCaseBodyList->getHeadNode() != 0L)
		{
			nodeCaseBodyList->pos()->copyBegin(nodeCaseBodyList->getHeadNode()->getPos());
		}
		nodeCase->pos()->copyEnd(nodeCaseBodyList->getPos());

		// case ノードの右辺に追加
		nodeCase->setRightNode(nodeCaseBodyList);
	}

	// case の let 宣言によるブロックスコープの対応
	EsItNode* nodeRight = nodeCaseList;
	if(_bcbCur->getCurrentBlockNode() != nodeRight)
	{
		nodeRight = _bcbCur->getCurrentBlockNode();
	}
	_bcbCur->setCurrentBlockNode(nodeBlockBk);

	// 積んだステートメントブロックを取り出す
	_bcbCur->popStatement();

	// switch ノード作成
	nodeRight->pos()->copyEnd(_tknprs->getCurToken()->getPos());
	EsItNode* nodeSwitch = makeBinaryNode(ttSwitch, otSwitch, posSwitch, nodeDisc, nodeRight);
	return nodeSwitch;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - while 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_while()
{
	EsTokenType ttWhile = _tknprs->getCurToken()->getType();
	EsOpcodeType otWhile = _tknprs->getCurToken()->getOpcodeTypeValue();
	const EsTokenPos* posWhile = _tknprs->getCurToken()->getPos();

	// 新しいステートメントブロックを積む
	EsStatement stmt(EsStatement::STMTT_WHILE);
	_bcbCur->pushStatement(&stmt, -1);

	// 条件文
	EsItNode* nodeCond = parseCondition();
	if(nodeCond == 0L)	{	return 0L;	}

	// ループ内容
	EsItNode* nodeStmt = parseStatement();
	if(nodeStmt == 0L)	{	return 0L;	}

	// 積んだステートメントブロックを取り出す
	_bcbCur->popStatement();

	// while ノード作成、条件とループ内容ノードを与える
	EsBinaryItNode* nodeWhile = makeBinaryNode(ttWhile, otWhile, posWhile, nodeCond, nodeStmt);
	return nodeWhile;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - do 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_do()
{
	EsTokenType ttDo = _tknprs->getCurToken()->getType();
	EsOpcodeType otDo = _tknprs->getCurToken()->getOpcodeTypeValue();
	const EsTokenPos* posDo = _tknprs->getCurToken()->getPos();

	// 新しいステートメントを積む
	EsStatement stmt(EsStatement::STMTT_DO);	// ⇒ js_PushStatement(tc, &stmtInfo, STMT_DO_LOOP, -1);
	_bcbCur->pushStatement(&stmt, -1);

	// ループの内容文
	EsItNode* nodeStmt = parseStatement();
	if(nodeStmt == 0L)	{	return 0L;	}

	// while で終わるはずである
	if(!mustMatchToken(TKN_WHILE, EsError::ID_WHILE_AFTER_DO))	{	return 0L;	}	// ⇒ MUST_MATCH_TOKEN(TOK_WHILE, JSMSG_WHILE_AFTER_DO);

	// ループ条件
	EsItNode* nodeCond = parseCondition();
	if(nodeCond == 0L)	{	return 0L;	}

	// ステートメントを戻す
	_bcbCur->popStatement();

	// バイナリノードに条件とループ内容ノードを設定する
	EsBinaryItNode* nodeDo = makeBinaryNode(ttDo, otDo, posDo, nodeStmt, nodeCond);

	// セミコロンの場合はポインタを送る
	_tknprs->matchToken(TKN_SEMI);	// ⇒ js_MatchToken(cx, ts, TOK_SEMI);

	return nodeDo;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - for 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_for()
{
	bool isLet = false;
	u32 iterflagsFor = 0;
	EsTokenType ttFor = _tknprs->getCurToken()->getType();
	EsOpcodeType otFor = _tknprs->getCurToken()->getOpcodeTypeValue();
	const EsTokenPos* posFor =  _tknprs->getCurToken()->getPos();
	EsItNode* nodeForHead = 0L;			// for ヘッド、つまり、for(x) の x 部ノード
	EsItNode* nodeForBody = 0L;			// for ループ内のノード
	EsItNode* nodeForVars = 0L;			// for ヘッド１項目の変数定義部ノード（in の場合も含む）
	EsNameItNode* nodeLet = 0L;
	EsListItNode* nodeSeq = 0L;
	EsBinaryItNode* nodeFor;
	EsItNode* nodeRet;
	bool isEnumerate;
	
	// 新しいステートメントを積む
	EsStatement stmt(EsStatement::STMTT_FOR);	// ⇒ js_PushStatement(tc, &stmtInfo, STMT_FOR_LOOP, -1);
	_bcbCur->pushStatement(&stmt, -1);

	otFor = OP_ITER;
	#if 0	// ※※※ 未実装 ※※※ each はサポートしない
		if(_tknprs->matchToken(TKN_NAME))
		{
			const EsAtom* atom = _tknprs->getCurToken()->getAtomValue();
			const EsKeyword* kw = _ctxRef->getKeywordSet()->findWord(EACH);
			if(false)
			{
				TFW_SET_FLAG(iterflagsFor, EsBinaryItNode::ITERF_FOREACH, true);
			}
			else
			{
				_tknprs->ungetToken();
			}
		}
	#endif

	// ( が来るはずである
	if(!mustMatchToken(TKN_LP, EsError::ID_PARENTHESIS_FOR_LEFT))	{	return 0L;	}

	// 次のトークンを得る
	/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, true);
	EsTokenType tt = _tknprs->peekToken();
	/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, false);

	// for の変数リスト
	if(tt == TKN_SEMI)	// 変数リストなし
	{
		if(TFW_IS_FLAG(iterflagsFor, EsBinaryItNode::ITERF_FOREACH))
		{
			goto ERROR_FOREACH;
		}
	}
	else	// 変数リストあり
	{
		TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_IN_FOR_INIT, true);	// ⇒ tc->flags |= TCF_IN_FOR_INIT;

		if(tt == TKN_VAR)		// 変数定義
		{
			_tknprs->nextToken();
			nodeForVars = parseVariables(false);
		}
		else if(tt == TKN_LET)	// let 文
		{
			isLet = true;
			_tknprs->nextToken();
			if(_tknprs->peekToken() == TKN_LP)
			{
				nodeForVars = parseLet(false);
				tt = TKN_LEXICALSCOPE;
			}
			else
			{
				EsStatement stmtBlock;
				nodeLet = pushLexicalScope(&stmtBlock);
				if(nodeLet == 0L)	{	return 0L;	}
				TFW_SET_FLAG(*stmtBlock.blockFlags(), EsStatement::BLOCKF_FOR, true);
				nodeForVars = parseVariables(false);
			}
		}
		else
		{
			nodeForVars = parseExpression();
			if(nodeForVars != 0L)
			{
				// 右括弧“)”が終わるまで送る
				while((nodeForVars != 0L) && (nodeForVars->getTokenType() == TKN_RP))
				{
					nodeForVars = nodeForVars->getChildNode();
				}
			}
		}

		TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_IN_FOR_INIT, false);	// ⇒ tc->flags &= ~TCF_IN_FOR_INIT;
		if(nodeForVars == 0L)	{	return 0L;	}
	}

	// for(xxx in xxx) xxx ループ
	isEnumerate = false;
	if((nodeForVars != 0L) && _tknprs->matchToken(TKN_IN))
	{
		TFW_SET_FLAG(iterflagsFor, EsBinaryItNode::ITERF_ENUMERATE, true);
		stmt.setType(EsStatement::STMTT_FORIN);

		// in の左辺が正常か確認する
		{
			bool isError = false;
			if((tt == TKN_VAR) || (tt == TKN_LET)) 	// ⇒ TOKEN_TYPE_IS_DECL(tt)
			{
				ASSERT(nodeForVars->getNodeType() == NT_LIST);
				if(	(((EsListItNode*)nodeForVars)->getNodeCount() > 1) ||
					(nodeForVars->getOpcodeType() == OP_DEFCONST)	)
				{
					isError = true;
				}
			}
			else switch(nodeForVars->getTokenType())
			{
			case TKN_NAME:
			case TKN_DOT:
			case TKN_LP:
			case TKN_LB:
				break;
			default:
				isError = true;
				break;
			}
			if(isError)
			{
				addCompileError(EsError::ID_BAD_FORIN_LEFTSIDE, 0L);
				return 0L;
			}
		}

		EsItNode* nodeVarHead = 0L;
		u32 defflag = EsNameItNode::DF_ASSIGNED;

		// in の左辺解析
		if((tt == TKN_VAR) || (tt == TKN_LET))	// ⇒ if (TOKEN_TYPE_IS_DECL(tt)) {
		{
			ASSERT(nodeForVars->getNodeType() == NT_LIST);
			EsListItNode* nodeForVarsList = (EsListItNode*)nodeForVars;

			TFW_SET_FLAG(*nodeForVarsList->listFlag(), EsListItNode::LF_FORINVAR, true);	// ⇒ pn1->pn_xflags |= PNX_FORINVAR;

			nodeVarHead = nodeForVarsList->getHeadNode();
			if(nodeVarHead->getTokenType() == TKN_NAME)
			{
				EsNameItNode* nodeVarHeadNam = (EsNameItNode*)nodeVarHead;
				if(nodeVarHeadNam->getExpressionNode() != 0L)	// in 左辺に式がある場合 ⇒ pn2->maybeExpr()
				{
					nodeSeq = makeListNode(TKN_SEQ, _tknprs->getCurToken()->getOpcodeTypeValue(), _tknprs->getCurToken()->getPos(), 0L);

					if(tt == TKN_LET)
					{
						// for(let aaa = bbb in ccc) の aaa をループ前に引き上げ、nodeSeq を介してくっつける
						EsUnaryItNode* nodeSemi = makeUnaryNode(TKN_SEMI, OP_NOP, _tknprs->getCurToken()->getPos(), 0L);
						EsItNode* nodeExpr = nodeVarHeadNam->getExpressionNode();
						nodeVarHeadNam->setExpressionNode(0L);
						if(!rebindLetData(nodeExpr))	{	return 0L;	}
						nodeSemi->setPos(nodeExpr->getPos());
						nodeSemi->setChildNode(nodeExpr);
						nodeSeq->resetList(nodeSemi);			// ⇒ pnseq->initList(pn3);
					}
					else
					{
						defflag = EsNameItNode::DF_INITED;

						TFW_SET_FLAG(*nodeForVarsList->listFlag(), EsListItNode::LF_FORINVAR, false);	// ⇒ pn1->pn_xflags &= ~PNX_FORINVAR;
						TFW_SET_FLAG(*nodeForVarsList->listFlag(), EsListItNode::LF_POPVAR, true);		// ⇒ pn1->pn_xflags |= PNX_POPVAR;
						nodeSeq->resetList(nodeForVarsList);	// ⇒ pnseq->initList(pn1);

						EsNameItNode* nodeNam = makeNameNode(OP_NAME, nodeVarHeadNam->getPos(), nodeVarHeadNam->getAtom(), 0L);
						if(nodeNam == 0L)	{	return 0L;	}
						if(TFW_IS_FLAG(nodeNam->getDefFlag(), EsNameItNode::DF_EXTYPE_DEFINE))	// ⇒ if (pn2->pn_defn)
						{
							setLexDefNodeLink(nodeNam, nodeVarHeadNam);	// ⇒ LinkUseToDef
						}
						nodeForVars = nodeNam;
						nodeVarHead = nodeNam;	// ⇒ pn2 = pn1;
					}
				}
			}
		}

		if(nodeVarHead == 0L)
		{
			nodeVarHead = nodeForVars;	// ⇒ pn2 = pn1;
			if(nodeVarHead->getTokenType() == TKN_LP)
			{
				if(!setSetCall(nodeVarHead))
				{
					addCompileError(EsError::ID_BAD_LEFTSIDE_OF_ASSIGN, 0L);
					return 0L;
				}
			}
		}

		if(nodeVarHead->getTokenType() == TKN_NAME)
		{
			// for(xx in xx) か var が無い場合
			setLeftValueFlags(nodeVarHead, defflag);	// ⇒ NoteLValue(cx, pn2, tc, dflag);
		}

		// in 右辺式を解析
		EsStatement* stmtBk = _bcbCur->topEnclosingStatement();	// ⇒ JSStmtInfo *save = tc->topStmt;
		if(isLet)
		{
			_bcbCur->setTopEnclosingStatement(stmtBk->getDownEnclosingStatement());	// ⇒ tc->topStmt = save->down;
		}
		EsItNode* nodeInRightExp = parseExpression();
		if(isLet)
		{
			_bcbCur->setTopEnclosingStatement(stmtBk);
		}

		// for ヘッドノードを in として作成
		EsItNode* nodeIn = this->makeBinaryNode(TKN_IN, OP_NOP, posFor, nodeForVars, nodeInRightExp);
		if(nodeIn == 0L)	{	return 0L;	}
		nodeForHead = nodeIn;
	}
	else	// in 以外の場合
	{
		EsItNode* nodeForLoopCond = 0L;	// for(x) の x 部のループ条件式（２項目）部ノード ⇒ pn2
		EsItNode* nodeForUpdate = 0L;	// for(x) の x 部の更新式（３項目）部ノード ⇒ pn3

		if(TFW_IS_FLAG(iterflagsFor, EsBinaryItNode::ITERF_FOREACH))
		{
			goto ERROR_FOREACH;
		}
		otFor = OP_NOP;

		// 変数初期化式（for の１項目）の終了
		if(!mustMatchToken(TKN_SEMI, EsError::ID_SEMI_AFTER_FOR_INIT))	{	return 0L;	}

		// ループ条件式（for の２項目）の解析
		/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, true); ⇒ ts->flags |= TSF_OPERAND;
		tt = _tknprs->peekToken();
		/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, false); ⇒ ts->flags &= ~TSF_OPERAND;
		if(tt != TKN_SEMI)	// 条件式が空以外の場合
		{
			nodeForLoopCond = parseExpression();
			if(nodeForLoopCond == 0L)	{	return 0L;	}
		}
		if(!mustMatchToken(TKN_SEMI, EsError::ID_SEMI_AFTER_FOR_INIT))	{	return 0L;	}

		// ループ更新式（for の３項目）
		/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, true); ⇒ ts->flags |= TSF_OPERAND;
		tt = _tknprs->peekToken();
		/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, false); ⇒ ts->flags &= ~TSF_OPERAND;
		if(tt != TKN_SEMI)	// 条件式が空以外の場合
		{
			nodeForUpdate = parseExpression();
			if(nodeForUpdate == 0L)		{	return 0L;	}
		}

		// for ヘッドノードの作成
		nodeForHead = makeTernaryNode(TKN_FORHEAD, OP_NOP, posFor, nodeForVars, nodeForLoopCond, nodeForUpdate);
	}

	// for( ... の後には ) が来るはず
	if(!mustMatchToken(TKN_RP, EsError::ID_PARENTHESIS_FOR_RIGHT))	{	return 0L;	}

	// ループ本体ノードの解析
	nodeForBody = parseStatement();
	if(nodeForBody == 0L)	{	return 0L;	}

	// バイナリノードに条件とループ内容ノードを設定する
	nodeFor = makeBinaryNode(ttFor, otFor, posFor, nodeForHead, nodeForBody);
	nodeFor->setIterFlag(iterflagsFor);
	nodeRet = nodeFor;

	// let の場合、スコープを一つ潜っているので、戻す
	if(nodeLet != 0L)
	{
		_bcbCur->popStatement();
		nodeLet->setExpressionNode(nodeFor);
		nodeRet = nodeLet;	// let を返す
	}

	// in 左辺に式がある場合
	if(nodeSeq != 0L)
	{
		nodeSeq->pos()->copyEnd(nodeFor->getPos());
		nodeSeq->addListNode(nodeFor);
		nodeRet = nodeSeq;	// リストを返す
	}

	// ステートメントを戻す
	_bcbCur->popStatement();

	return nodeRet;

ERROR_FOREACH:

	// ステートメントを戻す
	_bcbCur->popStatement();
	return 0L;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - break 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_break()
{
	EsTokenType ttBreak = _tknprs->getCurToken()->getType();
	EsOpcodeType otBreak = _tknprs->getCurToken()->getOpcodeTypeValue();
	const EsTokenPos* posBreak =  _tknprs->getCurToken()->getPos();

	// ラベルがあれば取得する
	const EsAtom* atomLabel = 0L;
	if(!matchLabel(&atomLabel))
	{
		return 0L;
	}

	// ステートメントが正常か確認する
	EsStatement* stmt = _bcbCur->topEnclosingStatement();
	if(atomLabel != 0L)	// break にラベルがある場合
	{
		while(true)
		{
			if(stmt == 0L)
			{
				addCompileError(EsError::ID_LABEL_NOT_FOUND, 0L);
				return 0L;
			}

			if((stmt->getType() == EsStatement::STMTT_LABEL) && (stmt->getLabelAtom() == atomLabel))
			{
				break;
			}

			stmt = stmt->getDownEnclosingStatement();
		}
	}
	else	// break にラベルがない場合
	{
		while(true)
		{
			if(stmt == 0L)
			{
				addCompileError(EsError::ID_BAD_BREAK, 0L);
				return 0L;
			}

			if(stmt->isLoop() || (stmt->getType() == EsStatement::STMTT_SWITCH))
			{
				break;
			}

			stmt = stmt->getDownEnclosingStatement();
		}
	}

	// break ノードの作成
	EsNameItNode* nodeBreak = makeNameNode(otBreak, posBreak, atomLabel, 0L);
	if(nodeBreak == 0L)	{	return 0L;	}
	nodeBreak->setTokenType(ttBreak);
	if(atomLabel != 0L)
	{
		nodeBreak->pos()->copyEnd(_tknprs->getCurToken()->getPos());
	}

	return nodeBreak;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - continue 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_continue()
{
	EsTokenType ttContinue = _tknprs->getCurToken()->getType();
	EsOpcodeType otContinue = _tknprs->getCurToken()->getOpcodeTypeValue();
	const EsTokenPos* posContinue =  _tknprs->getCurToken()->getPos();

	// ラベルがあれば取得する
	const EsAtom* atomLabel = 0L;
	if(!matchLabel(&atomLabel))
	{
		return 0L;
	}

	// ステートメントが正常か確認する
	EsStatement* stmt = _bcbCur->topEnclosingStatement();
	if(atomLabel != 0L)	// break にラベルがある場合
	{
		EsStatement* stmtUp = 0L;
		while(true)
		{
			if(stmt == 0L)
			{
				addCompileError(EsError::ID_LABEL_NOT_FOUND, 0L);
				return 0L;
			}

			if(stmt->getType() == EsStatement::STMTT_LABEL)
			{
				if(stmt->getLabelAtom() == atomLabel)
				{
					if((stmtUp == 0L) || !stmtUp->isLoop())
					{
						addCompileError(EsError::ID_BAD_CONTINUE, 0L);
						return 0L;
					}
					break;
				}
			}
			else
			{
				stmtUp = stmt;
			}

			stmt = stmt->getDownEnclosingStatement();
		}
	}
	else	// break にラベルがない場合
	{
		while(true)
		{
			if(stmt == 0L)
			{
				addCompileError(EsError::ID_BAD_CONTINUE, 0L);
				return 0L;
			}

			if(stmt->isLoop())
			{
				break;
			}

			stmt = stmt->getDownEnclosingStatement();
		}
	}

	// continue ノードの作成
	EsNameItNode* nodeContinue = makeNameNode(otContinue, posContinue, atomLabel, 0L);
	if(nodeContinue == 0L)	{	return 0L;	}
	nodeContinue->setTokenType(ttContinue);
	if(atomLabel != 0L)
	{
		nodeContinue->pos()->copyEnd(_tknprs->getCurToken()->getPos());
	}

	return nodeContinue;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - with 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_with()
{
	EsTokenType ttWith = _tknprs->getCurToken()->getType();
	EsOpcodeType otWith = _tknprs->getCurToken()->getOpcodeTypeValue();
	const EsTokenPos* posWith = _tknprs->getCurToken()->getPos();

	// 左括弧 ( が来るはず
	if(!mustMatchToken(TKN_LP, EsError::ID_PARENTHESIS_WITH_LEFT))	{	return 0L;	}

	// カッコ内の式の解析
	EsItNode* nodeLeft = parseParenthesisExpression();
	if(nodeLeft == 0L)	{	return 0L;	}

	// 右括弧 ) が来るはず
	if(!mustMatchToken(TKN_RP, EsError::ID_PARENTHESIS_WITH_RIGHT))	{	return 0L;	}

	// 新しいステートメントブロックを積む
	EsStatement stmtWith(EsStatement::STMTT_WITH);
	_bcbCur->pushStatement(&stmtWith, -1);

	// with 以下の文を解析
	EsItNode* nodeRight = parseStatement();
	if(nodeRight == 0L)	{	return 0L;	}

	// 積んだステートメントブロックを取り出す
	_bcbCur->popStatement();

	TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ, true);

	return makeBinaryNode(ttWith, otWith, posWith, nodeLeft, nodeRight);
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - var 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_var()
{
	EsItNode* node = parseVariables(false);
	if(node == 0L)	{	return 0L;	}
	TFW_SET_FLAG(*node->parseFlag(), EsItNode::PF_POPVAR, true);	// 最後に POP するようにフラグを立てる

	// セミコロン終行を確認 ⇒ MatchOrInsertSemicolon
	if(!matchOrInsertSemicolonToken())	{	return 0L;	}

	return node;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - let 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_let()
{
	// let 文 / let 式か、let 宣言かを判定する
	if(_tknprs->peekToken() == TKN_LP)	// let 文 / let 式である
	{
		// let 文解析
		EsItNode* nodeLet = parseLet(true);
		if((nodeLet == 0L) || (nodeLet->getOpcodeType() == OP_LEAVEBLOCK))
		{
			return nodeLet;
		}

		// セミコロン終行を確認 ⇒ MatchOrInsertSemicolon
		if(!matchOrInsertSemicolonToken())	{	return 0L;	}

		return nodeLet;
	}

	// let 宣言
	// ブロック内で行われているかを確認する
	EsStatement* stmt = _bcbCur->topEnclosingStatement();
	if(!stmt->isPerhapsScope() || TFW_IS_FLAG(stmt->getBlockFlags(), EsStatement::BLOCKF_FOR))
	{
		addCompileError(EsError::ID_LET_DECL_NOT_IN_BLOCK, 0L);
		return 0L;
	}

	EsObject* objBlock;
	EsObjectBox* objboxBlock;

	if((stmt != 0L) && TFW_IS_FLAG(stmt->getBlockFlags(), EsStatement::BLOCKF_SCOPE))
	{
		ASSERT(_bcbCur->getBlockChainObject() == stmt->getBlockObject());
		objBlock = _bcbCur->getBlockChainObject();
	}
	else
	{
		if((stmt == 0L) || TFW_IS_FLAG(stmt->getBlockFlags(), EsStatement::BLOCKF_BODY))
		{
			// トップレベルブロックや本体ブロックスコープにおいて、let 宣言は var を暗黙化しない
			EsToken* tknCur = _tknprs->curToken();
			tknCur->setType(TKN_VAR);
			tknCur->setOpcodeTypeValue(OP_DEFVAR);

			// var リスト解析
			EsListItNode* nodeVar = parseVariables(false);
			if(nodeVar == 0L)	{	return 0L;	}
			TFW_SET_FLAG(*nodeVar->listFlag(), EsListItNode::LF_POPVAR, true);	// ⇒ pn->pn_xflags |= PNX_POPVAR;
			
			// セミコロン終行を確認 ⇒ MatchOrInsertSemicolon
			if(!matchOrInsertSemicolonToken())	{	return 0L;	}

			return nodeVar;
		}

		// ステートメントはスコープでないはず
		// また、catch ブロックでもないはず（catch ブロックは語彙スコープであるはず）
		ASSERT(!TFW_IS_FLAG(stmt->getBlockFlags(), EsStatement::BLOCKF_SCOPE));
		ASSERT(stmt != _bcbCur->topScopeStatement());
		ASSERT((stmt->getType() == EsStatement::STMTT_BLOCK) || (stmt->getType() == EsStatement::STMTT_SWITCH) || (stmt->getType() == EsStatement::STMTT_TRY) || (stmt->getType() == EsStatement::STMTT_FINALLY));
		ASSERT(stmt->getDownScopeStatement() == 0L);

		// ブロックオブジェクトを作成
		objBlock = _ctxRef->newObjectWithGivenPrototype(EsSysClasses::getClass(EsSysClasses::CLASSTYPE_BLOCK), 0L, 0L);	// ⇒ JSObject *obj = js_NewBlockObject(tc->compiler->context);

		// ブロックオブジェクトボックスを作成
		objboxBlock = makeObjectBox(objBlock);
		if(objboxBlock == 0L)	{	return 0L;	}

		// ステートメントをスコープリンクドリストに挿入する
		TFW_SET_FLAG(*stmt->blockFlags(), EsStatement::BLOCKF_SCOPE, true);
		stmt->setDownScopeStatement(_bcbCur->topScopeStatement());
		_bcbCur->setTopScopeStatement(stmt);
		objBlock->slots()->setParentObject(_bcbCur->getBlockChainObject());	// ⇒ STOBJ_SET_PARENT(obj, tc->blockChain);
		_bcbCur->setBlockChainObject(objBlock);
		stmt->setBlockObject(objBlock);

		// 現在のブロック情報を得る
		EsItNode* nodeBlock = _bcbCur->getCurrentBlockNode();
		ASSERT(nodeBlock != 0L);
		u32 blockid = 0;
		switch(nodeBlock->getNodeType())
		{
		case NT_NAME:	blockid = ((EsNameItNode*)nodeBlock)->getBlockId();	break;
		case NT_LIST:	blockid = ((EsListItNode*)nodeBlock)->getBlockId();	break;
		}

		// 語彙スコープノードを作成する
		EsNameItNode* nodeLexScope = makeNameNode(OP_LEAVEBLOCK, nodeBlock->getPos(), 0L, nodeBlock);
		if(nodeLexScope == 0L)	{	return 0L;	}
		nodeLexScope->setTokenType(TKN_LEXICALSCOPE);
		nodeLexScope->setObjectBox(objboxBlock);
		nodeLexScope->setBlockId(blockid);
		_bcbCur->setCurrentBlockNode(nodeLexScope);	// 現在のブロックノードに設定する
	}

	// var リスト解析
	EsListItNode* nodeVar = parseVariables(false);
	if(nodeVar == 0L)	{	return 0L;	}
	TFW_SET_FLAG(*nodeVar->listFlag(), EsListItNode::LF_POPVAR, true);	// ⇒ pn->pn_xflags |= PNX_POPVAR;
			
	// セミコロン終行を確認 ⇒ MatchOrInsertSemicolon
	if(!matchOrInsertSemicolonToken())	{	return 0L;	}

	return nodeVar;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - return 文解析 ⇒ ReturnOrYield
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_return()
{
	EsTokenType ttRet = _tknprs->getCurToken()->getType();
	EsTokenPos posRet(*_tknprs->getCurToken()->getPos());

	// 関数内ではない場所での return はエラー
	if((_tknprs->getCurToken()->getType() == TKN_RETURN) && !TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION))
	{
		addCompileError(EsError::ID_BAD_RETURN, 0L);
		return 0L;
	}

	EsTokenType ttChild = _tknprs->peekTokenSameLine();
	if(ttChild == TKN_ERROR)
	{
		return 0L;
	}

	EsItNode* nodeChild = 0L;
	switch(ttChild)
	{
	case TKN_EOF:
	case TKN_EOL:
	case TKN_SEMI:
	case TKN_RC:
		TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_RET_VOID, true);
		break;
	default:
		{
			nodeChild = parseExpression();
			if(nodeChild == 0L)	{	return 0L;	}
			posRet.copyEnd(nodeChild->getPos());
		}
		TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_RET_EXPR, true);
		break;
	}

	// 返答する単項ノードを作成
	EsItNode* nodeRet = makeUnaryNode(ttRet, OP_NOP, &posRet, nodeChild);

	if(	TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_RET_EXPR) && 
		TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_FUNC_IN_GENERATOR)	)
	{
		addCompileError(EsError::ID_BAD_GENERATOR_RETURN, 0L);
		return 0L;
	}
	// ※※※ 実装保留 ※※※ ⇒ if (JS_HAS_STRICT_OPTION(cx) && ...

	return nodeRet;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - 左大括弧 { 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_lc()
{
	u32 bflagBk = _bcbCur->getBuildFlag();
	TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_CONTAIN_FUNC_STMT, false);

	// 新しいステートメントブロックを積む ⇒ PushBlocklikeStatement
	EsStatement stmt(EsStatement::STMTT_BLOCK);
	_bcbCur->pushStatement(&stmt, -1);
	stmt.setBlockId(_bcbCur->makeBlockId());

	// ブロックの内容文解析
	EsListItNode* node = parseStatementBlock();
	if(node == 0L)	{	return 0L;	}

	// 右大括弧 } で終わるはず
	if(!mustMatchToken(TKN_RC, EsError::ID_BRANCKET_BLOCK_RIGHT))	{	return 0L;	}

	// 積んだステートメントブロックを取り出す
	_bcbCur->popStatement();

	// 関数文を含み、トップレベルステートメントもしくはブロック内の場合、大括弧 {} が必要となる
	if(TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_CONTAIN_FUNC_STMT))
	{
		EsStatement* siTopEncl = _bcbCur->topEnclosingStatement();
		if((siTopEncl == 0L) || (siTopEncl->getType() == EsStatement::STMTT_BLOCK))
		{
			ASSERT(node->getNodeType() == NT_LIST);
			TFW_SET_FLAG(*((EsListItNode*)node)->listFlag(), EsListItNode::LF_NEED_BRACES, true);
		}
	}

	TFW_SET_FLAG(*_bcbCur->buildFlag(), bflagBk | (_bcbCur->getBuildFlag() & (EsBytecodeBuilder::BF_FUNC_FLAGS | EsBytecodeBuilder::BF_RET_FLAGS)), true);

	return node;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - セミコロン ; 文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_semi()
{
	EsItNode* node = makeUnaryNode(TKN_SEMI, OP_NOP, _tknprs->getCurToken()->getPos(), 0L);
	return node;
}

/*---------------------------------------------------------------------*//**
	命令文解析サブルーチン - その他の文解析
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseStatementSub_other()
{
	_tknprs->ungetToken();

	EsItNode* nodeExpr = parseExpression();
	if(nodeExpr == 0L)	{	return 0L;	}

	// ラベル処理
	if(_tknprs->peekToken() == TKN_COLON)
	{
		if(nodeExpr->getTokenType() != TKN_NAME)
		{
			addCompileError(EsError::ID_BAD_LABEL, 0L);
			return 0L;
		}
		ASSERT(nodeExpr->getNodeType() == NT_NAME);

		const EsAtom* atomLabel = ((EsNameItNode*)nodeExpr)->getAtom();
		for(EsStatement* stmt = _bcbCur->topEnclosingStatement(); stmt != 0L; stmt = stmt->getDownEnclosingStatement())
		{
			if((stmt->getType() == EsStatement::STMTT_LABEL) && (stmt->getLabelAtom() == atomLabel))
			{
				addCompileError(EsError::ID_DUPLICATE_LABEL, 0L);
				return 0L;
			}
		}
		unsetLexDefNodeLink((EsNameItNode*)nodeExpr);	// ⇒ ForgetUse(pn2);

		_tknprs->nextToken();

		// 新しいラベルステートメントを積む
		EsStatement stmt(EsStatement::STMTT_LABEL);
		_bcbCur->pushStatement(&stmt, -1);
		stmt.setLabelAtom(atomLabel);

		// ラベル後の文を解析する
		EsItNode* nodeAfterLabel = parseStatement();
		if(nodeAfterLabel == 0L)	{	return 0L;	}

		// ラベル後の文が空の場合はノードを空のリストに変更する
		if((nodeAfterLabel->getTokenType() == TKN_SEMI) && (nodeAfterLabel->getChildNode() == 0L))
		{
			EsItNode* nodeAfterLabelOld = nodeAfterLabel;
			nodeAfterLabel = makeListNode(TKN_LC, nodeAfterLabelOld->getOpcodeType(), nodeAfterLabelOld->getPos(), 0L);
			_ctxRef->addLeakNode(nodeAfterLabelOld);	// ツリー外で削除
		}

		// 積んだステートメントを戻す
		_bcbCur->popStatement();

		// コロンノードとして返る
		nodeExpr->setTokenType(TKN_COLON);
		nodeExpr->pos()->copyEnd(nodeAfterLabel->getPos());
		((EsNameItNode*)nodeExpr)->setExpressionNode(nodeAfterLabel);
		return nodeExpr;
	}

	// Statement ノードを作成
	EsUnaryItNode* nodeStmt = makeUnaryNode(TKN_SEMI, OP_NULL, nodeExpr->getPos(), nodeExpr);	// ⇒ NewParseNode(PN_UNARY, tc);
	if(nodeStmt == 0L)					{	return 0L;	}

	// セミコロン終行を確認
	if(!matchOrInsertSemicolonToken())	{	return 0L;	}

	return nodeStmt;
}

/*---------------------------------------------------------------------*//**
	命令文ブロック解析 ⇒ Statements
	※ 大括弧 {} で囲われたステートメントリスト
**//*---------------------------------------------------------------------*/
EsListItNode* EsParser::parseStatementBlock()
{
	EsTokenPos posList(*_tknprs->getCurToken()->getPos());
	EsListItNode* nodeList = makeListNode(TKN_LC, _tknprs->getCurToken()->getOpcodeTypeValue(), &posList, 0L);

	// ブロックの設定
	EsItNode* nodeBlockBk = _bcbCur->getCurrentBlockNode();
	nodeList->setBlockId( _bcbCur->getCurrentBlockId() );
	_bcbCur->setCurrentBlockNode( nodeList );

	while(true)
	{
		EsTokenType tt = _tknprs->peekToken();
		if((tt == TKN_EOF) || (tt == TKN_RC))
		{
			break;
		}
		if(tt == TKN_ERROR)
		{
			break;
		}

		// ステートメント解析
		EsItNode* node = parseStatement();
		if(node == 0L)
		{
			return 0L;
		}

		// ステートメントがファンクションだった場合のフラグ設定
		if(node->getTokenType() == TKN_FUNCTION)
		{
			if(_bcbCur->isTopLevelStatement())	// ⇒ tc->atTopLevel()
			{
				TFW_SET_FLAG(*nodeList->listFlag(), EsListItNode::LF_FUNCDEF, true);
			}
			else
			{
				TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_CONTAIN_FUNC_STMT, true);
			}
		}

		nodeList->addListNode(node);	// リストに追加 ⇒ pn->append(pn2);
	}

	// このブロックより下でバイトコード生成のブロックノードが書き換えられた場合を考慮する
	if(_bcbCur->getCurrentBlockNode() != nodeList)
	{
		nodeList = (EsListItNode*)_bcbCur->getCurrentBlockNode();
	}
	_bcbCur->setCurrentBlockNode(nodeBlockBk);

	// 終了位置のコピー
	EsTokenPos posCur(_tknprs->getCurScriptPos());
	posList.copyEnd(&posCur);
	nodeList->setPos(&posList);

	return nodeList;
}

/*---------------------------------------------------------------------*//**
	式解析（カンマ演算子以下） ⇒ Expr
		
		［カンマ演算子］
		列挙された左から右の式から順に評価され、
		左の式は切り捨てられ、最も右の式の値と型が最終的な戻り値になる
		例えば、Java では
			f(a, (t=3, t+2), c)
		の２番目の引数の結果は５になる
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseExpression()
{
	EsItNode* node = parseAssignExpression();	// 代入式解析
	if(node == 0L)	{	return 0L;	}

	if(_tknprs->matchToken(TKN_COMMA))	// カンマ“,”による列挙
	{
		// カンマリストを作成する
		EsListItNode* nodeCommaList = makeListNode(_tknprs->getCurToken()->getType(), _tknprs->getCurToken()->getOpcodeTypeValue(), _tknprs->getCurToken()->getPos(), 0L);
		if(nodeCommaList == 0L)
		{
			return 0L;
		}
		nodeCommaList->pos()->copyBegin(node->getPos());
		nodeCommaList->addListNode(node);
		node = nodeCommaList;

		// カンマで区切られた文を解析し、リストに追加する
		do
		{
			EsItNode* nodeAse = parseAssignExpression();
			if(nodeAse == 0L)	{	return 0L;	}
			nodeCommaList->addListNode(nodeAse);
		}
		while(_tknprs->matchToken(TKN_COMMA));

		const EsItNode* nodeLastNext = node->getLastNextNode();
		if(nodeLastNext != 0L)	{	node->pos()->copyEnd(nodeLastNext->getPos());	}
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	代入式解析 ⇒ AssignExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseAssignExpression()
{
	// ※※※ yield の対応 → 保留 ※※※

	EsItNode* nodeCond = parseConditionExpression();	// 条件式解析
	if(nodeCond == 0L)	{	return 0L;	}

	EsTokenType ttExpr = _tknprs->nextToken();

	// ※※※ getter, setter の対応 → 保留 ※※※

	if(ttExpr != TKN_ASSIGN)	// 等号でない場合
	{
		// トークンを巻き戻して返す
		_tknprs->ungetToken();
		return nodeCond;
	}
	EsOpcodeType otExpr = _tknprs->getCurToken()->getOpcodeTypeValue();
	const EsTokenPos* posExpr = _tknprs->getCurToken()->getPos();

	// 右括弧“)”が終わるまで送る
	EsItNode* nodeLeft = nodeCond;
	while((nodeLeft != 0L) && (nodeLeft->getTokenType() == TKN_RP))
	{
		nodeLeft = nodeLeft->getChildNode();
	}

	// 左辺の命令コードを設定する
	const EsToken* tknCur = _tknprs->getCurToken();
	switch(nodeLeft->getTokenType())
	{
	case TKN_NAME:
		nodeLeft->setOpcodeType(OP_SETNAME);
		setLeftValueFlags(nodeLeft, EsNameItNode::DF_ASSIGNED);	// ⇒ NoteLValue
		break;
	case TKN_DOT:	// “.”
		nodeLeft->setOpcodeType(OP_SETPROP);
		break;
	case TKN_LB:	// “[”
		nodeLeft->setOpcodeType(OP_SETELEM);
	case TKN_LP:
		if(!setSetCall(nodeLeft))
		{
			addCompileError(EsError::ID_BAD_LEFTSIDE_OF_ASSIGN, 0L);
			return 0L;
		}
		return 0L;
	default:
		addCompileError(EsError::ID_BAD_LEFTSIDE_OF_ASSIGN, 0L);
		ESCASSERT(false);
		return 0L;
	}

	// 右辺を得る
	EsItNode* nodeRight = parseAssignExpression();

	return makeBinaryNode(TKN_ASSIGN, otExpr, posExpr, nodeLeft, nodeRight);
}

/*---------------------------------------------------------------------*//**
	関数定義解析 ⇒ FunctionDef
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseFunctionDefine(bool isLambda)
{
	// 関数ノードの作成
	EsNameItNode* nodeFunc = makeFuncNode(_tknprs->getCurToken()->getPos());	// ⇒ NewParseNode(PN_FUNC, tc);

	{
		// 関数引数フラグ
		bool isTopLevelStmt = _bcbCur->isTopLevelStatement();
		TFW_SET_FLAG(*nodeFunc->defFlag(), EsNameItNode::DF_FUNCARG, isLambda || !isTopLevelStmt);	// ⇒ pn->pn_dflags = (lambda || !topLevel) ? PND_FUNARG : 0;

		// 関数名を取得
		const EsAtom* atomFunc = 0L;
		_tknprs->setParseFlag(EsTokenParser::PF_KEYWD_IS_NAME, true);	// キーワードは名前として解析
		EsTokenType tt = _tknprs->nextToken();
		_tknprs->setParseFlag(EsTokenParser::PF_KEYWD_IS_NAME, false);
		if(tt == TKN_NAME)
		{
			atomFunc = _tknprs->getCurToken()->getAtomValue();
		}
		else
		{
			#if 0	// 無名関数を許可
				if(!isLambda)
				{
					ESCASSERT(false);
					addCompileError(EsError::ID_NO_FUNCTION_NAME, 0L);
					goto FAILED;	// 失敗
				}
			#endif
			_tknprs->ungetToken();
		}

		// 関数名を宣言アトムリストに追加する
		if(!isLambda && (atomFunc != 0L))
		{
			// 宣言済み語彙リストから探す
			EsAtomList* alistDecl = _bcbCur->getDeclaredNameAtomList();
			EsAtomList::EsAtomListEntity* ale = alistDecl->find(atomFunc);	// ⇒ ale = tc->decls.lookup(funAtom);
			if(ale != 0L)	// 既にある場合
			{
				EsNameItNode* nodeDef = ale->getDefName();	// ⇒ JSDefinition *dn = ALE_DEFN(ale);
				EsNameItNode::DefKind defkind = nodeDef->getDefKind();

				ASSERT(!nodeDef->isHasLex());
				ASSERT(TFW_IS_FLAG(*nodeDef->defFlag(), EsNameItNode::DF_EXTYPE_DEFINE));

				if(OPT_STRICT || (defkind == EsNameItNode::DK_CONST))
				{
					const VcString* name = atomFunc->getString();	// ⇒ const char *name = js_AtomToPrintableString(cx, funAtom);
					VcStringArray ep((name != 0L) ? name : TypeUtils::getEmptyString());
					addCompileError(EsError::ID_REDECLARED_VAR, &ep);
					goto FAILED;
				}

				if(isTopLevelStmt)
				{
					ale->setDefName(nodeFunc);	// ⇒ ALE_SET_DEFN(ale, pn);
					TFW_SET_FLAG(*nodeFunc->defFlag(), EsNameItNode::DF_EXTYPE_DEFINE, true);	// ⇒ pn->pn_defn = true;
					nodeFunc->setLinkNode(nodeDef);	// ⇒ pn->dn_uses = dn;

					if(defineFuncIntoLexDefNode(nodeDef, nodeFunc, atomFunc) == 0L)	// ⇒ if (!MakeDefIntoUse(dn, pn, funAtom, tc))
					{
						goto FAILED;
					}
				}
			}
			else if(isTopLevelStmt)
			{
				// 未解決語彙アトムとして登録済みの場合は、リストから削除する
				EsAtomList* alistLexdeps = _bcbCur->getUnresolvedNameAtomList();
				EsAtomList::EsAtomListEntity* ale = alistLexdeps->find(atomFunc);	// ⇒ ale = tc->lexdeps.rawLookup(funAtom, hep);
				if(ale != 0L)
				{
					EsNameItNode* nodeDefFunc = ale->getDefName();	// ⇒ JSDefinition *fn = ALE_DEFN(ale);
					ASSERT(TFW_IS_FLAG(*nodeDefFunc->defFlag(), EsNameItNode::DF_EXTYPE_DEFINE));

					// 定義ノードを関数ノード化する
					TFW_SET_FLAG(*nodeDefFunc->defFlag(), EsNameItNode::DF_EXTYPE_FUNC, true);
					nodeDefFunc->setTokenType(TKN_FUNCTION);
					nodeDefFunc->pos()->copyBegin(nodeFunc->getPos());	// ⇒ fn->pn_pos.begin = pn->pn_pos.begin;
					nodeDefFunc->setExpressionNode(0L);	// ⇒ fn->pn_body = NULL;
					alistLexdeps->remove(ale);	// 語彙リストから削除する ⇒ tc->lexdeps.rawRemove(tc->compiler, ale, hep);
					_ctxRef->addLeakNode(nodeFunc);	// 元の関数ノードはツリー外削除対象へ
					nodeFunc = nodeDefFunc;
				}

				// 名称を宣言定義する
				if(!defineDeclaredName(nodeFunc, atomFunc, false))
				{
					goto FAILED;	// 失敗
				}
			}

			if(isTopLevelStmt)
			{
				TFW_SET_FLAG(*nodeFunc->defFlag(), EsNameItNode::DF_TOPLEVEL, true);

				// ネストされた関数
				if(TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION))
				{
					EsFunction* funcParent = _bcbCur->getCurrentFunction();
					ASSERT(funcParent != 0L);
					EsFunction::LocalVarKind lvk;
					if(funcParent->findLocalVar(&lvk, 0L, atomFunc))	// ⇒ localKind = js_LookupLocal(cx, tc->fun, funAtom, &index);
					{
						s32 numLv = funcParent->getLocalVarNum(lvk);
						switch(lvk)
						{
						case EsFunction::LVK_NULL:
						case EsFunction::LVK_ARG:
							if(!funcParent->addLocalVar(atomFunc, EsFunction::LVK_VAR))
							{
								goto FAILED;
							}
							// ↓
						case EsFunction::LVK_VAR:
							nodeFunc->setUpvarInfo((u16)_bcbCur->getNestDepth(), numLv);	// ⇒ pn->pn_cookie = MAKE_UPVAR_COOKIE(tc->staticLevel, index);
							TFW_SET_FLAG(*nodeFunc->defFlag(), EsNameItNode::DF_BOUND_LG, true);
							break;
						}

					}
				}
			}
		}

		// 関数定義に入る
		EsBytecodeBuilder bcbFunc(_ctxRef);
		EsFunctionBox* funcbox = enterFunction(nodeFunc, &bcbFunc, atomFunc, isLambda);
		if(funcbox == 0L)
		{
			goto FAILED;	// 失敗
		}
		EsFunction* func = funcbox->getFunction(); ASSERT(func != 0L);

		// 関数内解析中フラグを立てる
		TFW_SET_FLAG(*bcbFunc.buildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION, true);

		// バイトコードビルダを一時的に置き換える
		EsBytecodeBuilder* bcbTmpBk = _bcbCur;
		_bcbCur = &bcbFunc;

		// 引数リストを解析
		if(!mustMatchToken(TKN_LP, EsError::ID_PARENTHESIS_FUNC_LEFT))	{	goto FAILED;	}	// ( が来るはず
		if(!_tknprs->matchToken(TKN_RP))	// すぐに ) で閉じられていなければ
		{
			do
			{
				EsTokenType tt = _tknprs->nextToken();
				switch(tt)
				{
				case TKN_NAME:
					{
						// アトムを得て重複チェックを行う
						const EsAtom* atom = _tknprs->getCurToken()->getAtomValue();
						if(OPT_STRICT && func->findLocalVar(0L, 0L, atom))	// 既に同じ名前の変数が存在している ⇒  js_LookupLocal(cx, fun, atom, NULL)
						{
							const VcString* name = atomFunc->getString();
							VcStringArray ep((name != 0L) ? name : TypeUtils::getEmptyString());
							addCompileError(EsError::ID_DUPLICATE_LOCAL_VAR, &ep);
							goto FAILED;
						}
						// 引数を定義する
						if(!defineArg(nodeFunc, atom, func->getLocalVarNum(EsFunction::LVK_ARG)))
						{
							goto FAILED;	// 失敗
						}
						// 局所関数として記憶する
						if(!func->addLocalVar(atom, EsFunction::LVK_ARG))
						{
							goto FAILED;	// 失敗
						}
					}
					break;

				default:
					addCompileError(EsError::ID_MISSING_FORMAL, 0L);
					goto FAILED;	// 失敗

				case TKN_ERROR:
					goto FAILED;	// 失敗
				}
			}
			while(_tknprs->matchToken(TKN_COMMA));

			// ) が来るはず
			if(!mustMatchToken(TKN_RP, EsError::ID_PARENTHESIS_FUNC_RIGHT))	{	goto FAILED;	}
		}

		// { が来るはず
		if(!mustMatchToken(TKN_LC, EsError::ID_CURLY_BEFORE_BODY))	{	goto FAILED;	}

		// 関数内命令文取得
		EsItNode* nodeBody = parseFunctionBody();
		if(nodeBody == 0L)
		{
			goto FAILED;	// 失敗
		}

		// } が来るはず
		if(!mustMatchToken(TKN_RC, EsError::ID_CURLY_AFTER_BODY))	{	goto FAILED;	}

		// 終了位置の修正
		nodeFunc->pos()->copyEnd(_tknprs->getCurToken()->getPos());

		// 呼び出しフラグ（NEED_CALL_FUNC）の修正
		if(TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ))
		{
			TFW_SET_FLAG(*func->flags(), EsFunction::F_FUNC_NEED_CALLOBJ, true);
			TFW_SET_FLAG(*bcbTmpBk->buildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ, true);
		}
		else if(!isTopLevelStmt && !isLambda && (atomFunc != 0L))
		{
			TFW_SET_FLAG(*bcbTmpBk->buildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ, true);
		}

		// 関数ノードの演算子タイプと返答ノードを求める
		EsItNode* nodeRes = nodeFunc;
		EsOpcodeType ot = OP_NOP;
		if(isLambda)			// ラムダ関数
		{
			ot = OP_LAMBDA;
		}
		else if(atomFunc == 0L)	// 匿名関数
		{
			nodeRes = makeUnaryNode(TKN_SEMI, OP_NOP, nodeFunc->getPos(), nodeFunc);
			if(nodeRes == 0L)
			{
				goto FAILED;	// 失敗
			}
			ot = OP_LAMBDA;
		}
		else if(!isTopLevelStmt)
		{
			ot = OP_DEFFUN;
		}

		// 関数ボックス、関数ノードの最終調整
		funcbox->setChild(_bcbCur->getFunctionBoxTree());
		nodeFunc->setFunctionBox(funcbox);
		nodeFunc->setOpcodeType(ot);
		EsItNode* nodeBodyAbove = nodeFunc->getExpressionNode();
		if(nodeBodyAbove != 0L)
		{
			ASSERT(nodeBodyAbove->getNodeType() == NT_LIST);
			((EsListItNode*)nodeBodyAbove)->addListNode(nodeBody);
			nodeBodyAbove->pos()->copy(nodeBody->getPos());
		}
		else
		{
			nodeFunc->setExpressionNode(nodeBody);
		}
		nodeFunc->setBlockId(_bcbCur->getCurrentBlockId());

		// バイトコードビルダを戻す
		_bcbCur = bcbTmpBk;

		// 関数定義を抜ける
		if(!leaveFunction(nodeFunc, &bcbFunc, atomFunc, isLambda))
		{
			goto FAILED;	// 失敗
		}

		return nodeRes;
	}

FAILED:
	_ctxRef->addLeakNode(nodeFunc);	// ツリー外で削除
	return 0L;
}

/*---------------------------------------------------------------------*//**
	関数内命令文解析 ⇒ FunctionBody
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseFunctionBody()
{
	ASSERT(TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION));

	// 新しいブロック開始
	EsStatement stmt(EsStatement::STMTT_BLOCK);
	_bcbCur->pushStatement(&stmt, -1);
	TFW_SET_FLAG(*stmt.blockFlags(), EsStatement::BLOCKF_BODY, true);

	// バイトコードビルダーの生成フラグを設定する
	u32 bflagsBk = _bcbCur->getBuildFlag();
	*_bcbCur->buildFlag() &= ~(EsBytecodeBuilder::BF_RET_VOID | EsBytecodeBuilder::BF_RET_EXPR);

	// 最初の行番号を保存しておく
	u32 linenoFirst = _tknprs->getCurToken()->getPos()->getBeginLineNo();

	// 命令文解析
	EsListItNode* nodeStmts = parseStatementBlock();

	if(nodeStmts != 0L)
	{
		ASSERT(!TFW_IS_FLAG(_bcbCur->topEnclosingStatement()->getBlockFlags(), EsStatement::BLOCKF_SCOPE));

		// ブロック終了
		_bcbCur->popStatement();

		// 行番号を最初の行に修正する
		nodeStmts->pos()->setBeginLineNo(linenoFirst);

		// ※※※ 未実装 ※※※ 返り値がちゃんと設定されているかのチェック
	}

	// バイトコードビルダーの生成フラグの設定を戻す
	*_bcbCur->buildFlag() = bflagsBk | (_bcbCur->getBuildFlag() & EsBytecodeBuilder::BF_FUNC_FLAGS);

	return nodeStmts;
}

/*---------------------------------------------------------------------*//**
	条件文解析
	⇒ Condition
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseCondition()
{
	// ( が必ず来るはず
	if(!mustMatchToken(TKN_LP, EsError::ID_PARENTHESIS_CONDITION_LEFT))	{	return 0L;	}

	// 条件文解析
	EsItNode* nodeCond = parseParenthesisExpression();
	if(nodeCond == 0L)
	{
		return 0L;
	}

	// ) が必ず来るはず
	if(!mustMatchToken(TKN_RP, EsError::ID_PARENTHESIS_CONDITION_RIGHT))		{	return 0L;	}

	// == と = のミスタイプ防止エラーはとりあえず実装保留

	return nodeCond;
}

/*---------------------------------------------------------------------*//**
	“?:”三項条件式解析 ⇒ CondExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseConditionExpression()
{
	EsItNode* node = parseOrExpression();
	if(node == 0L)	{	return 0L;	}

	if(_tknprs->matchToken(TKN_HOOK))	// 三項条件式（xxx ? yyy : zzz）
	{
		// 分岐先１の解析
		u32 bflagBk = _bcbCur->getBuildFlag();
		TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_IN_FOR_INIT, false);	// ⇒ tc->flags &= ~TCF_IN_FOR_INIT;
		EsItNode* nodeExpr1 = parseAssignExpression();
		TFW_SET_FLAG(*_bcbCur->buildFlag(), bflagBk | (_bcbCur->getBuildFlag() & EsBytecodeBuilder::BF_FUNC_FLAGS), true);
		if(nodeExpr1 == 0L)	{	return 0L;	}

		// : が来るはず
		if(!mustMatchToken(TKN_COLON, EsError::ID_BAD_COLON_IN_HOOKCOND))	{	return 0L;	}

		// 分岐先２の解析
		EsItNode* nodeExpr2 = parseAssignExpression();
		if(nodeExpr2 == 0L)	{	return 0L;	}
		
		// 三項条件式のノードを作成する
		EsTokenPos pos(nodeExpr1->getPos(), nodeExpr2->getPos());
		EsTernaryItNode* nodeHook = makeTernaryNode(TKN_HOOK, OP_NOP, &pos, node, nodeExpr1, nodeExpr2);
		node = nodeHook;
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	OR 条件式解析 ⇒ OrExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseOrExpression()
{
	EsItNode* node = parseAndExpression();

	// “||”
	while((node != 0L) && _tknprs->matchToken(TKN_OR))
	{
		node = makeBinaryNode(TKN_OR, OP_OR, _tknprs->getCurToken()->getPos(), node, parseAndExpression());
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	AND 条件式解析 ⇒ AndExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseAndExpression()
{
	EsItNode* node = parseBitOrExpression();

	// “&&”
	while((node != 0L) && _tknprs->matchToken(TKN_AND))
	{
		node = makeBinaryNode(TKN_AND, OP_AND, _tknprs->getCurToken()->getPos(), node, parseBitOrExpression());
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	ビット OR 条件式解析 ⇒ BitOrExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseBitOrExpression()
{
	EsItNode* node = parseBitXorExpression();

	// “|”
	while((node != 0L) && _tknprs->matchToken(TKN_BITOR))
	{
		node = makeBinaryNode(TKN_BITOR, OP_BITOR, _tknprs->getCurToken()->getPos(), node, parseBitXorExpression());
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	ビット XOR 条件式解析 ⇒ BitXorExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseBitXorExpression()
{
	EsItNode* node = parseBitAndExpression();

	// “^”
	while((node != 0L) && _tknprs->matchToken(TKN_BITXOR))
	{
		node = makeBinaryNode(TKN_BITXOR, OP_BITXOR, _tknprs->getCurToken()->getPos(), node, parseBitAndExpression());
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	ビット AND 条件式解析 ⇒ BitAndExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseBitAndExpression()
{
	EsItNode* node = parseEqualExpression();

	// “&”
	while((node != 0L) && _tknprs->matchToken(TKN_BITAND))
	{
		node = makeBinaryNode(TKN_BITAND, OP_BITAND, _tknprs->getCurToken()->getPos(), node, parseEqualExpression());
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	等号式解析 ⇒ EqExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseEqualExpression()
{
	EsItNode* node = parseInequalityExpression();

	// “==”“!=”
	while((node != 0L) && _tknprs->matchToken(TKN_EQOP))
	{
		EsOpcodeType ot = _tknprs->getCurToken()->getOpcodeTypeValue();
		EsTokenPos pos(*_tknprs->getCurToken()->getPos());
		node = makeBinaryNode(TKN_EQOP, ot, &pos, node, parseInequalityExpression());
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	不等号式解析 ⇒ RelExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseInequalityExpression()
{
	// for の初期化式フラグを保存して一度外す
	bool isInForInit = TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_IN_FOR_INIT);
	TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_IN_FOR_INIT, false);

	EsItNode* node = parseShiftExpression();

	// “<”“<=”“>”“>=”
	while(
		(node != 0L) &&
		(	_tknprs->matchToken(TKN_INQOP) ||
			(!isInForInit && _tknprs->matchToken(TKN_IN)) ||	// for の初期化式ではないところでの in
			_tknprs->matchToken(TKN_INSTANCEOF)	)	)
	{
		EsTokenType tt = _tknprs->getCurToken()->getType();
		EsOpcodeType ot = _tknprs->getCurToken()->getOpcodeTypeValue();
		EsTokenPos pos(*_tknprs->getCurToken()->getPos());
		node = makeBinaryNode(tt, ot, &pos, node, parseShiftExpression());
	}

	// for の初期化式フラグを戻す
	if(isInForInit)	{	TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_IN_FOR_INIT, true);	}

	return node;
}

/*---------------------------------------------------------------------*//**
	ビットシフト式解析 ⇒ ShiftExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseShiftExpression()
{
	EsItNode* node = parseAddExpression();

	// “<<”“>>”“>>>”
	while((node != 0L) && _tknprs->matchToken(TKN_SFTOP))
	{
		EsOpcodeType ot = _tknprs->getCurToken()->getOpcodeTypeValue();
		EsTokenPos pos(*_tknprs->getCurToken()->getPos());
		node = makeBinaryNode(TKN_SFTOP, ot, &pos, node, parseAddExpression());
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	加算式解析 ⇒ AddExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseAddExpression()
{
	EsItNode* node = parseMultiplyExpression();

	// “+”“-”
	while((node != 0L) && (_tknprs->matchToken(TKN_PLUS) || _tknprs->matchToken(TKN_MINUS)))
	{
		EsTokenType tt = _tknprs->getCurToken()->getType();
		EsTokenPos pos(*_tknprs->getCurToken()->getPos());
		node = makeBinaryNode(tt, (tt == TKN_PLUS) ? OP_ADD : OP_SUB, &pos, node, parseMultiplyExpression());
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	乗算式解析 ⇒ MulExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseMultiplyExpression()
{
	EsItNode* node = parseUnaryExpression();

	// “*”“/”“%”
	while((node != 0L) && (_tknprs->matchToken(TKN_STAR) || _tknprs->matchToken(TKN_DIVOP)))
	{
		EsTokenType tt = _tknprs->getCurToken()->getType();
		EsOpcodeType ot = _tknprs->getCurToken()->getOpcodeTypeValue();
		EsTokenPos pos(*_tknprs->getCurToken()->getPos());
		node = makeBinaryNode(tt, ot, &pos, node, parseUnaryExpression());
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	単項式解析 ⇒ UnaryExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseUnaryExpression()
{
	// ※※※ 実装保留 ※※※ 再帰処理の深さチェック

	EsTokenType tt = _tknprs->nextToken();

	EsItNode* node = 0L;

	switch(tt)
	{
	case TKN_UNARYOP:
	case TKN_PLUS:
	case TKN_MINUS:
		{
			EsOpcodeType ot = _tknprs->getCurToken()->getOpcodeTypeValue();
			EsTokenPos pos(*_tknprs->getCurToken()->getPos());

			EsItNode* nodeChild = parseUnaryExpression();
			if(nodeChild == 0L)	{	return 0L;	}
			pos.end()->set(nodeChild->getPos()->getEndIndex(), nodeChild->getPos()->getEndLineNo());

			node = makeUnaryNode(TKN_UNARYOP, ot, &pos, nodeChild);	// ⇒ NewParseNode(PN_UNARY, tc);
			if(node == 0L)		{	return 0L;	}
		}
		break;

	case TKN_INC:
	case TKN_DEC:
		{
			EsTokenPos pos(*_tknprs->getCurToken()->getPos());

			EsItNode* nodeChild = parseMemberExpression(true);
			if(nodeChild == 0L)	{	return 0L;	}
			pos.end()->set(nodeChild->getPos()->getEndIndex(), nodeChild->getPos()->getEndLineNo());

			node = makeUnaryNode(tt, OP_NULL, &pos, nodeChild);		// ⇒ NewParseNode(PN_UNARY, tc);
			if(node == 0L)		{	return 0L;	}
			if(!setIncrementChild(node, nodeChild, true))	{	return 0L;	}	// ++x / --x
			node->pos()->copyEnd(nodeChild->getPos());	// ⇒ pn->pn_pos.end = pn2->pn_pos.end;
		}
		break;

	case TKN_DELETE:
		// ※※※ 実装保留 ※※※
		ASSERT(false);
		return 0L;

	case TKN_ERROR:
		ASSERT(false);
		return 0L;

	default:
		{
			_tknprs->ungetToken();

			node = parseMemberExpression(true);
			if(node == 0L)	{	return 0L;	}

			if(_tknprs->getCurScriptPos().getBeginLineNo() == node->getPos()->getBeginLineNo())	// 同じ行ならば
			{
				// “++”“--”
				EsTokenType tt = _tknprs->peekTokenSameLine();
				if((tt == TKN_INC) || (tt == TKN_DEC))
				{
					_tknprs->nextToken();	// トークンを送る
					EsTokenPos posInc(*node->getPos());
					EsItNode* nodeInc = makeUnaryNode(tt, OP_NULL, &posInc, node);	// ⇒ NewParseNode(PN_UNARY, tc);
					if(nodeInc == 0L)	{	return 0L;	}
					if(!setIncrementChild(nodeInc, node, false))	{	return 0L;	}	// x++ / x--
					nodeInc->pos()->copyBegin(node->getPos());
					node = nodeInc;
				}
			}

		}
		break;
	}

	return node;
}

/*---------------------------------------------------------------------*//**
	メンバ式解析 ⇒ MemberExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseMemberExpression(bool isAllowCallSyntax)
{
	EsTokenType tt = _tknprs->nextToken();

	// ※※※ 実装保留 ※※※ 再帰処理の深さチェック

	EsItNode* nodeRes = 0L;
	if(tt == TKN_NEW)
	{
		EsTokenPos posNew(*_tknprs->getCurToken()->getPos());

		EsItNode* nodeExpr = parseMemberExpression(false);
		if(nodeExpr == 0L)	{	return 0L;	}
		nodeExpr = checkImmediatelyAppliedLambda(nodeExpr);
		
		EsListItNode* nodeNew = makeListNode(tt, OP_NEW, nodeExpr->getPos(), nodeExpr);

		// 引数の解析
		if(_tknprs->matchToken(TKN_LP))
		{
			if(!parseArgumentList(nodeNew))	// 引数リスト解析
			{
				_ctxRef->addLeakNode(nodeNew);	// ツリー外で削除
				return 0L;
			}
		}
		if(nodeNew->getNodeCount() > NUM_ARGC_MAX)
		{
			addCompileError(EsError::ID_TOO_MANY_ARGC, 0L);
			return 0L;
		}

		nodeNew->pos()->copyEnd(nodeNew->getLastNode()->getPos());
		nodeRes = nodeNew;
	}
	else
	{
		nodeRes = parsePrimaryExpression(tt, false);	// 次のトークン下の解析
		if(nodeRes == 0L)	{	return 0L;	}

		// ※※※ 実装保留 ※※※ anyname, at, ::, OP_XMLNAME
	}

	while(true)
	{
		tt = _tknprs->nextToken();
		if((tt == TKN_ERROR) || (tt == TKN_EOF))	{	break;	}

		if(tt == TKN_DOT)	// xx.xx
		{
			// ドットの次は NAME が来るはず
			_tknprs->setParseFlag(EsTokenParser::PF_KEYWD_IS_NAME, true);
			if(!mustMatchToken(TKN_NAME, EsError::ID_NAME_AFTER_DOT))	{	return 0L;	}
			_tknprs->setParseFlag(EsTokenParser::PF_KEYWD_IS_NAME, false);

			// アクセサ取得ノード作成
			EsTokenPos pos(nodeRes->getPos(), _tknprs->getCurToken()->getPos());
			EsItNode* nodeDot = makeNameNode(OP_GETPROP, &pos, _tknprs->getCurToken()->getAtomValue(), nodeRes);
			nodeDot->setTokenType(TKN_DOT);

			nodeRes = nodeDot;
		}
		else if(tt == TKN_LB)	// xxx[yyy]
		{
			EsTokenType ttLb = _tknprs->getCurToken()->getType();
			const EsTokenPos* posLb = _tknprs->getCurToken()->getPos();

			// 角括弧内（yyy 部）の解析
			EsItNode* nodeChild = parseExpression();
			if(nodeChild == 0L)	{	return 0L;	}

			// ] で終わるはず
			if(!mustMatchToken(TKN_RB, EsError::ID_BAD_BRACKET_IN_INDEX))	{	return 0L;	}
			nodeChild->pos()->copyBegin(nodeRes->getPos());
			nodeChild->pos()->copyEnd(_tknprs->getCurToken()->getPos());

			EsItNode* nodeLb = 0L;
			// xxx['zzz'] は xxx.zzz に最適化する．
			// ただし、'zzz' が '123' などの数値の場合はインデックスとして扱う
			if(nodeChild->getTokenType() == TKN_STRING)
			{
				s32 index;
				const EsAtom* atom = ((EsNameItNode*)nodeChild)->getAtom();
				EsValue valAtom(atom);
				if(EsArray::isArrayIndexId(&index, &valAtom))	// ⇒ js_IdIsIndex(ATOM_TO_JSID(pn3->pn_atom), &index)
				{
					ASSERT(nodeChild->getNodeType() == NT_DOUBLE);
					if(nodeChild->getNodeType() == NT_DOUBLE)	// 念のため
					{
						nodeChild->setTokenType(TKN_NUMBER);
						nodeChild->setOpcodeType(OP_DOUBLE);
						((EsDoubleItNode*)nodeChild)->setDouble(index);
					}
				}
				else
				{
					// アクセサ取得（OP_GETPROP）ノードの作成
					nodeLb = makeNameNode(OP_GETPROP, posLb, atom, nodeRes);
					if(nodeLb == 0L)	{	return 0L;	}
					nodeLb->setTokenType(TKN_DOT);
				}
			}
			// 配列要素取得（OP_GETELEM）ノードの作成
			if(nodeLb == 0L)	// アクセサ取得（OP_GETPROP）ノードとして作成されなかった
			{
				nodeLb = makeBinaryNode(ttLb, OP_GETELEM, posLb, nodeRes, nodeChild);
				if(nodeLb == 0L)	{	return 0L;	}
			}

			nodeRes = nodeLb;
		}
		else if(isAllowCallSyntax && (tt == TKN_LP))	//	xx(xx)
		{
			// リストノード作成
			EsTokenType ttLp = _tknprs->getCurToken()->getType();
			EsTokenPos posLp(*_tknprs->getCurToken()->getPos());
			EsListItNode* nodeLp = makeListNode(ttLp, OP_CALL, &posLp, nodeRes);

			// 右括弧をスキップする
			while(nodeRes->getTokenType() == TKN_RP)
			{
				nodeRes = nodeRes->getChildNode();	if(nodeRes == 0L)	{	ASSERT(false);	addCompileError(EsError::ID_BAD_MEMORY_ACCESS, 0L);	return 0L;	}
			}

			if(nodeRes->getOpcodeType() == OP_NAME)
			{
				if(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_EVAL)->getAtom() == ((EsNameItNode*)nodeRes)->getAtom())
				{
					nodeLp->setOpcodeType(OP_EVAL);
					TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ, true);	// ⇒ tc->flags |= TCF_FUN_HEAVYWEIGHT;
				}
			}
			else if(nodeRes->getOpcodeType() == OP_GETPROP)
			{
				if(	(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_APPLY)->getAtom() == ((EsNameItNode*)nodeRes)->getAtom()) ||
					(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CALL)->getAtom() == ((EsNameItNode*)nodeRes)->getAtom())	)
				{
					nodeLp->setOpcodeType(OP_APPLY);
				}
			}
		
			// 引数の解析
			if(!parseArgumentList(nodeLp))	// 引数リスト解析
			{
				_ctxRef->addLeakNode(nodeLp);	// ツリー外で削除
				return 0L;
			}
			if(nodeLp->getNodeCount() > NUM_ARGC_MAX)
			{
				addCompileError(EsError::ID_TOO_MANY_ARGC, 0L);
				return 0L;
			}

			nodeLp->pos()->copyEnd(_tknprs->getCurToken()->getPos());	// 終了位置設定	⇒ pn2->pn_pos.end = CURRENT_TOKEN(ts).pos.end;
			nodeRes = nodeLp;
		}
		else
		{
			_tknprs->ungetToken();
			return nodeRes;
		}
	}

	if(tt == TKN_ERROR)
	{
		ASSERT(false);
		return 0L;
	}

	return nodeRes;
}

/*---------------------------------------------------------------------*//**
	最も小さな式の解析 ⇒ PrimaryExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parsePrimaryExpression(EsTokenType tt, bool isAfterDot)
{
	switch(tt)
	{
	case TKN_FUNCTION:
		return parseFunctionDefine(true);	// ⇒ pn = FunctionExpr(cx, ts, tc);
	case TKN_LB:
		return parsePrimaryExpression_lb();
	case TKN_LC:	// {
		return parsePrimaryExpression_lc();
	case TKN_LP:	// (
		{
			EsTokenPos posLp(*_tknprs->getCurToken()->getPos());

			// 括弧内の式のノードを得る
			EsItNode* nodeChild = parseParenthesisExpression();	// ⇒ pn2 = ParenExpr(cx, ts, tc, pn, &genexp);
			if(nodeChild == 0L)	{	return 0L;	}
			// ※※※ 未実装 ※※※ ⇒ if (genexp) return pn2;
			if(!mustMatchToken(TKN_RP, EsError::ID_PARENTHESIS_IN_PARENTHESIS))	{	return 0L;	}	// ⇒ MUST_MATCH_TOKEN(TOK_RP, JSMSG_PAREN_IN_PAREN);

			EsItNode* node;
			// 括弧は不要であったか確認する
			if(	(nodeChild->getTokenType() == TKN_RP) ||
				((EsOpcode::getSpec(nodeChild->getOpcodeType())->_precedence >= EsOpcode::getSpec(OP_GETPROP)->_precedence) && !isAfterDot))
			{
				node = nodeChild;
			}
			else
			{
				node = makeUnaryNode(TKN_RP, OP_NOP, &posLp, nodeChild);
			}
			node->pos()->end()->copy(_tknprs->getCurToken()->getPos()->getEnd());	// ⇒ pn->pn_pos.end = CURRENT_TOKEN(ts).pos.end;
			return node;
		}
		break;
	case TKN_STRING:
		{
			return makeStringNode(_tknprs->getCurToken()->getType(), _tknprs->getCurToken()->getOpcodeTypeValue(), _tknprs->getCurToken()->getPos(), _tknprs->getCurToken()->getAtomValue());
		}
		break;
	case TKN_NAME:
		{
			ASSERT(_tknprs->getCurToken()->getOpcodeTypeValue() == OP_NAME);
			EsNameItNode* node = makeNameNode(OP_NAME, _tknprs->getCurToken()->getPos(), _tknprs->getCurToken()->getAtomValue(), 0L);
			if(node == 0L)	{	return 0L;	}

			if(	TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION) &&
				(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ARGUMENTS)->getAtom() == node->getAtom()) )	// ⇒ pn->pn_atom == cx->runtime->atomState.argumentsAtom
			{
				setArgumentsFlags();	// ⇒ NoteArgumentsUse(tc);

				if(!isAfterDot && !_bcbCur->isInStatement(EsStatement::STMTT_WITH))	// ⇒ if (!afterDot && !(ts->flags & TSF_DESTRUCTURING) && !tc->inStatement(STMT_WITH))
				{
					node->setOpcodeType(OP_ARGUMENTS);
					TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG, true);
				}
			}
			else if(!isAfterDot)
			{
				EsStatement* stmt = 0L;
				_bcbCur->findLexical(&stmt, 0L, node->getAtom(), 0L);
				if((stmt == 0L) || (stmt->getType() != EsStatement::STMTT_WITH))
				{
					EsNameItNode* nodeDefNam = 0L;

					// 宣言済み語彙リストから探す
					EsAtomList* alistDecl = _bcbCur->getDeclaredNameAtomList();
					EsAtomList::EsAtomListEntity* ale = alistDecl->find(node->getAtom());	// ⇒ JSAtomListElement *ale = tc->decls.lookup(pn->pn_atom);
					if(ale != 0L)	// 宣言済み語彙が見つかった
					{
						nodeDefNam = ale->getDefName();

						while(	TFW_IS_FLAG(nodeDefNam->getDefFlag(), EsNameItNode::DF_LET) &&	// ⇒ dn->isLet()
								!checkBlockIdInScope(nodeDefNam->getBlockId())	)	// ⇒ !BlockIdInScope(dn->pn_blockid, tc)
						{
							do
							{
								ale = alistDecl->getEntity(ale->getIndex() + 1);	// ⇒ ale = ALE_NEXT(ale)
							}
							while((ale != 0L) && (ale->getAtom() != node->getAtom()));
							if(ale == 0L)
							{
								break;
							}
							nodeDefNam = ale->getDefName();
						}
					}
					if(ale != 0L)	// 宣言済み語彙が見つかった
					{
						nodeDefNam = ale->getDefName();
					}
					else			// 宣言済み語彙が見つからなかった
					{
						// 未解決語彙リストから探す
						EsAtomList* alistUn = _bcbCur->getUnresolvedNameAtomList();	// ⇒ ale = tc->lexdeps.lookup(pn->pn_atom);
						ale = alistDecl->find(node->getAtom());
						if(ale != 0L)	// 未解決語彙リストが見つかった
						{
							nodeDefNam = ale->getDefName();
						}
						else			// 未解決語彙リストが見つからなかった
						{
							// 代替語彙ノードを作成する
							nodeDefNam = makePlaceholder(node->getAtom());

							// 左括弧が続く場合は DF_FUNCARG（関数の引数）フラグを立てておく
							if(_tknprs->peekToken() == TKN_LP)
							{
								TFW_SET_FLAG(*nodeDefNam->defFlag(), EsNameItNode::DF_FUNCARG, true);
							}
						}
					}

					ASSERT(nodeDefNam != 0L);
					setLexDefNodeLink(node, nodeDefNam);	// ⇒ LinkUseToDef

					// 左括弧が続く場合は DF_FUNCARG（関数の引数）フラグを立てておく
					if(_tknprs->peekToken() == TKN_LP)
					{
						TFW_SET_FLAG(*node->defFlag(), *nodeDefNam->defFlag() & EsNameItNode::DF_FUNCARG, true);
					}
				}
			}

			return node;
		}
		break;
	case TKN_NUMBER:
		{
			ASSERT((_tknprs->getCurToken() != 0L) && (_tknprs->getCurToken()->getType() == TKN_NUMBER));
			return makeDoubleNode(_tknprs->getCurToken()->getPos(), *_tknprs->getCurToken()->getDoubleValue());	// ⇒ pn = NewParseNode(PN_NULLARY, tc); ...
		}
		break;
	case TKN_PRIMARY:
		return makeNullaryNode();		// ⇒ pn = NewParseNode(PN_NULLARY, tc);
	case TKN_ERROR:
		ESCASSERT(false);
		return 0L;
	default:
		addCompileError(EsError::ID_SYNTAX_ERROR, 0L);
		ESCASSERT(false);
		return 0L;
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	配列初期化式の解析 [～] （parsePrimaryExpression の TKN_LB）
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parsePrimaryExpression_lb()
{
	// リストノードを作成する
	EsListItNode* nodeList = makeListNode(TKN_RB, OP_NEWINIT, _tknprs->getCurToken()->getPos(), 0L);
	if(nodeList == 0L)	{	return 0L;	}

	/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, true);
	bool isMatchRb = _tknprs->matchToken(TKN_RB);
	/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, false);

	if(!isMatchRb)
	{
		s32 index = 0;
		while(true)
		{
			// 引数要素数の限界判定
			if(index >= NUM_ARRAY_INIT_MAX)	// ⇒ if (index == ARRAY_INIT_LIMIT)
			{
				addCompileError(EsError::ID_TOO_MANY_ARRAY_INIT, 0L);
				return 0L;
			}

			/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, true);
			EsTokenType tt = _tknprs->peekToken();
			/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, false);

			// カンマ後に ] で終わった
			if(tt == TKN_RB)
			{
				TFW_SET_FLAG(*nodeList->listFlag(), EsListItNode::LF_END_CAMMNA, true);	// カンマで終わっている
				break;
			}

			// [] 内の内容
			EsItNode* nodeChild;
			if(tt == TKN_COMMA)
			{
				// 空の配列初期化
				_tknprs->matchToken(TKN_COMMA);
				nodeChild = makeNullaryNode();
				TFW_SET_FLAG(*nodeList->listFlag(), EsListItNode::LF_HAS_HOLES, true);	// 配列初期化に空がある
			}
			else
			{
				// 代入式ノード取得
				nodeChild = parseAssignExpression();
			}

			// リストに追加
			nodeList->addListNode(nodeChild);

			if(tt != TKN_COMMA)	// 上記、空の配列初期化時は除外
			{
				if(!_tknprs->matchToken(TKN_COMMA))
				{
					break;	// カンマ以外が来たら終了
				}
			}

			index++;
		}

		if(!mustMatchToken(TKN_RB, EsError::ID_BRANCKET_LIST_RIGHT))	{	return 0L;	}
	}

	nodeList->pos()->end()->copy(_tknprs->getCurToken()->getPos()->getEnd());
	return nodeList;
}

/*---------------------------------------------------------------------*//**
	初期化式の解析 {～} （parsePrimaryExpression の TKN_LC）
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parsePrimaryExpression_lc()
{
	// リストノードを作成する
	EsListItNode* nodeList = makeListNode(TKN_RC, OP_NEWINIT, _tknprs->getCurToken()->getPos(), 0L);
	if(nodeList == 0L)	{	return 0L;	}

	bool isAfterComma = false;
	while(true)
	{
		EsTokenType tt;
		EsItNode* nodeId;

		_tknprs->setParseFlag(EsTokenParser::PF_KEYWD_IS_NAME, true);
		tt = _tknprs->nextToken();
		_tknprs->setParseFlag(EsTokenParser::PF_KEYWD_IS_NAME, false);
		switch(tt)
		{
		case TKN_NUMBER:
			nodeId = makeDoubleNode(_tknprs->getCurToken()->getPos(), *_tknprs->getCurToken()->getDoubleValue());
			break;
		case TKN_NAME:
		case TKN_STRING:
			nodeId = makeStringNode(tt, OP_NOP, _tknprs->getCurToken()->getPos(), _tknprs->getCurToken()->getAtomValue());
			break;
		case TKN_RC:		// 初期化式終了
			if(isAfterComma)
			{
				addCompileError(EsError::ID_TRAILING_COMMA, 0L);
				return 0L;
			}
			goto END_LC;
		default:
			addCompileError(EsError::ID_BAD_PROP_ID, 0L);
			return 0L;
		}

		tt = _tknprs->nextToken();
		EsOpcodeType ot;

		EsItNode* nodeVal;
		if(tt == TKN_COLON)	// 変数初期化のコロン
		{
			ot = _tknprs->getCurToken()->getOpcodeTypeValue();
			nodeVal = parseAssignExpression();	// 変数に代入する内容
		}
		else
		{
			// ID の後はコロンでなければならない
			addCompileError(EsError::ID_COLON_AFTER_ID, 0L);
			return 0L;
		}

		// コロン２項ノードを作成
		EsItNode* nodeColon = makeBinaryNode(TKN_COLON, ot, _tknprs->getCurToken()->getPos(), nodeId, nodeVal);
		if(nodeColon == 0L)	{	return 0L;	}

		// リストに追加
		nodeList->addListNode(nodeColon);

		tt = _tknprs->nextToken();
		// } で終了
		if(tt == TKN_RC)
		{
			goto END_LC;
		}
		// それ以外はカンマで列挙のみ許可
		if(tt != TKN_COMMA)
		{
			// リスト内の構文が不正
			addCompileError(EsError::ID_CURLY_AFTER_LIST, 0L);
			return 0L;
		}
		isAfterComma = true;	// カンマで列挙
	}

END_LC:
	nodeList->pos()->end()->copy(_tknprs->getCurToken()->getPos()->getEnd());
	return nodeList;
}

/*---------------------------------------------------------------------*//**
	引数リスト解析 ⇒ ArgumentList
**//*---------------------------------------------------------------------*/
bool EsParser::parseArgumentList(EsListItNode* nodeList)
{
	// 右括弧を探す（void 型）
	if(_tknprs->matchToken(TKN_RP))
	{
		return true;
	}

	while(true)
	{
		// 引数ノードを得る
		EsItNode* nodeArg = parseAssignExpression();
		if(nodeArg == 0L)	{	return false;	}

		// リストに追加する
		nodeList->addListNode(nodeArg);

		// カンマ以外がきたら終了
		if(!_tknprs->matchToken(TKN_COMMA))
		{
			break;
		}
	}

	// 右括弧が来るはず
	if(!_tknprs->matchToken(TKN_RP))
	{
		addCompileError(EsError::ID_PARENTHESIS_ARGS_RIGHT, 0L);
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	変数解析 ⇒ Variables
**//*---------------------------------------------------------------------*/
EsListItNode* EsParser::parseVariables(bool isLetTop)
{
	EsTokenType tt = _tknprs->getCurToken()->getType();
	bool isLet = ((tt == TKN_LET) || (tt == TKN_LP));

	// ブロックとスコープを考慮
	bool isPopScope = isLetTop || (isLet && TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_IN_FOR_INIT));
	EsStatement* stmtEnclBk = _bcbCur->topEnclosingStatement();
	EsStatement* stmtScopeBk = _bcbCur->topScopeStatement();

	// 正しいツリーコンテキストを得る
	EsStatement* stmtScope = _bcbCur->topScopeStatement();
	if(isLet)
	{
		while((stmtScope != 0L) && !TFW_IS_FLAG(stmtScope->getBlockFlags(), EsStatement::BLOCKF_SCOPE))
		{
			stmtScope = stmtScope->getDownScopeStatement();
		}
		ASSERT(stmtScope != 0L);
	}

	// 変数ノードを作成する
	EsOpcodeType otBind = isLet ? OP_NOP : _tknprs->getCurToken()->getOpcodeTypeValue();
	EsListItNode* nodeList = makeListNode(TKN_VAR, otBind, _tknprs->getCurToken()->getPos(), 0L);
	if(nodeList == 0L)	{	return 0L;	}

	bool isFreshBind = true;	// ⇒ data.fresh

	while(true)
	{
		tt = _tknprs->nextToken();

		if(tt != TKN_NAME)	// 変数名が来るはずである
		{
			if(tt != TKN_ERROR)
			{
				addCompileError(EsError::ID_NO_VARIABLE_NAME, 0L);
			}
			return 0L;
		}

		const EsAtom* atom = _tknprs->getCurToken()->getAtomValue();
		EsNameItNode* nodeName = makeNameNode(OP_NAME, _tknprs->getCurToken()->getPos(), atom, 0L);	// ⇒ NewBindingNode
		// データバインド処理 ⇒ data.binder(cx, &data, atom, tc)
		if(isLet)
		{
			if(!bindLetData(nodeName, otBind, &isFreshBind))	{	return 0L;	}	// ⇒ if (!data.binder(cx, &data, atom, tc)) -> BindLet
		}
		else
		{
			if(!bindVarOrConstData(nodeName, otBind, &isFreshBind))	{	return 0L;	}	// ⇒ if (!data.binder(cx, &data, atom, tc)) -> BindVarOrConst
		}
		nodeList->addListNode(nodeName);	// ⇒ pn->append(pn2);

		// = で変数に値が代入されていいる
		if(_tknprs->matchToken(TKN_ASSIGN))
		{
			if(_tknprs->getCurToken()->getOpcodeTypeValue() != OP_NOP)
			{
				goto ERROR_VAR_INIT;
			}

			// スコープの移動
			if(isPopScope)
			{
				_bcbCur->setTopEnclosingStatement(stmtEnclBk->getDownEnclosingStatement());
				_bcbCur->setTopScopeStatement(stmtScopeBk->getDownScopeStatement());
			}
			// = 後の式を解析
			EsItNode* nodeInit = parseAssignExpression();
			// スコープを戻す
			if(isPopScope)
			{
				_bcbCur->setTopEnclosingStatement(stmtEnclBk);
				_bcbCur->setTopScopeStatement(stmtScopeBk);
			}
			if(nodeInit == 0L)		{	return 0L;	}

			// 語彙ノード設定
			if(nodeName->isHasLex())	// 既に語彙ノードを持っている場合
			{
				nodeName = makeAssignmentNode(nodeName, nodeInit);
				if(nodeName == 0L)	{	return 0L;	}
			}
			else
			{
				nodeName->setExpressionNode(nodeInit);
			}

			// 演算子の決定
			EsOpcodeType otName;
			if(nodeName->getOpcodeType() == OP_ARGUMENTS)							{	otName = OP_SETNAME;	}
			else if(TFW_IS_FLAG(nodeName->getDefFlag(), EsNameItNode::DF_GVAR))		{	otName = OP_SETGVAR;	}
			else if(TFW_IS_FLAG(nodeName->getDefFlag(), EsNameItNode::DF_BOUND_LG))	{	otName = OP_SETLOCAL;	}
			else if(otBind == OP_DEFCONST)											{	otName = OP_SETCONST;	}
			else																	{	otName = OP_SETNAME;	}
			nodeName->setOpcodeType(otName);

			setLeftValueFlags(nodeName, isFreshBind ?  EsNameItNode::DF_INITED : EsNameItNode::DF_ASSIGNED);	// ⇒ NoteLValue(cx, pn2, tc, data.fresh ? PND_INITIALIZED : PND_ASSIGNED);
	
			nodeName->pos()->copyEnd(nodeInit->getPos());	// ⇒ pn2->pn_pos.end = init->pn_pos.end;

			// arguments アクセサ
			if(TFW_IS_FLAG(_bcbCur->getBuildFlag(), EsBytecodeBuilder::BF_IN_FUNCTION) &&	// ⇒ if ((tc->flags & TCF_IN_FUNCTION) &&
				(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ARGUMENTS)->getAtom() == nodeName->getAtom()) )	// ⇒ atom == cx->runtime->atomState.argumentsAtom
			{
				setArgumentsFlags();	// ⇒ NoteArgumentsUse(tc);

				if(!isLet)
				{
					TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_FUNC_NEED_CALLOBJ, true);	// ⇒ tc->flags |= TCF_FUN_HEAVYWEIGHT;
				}
			}
		}

		if(!_tknprs->matchToken(TKN_COMMA))	{	break;	}
	}

	nodeList->pos()->copyEnd(nodeList->getLastNode()->getPos());	// ⇒ pn->pn_pos.end = pn->last()->pn_pos.end;
	return nodeList;

ERROR_VAR_INIT:
	addCompileError(EsError::ID_BAD_VAR_INIT, 0L);
	return 0L;
}

/*---------------------------------------------------------------------*//**
	() 括弧付の式を解析 ⇒ ParenExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseParenthesisExpression()
{
	ASSERT(_tknprs->getCurToken()->getType() == TKN_LP);
	EsTokenPos posBegin(*_tknprs->getCurToken()->getPos());

	EsItNode* node = parseBracketExpression();
	if(node == 0L)	{	return 0L;	}

	return node;
}

/*---------------------------------------------------------------------*//**
	[] 括弧付の式を解析
	⇒ BracketedExpr
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseBracketExpression()
{
	u32 bflagBk = _bcbCur->getBuildFlag();
	TFW_SET_FLAG(*_bcbCur->buildFlag(), EsBytecodeBuilder::BF_AMONG_FOR_INIT, false);

	EsItNode* node = parseExpression();

	*_bcbCur->buildFlag() = bflagBk | (_bcbCur->getBuildFlag() & EsBytecodeBuilder::BF_FUNC_FLAGS);
	return node;
}

/*---------------------------------------------------------------------*//**
	let 文ブロックを解析
	⇒ LetBlock
**//*---------------------------------------------------------------------*/
EsItNode* EsParser::parseLet(bool isStatement)
{
	EsTokenType ttLet = _tknprs->getCurToken()->getType();
	EsOpcodeType otLet = _tknprs->getCurToken()->getOpcodeTypeValue();
	const EsTokenPos* posLet = _tknprs->getCurToken()->getPos();

	// ( が来るはずである
	if(!mustMatchToken(TKN_LP, EsError::ID_PARENTHESIS_LET_LEFT))	{	return 0L;	}	// ⇒ MUST_MATCH_TOKEN(TOK_WHILE, JSMSG_WHILE_AFTER_DO);

	// let ブロックスコープに入る
	EsStatement stmt;
	EsNameItNode* nodeBlock = pushLexicalScope(&stmt);
	EsItNode* nodeRet = nodeBlock;	// 返答ノード候補

	// let(xxx) における xxx 部の変数定義解析
	EsItNode* nodeLetLeft = parseVariables(true);

	// ) が来るはずである
	if(!mustMatchToken(TKN_RP, EsError::ID_PARENTHESIS_LET_RIGHT))	{	return 0L;	}	// ⇒ MUST_MATCH_TOKEN(TOK_WHILE, JSMSG_WHILE_AFTER_DO);
	
	/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, true);
	if(isStatement && !_tknprs->matchToken(TKN_LC))
	{
		EsUnaryItNode* node = makeUnaryNode(TKN_SEMI, OP_NOP, _tknprs->getCurToken()->getPos(), nodeBlock);
		nodeRet = node;	// 返答ノードを変更
		isStatement = false;
	}
	/// 不要かも _tknprs->setParseFlag(EsTokenParser::PF_OPERAND, false);

	EsItNode* nodeLetRight = 0L;
	if(isStatement)
	{
		nodeLetRight = parseStatementBlock();
		if(nodeLetRight == 0L)	{	return 0L;	}
		if(!mustMatchToken(TKN_RC, EsError::ID_CURLY_AFTER_LET))	{	return 0L;	}
	}
	else
	{
		nodeBlock->setOpcodeType(OP_LEAVEBLOCKEXPR);
		nodeLetRight = parseAssignExpression();
		if(nodeLetRight == 0L)	{	return 0L;	}
	}

	// let ノードを作る
	EsItNode* nodeLet = makeBinaryNode(ttLet, otLet, posLet, nodeLetLeft, nodeLetRight);
	nodeBlock->setExpressionNode(nodeLet);

	// ステートメントを戻す
	_bcbCur->popStatement();

	return nodeRet;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
