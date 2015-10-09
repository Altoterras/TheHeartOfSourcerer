/***********************************************************************//**
 *	EsBytecodeBuilder.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsBytecodeBuilder.h"

// Friends
#include "EsAtomList.h"
#include "EsBcExInfo.h"
#include "EsBlock.h"
#include "EsContext.h"
#include "EsSysClasses.h"
#include "EsSysPrototypes.h"
#include "EsFunction.h"
#include "EsInterlang.h"
#include "EsItNode.h"
#include "EsFunctionBox.h"
#include "EsKeywordSet.h"
#include "EsObject.h"
#include "EsObjectOp.h"
#include "EsProgramCounter.h"
#include "EsScope.h"
#include "EsScopeProperty.h"
#include "EsStackFrame.h"
#include "EsStatement.h"
#include "EsToken.h"
#include "EsTokenGetter.h"
#include "EsValue.h"

// External
#include "../../tfw/collection/InsArray.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/common/IdManager.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#if defined(_DEBUG)
	#if 0
		#define TRACE_OP		TRACE
	#else
		#define TRACE_OP		//
	#endif
#else
#define TRACE_OP		//
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsBytecodeBuilder メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ブロック ID を得る
	⇒ blockid()
**//*---------------------------------------------------------------------*/
u32 EsBytecodeBuilder::getCurrentBlockId() const
{
	return (_stmtTopEncl != 0L) ? _stmtTopEncl->getBlockId() : _blockidCur;
}

/*---------------------------------------------------------------------*//**
	トップレベルステートメントかどうかを得る
	⇒ atTopLevel
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::isTopLevelStatement() const
{
	if(_stmtTopEncl == 0L)	{	return true;	}
	return TFW_IS_FLAG(_stmtTopEncl->getBlockFlags(), EsStatement::BLOCKF_BODY);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ノードツリーからバイトコードを作成する ⇒ js_EmitTree
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildFromTree(EsItNode* nodeT)
{
	if(nodeT == 0L)	{	return false;	}
	nodeT->setBytecodeOffset(bcoffset());

	EsTokenType ttT = nodeT->getTokenType();
	switch(ttT)
	{
	case TKN_FUNCTION:
		return buildSub_function(nodeT);
	case TKN_LEXICALSCOPE:
		return buildSub_lexicalscope(nodeT);
	case TKN_NEW:
	case TKN_LP:			// (
		return buildSub_lp(nodeT);
	case TKN_RP:			// )
		return buildSub_rp(nodeT);
	case TKN_LC:			// {
		return buildSub_lc(nodeT);
	case TKN_RC:			// }
		return buildSub_rc(nodeT);
	case TKN_LB:			// [
		return buildSub_lb(nodeT);
	case TKN_RB:			// ]
		return buildSub_rb(nodeT);
	case TKN_SEQ:
		return buildSub_seq(nodeT);
	case TKN_SEMI:
		return buildSub_semi(nodeT);
	case TKN_RETURN:
		return buildSub_return(nodeT);
	case TKN_VAR:
		return buildSub_var(nodeT, false);
	case TKN_LET:
		return buildSub_let(nodeT);
	case TKN_UPVARS:
		return buildSub_upvars(nodeT);
	case TKN_NAME:
		return buildSub_name(nodeT);
	case TKN_NUMBER:
		return buildSub_number(nodeT);
	case TKN_STRING:
		return putupAtom((EsNameItNode*)nodeT, nodeT->getOpcodeType(), 0);
	case TKN_COLON:
		return buildSub_colon(nodeT);
	case TKN_COMMA:
		return buildSub_comma(nodeT);
		break;
	case TKN_ASSIGN:
		return buildSub_assign(nodeT);
	case TKN_DOT:
		return buildSub_dot(nodeT);
	case TKN_UNARYOP:
		return buildSub_unaryop(nodeT);
	case TKN_INC:
	case TKN_DEC:
		return buildSub_incdec(nodeT);
	case TKN_AND:
	case TKN_OR:
		return buildSub_andor(nodeT);
	case TKN_BITOR:
	case TKN_BITXOR:
	case TKN_BITAND:
	case TKN_EQOP:
	case TKN_INQOP:
	case TKN_IN:
	case TKN_INSTANCEOF:
	case TKN_SFTOP:
	case TKN_PLUS:
	case TKN_MINUS:
	case TKN_STAR:
	case TKN_DIVOP:
		return buildSub_binary(nodeT);
	case TKN_WHILE:
		return buildSub_while(nodeT);
	case TKN_DO:
		return buildSub_do(nodeT);
	case TKN_FOR:
		return buildSub_for(nodeT);
	case TKN_IF:
		return buildSub_if(nodeT);
	case TKN_HOOK:
		return buildSub_hook(nodeT);
	case TKN_SWITCH:
		return buildSub_switch(nodeT);
	case TKN_BREAK:
		return buildSub_break(nodeT);
	case TKN_CONTINUE:
		return buildSub_continue(nodeT);
	case TKN_WITH:
		return buildSub_with(nodeT);
	case TKN_ARGSBODY:
		return buildSub_argsbody(nodeT);
	case TKN_PRIMARY:
		return buildSub_primary(nodeT);
		
	case TKN_TRY:
	case TKN_CATCH:
	case TKN_THROW:
	case TKN_DELETE:
	case TKN_REGEXP:
		// ※※※ 実装保留 ※※※
		return false;

	default:
		// エラー
		ASSERT(false);
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 関数本体 ⇒ js_EmitFunctionScript
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildFunctionBody(EsItNode* nodeFunc)
{
	if(!buildFromTree(nodeFunc))	{	return false;	}
	if(!putupStop())				{	return false;	}

	ASSERT(_funcCur != 0L);
	EsInterlang* il = _ctxRef->newInterlang();
	if(!il->create(this))			{	return false;	}
	_funcCur->setInterlang(il);	// ⇒ fun->u.i.script = script;

	return true;
}

/*---------------------------------------------------------------------*//**
	最終調整処理
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::finalize()
{
	// グローバル変数とローカル変数のスロットインデックスの重複を解消する
	s32 numGvar = getGlobalVarsNum();
	if(numGvar > 0)
	{
		if(numGvar >= EsBytecodeBuilder::MAX_SLOTIDX)
		{
			_ctxRef->getError()->addCompileError(EsError::ID_TOO_MANY_LOCALS, EsTokenPos(), 0L);
			return false;
		}
		else
		{
			if(bc()->getLength() > 0)
			{
				EsProgramCounter pcWk(bc());
				while(true)
				{
					if(_isAddExInfo)
					{
						pcWk.advance(EsBcExInfo::LEN_BC);
					}
					EsOpcodeType ot = (EsOpcodeType)pcWk.get(0);
					s32 bclen = pcWk.getBytecodeLength(0);
					const EsOpcode::Spec* spec = EsOpcode::getSpec(ot);
					u32 type = spec->_format & EsOpcode::OPFMT_TYPEMASK;
					if((type == EsOpcode::OPFMT_LOCAL) || (type == EsOpcode::OPFMT_SLOTATOM))
					{
						// スロットインデックスをグローバル変数分移動
						s32 slotidx = pcWk.getU16(1);	// ⇒ slot = GET_SLOTNO(code);
						slotidx += numGvar;
						if(slotidx >= EsBytecodeBuilder::MAX_SLOTIDX)
						{
							_ctxRef->getError()->addCompileError(EsError::ID_TOO_MANY_LOCALS, EsTokenPos(), 0L);
							return false;
						}
						// スロットインデックスを書き換える
						_bcCur->code()->setAt(pcWk.getCurrentOffset() + 1, TFW_U16_TO_U8_HI(slotidx));
						_bcCur->code()->setAt(pcWk.getCurrentOffset() + 2, TFW_U16_TO_U8_LO(slotidx));
					}
					if(!pcWk.isSafeOffset(bclen))
					{
						break;	// 終了
					}
					pcWk.advance(bclen);
				}
			}
		}
	}

	// ストップバイトコードを追加して終了
	if(!putupStop())		{	return false;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	指定文字列のステートメント情報を検索する ⇒ js_LexicalLookup
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::findLexical(EsStatement** stmtOut, s32* slotidxOut, const EsAtom* atom, EsStatement* stmtStart)
{
	if(stmtStart == 0L)
	{
		stmtStart = _stmtTopScope;
	}

	EsStatement* stmtWk;
	for(stmtWk = stmtStart; stmtWk != 0L; stmtWk = stmtWk->getDownScopeStatement())
	{
		if(stmtWk->getType() == EsStatement::STMTT_WITH)
		{
			break;		// with の下位は検索不要
		}

		if(!TFW_IS_FLAG(stmtWk->getBlockFlags(), EsStatement::BLOCKF_SCOPE))
		{
			continue;	// スコープではない
		}

		EsObject* obj = stmtWk->getBlockObject();	ASSERT(obj != 0L);
		EsScope* scope = obj->getScope();			ASSERT(scope != 0L);
		EsValue valAtom(atom);
		EsScopeProperty* sprop = scope->findScopeProperty(&valAtom);
		if(sprop != 0L)
		{
			if(stmtOut != 0L)	{	*stmtOut = stmtWk;														}
			if(slotidxOut != 0L){	*slotidxOut = obj->getSlots()->getBlockDepth() + sprop->getShortId();	}	// ⇒ *slotp = JSVAL_TO_INT(obj->fslots[JSSLOT_BLOCK_DEPTH]) + sprop->shortid;
			return true;
		}
	}

	if(stmtOut != 0L)	{	*stmtOut = stmtWk;	}
	if(slotidxOut != 0L){	*slotidxOut = -1;	}
	return false;
}

/*---------------------------------------------------------------------*//**
	ステートメントを積む ⇒ js_PushStatement
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::pushStatement(EsStatement* stmt, s32 bcofsTop)
{
	stmt->setTopBcOffset(bcofsTop);
	stmt->setDownEnclosingStatement(_stmtTopEncl);
	_stmtTopEncl = stmt;

	if(stmt->isLinksScope())
	{
		stmt->setDownScopeStatement(_stmtTopScope);
		_stmtTopScope = stmt;
	}
	else
	{
		stmt->setDownScopeStatement(0L);
	}
}

/*---------------------------------------------------------------------*//**
	ステートメントを取り出す ⇒ js_PopStatement, PopStatement
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::popStatement(bool isDeleteScopeDecl)
{
	EsStatement* stmt = _stmtTopEncl;

	if(isDeleteScopeDecl)
	{
		if(TFW_IS_FLAG(stmt->getBlockFlags(), EsStatement::BLOCKF_SCOPE))
		{
			EsObject* obj = stmt->getBlockObject();
			EsScope* scope = obj->getScope();
			ASSERT(scope->getOwnerObject() == obj);

			for(EsScopeProperty* sprop = scope->getLastScopeProperty(); sprop != 0L; sprop = sprop->getParent())
			{
				ASSERT(sprop->getId()->isAtom());
				const EsAtom* atom = sprop->getId()->getAtom();

				if(atom == EsKeywordSet::getStatic(EsKeywordSet::S_EMPTY)->getAtom())
				{
					continue;
				}
				_alistDecl->remove(atom);
			}
		}
	}

	_stmtTopEncl = stmt->getDownEnclosingStatement();

	if(stmt->isLinksScope())
	{
		_stmtTopScope = stmt->getDownScopeStatement();
		if(TFW_IS_FLAG(stmt->getBlockFlags(), EsStatement::BLOCKF_SCOPE))
		{
			_objBlockChain = stmt->getBlockObject()->slots()->parentObject();	// ⇒ tc->blockChain = STOBJ_GET_PARENT(stmt->blockObj);
		}
	}
}

/*---------------------------------------------------------------------*//**
	指定ステートメント内かを調べる ⇒ inStatement
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::isInStatement(s32 statementtype)
{
	for(EsStatement* stmt = _stmtTopEncl; stmt != 0L; stmt = stmt->getDownEnclosingStatement())
	{
		if(stmt->getType() == statementtype)
		{
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	ブロックスコープを積む ⇒ js_PushBlockScope
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::pushBlockScope(EsStatement* stmt, EsObject* objBlock, s32 bcofsTop)
{
	pushStatement(stmt, bcofsTop);
	TFW_SET_FLAG(*stmt->blockFlags(), EsStatement::BLOCKF_SCOPE, true);
	objBlock->slots()->setParentObject(_objBlockChain);
	stmt->setDownScopeStatement(_stmtTopScope);
	_stmtTopScope = stmt;
	_objBlockChain = objBlock;
	stmt->setBlockObject(objBlock);
}

/*---------------------------------------------------------------------*//**
	ブロック ID を作成する ⇒ GenerateBlockId
**//*---------------------------------------------------------------------*/
u32 EsBytecodeBuilder::makeBlockId()
{
	return ++_blockidGenCnt;
}

/*---------------------------------------------------------------------*//**
	子ブロック解析開始時の親からの設定継承
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::inheritForChildBlockParseStart(EsBytecodeBuilder* parent)
{
	_parent = parent;
	_bflags |= parent->_bflags;
	_blockidGenCnt = parent->_blockidGenCnt + 1;
	_depthNest = parent->_depthNest + 1;
	_isAddExInfo = parent->_isAddExInfo;
}

/*---------------------------------------------------------------------*//**
	子ブロック解析終了時の子からの設定継承
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::inheritForChildBlockParseEnd(EsBytecodeBuilder* child)
{
	_blockidGenCnt = child->_blockidGenCnt + 1;
}

/*---------------------------------------------------------------------*//**
	子ブロック生成開始時の親からの設定継承
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::inheritForChildBlockBuildStart(EsBytecodeBuilder* parent, EsFunctionBox* funcbox)
{
	_parent = parent;
	_bflags = funcbox->getBuildFlag() | BF_IN_FUNCTION;
	_funcCur = funcbox->getFunction();
	_funcboxCur = funcbox;
	_depthNest = parent->_depthNest + 1;	// ⇒ cg2->staticLevel = cg->staticLevel + 1;
	_isAddExInfo = parent->_isAddExInfo;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsBytecodeBuilder::EsBytecodeBuilder(EsContext* ctxRef)
	: _ctxRef(ctxRef)
	, _parent(0L)
	, _alistDecl(new EsAtomList())
	, _alistUnrslv(new EsAtomList())
	, _stmtTopEncl(0L)
	, _stmtTopScope(0L)
	, _objScopeChain(0L)
	, _objBlockChain(0L)
	, _isAddExInfo(false)
	, _bflags(0)
	, _bcCur(&_bcMain)
	, _alistLiteral(new EsAtomList())
	, _alistConst(new EsAtomList())
	, _alistUpvar(new EsAtomList())
	, _arrUpvarMap(0L)
	, _depthStack(0)
	, _depthStackMax(0)
	, _numGlobalVars(0)
	, _depthNest(0)
	, _blockidGenCnt(0)
	, _blockidCur(0)
	, _nodeBlockCur(0L)
	, _funcCur(0L)
	, _funcboxCur(0L)
	, _funcboxTree(0L)
	, _listObjbox(new List<EsObjectBox*>(false))
	, _sfrmCaller(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsBytecodeBuilder::~EsBytecodeBuilder()
{
	delete _listObjbox;
	delete _arrUpvarMap;
	delete _alistUpvar;
	delete _alistConst;
	delete _alistLiteral;
	delete _alistUnrslv;
	delete _alistDecl;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 関数 ⇒ case TOK_FUNCTION:
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_function(EsItNode* nodeT)
{
	// 関数に関する情報を取得する
	ASSERT(nodeT->getNodeType() == NT_NAME);
	EsNameItNode* nodeFunc = (EsNameItNode*)nodeT;
	EsFunctionBox* funcbox = nodeFunc->getFunctionBox(); ASSERT(funcbox != 0L);
	EsFunction* func = funcbox->getFunction(); ASSERT(func != 0L);
	ASSERT(func->isInterpreted());

	// 中間言語生成済みの場合は NOP を生成する
	if(func->getInterlang() != 0L)	// 生成済み ⇒ if (fun->u.i.script)
	{
		ASSERT(nodeFunc->getOpcodeType() == OP_NOP);
		ASSERT(TFW_IS_FLAG(_bflags, BF_IN_FUNCTION));
		if(!buildSub_functionNop())	{	return false;	}
		return true;
	}

	// 本体生成
	{
		EsBytecodeBuilder bcbFunc(_ctxRef);
		bcbFunc.inheritForChildBlockBuildStart(this, funcbox);
		if(!bcbFunc.buildFunctionBody(nodeFunc->getExpressionNode()))	{	return false;	}	// ⇒ if (!js_EmitFunctionScript(cx, cg2, pn->pn_body))
	}

	// 外部スクリプト関数として追加し、そのインデックスを得る
	s32 indexObjectBoxList = _listObjbox->getCount();
	_listObjbox->addTail(funcbox);

	// 直接定義の閉じたオブジェクトのバイトコードを生成する
	EsOpcodeType ot = nodeFunc->getOpcodeType();
	if(ot != OP_NOP)
	{
		if(TFW_IS_FLAG(funcbox->getBuildFlag(), EsBytecodeBuilder::BF_GENEXP_LAMBDA))
		{
			return false;
		}

		// オブジェクトボックスのインデックスを出力する
		if(!putupIndex(ot, indexObjectBoxList, nodeFunc, 0L))	{	return false;	}
		return true;
	}

	// トップレベルの関数は前もって _bcProg に生成される
	// しかし、メインスクリプトはその必要は無く _bcMain に生成される
	if(!TFW_IS_FLAG(_bflags, BF_IN_FUNCTION))
	{
		ASSERT(_stmtTopEncl == 0L);

		// オブジェクトインデックス出力
		switchToProg();	// _bcProg へ生成
		ot = TFW_IS_FLAG(func->getFlags(), EsFunction::F_CLOSURE_FLAT) ? OP_DEFFUN_FC : OP_DEFFUN;	// ⇒ op = FUN_FLAT_CLOSURE(fun) ? JSOP_DEFFUN_FC : JSOP_DEFFUN;
		if(!putupIndex(ot, indexObjectBoxList, nodeFunc, 0L))	{	return false;	}
		switchToMain();	// _bcMain へ生成

		if(!buildSub_functionNop())		{	return false;	}

		// 外部拡張に関数情報を追加する
		if(nodeFunc->getAtom() != 0L)
		{
			_ctxRef->addParsedFunctionInfoForExternalExtension(nodeFunc->getAtom(), true);
		}
	}
	else	// ローカル関数定義
	{
		ASSERT(_funcCur != 0L);

		// スロットインデックス、オブジェクトインデックス出力
		EsFunction::LocalVarKind lvk = EsFunction::NUM_LVK;
		u32 slotidx = 0;
		_funcCur->findLocalVar(&lvk, &slotidx, _funcCur->getAtom());	// ⇒ js_LookupLocal(cx, cg->fun, fun->atom, &slot);
		ASSERT((lvk == EsFunction::LVK_VAR) || (lvk == EsFunction::LVK_CONST));
		nodeFunc->setFunctionIndex(indexObjectBoxList);					// ⇒ pn->pn_index = index;
		ot = TFW_IS_FLAG(func->getFlags(), EsFunction::F_CLOSURE_FLAT) ? OP_DEFLOCALFUN_FC : OP_DEFLOCALFUN;	// ⇒ op = FUN_FLAT_CLOSURE(fun) ? JSOP_DEFLOCALFUN_FC : JSOP_DEFLOCALFUN;
		if(!putupSlotIndex(ot, slotidx, indexObjectBoxList, nodeFunc, 0L))	{	return false;	}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 生成済み関数用
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_functionNop()
{
	if(!appendExInfoBc(0L, 0L))	{	return false;	}
	return append1Bc(OP_NOP, 0L);
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - TKN_LEXICALSCOPE
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_lexicalscope(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_NAME);
	EsNameItNode* nodeLxscp = (EsNameItNode*)nodeT;

	EsStatement stmt;
	EsObjectBox* objbox = nodeLxscp->getObjectBox();
	pushBlockScope(&stmt, objbox->getBlockObject(), bcoffset());	// ⇒ js_PushBlockScope(cg, &stmtInfo, objbox->object, CG_OFFSET(cg));

	if(!putupEnterBlock(nodeLxscp, 0L))	{	return false;	}

	if(!buildFromTree(nodeLxscp->getExpressionNode()))	{	return false;	}

	s32 blockcnt = objbox->getBlockObject()->getScope()->getScopePropertyNum();	// ⇒ count = OBJ_BLOCK_COUNT(cx, objbox->object);
	if(!appendExInfoBc(0L, 0L))	{	return false;	}
	if(!append3Bc(nodeLxscp->getOpcodeType(), TFW_U16_TO_U8_HI(blockcnt), TFW_U16_TO_U8_LO(blockcnt), 0L))	{	return false;	}	// ⇒ EMIT_UINT16_IMM_OP(op, count);
	
	popStatementAndWriteJumpOffset();

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 左括弧 (
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_lp(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_LIST);

	EsListItNode* nodeListT = (EsListItNode*)nodeT;
	EsItNode* nodeListHead = nodeListT->getHeadNode();
	switch(nodeListHead->getTokenType())
	{
	case TKN_NAME:
		if(!putupName((EsNameItNode*)nodeListHead, true, 0L))	{	return false;	}
		break;

	case TKN_DOT:
		ASSERT(nodeListHead->getNodeType() == NT_NAME);
		if(!putupProperty((EsNameItNode*)nodeListHead, nodeListHead->getOpcodeType(), true, 0L))	{	return false;	}
		break;

	case TKN_LB:
		if(!putupElement(nodeListHead, OP_CALLELEM, 0L))	{	return false;	}
		break;

	case TKN_UNARYOP:
	default:
		if(!buildFromTree(nodeListHead))	{	return false;	}
		if(!appendExInfoBc(nodeListT, 0L))	{	return false;	}
		if(!append1Bc(OP_NULL, 0L))			{	return false;	}
	}

	// 引数
	EsItNode* nodeListWk = nodeListHead->getNextNode();
	while(nodeListWk != 0L)
	{
		if(!buildFromTree(nodeListWk))		{	return false;	}
		nodeListWk = nodeListWk->getNextNode();
	}

	// 自身の命令演算子と引数の数を出力
	u32 cntArg = nodeListT->getNodeCount() - 1;
	if(!appendExInfoBc(nodeListT, 0L))	{	return false;	}
	if(!append3Bc(nodeListT->getOpcodeType(), TFW_U16_TO_U8_HI(cntArg), TFW_U16_TO_U8_LO(cntArg), 0L))	{	return false;	}

	if(nodeListT->getOpcodeType() == OP_EVAL)
	{
		u32 lineno = nodeListT->getPos()->getBeginLineNo();
		if(!appendExInfoBc(nodeListT, 0L))	{	return false;	}
		if(!append3Bc(OP_LINENO, TFW_U16_TO_U8_HI(lineno), TFW_U16_TO_U8_LO(lineno), 0L))	{	return false;	}	// ⇒ EMIT_UINT16_IMM_OP(JSOP_LINENO, pn->pn_pos.begin.lineno);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 右括弧 )
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_rp(EsItNode* nodeT)
{
	u32 bflagsBk = _bflags;
	TFW_SET_FLAG(_bflags, BF_IN_FOR_INIT, false);
	if(!buildFromTree(nodeT->getChildNode()))		{	return false;	}
	TFW_SET_FLAG(_bflags, BF_IN_FOR_INIT, TFW_IS_FLAG(bflagsBk, BF_IN_FOR_INIT));
	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 左大括弧 {
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_lc(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_LIST);

	// ⇒ Note?

	// 新しいステートメントを積む
	EsStatement stmt(EsStatement::STMTT_BLOCK);
	pushStatement(&stmt, nodeT->getBytecodeOffset());

	EsListItNode* nodeListT = (EsListItNode*)nodeT;
	EsItNode* nodeListChild = nodeListT->getHeadNode();

	if(TFW_IS_FLAG(*nodeListT->listFlag(), EsListItNode::LF_FUNCDEF))
	{
		// 関数定義
		ASSERT(TFW_IS_FLAG(_bflags, BF_IN_FUNCTION));

		// ※※※ 実装無 ※※※ ⇒ if (pn->pn_xflags & PNX_DESTRUCT) { ... }

		EsItNode* nodeListWk = nodeListChild;
		while(nodeListWk != 0L)
		{
			if(nodeListWk->getTokenType() == TKN_FUNCTION)
			{
				if(nodeListWk->getOpcodeType() == OP_NOP)
				{
					if(!buildFromTree(nodeListWk))	{	return false;	}
				}
				else
				{
					// OP_DEFFUN はトップレベルブロックにの関数定義である
					// 『if (true) function x() {}』は『if (true)』に最適化する
					ASSERT(nodeListWk->getOpcodeType() == OP_DEFFUN);
				}
			}

			nodeListWk = nodeListWk->getNextNode();
		}
	}

	// 子要素をそれぞれ出力
	EsItNode* nodeListWk = nodeListChild;
	while(nodeListWk != 0L)
	{
		if(!buildFromTree(nodeListWk))	{	return false;	}
		nodeListWk = nodeListWk->getNextNode();
	}

	// ⇒ Note?

	// ステートメントを戻す
	popStatementAndWriteJumpOffset();

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 右大括弧 }
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_rc(EsItNode* nodeT)
{
	if(!appendExInfoBc(nodeT, 0L))		{	return false;	}
	if(!append2Bc(OP_NEWINIT, 0, 0L))	{	return false;	}

	ASSERT(nodeT->getNodeType() == NT_LIST);
	for(EsItNode* nodeWk = ((EsListItNode*)nodeT)->getHeadNode(); nodeWk != 0L; nodeWk = nodeWk->getNextNode())
	{
		ASSERT(nodeWk->getNodeType() == NT_BINARY);
		EsItNode* nodeLeft = ((EsBinaryItNode*)nodeWk)->getLeftNode();
		EsItNode* nodeRight = ((EsBinaryItNode*)nodeWk)->getRightNode();

		if(nodeLeft->getTokenType() == TKN_NUMBER)
		{
			ASSERT(nodeLeft->getNodeType() == NT_DOUBLE);
			if(!putupNumber(((EsDoubleItNode*)nodeLeft)->getDouble(), nodeLeft, 0L))	{	return false;	}
		}

		if(!buildFromTree(nodeRight))		{	return false;	}

		if(nodeLeft->getTokenType() == TKN_NUMBER)
		{
			if(!appendExInfoBc(nodeLeft, 0L))	{	return false;	}
			if(!append1Bc(OP_INITELEM, 0L))		{	return false;	}
		}
		else
		{
			ASSERT((nodeLeft->getTokenType() == TKN_NAME) || (nodeLeft->getTokenType() == TKN_STRING));
			ASSERT(nodeLeft->getNodeType() == NT_NAME);
			EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(((EsNameItNode*)nodeLeft)->getAtom());	// ⇒ ale = cg->atomList.add(cg->compiler, pn3->pn_atom);
			if(ale == 0L)	{	return false;	}
			if(!putupIndex(OP_INITPROP, ale->getIndex(), nodeLeft, 0L))	{	return false;	}
		}
	}

	if(!appendExInfoBc(nodeT, 0L))	{	return false;	}
	if(!append1Bc(OP_ENDINIT, 0L))	{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 左角括弧 [
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_lb(EsItNode* nodeT)
{
	return putupElement(nodeT, nodeT->getOpcodeType(), 0L);
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 右角括弧 ]
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_rb(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_LIST);
	EsListItNode* nodeRb = (EsListItNode*)nodeT;

	EsOpcodeType ot;
	if((nodeRb->getNodeCount() <= 0xffff) && (TFW_IS_FLAG(_bflags, EsBytecodeBuilder::BF_IN_FUNCTION)))
	{
		ot = OP_NEWARRAY;
	}
	else
	{
		ot = OP_NEWINIT;

		if(!appendExInfoBc(nodeT, 0L))								{	return false;	}
		if(!append2Bc(ot, EsSysPrototypes::PTYPEKIND_ARRAY, 0L))	{	return false;	}	// ⇒ js_Emit2(cx, cg, op, (jsbytecode) JSProto_Array)
	}

	EsItNode* nodeWk = nodeRb->getHeadNode();
	s32 atomidx = 0;
	while(nodeWk != 0L)
	{
		if(ot == OP_NEWINIT)
		{
			if(!putupNumber(atomidx, nodeWk, 0L))	{	return false;	}
		}

		if(nodeWk->getTokenType() == TKN_COMMA)
		{
			if(!appendExInfoBc(nodeT, 0L))			{	return false;	}
			if(!append1Bc(OP_HOLE, 0L))				{	return false;	}
		}
		else
		{
			if(!buildFromTree(nodeWk))				{	return false;	}
		}

		if(ot == OP_NEWINIT)
		{
			if(!appendExInfoBc(nodeT, 0L))			{	return false;	}
			if(!append1Bc(OP_INITELEM, 0L))			{	return false;	}
		}

		nodeWk = nodeWk->getNextNode();
		atomidx++;
	}
	ASSERT(atomidx == nodeRb->getNodeCount());

	if(ot == OP_NEWINIT)
	{
		if(!appendExInfoBc(nodeT, 0L))			{	return false;	}
		if(!append1Bc(OP_ENDINIT, 0L))			{	return false;	}
		return true;
	}

	ASSERT(atomidx <= 0xffff);
	if(!appendExInfoBc(nodeT, 0L))			{	return false;	}
	if(!append3Bc(OP_NEWARRAY, TFW_U16_TO_U8_HI(atomidx), TFW_U16_TO_U8_LO(atomidx), 0L))	{	return false;	}	// ⇒ EMIT_UINT16_IMM_OP(JSOP_NEWARRAY, atomIndex);
	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - TKN_SEQ
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_seq(EsItNode* nodeT)
{
	// 新しいステートメントを積む
	EsStatement stmt(EsStatement::STMTT_SEQ);
	pushStatement(&stmt, nodeT->getBytecodeOffset());

	ASSERT(nodeT->getNodeType() == NT_LIST);
	for(EsItNode* nodeWk = ((EsListItNode*)nodeT)->getHeadNode(); nodeWk != 0L; nodeWk = nodeWk->getNextNode())
	{
		if(!buildFromTree(nodeWk))
		{
			return false;
		}
	}

	// ステートメントを戻す
	popStatementAndWriteJumpOffset();
	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - セミコロン
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_semi(EsItNode* nodeT)
{
	EsItNode* nodeChild = nodeT->getChildNode();
	if(nodeChild == 0L)
	{
		return true;
	}

	bool isWantV = !TFW_IS_FLAG(_bflags, BF_IN_FUNCTION | BF_DONOT_WANT_RET_VAL);
	bool isUseful = isWantV;

	if(!isUseful)
	{
		if(!decideUsefulSideNodeOperation(&isUseful, nodeChild))	{	return false;	}
	}
	if(!isUseful)
	{
		if(	(_stmtTopEncl == 0L) ||
			(_stmtTopEncl->getType() != EsStatement::STMTT_LABEL) ||
			(_stmtTopEncl->getUpdateBcOffset() < bcoffset())	)
		{
			// エラー
			_ctxRef->getError()->addCompileError(EsError::ID_USELESS_EXPRESSION, *nodeT->getPos(), 0L);
			return true;
		}
	}

	EsOpcodeType ot = isWantV ? OP_POPV : OP_POP;

	if(ot != OP_NOP)
	{
		if(!buildFromTree(nodeChild))		{	return false;	}

		// 自身の出力
		if(!appendExInfoBc(nodeT, 0L))		{	return false;	}
		if(!append1Bc(ot, 0L))				{	return false;	}
		
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 変数 ⇒ EmitVariables
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_var(EsItNode* nodeT, bool isInLetHead)
{
	ASSERT(nodeT->getNodeType() == NT_LIST);
	EsListItNode* nodeVar = (EsListItNode*)nodeT;
	EsOpcodeType otVar = nodeVar->getOpcodeType();

	bool isLet = (otVar == OP_NOP);
	bool isForInVar = TFW_IS_FLAG(*nodeVar->listFlag(), EsListItNode::LF_FORINVAR);
	bool isForInLet = isLet && isForInVar;
	bool isPopScope = isInLetHead || (isLet && TFW_IS_FLAG(getBuildFlag(), EsBytecodeBuilder::BF_IN_FOR_INIT));

	EsStatement* stmtSaveEncl = topEnclosingStatement();
	EsStatement* stmtSaveScope = topScopeStatement();

	EsNameItNode* nodeListWk = (EsNameItNode*)nodeVar->getHeadNode();
	while(nodeListWk != 0L)
	{
		EsItNode* nodeInit;
		if(nodeListWk->getTokenType() != TKN_NAME)
		{
			ASSERT(nodeListWk->getTokenType() == TKN_ASSIGN);
			ASSERT(!isForInVar);
			ASSERT((nodeListWk->getNodeType() == NT_BINARY) || ((nodeListWk->getNodeType() == NT_NAME)));
			ASSERT(((EsBinaryItNode*)nodeListWk)->getLeftNode()->getTokenType() == TKN_NAME);
			nodeInit = ((EsBinaryItNode*)nodeListWk)->getRightNode();
			nodeListWk = (EsNameItNode*)((EsBinaryItNode*)nodeListWk)->getLeftNode();
		}
		else
		{
			nodeInit = nodeListWk->getExpressionNode();
		}

		if(!bindNameAndSlot(nodeListWk))
		{
			return false;
		}

		u32 atomidx = 0;
		EsOpcodeType ot = nodeListWk->getOpcodeType();
		if(ot == OP_ARGUMENTS)
		{
			// 引数には初期化式はない
			nodeInit = 0L;
		}
		else
		{
			if(!putupVarDecl(nodeListWk, otVar, 0L, &atomidx))	{	return false;	}

			if(nodeInit != 0L)
			{
				ASSERT(!isForInVar);
				if(ot == OP_SETNAME)
				{
					if(!putupIndex(OP_BINDNAME, atomidx, nodeInit, 0L))		{	return false;	}
				}
				if(otVar == OP_DEFCONST)
				{
					if(!defineConstant(nodeListWk->getAtom(), nodeInit))	{	return false;	}	// ⇒ js_DefineCompileTimeConstant
				}

				// スコープの移動
				if(isPopScope)
				{
					setTopEnclosingStatement(stmtSaveEncl->getDownEnclosingStatement());
					setTopScopeStatement(stmtSaveScope->getDownScopeStatement());
				}
				// 式を出力
				u32 bflagsBk = _bflags;
				_bflags &= ~BF_IN_FOR_INIT;
				if(!buildFromTree(nodeInit))		{	return false;	}
				_bflags |= bflagsBk & BF_IN_FOR_INIT;
				// スコープを戻す
				if(isPopScope)
				{
					setTopEnclosingStatement(stmtSaveEncl);
					setTopScopeStatement(stmtSaveScope);
				}
			}
		}

		if(isForInVar)
		{
			break;
		}

		// ⇒ Note?

		// リスト要素の演算子とアトムインデックスを書き込む
		if(ot == OP_ARGUMENTS)
		{
			if(!appendExInfoBc(nodeT, 0L))		{	return false;	}
			if(!append1Bc(ot, 0L))				{	return false;	}
		}
		else if(!nodeListWk->getUpvarInfo()->isFree())
		{
			if(!appendExInfoBc(nodeT, 0L))		{	return false;	}
			if(!append3Bc(ot, TFW_U16_TO_U8_HI(atomidx), TFW_U16_TO_U8_LO(atomidx), 0L))	{	return false;	}	// ⇒ EMIT_UINT16_IMM_OP(op, index);
		}
		else
		{
			if(!putupIndex(ot, atomidx, nodeT, 0L))	{	return false;	}
		}

		nodeListWk = (EsNameItNode*)nodeListWk->getNextNode();
	}

	if(TFW_IS_FLAG(*nodeVar->parseFlag(), EsItNode::PF_POPVAR))
	{
		if(!appendExInfoBc(nodeT, 0L))			{	return false;	}
		if(!append1Bc(OP_POP, 0L))				{	return false;	}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - let
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_let(EsItNode* nodeT)
{
	EsItNode* nodeLet = nodeT;
	EsItNode* nodeBody = 0L;

	if(nodeT->getNodeType() == NT_BINARY)
	{
		nodeLet = ((EsBinaryItNode*)nodeT)->getLeftNode();
		nodeBody = ((EsBinaryItNode*)nodeT)->getRightNode();
	}

	// 変数宣言の出力
	ASSERT(nodeLet->getNodeType() == NT_LIST);
	if(!buildSub_var(nodeLet, nodeBody != 0L))	{	return false;	}

	// let ブロックもしくは式本体の出力
	if(nodeBody != 0L)
	{
		if(!buildFromTree(nodeBody))	{	return false;	}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - TKN_UPVARS
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_upvars(EsItNode* nodeT)
{
	ASSERT(_alistUnrslv->getCount() == 0);
	ASSERT(nodeT->getNodeType() == NT_NAMESET);
	EsNameSetItNode* nodeUpvars = (EsNameSetItNode*)nodeT;
	ASSERT(nodeUpvars->getNamesSet()->getCount() > 0);
	_alistUnrslv->copy(nodeUpvars->getNamesSet());
	return buildFromTree(nodeUpvars->getSubTree());
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - return
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_return(EsItNode* nodeT)
{
	EsItNode* nodeChild = nodeT->getChildNode();
	if(nodeChild != 0L)
	{
		if(!buildFromTree(nodeChild))				{	return false;	}
	}
	else
	{
		if(!appendExInfoBc(nodeT, 0L))				{	return false;	}
		if(!append1Bc(OP_PUSH, 0L))					{	return false;	}
	}

	s32 bcofsAfterReturn;
	if(!appendExInfoBc(nodeT, 0L))					{	return false;	}
	if(!append1Bc(OP_RETURN, &bcofsAfterReturn))	{	return false;	}
	if(!putupNonLocalJump(0L, nodeT, 0L))			{	return false;	}

	if(bcofsAfterReturn != bcoffset())	// putupNonLocalJump によるバイトコードが出力されている場合 ⇒ if (top + JSOP_RETURN_LENGTH != CG_OFFSET(cg))
	{
		bc()->setAt(bcofsAfterReturn, (u8)OP_SETRVAL);	// OP_SETRVAL に書き換える ⇒ CG_BASE(cg)[top] = JSOP_SETRVAL;
		if(!appendExInfoBc(nodeT, 0L))	{	return false;	}
		if(!append1Bc(OP_RETRVAL, 0L))	{	return false;	}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 数値
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_number(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_DOUBLE);
	return putupNumber(((EsDoubleItNode*)nodeT)->getDouble(), nodeT, 0L);
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 名前ノード
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_name(EsItNode* nodeT)
{
	EsOpcodeType ot = nodeT->getOpcodeType();
	if(ot == OP_NOP)	{	return true;	}

	ASSERT(nodeT->getNodeType() == NT_NAME);
	if(!putupName((EsNameItNode*)nodeT, false, 0L))	{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - カンマ（,）
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_comma(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_LIST);
	EsListItNode* nodeComma = (EsListItNode*)nodeT;

	// リストの各ノードのバイトコード出力
	EsItNode* nodeWk = nodeComma->getHeadNode();
	while(true)
	{
		if(!buildFromTree(nodeWk))					{	return false;	}

		if(nodeWk->getNextNode() == 0L)	{	break;	}

		if(!appendExInfoBc(nodeWk, 0L))				{	return false;	}
		if(!append1Bc(OP_POP, 0L))					{	return false;	}

		nodeWk = nodeWk->getNextNode();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 等号 ⇒ js_EmitTree case TOK_ASSIGN
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_assign(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_BINARY);
	EsBinaryItNode* nodeBin = (EsBinaryItNode*)nodeT;
	EsItNode* nodeL = nodeBin->getLeftNode();
	ASSERT(nodeL->getTokenType() != TKN_RP);

	u32 atomidx = 0;

	// 左ノードオペランドの確認と専用コードの生成
	switch(nodeL->getTokenType())
	{
	case TKN_NAME:
		{
			ASSERT(nodeL->getNodeType() == NT_NAME);
			EsNameItNode* nodeNameL = (EsNameItNode*)nodeL;
			// ネームとスロットを関連付ける
			if(!bindNameAndSlot(nodeNameL))	{	return false;	}
			// アトムインデックスを得る
			if(!nodeNameL->getUpvarInfo()->isFree())
			{
				atomidx = nodeNameL->getUpvarInfo()->getSlotIndex();
			}
			else
			{
				EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(nodeNameL->getAtom());
				if(ale == 0L)	{	return false;	}
				atomidx = ale->getIndex();

				if(!TFW_IS_FLAG(*nodeNameL->defFlag(), EsNameItNode::DF_CONST))
				{
					if(!putupIndex(OP_BINDNAME, atomidx, nodeL, 0L))	{	return false;	}
				}
			}
		}
		break;

	case TKN_DOT:
		{
			ASSERT(nodeL->getNodeType() == NT_NAME);
			EsNameItNode* nodeNameL = (EsNameItNode*)nodeL;
			if(!buildFromTree(nodeNameL->getExpressionNode()))	{	return false;	}

			EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(nodeNameL->getAtom());
			if(ale == 0L)	{	return false;	}
			atomidx = ale->getIndex();
		}
		break;

	case TKN_LB:
		{
			ASSERT(nodeL->getNodeType() == NT_BINARY);
			EsBinaryItNode* nodeBinL = (EsBinaryItNode*)nodeL;
			if(!buildFromTree(nodeBinL->getLeftNode()))		{	return false;	}
			if(!buildFromTree(nodeBinL->getRightNode()))	{	return false;	}
		}
		break;

	case TKN_LP:
		{
			if(!buildFromTree(nodeL))		{	return false;	}
		}
		break;

	default:
		ASSERT(false);
		return false;
	}

	EsOpcodeType otT = nodeBin->getOpcodeType();

	// += などの対応．左ノードを展開し、値を得る
	if(otT != OP_NOP)
	{
		switch(nodeL->getTokenType())
		{
		case TKN_NAME:
			ASSERT(nodeL->getNodeType() == NT_NAME);
			if(TFW_IS_FLAG(((EsNameItNode*)nodeL)->getDefFlag(), EsNameItNode::DF_CONST))	// ⇒ if (pn2->isConst())
			{
				if(nodeL->getOpcodeType() == OP_CALLEE)
				{
					if(!appendExInfoBc(nodeL, 0L))				{	return false;	}
					if(!append1Bc(OP_CALLEE, 0L))				{	return false;	}
				}
				else
				{
					if(!putupIndex(nodeL->getOpcodeType(), atomidx, nodeL, 0L))	{	return false;	}
				}
			}
			else if(nodeL->getOpcodeType() == OP_SETNAME)
			{
				if(!appendExInfoBc(nodeL, 0L))				{	return false;	}
				if(!append1Bc(OP_DUP, 0L))					{	return false;	}
				if(!putupIndex(OP_GETXPROP, atomidx, nodeL, 0L))	{	return false;	}
			}
			else
			{
				EsOpcodeType otL;
				if(nodeL->getOpcodeType() == OP_SETGVAR)		{	otL = OP_GETGVAR;	}
				else if(nodeL->getOpcodeType() == OP_GETUPVAR)	{	otL = OP_GETUPVAR;	}
				else if(nodeL->getOpcodeType() == OP_SETARG)	{	otL = OP_GETARG;	}
				else											{	otL = OP_GETLOCAL;	}
				if(!putupIndex(otL, atomidx, nodeL, 0L))		{	return false;	}
			}
			break;

		case TKN_DOT:
			{
				if(!appendExInfoBc(nodeL, 0L))					{	return false;	}
				if(!append1Bc(OP_DUP, 0L))						{	return false;	}

				ASSERT(nodeL->getNodeType() == NT_NAME);
				const EsAtom* atom = ((EsNameItNode*)nodeL)->getAtom();
				if(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom() == atom)
				{
					// length 固定アクセサ
					if(!appendExInfoBc(nodeL, 0L))						{	return false;	}
					if(!append1Bc(OP_LENGTH, 0L))						{	return false;	}
				}
#if 0	// ※MozillaJs の特殊プロパティのためサポートしない
				else if(_ctxRef->getKeywordSet()->findFixedProperty(EsKeywordSet::FPK_PROTO, atom) != 0L)
				{
					// prototype 固定アクセサ
					if(!putupIndex(OP_QNAMEPART, atomidx, nodeL, 0L))	{	return false;	}
					if(!appendExInfoBc(nodeL, 0L))						{	return false;	}
					if(!append1Bc(OP_GETELEM, 0L))						{	return false;	}
				}
#endif
				else
				{
					if(!putupIndex(OP_GETPROP, atomidx, nodeL, 0L))		{	return false;	}
				}
			}
			break;

		case TKN_LB:
		case TKN_LP:
			if(!appendExInfoBc(nodeL, 0L))				{	return false;	}
			if(!append1Bc(OP_DUP2, 0L))					{	return false;	}
			if(!appendExInfoBc(nodeL, 0L))				{	return false;	}
			if(!append1Bc(OP_GETELEM, 0L))				{	return false;	}
			break;
		}
	}

	// 右ノードの出力
	if(!buildFromTree(nodeBin->getRightNode()))		{	return false;	}

	// += などの対応
	if(otT != OP_NOP)
	{
		if(!appendExInfoBc(nodeL, 0L))				{	return false;	}
		if(!append1Bc(otT, 0L))						{	return false;	}
	}

	// 専用コード
	switch(nodeL->getTokenType())
	{
	case TKN_NAME:
		{
			ASSERT(nodeL->getNodeType() == NT_NAME);
			EsNameItNode* nodeNameL = (EsNameItNode*)nodeL;
			if(TFW_IS_FLAG(nodeNameL->getDefFlag(), EsNameItNode::DF_CONST))
			{
				break;
			}
			if(!putupIndex(nodeL->getOpcodeType(), atomidx, nodeL, 0L))	{	return false;	}
		}
		break;

	case TKN_DOT:
		if(!putupIndex(nodeL->getOpcodeType(), atomidx, nodeL, 0L))		{	return false;	}
		break;

	case TKN_LB:
	case TKN_LP:
		if(!appendExInfoBc(nodeL, 0L))	{	return false;	}
		if(!append1Bc(OP_SETELEM, 0L))	{	return false;	}
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - ドット（.）
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_dot(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_NAME);
	return putupProperty((EsNameItNode*)nodeT, nodeT->getOpcodeType(), false, 0L);
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - プラス（+）, マイナス（-）, typeof など
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_unaryop(EsItNode* nodeT)
{
	EsOpcodeType otT = nodeT->getOpcodeType();
	EsItNode* nodeChild = nodeT->getChildNode();

	if(otT == OP_TYPEOF)
	{
		EsItNode* nodeWk = nodeChild;
		while(nodeWk->getTokenType() == TKN_RP)
		{
			nodeWk = nodeWk->getChildNode();
		}
		if(nodeWk->getTokenType() != TKN_NAME)
		{
			otT = OP_TYPEOFEXPR;
		}
	}

	u32 bflagsBk = _bflags;
	TFW_SET_FLAG(_bflags, BF_IN_FOR_INIT, false);									// ⇒ cg->flags &= ~TCF_IN_FOR_INIT;
	if(!buildFromTree(nodeChild))	{	return false;	}
	TFW_SET_FLAG(_bflags, BF_IN_FOR_INIT, TFW_IS_FLAG(bflagsBk, BF_IN_FOR_INIT));	// ⇒ cg->flags |= oldflags & TCF_IN_FOR_INIT;

	if(!appendExInfoBc(nodeT, 0L))	{	return false;	}
	if(!append1Bc(otT, 0L))			{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - インクリメント（++）／デクリメント（--）
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_incdec(EsItNode* nodeT)
{
	EsItNode* nodeChild = nodeT->getChildNode();
	ASSERT((nodeChild != 0L) && (nodeChild->getTokenType() != TKN_RP));

	EsOpcodeType otT = nodeT->getOpcodeType();

	switch(nodeChild->getTokenType())
	{
	case TKN_NAME:
		{
			ASSERT(nodeChild->getNodeType() == NT_NAME);
			EsNameItNode* nodeNameChild = (EsNameItNode*)nodeChild;
			nodeChild->setOpcodeType(otT);
			if(!bindNameAndSlot(nodeNameChild))	{	return false;	}

			EsOpcodeType ot = nodeChild->getOpcodeType();
			if(ot == OP_CALLEE)
			{
				if(!appendExInfoBc(nodeChild, 0L))				{	return false;	}
				if(!append1Bc(ot, 0L))							{	return false;	}
			}
			else if(!nodeNameChild->getUpvarInfo()->isFree())
			{
				u32 atomidx = nodeNameChild->getUpvarInfo()->getSlotIndex();
				if(!putupIndex(ot, atomidx, nodeChild, 0L))	{	return false;	}
			}
			else
			{
				if(!putupAtom(nodeNameChild, ot, 0L))	{	return false;	}
			}

			if(TFW_IS_FLAG(nodeNameChild->getDefFlag(), EsNameItNode::DF_CONST))
			{
				if(!appendExInfoBc(nodeChild, 0L))				{	return false;	}
				if(!append1Bc(OP_POS, 0L))						{	return false;	}

				const EsOpcode::Spec* spec = EsOpcode::getSpec(otT);
				if(!TFW_IS_FLAG(spec->_format, EsOpcode::OPFMT_POST))
				{
					if(!appendExInfoBc(nodeChild, 0L))			{	return false;	}
					if(!append1Bc(OP_ONE, 0L))					{	return false;	}
					ot = TFW_IS_FLAG(spec->_format, EsOpcode::OPFMT_INC) ? OP_ADD : OP_SUB;
					if(!appendExInfoBc(nodeChild, 0L))			{	return false;	}
					if(!append1Bc(ot, 0L))						{	return false;	}
				}
			}
		}
		break;

	case TKN_DOT:
		ASSERT(nodeChild->getNodeType() == NT_NAME);
		if(!putupProperty((EsNameItNode*)nodeChild, otT, false, 0L))	{	return false;	}
		break;

	case TKN_LB:
		if(!putupElement(nodeChild, otT, 0L))		{	return false;	}
		break;

	case TKN_LP:
		if(!buildFromTree(nodeChild))				{	return false;	}
		if(!appendExInfoBc(nodeChild, 0L))			{	return false;	}
		if(!append1Bc(otT, 0L))						{	return false;	}
		break;

	default:
		ASSERT(false);
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 論理積（&&）／論理和（||）
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_andor(EsItNode* nodeT)
{
	if(nodeT->getNodeType() == NT_LIST)
	{
		EsListItNode* nodeList = (EsListItNode*)nodeT;

		s32 bcofsListTopPos = bcoffset();

		// 先頭ノードの出力
		EsItNode* nodeWk = nodeList->getHeadNode();
		if(!buildFromTree(nodeWk))												{	return false;	}
		s32 bcofsBppTopPos = bcoffset();
		if(!putupJump(OP_BACKPATCH_POP, 0, nodeWk, &bcofsBppTopPos))			{	return false;	}
		s32 bcofsBppPosPrev = bcofsBppTopPos;

		// 先頭と末尾の間のノードの出力
		s32 bcofsJumpPos = bcofsBppTopPos; 
		nodeWk = nodeWk->getNextNode();
		ASSERT(nodeWk != 0L);
		while(nodeWk->getNextNode() != 0L)
		{
			if(!buildFromTree(nodeWk))												{	return false;	}
			s32 bcofsBppPosCur;
			if(!putupJump(OP_BACKPATCH_POP, 0, nodeWk, &bcofsBppPosCur))			{	return false;	}

			// 一つ前の OP_BACKPATCH_POP との差を仮で書き換える
			writeJumpOffsetLater(bcofsBppPosPrev, bcofsBppPosCur - bcofsBppPosPrev);

			bcofsBppPosPrev = bcofsBppPosCur;
			nodeWk = nodeWk->getNextNode();
		}

		// 末尾ノードの出力
		ASSERT(nodeWk != 0L);
		if(!buildFromTree(nodeWk))												{	return false;	}

		// OP_BACKPATCH_POP の飛び先を設定する
		nodeWk = nodeList->getHeadNode();
		s32 bcofsTermPos = bcoffset();
		s32 bcofsBppPosCur = bcofsBppTopPos;
		while(nodeWk->getNextNode() != 0L)
		{
			// 仮で書き換えた OP_BACKPATCH_POP のオフセットから次の位置を得る
			s32 bcofsBppPosNext = bc()->getAt(bcofsBppPosCur + 1) << 8 | bc()->getAt(bcofsBppPosCur + 2);	// ⇒ GetJumpOffset, GET_JUMP_OFFSET

			// OP_BACKPATCH_POP の本当の飛び先を決定する ⇒ CHECK_AND_SET_JUMP_OFFSET_AT
			writeJumpOffsetLater(bcofsBppPosCur, bcofsTermPos - bcofsBppPosCur);

			bc()->setAt(bcofsBppPosCur, nodeT->getOpcodeType());	// ⇒ *pc = pn->pn_op;

			bcofsBppPosCur = bcofsBppPosNext;
			nodeWk = nodeWk->getNextNode();
		}

		return true;
	}
	else if(nodeT->getNodeType() == NT_BINARY)
	{
		EsBinaryItNode* nodeBin = (EsBinaryItNode*)nodeT;
		EsItNode* nodeLeft = nodeBin->getLeftNode();
		EsItNode* nodeRight = nodeBin->getRightNode();

		// 左ノード出力
		if(!buildFromTree(nodeLeft))											{	return false;	}
		s32 bcofsBppPosLeft;
		if(!putupJump(OP_BACKPATCH_POP, 0, nodeLeft, &bcofsBppPosLeft))		{	return false;	}

		// 右ノード出力
		if(!buildFromTree(nodeRight))											{	return false;	}

		// OP_BACKPATCH_POP の飛び先を決定する ⇒ CHECK_AND_SET_JUMP_OFFSET_AT
		writeJumpOffsetLater(bcofsBppPosLeft, bcoffset() - bcofsBppPosLeft);

		bc()->setAt(bcofsBppPosLeft, nodeT->getOpcodeType());	// ⇒ *pc = pn->pn_op;

		return true;
	}
	
	ASSERT((nodeT->getNodeType() == NT_LIST) || (nodeT->getNodeType() == NT_BINARY));	// NT_LIST か NT_BINARY のはず
	return false;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - 二項演算
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_binary(EsItNode* nodeT)
{
	if(nodeT->getNodeType() == NT_LIST)
	{
		EsListItNode* nodeListT = (EsListItNode*)nodeT;
		EsOpcodeType otT = nodeT->getOpcodeType();

		// リスト子要素についてそれぞれ出力する
		EsItNode* nodeListWk = nodeListT->getHeadNode();
		while(nodeListWk != 0L)
		{
			if(!buildFromTree(nodeListWk))	{	return false;	}

			if(nodeListWk != nodeListT->getHeadNode())	// 最初の要素以外
			{
				// 自身の演算子を出力する
				if(!appendExInfoBc(nodeT, 0L))		{	return false;	}
				if(!append1Bc(otT, 0L))				{	return false;	}
			}

			nodeListWk = nodeListWk->getNextNode();
		}

		return true;
	}
	else if(nodeT->getNodeType() == NT_BINARY)
	{
		EsBinaryItNode* nodeBinT = (EsBinaryItNode*)nodeT;

		// 左右要素についてそれぞれ出力する
		if(!buildFromTree(nodeBinT->getLeftNode()))		{	return false;	}
		if(!buildFromTree(nodeBinT->getRightNode()))	{	return false;	}

		// 自身の演算子を出力する
		EsOpcodeType otT = nodeT->getOpcodeType();
		if(!appendExInfoBc(nodeT, 0L))		{	return false;	}
		if(!append1Bc(otT, 0L))				{	return false;	}

		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - while
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_while(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_BINARY);
	EsBinaryItNode* nodeWhile = (EsBinaryItNode*)nodeT;
	EsItNode* nodeCond = nodeWhile->getLeftNode();
	EsItNode* nodeLoopBody = nodeWhile->getRightNode();

	// 新しいステートメントを積む
	EsStatement stmt(EsStatement::STMTT_WHILE);
	pushStatement(&stmt, nodeT->getBytecodeOffset());

	// ⇒ Note?

	// まず条件を評価するための goto を生成
	s32 bcofsGotoPos = 0;				// ⇒ jmp
	if(!putupJump(OP_GOTO, 0, nodeCond, &bcofsGotoPos))	{	return false;	}

	// ループの戻り先を生成
	s32 bcofsLoopTopPos = bcoffset();	// ⇒ top
	if(!appendExInfoBc(nodeLoopBody, 0L))					{	return false;	}
	if(!append1Bc(OP_LOOP, 0L))								{	return false;	}

	// ループ内のステートメントを出力
	if(!buildFromTree(nodeLoopBody))						{	return false;	}

	// 前の goto の飛び先（ここ : 条件文）を決定する ⇒ CHECK_AND_SET_JUMP_OFFSET_AT
	s32 bcofsGotoToCondOffset = bcoffset() - bcofsGotoPos;
	writeJumpOffsetLater(bcofsGotoPos, bcofsGotoToCondOffset);

	// 条件を出力
	if(!buildFromTree(nodeCond))							{	return false;	}
	s32 bcofsIfnePos;
	if(!putupJump(OP_IFNE, 0, nodeCond, &bcofsIfnePos))	{	return false;	}
	writeJumpOffsetLater(bcofsIfnePos, bcofsLoopTopPos - bcofsIfnePos);	// ⇒ beq = EmitJump(cx, cg, JSOP_IFNE, top - CG_OFFSET(cg));

	// ⇒ Note?

	// ステートメントを戻す
	popStatementAndWriteJumpOffset();
	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - do
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_do(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_BINARY);
	EsBinaryItNode* nodeDo = (EsBinaryItNode*)nodeT;
	EsItNode* nodeLoopBody = nodeDo->getLeftNode();
	EsItNode* nodeCond = nodeDo->getRightNode();

	// ⇒ Note?

	// ループの戻り先を生成
	s32 bcofsLoopTopPos = bcoffset();	// ⇒ top
	if(!appendExInfoBc(nodeLoopBody, 0L))								{	return false;	}
	if(!append1Bc(OP_LOOP, 0L))											{	return false;	}

	// 新しいステートメントを積む ⇒ js_PushStatement(cg, &stmtInfo, STMT_DO_LOOP, top);
	EsStatement stmt(EsStatement::STMTT_DO);
	pushStatement(&stmt, bcofsLoopTopPos);

	// ループ内のステートメントを出力 ⇒ if (!js_EmitTree(cx, cg, pn->pn_left))
	if(!buildFromTree(nodeLoopBody))									{	return false;	}

	// continue 処理のためにループや閉じたステートメントラベルのバイトコード
	EsStatement* stmtWk = &stmt;
	while(true)
	{
		stmtWk->setUpdateBcOffset(bcoffset());			// ⇒ stmt->update = CG_OFFSET(cg);
		stmtWk = stmtWk->getDownEnclosingStatement();
		if(stmtWk == 0L)									{	break;	}
		if(stmtWk->getType() != EsStatement::STMTT_LABEL)	{	break;	}
	}

	// 条件を出力
	if(!buildFromTree(nodeCond))												{	return false;	}
	s32 bcofsIfnePos;
	if(!putupJump(OP_IFNE, 0, nodeCond, &bcofsIfnePos))						{	return false;	}
	writeJumpOffsetLater(bcofsIfnePos, bcofsLoopTopPos - bcofsIfnePos);	// ⇒ beq = EmitJump(cx, cg, JSOP_IFNE, top - CG_OFFSET(cg))

	// ⇒ Note?

	// ステートメントを戻す
	popStatementAndWriteJumpOffset();
	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - for
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_for(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_BINARY);
	EsBinaryItNode* nodeFor = (EsBinaryItNode*)nodeT;
	EsItNode* nodeForHead = nodeFor->getLeftNode();
	EsItNode* nodeLoopBody = nodeFor->getRightNode();

	// 新しいステートメントを積む
	EsStatement stmt(EsStatement::STMTT_FOR);
	pushStatement(&stmt, nodeT->getBytecodeOffset());

	if(nodeForHead->getTokenType() == TKN_IN)	// for in の場合
	{
		stmt.setType(EsStatement::STMTT_FORIN);

		// in の左辺の変数定義部を出力
		EsItNode* nodeForVars = ((EsBinaryItNode*)nodeForHead)->getLeftNode();
		EsTokenType ttForVars = nodeForVars->getTokenType();
		if((ttForVars == TKN_VAR) || (ttForVars == TKN_LET))	// ⇒ if (TOKEN_TYPE_IS_DECL(tt)) {
		{
			TFW_SET_FLAG(_bflags, BF_IN_FOR_INIT, true);
			if(!buildFromTree(nodeForVars))		{	return false;	}
			TFW_SET_FLAG(_bflags, BF_IN_FOR_INIT, false);
		}

		// in の右辺の出力
		if(!buildFromTree(((EsBinaryItNode*)nodeForHead)->getRightNode()))	{	return false;	}

		// イテレータ命令の出力
		ASSERT(nodeFor->getOpcodeType() == OP_ITER);
		if(!appendExInfoBc(nodeFor, 0L))									{	return false;	}
		if(!append2Bc(OP_ITER, nodeFor->getIterFlag(), 0L))					{	return false;	}

		// ⇒ Note?

		// for ヘッダ処理への goto を生成
		s32 bcofsGotoPos = 0;																		// ⇒ jmp
		if(!putupJump(OP_GOTO, 0, nodeForHead, &bcofsGotoPos))				{	return false;	}	// ⇒ jmp = EmitJump(cx, cg, JSOP_GOTO, 0);

		// ループの戻り先を生成
		s32 bcofsLoopTopPos = bcoffset();	// ⇒ top
		stmt.setTopBcOffset(bcofsLoopTopPos);
		if(!appendExInfoBc(nodeLoopBody, 0L))	{	return false;	}
		if(!append1Bc(OP_LOOP, 0L))				{	return false;	}

		s32 depthLoop = _depthStack;

		EsOpcodeType ot = OP_SETNAME;
		switch(ttForVars)
		{
		case TKN_LET:
		case TKN_VAR:
		case TKN_NAME:
			// let, var は変数自体を参照する
			if((ttForVars == TKN_LET) || (ttForVars == TKN_VAR))
			{
				ASSERT(nodeForVars->getNodeType() == NT_LIST);
				ASSERT(((EsListItNode*)nodeForVars)->getNodeCount() == 1);
				nodeForVars = ((EsListItNode*)nodeForVars)->getHeadNode();
			}

			// for ローカル変数定義の出力
			{
				// ⇒ Note?

				EsNameItNode* nodeNameForVars = (EsNameItNode*)nodeForVars;
				if(!nodeNameForVars->getUpvarInfo()->isFree())	// ⇒ if(pn3->pn_cookie != FREE_UPVAR_COOKIE)
				{
					ot = nodeNameForVars->getOpcodeType();
					switch(ot)
					{
					case OP_GETARG:	case OP_SETARG:		ot = OP_FORARG;		break;
					case OP_GETGVAR: case OP_SETGVAR:	ot = OP_FORNAME;	break;
					case OP_GETLOCAL: case OP_SETLOCAL:	ot = OP_FORLOCAL;	break;
					default:							ASSERT(false);		break;
					}
				}
				else
				{
					nodeNameForVars->setOpcodeType(OP_FORNAME);
					if(!bindNameAndSlot(nodeNameForVars))
					{
						return false;
					}
					ot = nodeNameForVars->getOpcodeType();
				}

				if(TFW_IS_FLAG(nodeNameForVars->getDefFlag(), EsNameItNode::DF_CONST))	// ⇒ pn3->isConst()
				{
					addCompileError(EsError::ID_BAD_FORIN_LEFTSIDE, 0L);
					return false;
				}

				if(!nodeNameForVars->getUpvarInfo()->isFree())	// ⇒ if(pn3->pn_cookie != FREE_UPVAR_COOKIE)
				{
					s32 atomidx = nodeNameForVars->getUpvarInfo()->getSlotIndex();
					if(!putupIndex(ot, atomidx, nodeNameForVars, 0L))	{	return false;	}	// ⇒ EMIT_UINT16_IMM_OP(op, atomIndex);
				}
				else
				{
					if(!putupAtom(nodeNameForVars, ot, 0L))			{	return false;	}	// ⇒ EmitAtomOp(cx, pn3, op, cg)
				}
			}
			break;

		default:
			// for(xxx.yyy in zzz) のときの OP_FORPROP 出力
			if(ttForVars == TKN_DOT)
			{
				bool isUseful = false;
				if(!decideUsefulSideNodeOperation(&isUseful, ((EsNameItNode*)nodeForVars)->getExpressionNode()))	{	return false;	}	// 隣接ノードが有効な操作かどうかを判定する
				if(!isUseful)
				{
					if(!putupProperty((EsNameItNode*)nodeForVars, OP_FORPROP, false, 0L))
					{
						return false;
					}
				}
			}

			if(!appendExInfoBc(nodeForVars, 0L))	{	return false;	}
			if(!append1Bc(OP_FORELEM, 0L))			{	return false;	}
			ASSERT(_depthStack >= 3);

			if(nodeForVars->getTokenType() == TKN_LP)
			{
				// 複数列挙定義
				ASSERT(nodeForVars->getOpcodeType() == OP_SETCALL);
				if(!buildFromTree(nodeForVars))			{	return false;	}
				if(!appendExInfoBc(nodeForVars, 0L))	{	return false;	}
				if(!append1Bc(OP_ENUMELEM, 0L))			{	return false;	}
			}
			else
			{
				if(!appendExInfoBc(nodeForVars, 0L))	{	return false;	}
				if(!append1Bc(OP_ENUMELEM, 0L))			{	return false;	}
			}

			if(!putupElement(nodeForVars, OP_ENUMELEM, 0L))	{	return false;	}
			break;
		}

		// ループの本体（内容）のバイトコードを出力
		ASSERT(depthLoop == _depthStack);
		if(!buildFromTree(nodeFor->getRightNode()))	{	return false;	}

		// continue のためのオフセットをステートメントに設定する
		EsStatement* stmtWk = &stmt;
		do
		{
			stmtWk->setUpdateBcOffset(bcoffset());
			stmtWk = stmtWk->getDownEnclosingStatement();
		}
		while((stmtWk != 0L) && (stmtWk->getType() == EsStatement::STMTT_LABEL));

		// 前の goto の飛び先（ここ : OP_NEXTITER と条件文）を決定する ⇒ CHECK_AND_SET_JUMP_OFFSET_AT
		s32 bcofsGotoToNextOffset = bcoffset() - bcofsGotoPos;
		writeJumpOffsetLater(bcofsGotoPos, bcofsGotoToNextOffset);

		// OP_NEXTITER と条件文を出力
		if(!appendExInfoBc(nodeForHead, 0L))	{	return false;	}
		if(!append1Bc(OP_NEXTITER, 0L))			{	return false;	}
		s32 bcofsIfnePos;
		if(!putupJump(OP_IFNE, 0, nodeForHead, &bcofsIfnePos))		{	return false;	}
		writeJumpOffsetLater(bcofsIfnePos, bcofsLoopTopPos - bcofsIfnePos);	// ⇒ beq = EmitJump(cx, cg, JSOP_IFNE, top - CG_OFFSET(cg));
		
		// ⇒ Note?
	}
	else	// for in ではない、C スタイルの for(初期化; 条件; 更新) ループの場合
	{
		ASSERT(nodeForHead->getNodeType() == NT_TERNARY);
		EsTernaryItNode* nodeForHeadTr = (EsTernaryItNode*)nodeForHead;

		// 初期化部
		EsOpcodeType ot = OP_POP;
		EsItNode* nodeForInit = nodeForHeadTr->getChildNode1();
		if(nodeForInit == 0L)
		{
			ot = OP_NOP;
		}
		else
		{
			TFW_SET_FLAG(*buildFlag(), EsBytecodeBuilder::BF_IN_FOR_INIT, true);	// ⇒ cg->flags |= TCF_IN_FOR_INIT;

			if(ot == OP_POP)
			{
				if(!buildFromTree(nodeForInit))	{	return false;	}	// 初期化文の出力
				if((nodeForInit->getTokenType() == TKN_VAR) || (nodeForInit->getTokenType() == TKN_LET)) 	// ⇒ TOKEN_TYPE_IS_DECL(tt)
				{
					ASSERT(nodeForInit->getNodeType() == NT_LIST);
					if(TFW_IS_FLAG(*((EsListItNode*)nodeForInit)->listFlag(), EsListItNode::LF_GROUPINIT))
					{
						ot = OP_NOP;
					}
				}
			}

			TFW_SET_FLAG(*buildFlag(), EsBytecodeBuilder::BF_IN_FOR_INIT, false);	// ⇒ cg->flags &= ~TCF_IN_FOR_INIT;
		}
		if(!appendExInfoBc(nodeForInit, 0L))	{	return false;	}
		if(!append1Bc(ot, 0L))					{	return false;	}

		// 条件部への goto を生成（for 初期化後、まず条件評価を行う）
		s32 bcofsTmp = bcoffset();	// ⇒ tmp
		s32 bcofsGotoPos = 0;		// ⇒ jmp
		EsItNode* nodeForCond = nodeForHeadTr->getChildNode2();
		if(nodeForCond != 0L)
		{
			if(!putupJump(OP_GOTO, 0, nodeForCond, &bcofsGotoPos))	{	return false;	}	// ⇒ jmp = EmitJump(cx, cg, JSOP_GOTO, 0);
		}

		// ループの戻り先を生成
		s32 bcofsLoopTopPos = bcoffset();	// ⇒ top
		stmt.setTopBcOffset(bcofsLoopTopPos);
		if(!appendExInfoBc(nodeLoopBody, 0L))					{	return false;	}
		if(!append1Bc(OP_LOOP, 0L))								{	return false;	}

		// ループ本体を出力
		if(!buildFromTree(nodeLoopBody))						{	return false;	}

		// continue のためのオフセットをステートメントに設定する
		EsStatement* stmtWk = &stmt;
		do
		{
			stmtWk->setUpdateBcOffset(bcoffset());
			stmtWk = stmtWk->getDownEnclosingStatement();
		}
		while((stmtWk != 0L) && (stmtWk->getType() == EsStatement::STMTT_LABEL));

		// 更新部
		EsItNode* nodeForUpdate = nodeForHeadTr->getChildNode3();
		if(nodeForUpdate != 0L)
		{
			ot = OP_POP;
			if(ot == OP_POP)
			{
				if(!buildFromTree(nodeForUpdate))	{	return false;	}	// 更新文の出力
			}
			if(!appendExInfoBc(nodeForUpdate, 0L))	{	return false;	}
			if(!append1Bc(ot, 0L))					{	return false;	}

			// ⇒ Note?
		}

		// ⇒ Note?

		// 条件部の出力
		if(nodeForCond != 0L)
		{
			// 前の条件部 goto の飛び先（ここ : for 条件部）を決定する ⇒ CHECK_AND_SET_JUMP_OFFSET_AT
			s32 bcofsGotoToCondOffset = bcoffset() - bcofsGotoPos;
			writeJumpOffsetLater(bcofsGotoPos, bcofsGotoToCondOffset);

			// 条件文の出力
			if(!buildFromTree(nodeForCond))							{	return false;	}

			// 条件を満たさなかった場合？のジャンプを出力
			s32 bcofsIfnePos;
			if(!putupJump(OP_IFNE, 0, nodeForCond, &bcofsIfnePos))	{	return false;	}
			writeJumpOffsetLater(bcofsIfnePos, bcofsLoopTopPos - bcofsIfnePos);		// ⇒ beq = EmitJump(cx, cg, JSOP_IFNE, top - CG_OFFSET(cg));
		}
		else
		{
			// 条件なしのループのジャンプを出力
			s32 bcofsNocondPos;
			if(!putupJump(OP_GOTO, 0, nodeForCond, &bcofsNocondPos))	{	return false;	}
			writeJumpOffsetLater(bcofsNocondPos, bcofsLoopTopPos - bcofsNocondPos);	// ⇒ jmp = EmitJump(cx, cg, JSOP_GOTO, top - CG_OFFSET(cg));
		}
	}

	// ステートメントを戻す
	popStatementAndWriteJumpOffset();

	// for in の場合、OP_ENDITER を出力
	if(nodeForHead->getTokenType() == TKN_IN)
	{
		if(!appendExInfoBc(nodeForHead, 0L))	{	return false;	}
		if(!append1Bc(OP_ENDITER, 0L))			{	return false;	}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - if
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_if(EsItNode* nodeT)
{
	s32 bcofsTop = bcoffset();
	s32 bcofsIfeqPos = -1;
	s32 bcofsJumpPos = -1;

	ASSERT(nodeT->getNodeType() == NT_TERNARY);
	EsTernaryItNode* nodeIf = (EsTernaryItNode*)nodeT;

	// ステートメント情報を作成する
	EsStatement stmt(EsStatement::STMTT_IF);

	while(true)
	{
		// 条件文を出力
		EsItNode* nodeCond = nodeIf->getChildNode1();
		if(!buildFromTree(nodeCond))					{	return false;	}
		if(stmt.getType() == EsStatement::STMTT_IF)	// if 最初は必ずここ
		{
			// 新しいステートメントを積む
			pushStatement(&stmt, bcoffset());
		}
		else	// else if ２回目以降
		{
			ASSERT(stmt.getType() == EsStatement::STMTT_ELSE);
			stmt.setType(EsStatement::STMTT_IF);
			stmt.setUpdateBcOffset(bcofsTop);			// ⇒ stmtInfo.update = top;
		}
		// OP_IFEQ 出力
		if(!putupJump(OP_IFEQ, 0, nodeCond, &bcofsIfeqPos))			{	return false;	}

		// if 条件を満たした場合の文の出力
		EsItNode* nodeChild = nodeIf->getChildNode2();
		if(!buildFromTree(nodeChild))									{	return false;	}

		// if 条件が満たなかったときの else 文を出力
		EsItNode* nodeElse = nodeIf->getChildNode3();
		if(nodeElse != 0L)	// else がある場合
		{
			stmt.setType(EsStatement::STMTT_ELSE);

			// OP_BACKPATCH 出力
			s32 bsofsBreak = stmt.getLastBreakBcOffset();
			if(!putupGoto(&stmt, &bsofsBreak, nodeElse, 0L))				{	return false;	}
			stmt.setLastBreakBcOffset(bsofsBreak);
			bcofsJumpPos = bcoffset() - 3;	// -3 は putupGoto 最後の putupBackPatch 文分

			// 前の IFEQ の飛び先（ここ : else 時の処理内容）を決定する ⇒ CHECK_AND_SET_JUMP_OFFSET_AT
			s32 bcofsIfeqToElsecontentsOffset = bcoffset() - bcofsIfeqPos;
			writeJumpOffsetLater(bcofsIfeqPos, bcofsIfeqToElsecontentsOffset);

			// else if だった場合
			if(nodeElse->getTokenType() == TKN_IF)
			{
				nodeIf = (EsTernaryItNode*)nodeElse;
				continue;	// if 条件出力から再度実行
			}

			// else 時の処理内容出力
			if(!buildFromTree(nodeElse))							{	return false;	}
		}
		else
		{
			// 前の IFEQ の飛び先（ここ : if ブロックの終了）を決定する ⇒ CHECK_AND_SET_JUMP_OFFSET_AT
			s32 bcofsIfeqToElsecontentsOffset = bcoffset() - bcofsIfeqPos;
			writeJumpOffsetLater(bcofsIfeqPos, bcofsIfeqToElsecontentsOffset);
		}
		break;
	}

	// ステートメントを戻す
	popStatementAndWriteJumpOffset();

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - TKN_HOOK (xxx ? yyy : zzz)
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_hook(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_TERNARY);
	EsTernaryItNode* nodeHook = (EsTernaryItNode*)nodeT;

	// 条件文の出力
	if(!buildFromTree(nodeHook->getChildNode1()))	{	return false;	}

	// OP_IFEQ 出力
	s32 bcofsIfeqPos = 0;
	if(!putupJump(OP_IFEQ, 0, nodeHook->getChildNode1(), &bcofsIfeqPos))	{	return false;	}

	// 真のときの文を出力
	if(!buildFromTree(nodeHook->getChildNode2()))	{	return false;	}

	// OP_GOTO 出力
	s32 bcofsGotoPos = 0;
	if(!putupJump(OP_GOTO, 0, nodeHook->getChildNode3(), &bcofsGotoPos))	{	return false;	}
	if(!writeJumpOffsetLater(bcofsIfeqPos, bcoffset() - bcofsIfeqPos))		{	return false;	}	// ⇒ CHECK_AND_SET_JUMP_OFFSET_AT(cx, cg, beq);

	// スタックの深さを調整する
	_depthStack--;

	// 偽のときの文を出力
	if(!buildFromTree(nodeHook->getChildNode3()))	{	return false;	}
	if(!writeJumpOffsetLater(bcofsGotoPos, bcoffset() - bcofsGotoPos))		{	return false;	}	// ⇒ CHECK_AND_SET_JUMP_OFFSET_AT(cx, cg, jmp);

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - switch ⇒ EmitSwitch
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_switch(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_BINARY);
	EsBinaryItNode* nodeSwitch = (EsBinaryItNode*)nodeT;
	EsOpcodeType otSwitch = OP_TABLESWITCH;

	bool isHasDefault = false;
	bool isConstPropageted = false;
	EsStatement stmt;
	s32 bcofsTop = 0;
	s32 bcofsDefaultOp = 0;
	s32 bcofsDefaultStmt = 0;
	s32 bcofsSwitchOpBc = 0;
	s32 bcofsWk = 0;
	s32 ivalCaseLow = S32_MAX;
	s32 ivalCaseHi = S32_MIN;
	s32 lenTable = 0;
	s32 numCase = 0;
	s32 sizeSwitchOpBc = 0;
	EsItNode* nodeSwitchRight = nodeSwitch->getRightNode();
	EsListItNode* nodeCaseList = 0L;
	InsArray<u8> bcarrSwitchOpBc(true, true);
	InsArray<EsBinaryItNode*> nodearrTable(true, true);

	// スイッチ本体に let 変数が含まれている場合、スイッチ判別式の前にブロックをスタックに積む
	if(nodeSwitchRight->getTokenType() == TKN_LEXICALSCOPE)
	{
		// switch 本体ブロークスコープを積む
		ASSERT(nodeSwitchRight->getNodeType() == NT_NAME);
		EsObjectBox* objbox = ((EsNameItNode*)nodeSwitchRight)->getObjectBox();
		s32 count = objbox->getBlockObject()->getScope()->getScopePropertyNum();
		stmt.setType(EsStatement::STMTT_SWITCH);
		pushBlockScope(&stmt, objbox->getBlockObject(), -1);

		// OP_ENTERBLOCK 出力
		if(!putupEnterBlock(nodeSwitchRight, 0L))	{	return false;	}

		// switch ステートメントを設定する
		_stmtTopEncl = stmt.getDownEnclosingStatement();	// ⇒ cg->topStmt = stmtInfo->down;
		_stmtTopScope = stmt.getDownScopeStatement();		// ⇒ cg->topScopeStmt = stmtInfo->downScope;
	}

	// switch 判別式の出力
	if(!buildFromTree(nodeSwitch->getLeftNode()))	{	return false;	}

	// ここから最後の case の末尾まで実行される
	bcofsTop = bcoffset();	// ⇒ top = CG_OFFSET(cg);
	if(nodeSwitchRight->getTokenType() == TKN_LC)
	{
		stmt.setType(EsStatement::STMTT_SWITCH);
		pushStatement(&stmt, bcofsTop);
	}
	else
	{
		// ステートメントの再設定
		_stmtTopEncl = stmt.getDownEnclosingStatement();	// ⇒ cg->topStmt = stmtInfo->down;

		// ステートメントの先頭バイトコード位置を設定する
		stmt.setUpdateBcOffset(bcofsTop);

		ASSERT(nodeSwitchRight->getNodeType() == NT_NAME);
		nodeSwitchRight = ((EsNameItNode*)nodeSwitchRight)->getExpressionNode();
	}

	ASSERT(nodeSwitchRight->getNodeType() == NT_LIST);
	nodeCaseList = (EsListItNode*)nodeSwitchRight;
	numCase = (s32)nodeCaseList->getNodeCount();

	if(	(numCase == 0) ||
		((numCase == 1) && (nodeCaseList->getHeadNode()->getTokenType() == TKN_DEFAULT)))
	{
		// 選択肢が無い場合
		numCase = 0;
		ivalCaseLow = 0;
		ivalCaseHi = -1;
	}
	else
	{
		IdManager idmngCaseIval;	// case 値重複チェック用 ID マネージャ

		for(EsItNode* nodeCase = nodeCaseList->getHeadNode(); nodeCase != 0L; nodeCase = nodeCase->getNextNode())
		{
			// default の場合
			if(nodeCase->getTokenType() == TKN_DEFAULT)
			{
				isHasDefault = true;
				numCase--;
				continue;
			}

			ASSERT(nodeCase->getTokenType() == TKN_CASE);
			if(otSwitch == OP_CONDSWITCH)
			{
				continue;
			}

			ASSERT(nodeCase->getNodeType() == NT_BINARY);
			EsBinaryItNode* nodeBinCase = (EsBinaryItNode*)nodeCase;
			EsItNode* nodeCaseCond = ((EsBinaryItNode*)nodeCase)->getLeftNode();
			while(nodeCaseCond->getTokenType() == TKN_RP)
			{
				nodeCaseCond = nodeCaseCond->getChildNode();
			}
			switch(nodeCaseCond->getTokenType())
			{
			case TKN_NUMBER:
				ASSERT(nodeCaseCond->getNodeType() == NT_DOUBLE);
				{
					f64 fval = ((EsDoubleItNode*)nodeCaseCond)->getDouble();
					if(TFW_F64_IS_INT(fval))
					{
						EsValue valF((s32)fval);
						nodeBinCase->setSwitchCaseValue(&valF);
					}
					else
					{
						EsValue valAtom(_ctxRef->makeDoubleAtom(&fval));
						nodeBinCase->setSwitchCaseValue(&valAtom);	// ⇒ atom = js_AtomizeDouble(cx, d); pn3->pn_val = ATOM_KEY(atom);
					}
				}
				break;

			case TKN_STRING:
				{
					ASSERT(nodeCaseCond->getNodeType() == NT_NAME);
					EsValue valAtom(((EsNameItNode*)nodeCaseCond)->getAtom());
					nodeBinCase->setSwitchCaseValue(&valAtom);
				}
				break;

			case TKN_NAME:
				if(((EsNameItNode*)nodeCaseCond)->getExpressionNode() != 0L)
				{
					EsValue val;
					if(!findConstant(&val, ((EsNameItNode*)nodeCaseCond)->getAtom()))	{	goto FAILED;	}	// ⇒ LookupCompileTimeConstant
					if(!val.isPrimitive())
					{
						otSwitch = OP_CONDSWITCH;
						continue;
					}
					nodeBinCase->setSwitchCaseValue(&val);	// ⇒ pn3->pn_val = v;
					isConstPropageted = true;
					break;
				}
				// ↓

			case TKN_PRIMARY:
				if(nodeCaseCond->getOpcodeType() == OP_TRUE)
				{
					EsValue valTrue(true);
					nodeBinCase->setSwitchCaseValue(&valTrue);
					break;		
				}
				if(nodeCaseCond->getOpcodeType() == OP_FALSE)
				{
					EsValue valFalse(false);
					nodeBinCase->setSwitchCaseValue(&valFalse);
					break;		
				}
				// ↓

			default:
				otSwitch = OP_CONDSWITCH;
				continue;
			}

			ASSERT(nodeBinCase->getSwitchCaseValue()->isPrimitive());

			if(otSwitch != OP_TABLESWITCH)
			{
				continue;
			}
			if(!nodeBinCase->getSwitchCaseValue()->isInt())
			{
				otSwitch = OP_LOOKUPSWITCH;
				continue;
			}

			s32 ival = nodeBinCase->getSwitchCaseValue()->getInt();
			if((ival + 0x8000) >= 0x10000)	// ⇒ if ((jsuint)(i + (jsint)JS_BIT(15)) >= (jsuint)JS_BIT(16))
			{
				otSwitch = OP_LOOKUPSWITCH;
				continue;
			}
			if(ivalCaseLow > ival)
			{
				ivalCaseLow = ival;
			}
			if(ivalCaseHi < ival)
			{
				ivalCaseHi = ival;
			}

			// 重複チェック
			if(ival < 0)
			{
				ival += 0x10000;	// 0 ～ 0xffff に収める
			}
			// ID マネージャの作成
			if(idmngCaseIval.getIdNum() == 0)	// 未初期化
			{
				if(!idmngCaseIval.create(1024))			{	goto FAILED;	}	// 初期 ID 数
			}
			else if(ival >= (s32)idmngCaseIval.getIdNum())
			{
				if(!idmngCaseIval.recreate(0x10000))	{	goto FAILED;	}	// 最初期化
				ASSERT(ival < (s32)idmngCaseIval.getIdNum());
			}
			u16 id = (u16)ival;
			if(idmngCaseIval.isUsedId(id))	// 既存である ⇒ if (JS_TEST_BIT(intmap, i))
			{
				otSwitch = OP_LOOKUPSWITCH;
				continue;
			}
			idmngCaseIval.setUsedId(id, true);
		}

		// テーブル長の算出する．もしテーブル長が半分以上であれば LOOKUP を選択する
		if(otSwitch == OP_TABLESWITCH)
		{
			lenTable = ivalCaseHi - ivalCaseLow + 1;
			if((lenTable >= 0x10000) || (lenTable > (2 * numCase)))
			{
				otSwitch = OP_LOOKUPSWITCH;
			}
		}
		else if(otSwitch == OP_LOOKUPSWITCH)
		{
			if((numCase + _alistLiteral->getCount()) > 0x10000)
			{
				otSwitch = OP_CONDSWITCH;
			}
		}

		idmngCaseIval.destroy();
	}

	// OP_*SWITCH バイトコードの長さを求める
	// OP_*SWITCH の後はジャンプオフセットが連続する
	if(otSwitch == OP_TABLESWITCH)
	{
		s32 sizeHeader = SIZE_JUMP_OFFSET_BC * 3;							// テーブル部の前に書き込むヘッダ部：オフセット値数×３（= 長さ, ケース最小値, ケース最大値）
		s32 sizeTable = SIZE_JUMP_OFFSET_BC * lenTable;						// テーブル部：オフセット×テーブル長
		sizeSwitchOpBc = sizeHeader + sizeTable;
	}
	else if(otSwitch == OP_LOOKUPSWITCH)
	{
		s32 sizeHeader = SIZE_JUMP_OFFSET_BC + SIZE_INDEX_BC;				// ケース配列部の前に書き込むヘッダ部：長さ + 要素数の後
		s32 sizeCases = (SIZE_INDEX_BC + SIZE_JUMP_OFFSET_BC) * numCase;	// ケース配列部：（atom インデックス + ジャンプオフセット）×要素数
		sizeSwitchOpBc = sizeHeader + sizeCases;
	}
	else
	{
		ASSERT(otSwitch == OP_CONDSWITCH);
		sizeSwitchOpBc = 0;
	}
	// OP_*SWITCH バイトコード出力（内容は後に上書き）
	if(sizeSwitchOpBc > 0)
	{
		if(!appendExInfoBc(nodeCaseList, 0L))		{	goto FAILED;	}
		bcarrSwitchOpBc.alloc(sizeSwitchOpBc);
		if(!appendManyBc(otSwitch, bcarrSwitchOpBc.getPointer(0), sizeSwitchOpBc, &bcofsSwitchOpBc))	{	goto FAILED;	}
	}
	else
	{
		if(!appendExInfoBc(nodeCaseList, 0L))		{	goto FAILED;	}
		if(!append1Bc(otSwitch, &bcofsSwitchOpBc))	{	goto FAILED;	}
	}
	
	// 各ケースの値もしくは式のバイトコードを出力
	if(otSwitch == OP_CONDSWITCH)
	{
		// 各ケース式を評価し、ケース文にジャンプするバイトコードを出力
		for(EsItNode* nodeCase = nodeCaseList->getHeadNode(); nodeCase != 0L; nodeCase = nodeCase->getNextNode())
		{
			ASSERT(nodeCase->getNodeType() == NT_BINARY);
			EsItNode* nodeLeft = ((EsBinaryItNode*)nodeCase)->getLeftNode();
			if(nodeLeft != 0L)
			{
				if(!buildFromTree(nodeLeft))	{	goto FAILED;	}
			}
			else
			{
				ASSERT(nodeCase->getTokenType() == TKN_DEFAULT);
				continue;
			}

			s32 bcofsCase = 0;
			if(!putupJump(OP_CASE, 0, nodeCase, &bcofsCase))	{	goto FAILED;	}
			nodeCase->setBytecodeOffset(bcofsCase);
		}

		if(!putupJump(OP_DEFAULT, 0, nodeCaseList, &bcofsDefaultOp))	{	goto FAILED;	}
	}
	else
	{
		bcofsWk = bcofsSwitchOpBc + SIZE_JUMP_OFFSET_BC;
		if(otSwitch == OP_TABLESWITCH)
		{
			// ケース最小値、ケース最大値を書き込む
			writeJumpOffsetLater16(bcofsWk, (u16)ivalCaseLow);
			bcofsWk += SIZE_JUMP_OFFSET_BC;
			writeJumpOffsetLater16(bcofsWk, (u16)ivalCaseHi);
			bcofsWk += SIZE_JUMP_OFFSET_BC;

			if(lenTable > 0)
			{
				// ノードテーブル作成
				if(!nodearrTable.alloc(lenTable))	{	goto FAILED;	}
				for(EsItNode* nodeCase = nodeCaseList->getHeadNode(); nodeCase != 0L; nodeCase = nodeCase->getNextNode())
				{
					if(nodeCase->getTokenType() == TKN_DEFAULT)
					{
						continue;
					}

					ASSERT(nodeCase->getNodeType() == NT_BINARY);
					EsBinaryItNode* nodeBinCase = (EsBinaryItNode*)nodeCase;
					s32 ival = nodeBinCase->getSwitchCaseValue()->getInt();
					ival -= ivalCaseLow;
					ASSERT((0 <= ival) && (ival < (s32)lenTable));
					nodearrTable.setAt(ival, nodeBinCase);
				}
			}

			// 語彙アトムの追加
			if(isConstPropageted)
			{
				for(u32 i = 0; i < nodearrTable.getCount(); i++)
				{
					EsBinaryItNode* nodeBinCase = nodearrTable.getAt(i);
					if((nodeBinCase != 0L) && (nodeBinCase->getLeftNode() != 0L))
					{
						EsItNode* nodeLeft = nodeBinCase->getLeftNode();
						if(nodeLeft->getTokenType() == TKN_NAME)
						{
							EsNameItNode* nodeNameLeft = (EsNameItNode*)nodeLeft;
							ASSERT(nodeNameLeft->getExpressionNode() != 0L);
							EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(nodeNameLeft->getAtom());
							if(ale == 0L)
							{
								goto FAILED;
							}
						}
					}
				}
			}
		}
		else if(otSwitch == OP_LOOKUPSWITCH)
		{
			// ケース数を書き込む
			writeIndexLater16(bcofsWk, (u16)numCase);
			bcofsWk += SIZE_JUMP_OFFSET_BC;

			// 語彙アトムの追加
			if(isConstPropageted)
			{
				for(EsItNode* nodeCase = nodeCaseList->getHeadNode(); nodeCase != 0L; nodeCase = nodeCase->getNextNode())
				{
					ASSERT(nodeCase->getNodeType() == NT_BINARY);
					EsItNode* nodeLeft = ((EsBinaryItNode*)nodeCase)->getLeftNode();
					if((nodeLeft != 0L) && (nodeLeft->getTokenType() == TKN_NAME))
					{
						EsNameItNode* nodeNameLeft = (EsNameItNode*)nodeLeft;
						ASSERT(nodeNameLeft->getExpressionNode() != 0L);
						EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(nodeNameLeft->getAtom());
						if(ale == 0L)
						{
							goto FAILED;
						}
					}
				}
			}
		}
		else
		{
			ASSERT(false);
		}
	}

	// 各ケース下の内容文のバイトコードを出力
	for(EsItNode* nodeCase = nodeCaseList->getHeadNode(); nodeCase != 0L; nodeCase = nodeCase->getNextNode())	// ⇒ for (pn3 = pn2->pn_head; pn3; pn3 = pn3->pn_next) {
	{
		ASSERT(nodeCase->getNodeType() == NT_BINARY);
		if((otSwitch == OP_CONDSWITCH) && (nodeCase->getTokenType() != TKN_DEFAULT))
		{
			s32 bcofsCaseOffset = bcoffset() - nodeCase->getBytecodeOffset();
			if(!writeJumpOffsetLater(nodeCase->getBytecodeOffset(), bcofsCaseOffset))	{	goto FAILED;	}
		}
		EsItNode* nodeRight = ((EsBinaryItNode*)nodeCase)->getRightNode();
		if(!buildFromTree(nodeRight))	{	goto FAILED;	}
		nodeCase->setBytecodeOffset(nodeRight->getBytecodeOffset());
		if(nodeCase->getTokenType() == TKN_DEFAULT)
		{
			bcofsDefaultStmt = nodeCase->getBytecodeOffset() - bcofsSwitchOpBc;
		}
	}
	if(!isHasDefault)
	{
		// default がない場合は、switch の終端オフセット値のオフセットを得る
		bcofsDefaultStmt = bcoffset() - bcofsSwitchOpBc;
	}

	// default オフセット値のバイトコード書き込み（default がない場合は、switch の終端オフセット値）
	if(otSwitch == OP_CONDSWITCH)
	{
		if(!writeJumpOffsetLater16(bcofsDefaultOp, bcofsDefaultStmt - (bcofsDefaultOp - bcofsSwitchOpBc)))	{	goto FAILED;	}	// ⇒ js_SetJumpOffset だが、状況的に 16bit 限定のメソッドで実装する
	}
	else
	{
		if(!writeJumpOffsetLater16(bcofsSwitchOpBc, bcofsDefaultStmt))	{	goto FAILED;	}
	}

	// テーブル部、ケース配列部のバイトコード書き込み
	if(otSwitch == OP_TABLESWITCH)
	{
		bcofsWk = bcofsSwitchOpBc + (SIZE_JUMP_OFFSET_BC * 3);	// ヘッダ部をスキップ
		for(u32 i = 0; i < nodearrTable.getCount(); i++)
		{
			EsBinaryItNode* nodeBinCase = nodearrTable.getAt(i);
			s32 bcofsEntity = (nodeBinCase != 0L) ? nodeBinCase->getBytecodeOffset() - bcofsSwitchOpBc : 0;
			if(!writeJumpOffsetLater16(bcofsWk, bcofsEntity))	{	goto FAILED;	}
			bcofsWk += SIZE_JUMP_OFFSET_BC;
		}
	}
	else if(otSwitch == OP_LOOKUPSWITCH)
	{
		bcofsWk = bcofsSwitchOpBc + SIZE_JUMP_OFFSET_BC + SIZE_INDEX_BC;			// ヘッダ部をスキップ
		for(EsItNode* nodeCase = nodeCaseList->getHeadNode(); nodeCase != 0L; nodeCase = nodeCase->getNextNode())
		{
			if(nodeCase->getTokenType() == TKN_DEFAULT)
			{
				continue;
			}
			ASSERT(nodeCase->getNodeType() == NT_BINARY);
			const EsAtom* atom = _ctxRef->makePrimitiveValueAtom(((EsBinaryItNode*)nodeCase)->getSwitchCaseValue());	// ⇒ js_AtomizePrimitiveValue(cx, pn3->pn_val, &atom)
			EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(atom);
			if(ale == 0L)	{	goto FAILED;	}
			if(!writeIndexLater16(bcofsWk, (u16)ale->getIndex()))	{	goto FAILED;	}	// ⇒ SET_INDEX(pc, ALE_INDEX(ale));
			bcofsWk += SIZE_INDEX_BC;

			s32 bcofsCase = nodeCase->getBytecodeOffset() - bcofsSwitchOpBc;
			if(!writeJumpOffsetLater16(bcofsWk, bcofsCase))	{	goto FAILED;	}	// ⇒ js_SetJumpOffset だが、状況的に 16bit 限定のメソッドで実装する
			bcofsWk += SIZE_JUMP_OFFSET_BC;
		}
	}

	popStatementAndWriteJumpOffset();
	return true;

FAILED:
	popStatementAndWriteJumpOffset();
	return false;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - break
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_break(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_NAME);
	EsNameItNode* nodeBreak = (EsNameItNode*)nodeT;

	EsStatement* stmt = _stmtTopEncl;
	const EsAtom* atom = nodeBreak->getAtom();
	if(atom != 0L)
	{
		EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(atom);	// ⇒ ale = cg->atomList.add(cg->compiler, atom);
		if(ale == 0)	{	return false;	}

		while((stmt != 0L) && ((stmt->getType() != EsStatement::STMTT_LABEL) || (stmt->getLabelAtom() != atom)))
		{
			stmt = stmt->getDownEnclosingStatement();
			ASSERT(stmt != 0L);
		}
	}
	else
	{
		while((stmt != 0L) && !stmt->isLoop() && (stmt->getType() != EsStatement::STMTT_SWITCH))
		{
			stmt = stmt->getDownEnclosingStatement();
			ASSERT(stmt != 0L);
		}
	}

	s32 bsofsBreak = stmt->getLastBreakBcOffset();
	if(!putupGoto(stmt, &bsofsBreak, nodeT, 0L))	{	return false;	}
	stmt->setLastBreakBcOffset(bsofsBreak);

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - continue
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_continue(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_NAME);
	EsNameItNode* nodeContinue = (EsNameItNode*)nodeT;

	EsStatement* stmt = _stmtTopEncl;
	const EsAtom* atom = nodeContinue->getAtom();
	if(atom != 0L)
	{
		EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(atom);	// ⇒ ale = cg->atomList.add(cg->compiler, atom);
		if(ale == 0)	{	return false;	}

		EsStatement* stmtLoop = stmt;
		while((stmt != 0L) && ((stmt->getType() != EsStatement::STMTT_LABEL) || (stmt->getLabelAtom() != atom)))
		{
			if(stmt->isLoop())
			{
				stmtLoop = stmt;
			}
			stmt = stmt->getDownEnclosingStatement();
			ASSERT(stmt != 0L);
		}
		stmt = stmtLoop;
	}
	else
	{
		while((stmt != 0L) && !stmt->isLoop())
		{
			stmt = stmt->getDownEnclosingStatement();
			ASSERT(stmt != 0L);
		}
	}

	s32 bsofsContinue = stmt->getLastContinueBcOffset();
	if(!putupGoto(stmt, &bsofsContinue, nodeT, 0L))	{	return false;	}
	stmt->setLastContinueBcOffset(bsofsContinue);

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - コロン（:）
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_colon(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_NAME);
	EsNameItNode* nodeColon = (EsNameItNode*)nodeT;

	const EsAtom* atom = nodeColon->getAtom();
	ASSERT(atom != 0L);

	EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(atom);	// ⇒ ale = cg->atomList.add(cg->compiler, atom);
	if(ale == 0)	{	return false;	}

	EsItNode* nodeExpr = nodeColon->getExpressionNode();
	ASSERT(nodeExpr != 0L);

	// 新しいステートメントを積む
	EsStatement stmt(EsStatement::STMTT_LABEL);
	pushStatement(&stmt, bcoffset());
	stmt.setLabelAtom(atom);

	// ラベルされた文を出力
	if(!buildFromTree(nodeExpr))	{	return false;	}

	// ステートメントを戻す
	popStatementAndWriteJumpOffset();

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - with
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_with(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_BINARY);
	EsBinaryItNode* nodeWith = (EsBinaryItNode*)nodeT;

	// width(xxx) の xxx 定義部のバイトコード出力
	if(!buildFromTree(nodeWith->getLeftNode()))		{	return false;	}

	// 新しいステートメントを積む
	EsStatement stmtWith(EsStatement::STMTT_WITH);
	pushStatement(&stmtWith, bcoffset());

	// with に入る
	if(!appendExInfoBc(nodeT, 0L))		{	return false;	}
	if(!append1Bc(OP_ENTERWITH, 0L))	{	return false;	}

	// with 以下のバイトコードの出力
	if(!buildFromTree(nodeWith->getRightNode()))	{	return false;	}

	// with から出る
	if(!appendExInfoBc(nodeT, 0L))		{	return false;	}
	if(!append1Bc(OP_LEAVEWITH, 0L))	{	return false;	}

	// ステートメントを戻す
	popStatementAndWriteJumpOffset();

	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - TKN_ARGSBODY
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_argsbody(EsItNode* nodeT)
{
	ASSERT(nodeT->getNodeType() == NT_LIST);
	return buildFromTree(((EsListItNode*)nodeT)->getLastNode());
}

/*---------------------------------------------------------------------*//**
	バイトコード生成サブメソッド - TKN_PRIMARY
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::buildSub_primary(EsItNode* nodeT)
{
	if(!appendExInfoBc(nodeT, 0L))					{	return false;	}
	if(!append1Bc(nodeT->getOpcodeType(), 0L))		{	return false;	}	// ⇒ if (js_Emit1(cx, cg, PN_OP(pn)) < 0)
	return true;
}

/*---------------------------------------------------------------------*//**
	名前ノードをスロットに結びつける ⇒ BindNameToSlot
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::bindNameAndSlot(EsNameItNode* node)
{
	ASSERT(node->getTokenType() == TKN_NAME);

	// 二重配置防止（かな？）
	if(TFW_IS_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG))	{	return true;	}

	// 定義ノードを得る
	EsNameItNode* nodeDefNam = 0L;
	if(node->isHasLex())	// ⇒ if (pn->pn_used) {
	{
		ASSERT(node->getUpvarInfo()->isFree());
		nodeDefNam = node->getLexDefNode();
		ASSERT(TFW_IS_FLAG(*nodeDefNam->defFlag(), EsNameItNode::DF_EXTYPE_DEFINE));
		TFW_SET_FLAG(*node->defFlag(), *nodeDefNam->defFlag() & EsNameItNode::DF_CONST, true);
	}
	else
	{
		if(!TFW_IS_FLAG(node->getDefFlag(), EsNameItNode::DF_EXTYPE_DEFINE))
		{
			return true;
		}
		nodeDefNam = node;
	}
	ASSERT(nodeDefNam != 0L);

	EsOpcodeType ot = node->getOpcodeType();
	if(ot == OP_NOP)
	{
		return true;
	}

	const EsAtom* atom = node->getAtom();
	EsUpvarInfo upvariDef = *nodeDefNam->getUpvarInfo();		// ⇒ level
	EsNameItNode::DefKind defkind = nodeDefNam->getDefKind();

	switch(ot)
	{
	case OP_NAME:
	case OP_SETCONST:
		break;
	case OP_DELNAME:
		if(defkind != EsNameItNode::DK_NULL)
		{
			if((_sfrmCaller != 0L) && (_funcboxCur == 0L))
			{
				// ※※※ 実装しない ※※※（OP_DELNAME はサポートしない）
				ASSERT(false);
			}
			else
			{
				node->setOpcodeType(OP_FALSE);
			}
			TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG, true);	// ⇒ pn->pn_dflags |= PND_BOUND;
			return true;
		}
		break;
	default:
		if(TFW_IS_FLAG(node->getDefFlag(), EsNameItNode::DF_CONST))
		{
			ot = OP_NAME;
			node->setOpcodeType(OP_NAME);
		}
		break;
	}

	if(upvariDef.isFree())	// ⇒ if (cookie == FREE_UPVAR_COOKIE)
	{
		if(_sfrmCaller != 0L)
		{
			// ※※※ 実装しない ※※※（BF_OPT_NAME_REF_ON_SCOPE はサポートしない）
			ASSERT(false);
		}

		return true;
	}

	// 大域（グローバル）変数化
	if(TFW_IS_FLAG(*nodeDefNam->defFlag(), EsNameItNode::DF_GVAR))
	{
		if(TFW_IS_FLAG(_bflags, BF_IN_FUNCTION))
		{
			return true;
		}

		ASSERT((defkind == EsNameItNode::DK_VAR) || (defkind == EsNameItNode::DK_CONST));

		switch(ot)
		{
		case OP_NAME:		ot = OP_GETGVAR;	break;
		case OP_SETNAME:	ot = OP_SETGVAR;	break;
		case OP_INCNAME:	ot = OP_INCGVAR;	break;
		case OP_NAMEINC:	ot = OP_GVARINC;	break;
		case OP_DECNAME:	ot = OP_DECGVAR;	break;
		case OP_NAMEDEC:	ot = OP_GVARDEC;	break;
		}

		node->setOpcodeType(ot);
		node->setUpvarInfo(&upvariDef);
		TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG, true);
		return true;
	}

	ASSERT(_depthNest >= upvariDef.getSkipDepth());

	// 定義ノード（nodeDefNam）は上域変数化できない、そのため名前ノード（node）を変わりに上域変数化する
	if(nodeDefNam->getOpcodeType() == OP_GETUPVAR)
	{
		if(ot != OP_NAME)
		{
			return true;
		}

		if(_depthNest == upvariDef.getSkipDepth())
		{
			node->setOpcodeType(OP_GETUPVAR);
			node->setUpvarInfo(&upvariDef);
			TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG, true);
			return true;
		}

		return changeUpvarForEvalNameNode(node);
	}

	u32 skipdepDiff = _depthNest - upvariDef.getSkipDepth();	// ⇒ skip
	if(skipdepDiff != 0)
	{
		ASSERT(TFW_IS_FLAG(_bflags, BF_IN_FUNCTION));
		ASSERT(_alistUnrslv->find(atom) != 0L);
		ASSERT(_funcCur->getSkipDepth() <= skipdepDiff);

		if(ot != OP_NAME)
		{
			return true;
		}
		if(upvariDef.getSkipDepth() >= EsContext::SIZE_SF_DISPLAY)
		{
			return true;
		}
		if(TFW_IS_FLAG(_bflags, BF_FUNC_NEED_CALLOBJ))
		{
			return true;
		}

		if(TFW_IS_FLAG(_funcCur->getFlags(), EsFunction::F_CLOSURE_FLAT))
		{
			ot = OP_GETDSLOT;
		}
		else
		{
			if(TFW_IS_FLAG(_funcboxCur->getFuncNode()->getDefFlag(), EsNameItNode::DF_FUNCARG))
			{
				return true;
			}

			if(TFW_IS_FLAG(_bflags, BF_FUNC_IN_GENERATOR))
			{
				return true;
			}

			ot = OP_GETUPVAR;
		}

		// 上域変数リストから検索．無ければリストに追加する
		EsAtomList::EsAtomListEntity* ale = _alistUpvar->find(atom);	// ⇒ ale = cg->upvarList.lookup(atom);
		u32 index;
		if(ale != 0L)
		{
			index = ale->getIndex();
		}
		else
		{
			if(!_funcCur->addLocalVar(atom, EsFunction::LVK_UPVAR))	{	ESCASSERT(false);	return false;	}

			// 上域変数リストに追加
			ale = _alistUpvar->add(atom);
			if(ale == 0L)	{	ESCASSERT(false);	return false;	}
			index = ale->getIndex();
			ASSERT(index == _alistUpvar->getCount() - 1);

			// 上域変数情報配列の作成 ⇒ cg->upvarMap.vector
			if(_arrUpvarMap == 0L)
			{
				_arrUpvarMap = new InsArray<EsUpvarInfo>;
				_arrUpvarMap->realloc(_alistUnrslv->getCount());	// ⇒ uint32 length = cg->lexdeps.count; vector = (uint32 *) calloc(length, sizeof *vector);
				for(int i = 0; i < _alistUnrslv->getCount(); i++) { _arrUpvarMap->setAt(i, EsUpvarInfo()); }
			}

			u32 slotidx = upvariDef.getSlotIndex();
			if(defkind != EsNameItNode::DK_ARG)
			{
				EsBytecodeBuilder* bcbWk = this;
				do
				{
					bcbWk = bcbWk->_parent;
				}
				while(bcbWk->_depthNest != upvariDef.getSkipDepth());	// ⇒ while (tc->staticLevel != level);
				if(TFW_IS_FLAG(_bflags, BF_IN_FUNCTION))
				{
					slotidx += _funcCur->getLocalVarNum(EsFunction::LVK_ARG);
				}
			}

			// 上域変数情報に値を設定する
			_arrUpvarMap->setAt(index, EsUpvarInfo(skipdepDiff, slotidx));
		}

		node->setOpcodeType(ot);
		node->setUpvarInfo(0, index);
		TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG, true);
		return true;
	}

	switch(defkind)
	{
	case EsNameItNode::DK_NULL:	// ⇒ case JSDefinition::UNKNOWN:
		return true;
	case EsNameItNode::DK_LET:
		switch(ot)
		{
		case OP_NAME:		ot = OP_GETLOCAL;	break;
		case OP_SETNAME:	ot = OP_SETLOCAL;	break;
		case OP_INCNAME:	ot = OP_INCLOCAL;	break;
		case OP_NAMEINC:	ot = OP_LOCALINC;	break;
		case OP_DECNAME:	ot = OP_DECLOCAL;	break;
		case OP_NAMEDEC:	ot = OP_LOCALDEC;	break;
		case OP_FORNAME:	ot = OP_FORLOCAL;	break;
		}
		break;
	case EsNameItNode::DK_ARG:
		switch(ot)
		{
		case OP_NAME:		ot = OP_GETARG;		break;
		case OP_SETNAME:	ot = OP_SETARG;		break;
		case OP_INCNAME:	ot = OP_INCARG;		break;
		case OP_NAMEINC:	ot = OP_ARGINC;		break;
		case OP_DECNAME:	ot = OP_DECARG;		break;
		case OP_NAMEDEC:	ot = OP_ARGDEC;		break;
		case OP_FORNAME:	ot = OP_FORARG;		break;
		}
		ASSERT(!TFW_IS_FLAG(node->getDefFlag(), EsNameItNode::DF_CONST));
		break;
	default:
		if((defkind == EsNameItNode::DK_VAR) && (nodeDefNam->getOpcodeType() == OP_CALLEE))
		{
			ASSERT(ot != OP_CALLEE);
			ASSERT(TFW_IS_FLAG(_funcCur->getFlags(), EsFunction::F_LAMBDA) && (atom == _funcCur->getAtom()));

			ASSERT(ot != OP_DELNAME);
			if(!TFW_IS_FLAG(_bflags, BF_FUNC_NEED_CALLOBJ))
			{
				ot = OP_CALLEE;
				TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_CONST, true);
			}

			node->setOpcodeType(ot);
			TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG, true);
			return true;
		}

		switch(ot)
		{
		case OP_NAME:		ot = OP_GETLOCAL;	break;
		case OP_SETNAME:	ot = OP_SETLOCAL;	break;
		case OP_SETCONST:	ot = OP_SETLOCAL;	break;
		case OP_INCNAME:	ot = OP_INCLOCAL;	break;
		case OP_NAMEINC:	ot = OP_LOCALINC;	break;
		case OP_DECNAME:	ot = OP_DECLOCAL;	break;
		case OP_NAMEDEC:	ot = OP_LOCALDEC;	break;
		case OP_FORNAME:	ot = OP_FORLOCAL;	break;
		}
		break;
	}

	ASSERT(ot != node->getOpcodeType());
	node->setOpcodeType(ot);
	node->setUpvarInfo(0, upvariDef.getSlotIndex());
	TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG, true);
	return true;
}

/*---------------------------------------------------------------------*//**
	eval のための上域変数に名前ノードを変更する ⇒ MakeUpvarForEval
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::changeUpvarForEvalNameNode(EsNameItNode* node)
{
	EsFunction* func = _sfrmCaller->getFunction();			// ⇒ JSFunction *fun = cg->compiler->callerFrame->fun
	u32 depthUpver = func->getInterlang()->getNestDepth();	// ⇒ fun->u.i.script->staticLevel

	EsFunctionBox* funcbox =  _funcboxCur;
	if(funcbox != 0L)
	{
		if(funcbox->getNestDepth() == (depthUpver + 1))
		{
			if(TFW_IS_FLAG(*funcbox->getFunction()->flags(), EsFunction::F_LAMBDA))
			{
				return true;
			}
		}

		while(funcbox->getNestDepth() >= depthUpver)
		{
			if(TFW_IS_FLAG(*funcbox->getFuncNode()->defFlag(), EsNameItNode::DF_FUNCARG))
			{
				return true;
			}
			funcbox = funcbox->getParent();
			if(funcbox == 0L)
			{
				break;
			}
		}
	}

	const EsAtom* atom = node->getAtom();

	EsFunction::LocalVarKind lvk = EsFunction::NUM_LVK;
	u32 slotidx = 0;
	_funcCur->findLocalVar(&lvk, &slotidx, func->getAtom());	// ⇒ JSLocalKind localKind = js_LookupLocal(cx, fun, atom, &index);
	if(lvk == EsFunction::LVK_NULL)
	{
		return true;
	}

	if((_depthNest >= EsContext::SIZE_SF_DISPLAY) || (depthUpver >= EsContext::SIZE_SF_DISPLAY))	// ⇒ if (cg->staticLevel >= JS_DISPLAY_SIZE || upvarLevel >= JS_DISPLAY_SIZE)
	{
		return true;
	}

	// 上域変数アトムリストから検索し、無ければリストに追加する
	EsAtomList::EsAtomListEntity* ale = _alistUpvar->find(atom);	// ⇒ JSAtomListElement *ale = cg->upvarList.lookup(atom);
	if(ale == 0L)	// 既存のアトムは上域変数アトムリストになかった
	{
		if(TFW_IS_FLAG(_bflags, BF_IN_FUNCTION))
		{
			if(!func->addLocalVar(atom, EsFunction::LVK_UPVAR)) { return false; }
		}

		// 上域変数アトムリストに追加
		ale = _alistUpvar->add(atom);
		if(ale == 0L)	{	return false;	}
		ASSERT(ale->getIndex() == (_alistUpvar->getCount() - 1));

		// 上域変数情報配列の配列数確保
		u32 lengthUpvarMap = _arrUpvarMap->getCount();
		if(ale->getIndex() == lengthUpvarMap)
		{
			lengthUpvarMap = TFW_MAX(2, lengthUpvarMap) * 2;
			if(!_arrUpvarMap->realloc(lengthUpvarMap))	{	return false;	}
		}

		if(lvk != EsFunction::LVK_ARG)
		{
			slotidx += func->getLocalVarNum(EsFunction::LVK_ARG);	// ⇒ index += fun->nargs;
		}
		ASSERT(slotidx <= 0xffff);

		// 上域変数情報に値を設定する
		_arrUpvarMap->setAt(ale->getIndex(), EsUpvarInfo(_depthNest - depthUpver, slotidx));
	}

	node->setOpcodeType(OP_GETUPVAR);
	node->setUpvarInfo(_depthNest, ale->getIndex());
	TFW_SET_FLAG(*node->defFlag(), EsNameItNode::DF_BOUND_LG, true);
	return true;
}

/*---------------------------------------------------------------------*//**
	１バイトのバイトコードを追加する
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::append1Bc(EsOpcodeType ot, s32* bcofsLastOp)
{
	s32 bcofs = bcoffset();

	TRACE_OP("{EsBytecodeBuilder::append1Bc} OP_%s\n", EsOpcode::getSpec(ot)->getName());
	*bc() += (u8)ot;
	updateStackDepth(ot, bcofs);

	if(bcofsLastOp != 0L)	{	*bcofsLastOp = bcofs;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	２バイトのバイトコードを追加する
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::append2Bc(EsOpcodeType ot, u8 bc1, s32* bcofsLastOp)
{
	s32 bcofs = bcoffset();

	TRACE_OP("{EsBytecodeBuilder::append2Bc} OP_%s %02x\n", EsOpcode::getSpec(ot)->getName(), bc1);
	*bc() += (u8)ot;
	*bc() += bc1;
	updateStackDepth(ot, bcofs);

	if(bcofsLastOp != 0L)	{	*bcofsLastOp = bcofs;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	３バイトのバイトコードを追加する
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::append3Bc(EsOpcodeType ot, u8 bc1, u8 bc2, s32* bcofsLastOp)
{
	s32 bcofs = bcoffset();

	TRACE_OP("{EsBytecodeBuilder::append3Bc} OP_%s %02x %02x\n", EsOpcode::getSpec(ot)->getName(), bc1, bc2);
	*bc() += (u8)ot;
	*bc() += bc1;
	*bc() += bc2;
	updateStackDepth(ot, bcofs);

	if(bcofsLastOp != 0L)	{	*bcofsLastOp = bcofs;	}
	FTRACE("{EsBytecodeBuilder::append3Bc} 999.\n");
	return true;
}

/*---------------------------------------------------------------------*//**
	４バイトのバイトコードを追加する
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::append4Bc(EsOpcodeType ot, u8 bc1, u8 bc2, u8 bc3, s32* bcofsLastOp)
{
	s32 bcofs = bcoffset();

	TRACE_OP("{EsBytecodeBuilder::append4Bc} OP_%s %02x %02x %02x\n", EsOpcode::getSpec(ot)->getName(), bc1, bc2, bc3);
	*bc() += (u8)ot;
	*bc() += bc1;
	*bc() += bc2;
	*bc() += bc3;
	updateStackDepth(ot, bcofs);

	if(bcofsLastOp != 0L)	{	*bcofsLastOp = bcofs;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	５バイトのバイトコードを追加する
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::append5Bc(EsOpcodeType ot, u8 bc1, u8 bc2, u8 bc3, u8 bc4, s32* bcofsLastOp)
{
	s32 bcofs = bcoffset();

	TRACE_OP("{EsBytecodeBuilder::append5Bc} OP_%s %02x %02x %02x %02x\n", EsOpcode::getSpec(ot)->getName(), bc1, bc2, bc3, bc4);
	*bc() += (u8)ot;
	*bc() += bc1;
	*bc() += bc2;
	*bc() += bc3;
	*bc() += bc4;
	updateStackDepth(ot, bcofs);

	if(bcofsLastOp != 0L)	{	*bcofsLastOp = bcofs;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	たくさんのバイトコードを追加する ⇒ js_EmitN
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::appendManyBc(EsOpcodeType ot, u8* bcarr, s32 num, s32* bcofsLastOp)
{
	s32 bcofs = bcoffset();

	TRACE_OP("{EsBytecodeBuilder::appendManyBc} OP_%s\n", EsOpcode::getSpec(ot)->getName());
	*bc() += (u8)ot;
	for(s32 i  = 0; i < num; i++)
	{
		*bc() += bcarr[i];
	}
	updateStackDepth(ot, bcofs);

	if(bcofsLastOp != 0L)	{	*bcofsLastOp = bcofs;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	デバッグ情報をバイトコードに追加する
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::appendExInfoBc(EsItNode* node, s32* bcofsLastOp)
{
	if(!_isAddExInfo)	{	return true;	}

	if(node != 0L)
	{
		EsBcExInfo exi((u32)node->getPos()->getBeginIndex(), (u32)node->getPos()->getEndIndex());
		u8 bbc[EsBcExInfo::LEN_BC];
		exi.toBytecode(bbc);
		for(int i = 0; i < EsBcExInfo::LEN_BC; i++)
		{
			*bc() += bbc[i];
		}
	}
	else
	{
		EsBcExInfo exi;
		u8 bbc[EsBcExInfo::LEN_BC];
		exi.toBytecode(bbc);
		for(int i = 0; i < EsBcExInfo::LEN_BC; i++)
		{
			*bc() += bbc[i];
		}
	}

	if(bcofsLastOp != 0L)	{	*bcofsLastOp += EsBcExInfo::LEN_BC;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	インデックスバイトコードを追加する ⇒ EmitIndexOp
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupIndex(EsOpcodeType ot, u32 index, EsItNode* nodeInfo, s32* bcofsLastOp)
{
	if(index > 0x00ffffff)
	{
		// 大きすぎるインデックス
		addCompileError(EsError::ID_TOO_MANY_LITERALS, 0L);
		ESCASSERT(false);
		return false;
	}
	
	if(index > 0x0000ffff)	// 16 ビット表現を超えるインデックスの場合
	{
		/// ※※※ 実装なし ※※※ OP_RESETBASE0, JSOP_INDEXBASE1, JSOP_INDEXBASE2, JSOP_INDEXBASE3

		// 0x00ff0000 の部分を出力 ⇒ EmitBigIndexPrefix
		s32 indexHi = index >> 16;
		if(!appendExInfoBc(nodeInfo, bcofsLastOp))											{	return false;	}
		if(!append2Bc(OP_INDEXBASE, (u8)indexHi, bcofsLastOp))								{	return false;	}	// ⇒ if (js_Emit2(cx, cg, JSOP_INDEXBASE, (JSOp)indexBase) < 0)
		if(!appendExInfoBc(nodeInfo, bcofsLastOp))											{	return false;	}
		if(!append3Bc(ot, TFW_U16_TO_U8_HI(index), TFW_U16_TO_U8_LO(index), bcofsLastOp))	{	return false;	}	// ⇒ EMIT_UINT16_IMM_OP(op, index);
		if(!appendExInfoBc(nodeInfo, bcofsLastOp))											{	return false;	}
		if(!append1Bc(OP_RESETBASE, bcofsLastOp))											{	return false;	}	// ⇒ js_Emit1(cx, cg, bigSuffix)
	}
	else						// 16 ビット表現内のインデックスの場合
	{
		if(!appendExInfoBc(nodeInfo, bcofsLastOp))											{	return false;	}
		if(!append3Bc(ot, TFW_U16_TO_U8_HI(index), TFW_U16_TO_U8_LO(index), bcofsLastOp))	{	return false;	}	// ⇒ EMIT_UINT16_IMM_OP(op, index);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	スロットインデックスバイトコードを追加する ⇒ EmitSlotIndexOp
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupSlotIndex(EsOpcodeType ot, u32 slotidx, u32 index, EsItNode* nodeInfo, s32* bcofsLastOp)
{
	if(index >= 0x00ffffff)
	{
		// 大きすぎるインデックス
		addCompileError(EsError::ID_TOO_MANY_LITERALS, 0L);
		ESCASSERT(false);
		return false;
	}
	
	if(index > 0x0000ffff)	// 16 ビット表現を超えるインデックスの場合
	{
		// 0x00ff0000 の部分を出力 ⇒ EmitBigIndexPrefix
		s32 indexHi = index >> 16;
		if(!appendExInfoBc(nodeInfo, bcofsLastOp))				{	return false;	}
		if(!append2Bc(OP_INDEXBASE, (u8)indexHi, bcofsLastOp))	{	return false;	}	// ⇒ if (js_Emit2(cx, cg, JSOP_INDEXBASE, (JSOp)indexBase) < 0)
	}

	// 演算子、スロットインデックス、インデックスを生成 ⇒ Emit [op, slot, index].
	ASSERT(slotidx <= 0xffff);
	if(!appendExInfoBc(nodeInfo, bcofsLastOp))					{	return false;	}
	if(!append5Bc(
		ot,
		TFW_U32_TO_U8_B3(slotidx),
		TFW_U32_TO_U8_B4(slotidx),
		TFW_U32_TO_U8_B3(index),
		TFW_U32_TO_U8_B4(index),
		bcofsLastOp))
	{
		return false;
	}

	if(index > 0x0000ffff)	// 16 ビット表現を超えるインデックスの場合
	{
		if(!appendExInfoBc(nodeInfo, bcofsLastOp))				{	return false;	}
		if(!append1Bc(OP_RESETBASE, bcofsLastOp))				{	return false;	}	// ⇒ js_Emit1(cx, cg, bigSuffix)
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ネームノードのバイトコードを追加する ⇒ EmitNameOp
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupName(EsNameItNode* node, bool isCallOp, s32* bcofsLastOp)
{
	if(!bindNameAndSlot(node))
	{
		return false;
	}

	EsOpcodeType ot = node->getOpcodeType();

	if(isCallOp)
	{
		switch(ot)
		{
		case OP_NAME:		ot = OP_CALLNAME;	break;
		case OP_GETGVAR:	ot = OP_CALLGVAR;	break;
		case OP_GETARG:		ot = OP_CALLARG;	break;
		case OP_GETLOCAL:	ot = OP_CALLLOCAL;	break;
		case OP_GETUPVAR:	ot = OP_CALLUPVAR;	break;
		case OP_GETDSLOT:	ot = OP_CALLDSLOT;	break;
		default:			ASSERT((ot == OP_ARGUMENTS) || (ot == OP_CALLEE));	break;
		}

		// 外部拡張に関数情報を追加する
		if(ot == OP_CALLNAME)	{	_ctxRef->addParsedFunctionInfoForExternalExtension(node->getAtom(), false);	}
	}

	if((ot == OP_ARGUMENTS) || (ot == OP_CALLEE))
	{
		if(!appendExInfoBc(node, bcofsLastOp))		{	return false;	}
		if(!append1Bc(ot, bcofsLastOp))				{	return false;	}
		if(!appendExInfoBc(node, bcofsLastOp))		{	return false;	}
		if(!append1Bc(OP_NULL, bcofsLastOp))		{	return false;	}
	}
	else
	{
		if(!node->getUpvarInfo()->isFree())	// ⇒ if(pn->pn_cookie != FREE_UPVAR_COOKIE)
		{
			if(!appendExInfoBc(node, bcofsLastOp))		{	return false;	}
			if(!append3Bc(ot, (u8)node->getUpvarInfo()->getSkipDepth(), (u8)node->getUpvarInfo()->getSlotIndex(), bcofsLastOp))	{	return false;	}	// ⇒ EMIT_UINT16_IMM_OP(op, pn->pn_cookie);
		}
		else
		{
			if(!putupAtom(node, ot, bcofsLastOp))		{	return false;	}
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	アトムノードのバイトコードを追加する
	⇒ EmitAtomOp
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupAtom(EsNameItNode* node, EsOpcodeType ot, s32* bcofsLastOp)
{
	// length アクセサ
	if(ot == OP_GETPROP)	// アクセサ取得の場合
	{
		if(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom() == node->getAtom())
		{
			if(!appendExInfoBc(node, bcofsLastOp))	{	return false;	}
			return append1Bc(OP_LENGTH, bcofsLastOp);
		}
	}

	const EsAtom* atom = node->getAtom();
	if(atom == 0L)		{	ESCASSERT(false);	return false;	}
	EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(atom);	// ⇒  ale = cg->atomList.add(cg->compiler, pn->pn_atom);
	if(ale == 0L)		{	ESCASSERT(false);	return false;	}

	if(!putupIndex(ot, ale->getIndex(), node, bcofsLastOp))	{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	変数定義をバイトコードに追加する ⇒ EmitObjectOp
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupObject(EsObjectBox* objbox, EsOpcodeType ot, EsItNode* nodeInfo, s32* bcofsLastOp)
{
	_listObjbox->addTail(objbox);
	s32 index = _listObjbox->getCount() - 1;	// ⇒ JSCGObjectList::index
	return putupIndex(ot, index, nodeInfo, bcofsLastOp);
}

/*---------------------------------------------------------------------*//**
	変数定義をバイトコードに追加する ⇒ MaybeEmitVarDecl
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupVarDecl(EsNameItNode* node, EsOpcodeType ot, s32* bcofsLastOp, u32* atomidxOut)
{
	u32 atomidx = 0;
	if(!node->getUpvarInfo()->isFree())
	{
		atomidx = node->getUpvarInfo()->getSlotIndex();
	}
	else
	{
		EsAtomList::EsAtomListEntity* entity = _alistLiteral->add(node->getAtom());
		if(entity == 0L)
		{
			return false;
		}
		atomidx = entity->getIndex();
	}

	const EsOpcode::Spec* spec = EsOpcode::getSpec(node->getOpcodeType());
	u32 type = spec->_format & EsOpcode::OPFMT_TYPEMASK;
	if(type == EsOpcode::OPFMT_ATOM)
	{
		if(!TFW_IS_FLAG(_bflags, BF_IN_FUNCTION) || TFW_IS_FLAG(_bflags, BF_FUNC_NEED_CALLOBJ))
		{
			switchToProg();	// _bcProg へ生成 ⇒ CG_SWITCH_TO_PROLOG(cg);
			// ⇒ if (!UpdateLineNumberNotes(cx, cg, pn->pn_pos.begin.lineno)) return JS_FALSE;
			if(!putupIndex(ot, atomidx, node, bcofsLastOp))	{	return false;	}	// ⇒ EMIT_INDEX_OP(prologOp, atomIndex);
			switchToMain();	// _bcMain へ生成 ⇒ CG_SWITCH_TO_MAIN(cg);
		}
	}

	if(atomidxOut != 0L)
	{
		*atomidxOut = atomidx;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ジャンプ命令をバイトコードに追加する ⇒ EmitJump
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupJump(EsOpcodeType ot, s32 bcofsJump, EsItNode* nodeInfo, s32* bcofsLastOp)
{
	// extend?

	if(!appendExInfoBc(nodeInfo, bcofsLastOp))								{	return false;	}
	if(!append3Bc(ot, (u8)(bcofsJump >> 8), (u8)(bcofsJump), bcofsLastOp))	{	return false;	}

	// AddSpanDep?

	return true;
}

/*---------------------------------------------------------------------*//**
	ローカル外のジャンプ命令をバイトコードに追加する
	⇒ EmitNonLocalJumpFixup
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupNonLocalJump(EsStatement* stmtTo, EsItNode* nodeInfo, s32* bcofsLastOp)
{
	s32 depthStack = _depthStack;
	s32 npops = 0;

	for(EsStatement* stmt = _stmtTopEncl; stmt != stmtTo; stmt = stmt->getDownEnclosingStatement())
	{
		switch(stmt->getType())
		{
		case EsStatement::STMTT_FINALLY:
			{
				s32 bcofsBreak = stmt->getLastBreakBcOffset();
				if(!putupBackPatch(OP_BACKPATCH, &bcofsBreak, nodeInfo, bcofsLastOp))	{	return false;	}
				stmt->setLastBreakBcOffset(bcofsBreak);
			}
			break;

		case EsStatement::STMTT_WITH:
			if(npops > 0)	{	if(!putupPopsFlush(npops, nodeInfo, bcofsLastOp))	{	return false;	}	npops = 0;	}	// ⇒ FLUSH_POPS -> FlushPops
			if(!appendExInfoBc(nodeInfo, bcofsLastOp))	{	return false;	}
			if(!append1Bc(OP_LEAVEWITH, bcofsLastOp))	{	return false;	}
			break;

		case EsStatement::STMTT_FORIN:
			if(npops > 0)	{	if(!putupPopsFlush(npops, nodeInfo, bcofsLastOp))	{	return false;	}	npops = 0;	}	// ⇒ FLUSH_POPS -> FlushPops
			if(!appendExInfoBc(nodeInfo, bcofsLastOp))	{	return false;	}
			if(!append1Bc(OP_ENDITER, bcofsLastOp))		{	return false;	}
			break;

		case EsStatement::STMTT_SUBROUTINE:
			npops += 2;
			break;
		}

		if(TFW_IS_FLAG(stmt->getBlockFlags(), EsStatement::BLOCKF_SCOPE))
		{
			if(npops > 0)	{	if(!putupPopsFlush(npops, nodeInfo, bcofsLastOp))	{	return false;	}	npops = 0;	}	// ⇒ FLUSH_POPS -> FlushPops

			ASSERT(stmt->getBlockObject() != 0L);
			ASSERT(stmt->getBlockObject()->getScope() != 0L);
			s32 num = stmt->getBlockObject()->getScope()->getScopePropertyNum();
			if(!appendExInfoBc(nodeInfo, bcofsLastOp))														{	return false;	}
			if(!append3Bc(OP_LEAVEBLOCK, TFW_U16_TO_U8_HI(npops), TFW_U16_TO_U8_LO(npops), bcofsLastOp))	{	return false;	}	// ⇒ EMIT_UINT16_IMM_OP(JSOP_LEAVEBLOCK, i);
		}
	}

	if(npops > 0)	{	if(!putupPopsFlush(npops, nodeInfo, bcofsLastOp))	{	return false;	}	}	// ⇒ FLUSH_POPS -> FlushPops
	_depthStack = depthStack;
	return true;
}

/*---------------------------------------------------------------------*//**
	goto ジャンプ命令をバイトコードに追加する ⇒ EmitGoto
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupGoto(EsStatement* stmtTo, s32* bcofsLastJump, EsItNode* nodeInfo, s32* bcofsLastOp)
{
	if(!putupNonLocalJump(stmtTo, nodeInfo, bcofsLastOp))	{	return false;	}

	return putupBackPatch(OP_BACKPATCH, bcofsLastJump, nodeInfo, bcofsLastOp);
}

/*---------------------------------------------------------------------*//**
	バックパッチをバイトコードに追加する ⇒ EmitBackPatchOp
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupBackPatch(EsOpcodeType ot, s32* bcofsLastJump, EsItNode* nodeInfo, s32* bcofsLastOp)
{
	s32 bcofsTo = bcoffset();
	s32 bcofsFrom = *bcofsLastJump;
	*bcofsLastJump = bcofsTo;
	return putupJump(ot, bcofsTo - bcofsFrom, nodeInfo, bcofsLastOp);
}

/*---------------------------------------------------------------------*//**
	数値をバイトコードに追加する ⇒ EmitNumberOp
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupNumber(f64 fval, EsItNode* nodeInfo, s32* bcofsLastOp)
{
	s32 ival = (s32)fval;
	if(TFW_F64_IS_FINITE(fval) && !TFW_F64_IS_NZERO(fval) && (ival == fval))	// 有効な整数値である
	{
		if(!appendExInfoBc(nodeInfo, bcofsLastOp))	{	return false;	}
		if(ival == 0)
		{
			return append1Bc(OP_ZERO, bcofsLastOp);
		}
		if(ival == 1)
		{
			return append1Bc(OP_ONE, bcofsLastOp);
		}
		if((-127 <= ival) && (ival <= 128))
		{
			return append2Bc(OP_INT8, (s8)ival, bcofsLastOp);
		}
		u32 uval = (u32)ival;
		if(uval <= 0xffff)
		{
			return append3Bc(OP_UINT16, TFW_U16_TO_U8_HI(uval), TFW_U16_TO_U8_LO(uval), bcofsLastOp);
		}
		if(uval <= 0xffffff)
		{
			return append4Bc(OP_UINT24, TFW_U32_TO_U8_B2(uval), TFW_U32_TO_U8_B3(uval), TFW_U32_TO_U8_B4(uval), bcofsLastOp);
		}
		return append5Bc(OP_INT32, TFW_U32_TO_U8_B1(uval), TFW_U32_TO_U8_B2(uval), TFW_U32_TO_U8_B3(uval), TFW_U32_TO_U8_B4(uval), bcofsLastOp);
	}

	const EsAtom* atom = _ctxRef->makeDoubleAtom(&fval);
	if(atom == 0L)	{	ESCASSERT(false);	return false;	}

	EsAtomList::EsAtomListEntity* entry = _alistLiteral->add(atom);
	if(entry == 0L)	{	ESCASSERT(false);	return false;	}

	return putupIndex(OP_DOUBLE, entry->getIndex(), nodeInfo, bcofsLastOp);
}

/*---------------------------------------------------------------------*//**
	アクセサ命令バイトコードを追加する ⇒ EmitPropOp
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupProperty(EsNameItNode* node, EsOpcodeType ot, bool isCallContext, s32* bcofsLastOp)
{
	EsItNode* nodeExpr = node->getExpressionNode();

	// 特殊なアクセサ __proto__, __count__, __parent__ の対応
	// ※※※ 実装なし ※※※

	if(isCallContext)
	{
		ASSERT(node->getTokenType() == TKN_DOT);
		ASSERT(ot == OP_GETPROP);
		ot = OP_CALLPROP;
	}
	else if((ot == OP_GETPROP) && (node->getTokenType() == TKN_DOT))
	{
		if(nodeExpr->getOpcodeType() == OP_THIS)
		{
			// this.xxx の出力
			if(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom() != node->getAtom())
			{
				return putupAtom(node, OP_GETTHISPROP, bcofsLastOp);
			}
		}
		else if(nodeExpr->getTokenType() == TKN_NAME)
		{
			// arguments.length -> OP_ARGCNT
			// arg_name.prop -> OP_GETARGPROP
			// local_name.prop -> OP_GETLOCALPROP
			if(!bindNameAndSlot((EsNameItNode*)nodeExpr))
			{
				return false;
			}
			if(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom() == node->getAtom())
			{
				if(nodeExpr->getOpcodeType() == OP_ARGUMENTS)
				{
					if(!appendExInfoBc(node, bcofsLastOp))	{	return false;	}
					if(!append1Bc(OP_ARGCNT, bcofsLastOp))	{	return false;	}	// ⇒ js_Emit1(cx, cg, JSOP_ARGCNT) >= 0;
				}
			}
			else
			{
				if((nodeExpr->getOpcodeType() == OP_GETARG) || (nodeExpr->getOpcodeType() == OP_GETLOCAL))
				{
					switch(nodeExpr->getOpcodeType())
					{
					case OP_GETARG:		ot = OP_GETARGPROP;		break;
					case OP_GETLOCAL:	ot = OP_GETLOCALPROP;	break;
					}
					EsAtomList::EsAtomListEntity* ale = _alistLiteral->add(node->getAtom());	// ⇒ ale = cg->atomList.add(cg->compiler, pn->pn_atom);
					if(ale == 0L)	{	ESCASSERT(false);	return false;	}
					ASSERT(nodeExpr->getNodeType() == NT_NAME);
					return putupSlotIndex(ot, ((EsNameItNode*)nodeExpr)->getUpvarInfo()->getSlotIndex(), ale->getIndex(), nodeExpr, bcofsLastOp);	// ⇒ EmitSlotIndexOp(cx, op, pn2->pn_cookie, atomIndex, cg);
				}
			}
		}
	}

	// ドット演算子で連結されている場合とそうでない場合
	if(nodeExpr->getTokenType() == TKN_DOT)	// ドット演算子対応
	{
		ASSERT(nodeExpr->getNodeType() == NT_NAME);
		EsNameItNode* nodeDot = (EsNameItNode*)nodeExpr;
		EsNameItNode* nodeUp = 0L;
		EsItNode* nodeDown = 0L;

		// ドット演算子で結合された最も右辺のノードを得る
		s32 bcofsTop = bcoffset();	// ⇒ top = CG_OFFSET(cg);
		while(true)
		{
			nodeDot->setBytecodeOffset(bcofsTop);
			//ASSERT(!nodeDot->isHasLex());
			nodeDown = nodeDot->getExpressionNode();
			ASSERT(nodeDown != 0L);
			nodeDot->setExpressionNode(nodeUp);
			if(nodeDown->getTokenType() != TKN_DOT)
			{
				break;
			}
			ASSERT(nodeDown->getNodeType() == NT_NAME);
			nodeUp = nodeDot;
			nodeDot = (EsNameItNode*)nodeDown;
		}

		// 最右辺ノードのバイトコードを出力する
		if(!buildFromTree(nodeDown))
		{
			return false;
		}

		do
		{
			// ⇒ Note?

			// 特殊なアクセサ __proto__, __count__, __parent__ の対応
			// ※※※ 実装なし ※※※
			// アトムバイトコードを出力する
			if(!putupAtom(nodeDot, nodeDot->getOpcodeType(), bcofsLastOp))
			{
				return false;
			}

			// リンクを逆戻りループする
			nodeUp = (EsNameItNode*)nodeDot->getExpressionNode();
			nodeDot->setExpressionNode(nodeDown);
			nodeDot = nodeUp;
		}
		while(nodeDot != 0L);
	}
	else
	{
		if(!buildFromTree(nodeExpr))
		{
			return false;
		}
	}

	// ⇒ Note?

	return putupAtom(node, ot, bcofsLastOp);
}

/*---------------------------------------------------------------------*//**
	要素バイトコードを追加する ⇒ EmitElemOp
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupElement(EsItNode* node, EsOpcodeType ot, s32* bcofsLastOp)
{
	s32 bcofsTop = bcoffset();	// ⇒ top = CG_OFFSET(cg);

	EsItNode* nodeRight = 0L;
	EsNameItNode nodeTmp1;

	if(node->getNodeType() == NT_LIST)
	{
		EsListItNode* nodeList = (EsListItNode*)node;
		ASSERT(nodeList->getOpcodeType() == OP_GETELEM);
		ASSERT(nodeList->getNodeCount() >= 3);

		EsItNode* nodeHead = nodeList->getHeadNode();
		EsItNode* nodeNext = nodeHead->getNextNode();
		EsItNode* nodeTail = nodeHead;
		while(nodeTail->getNextNode() != 0L)	{	nodeTail = nodeTail->getNextNode();	}

		// arguments[0][xxx] の最適化
		if((nodeHead->getTokenType() == TKN_NAME) && (nodeNext->getTokenType() == TKN_NUMBER))
		{
			ASSERT(nodeHead->getNodeType() == NT_NAME);
			if(!bindNameAndSlot((EsNameItNode*)nodeHead))	{	return false;	}
			if(nodeHead->getOpcodeType() == OP_ARGUMENTS)
			{
				f64 dSlotIdx = ((EsDoubleItNode*)nodeNext)->getDouble();
				if(TFW_F64_IS_INT(dSlotIdx))	// ⇒ JSDOUBLE_IS_INT
				{
					u32 iSlotIdx = (s32)dSlotIdx;
					if(iSlotIdx <= 0xffff)	// ⇒ (jsuint)slot < JS_BIT(16)
					{
						// arguments[xxx]() の対応．this オブジェクトの要求
						ASSERT((ot != OP_CALLELEM) || (nodeNext->getNextNode() != 0L));
						nodeHead->setBytecodeOffset(bcofsTop);
						nodeNext->setBytecodeOffset(bcofsTop);
						if(!appendExInfoBc(nodeNext, 0L))		{	return false;	}
						if(!append3Bc(OP_ARGSUB, TFW_U16_TO_U8_HI(iSlotIdx), TFW_U16_TO_U8_LO(iSlotIdx), 0L))	{	return false;	}	// EMIT_UINT16_IMM_OP(JSOP_ARGSUB, (jsatomid)slot);
						nodeHead = nodeNext;
						nodeNext = nodeHead->getNextNode();
					}
				}
			}
		}

		// 最初のリストノードの出力
		if(nodeHead == nodeList->getHeadNode())
		{
			if(!buildFromTree(nodeHead))		{	return false;	}
		}

		// 最初のリストノードと最後のリストノード以外の出力
		while(nodeNext != nodeTail)
		{
			if(!buildFromTree(nodeNext))		{	return false;	}
			if(!appendExInfoBc(nodeNext, 0L))	{	return false;	}
			if(!append1Bc(OP_GETELEM, 0L))		{	return false;	}

			nodeNext = nodeNext->getNextNode();
		}
	}
	else
	{
		EsItNode* nodeLeft = 0L;
		EsNameItNode nodeTmp2;

		if(node->getNodeType() == NT_NAME)
		{
			const EsAtom* atom = ((EsNameItNode*)node)->getAtom();
			ASSERT(atom->getType() == EsAtom::ATYPE_STRING);

			nodeLeft = ((EsNameItNode*)node)->getExpressionNode();
			if(nodeLeft == 0L)
			{
				nodeTmp2.setTokenType(TKN_STRING);
				nodeTmp2.setOpcodeType(OP_BINDNAME);
				nodeTmp2.setPos(node->getPos());
				nodeTmp2.setAtom(atom);
				nodeLeft = &nodeTmp2;
			}
			nodeTmp1.setTokenType(TKN_STRING);
			nodeTmp1.setOpcodeType(EsTokenGetter::Cu::isIdentifier(atom->getString()) ? OP_QNAMEPART : OP_STRING);
			nodeTmp1.setPos(node->getPos());
			nodeTmp1.setAtom(atom);
			nodeRight = &nodeTmp1;
		}
		else
		{
			ASSERT(node->getNodeType() == NT_BINARY);
			nodeLeft = ((EsBinaryItNode*)node)->getLeftNode();
			nodeRight = ((EsBinaryItNode*)node)->getRightNode();
		}

		// OP_ARGSUB 最適化（例えば、arguments[0]）
		if(	(ot == OP_GETELEM) &&
			(nodeLeft->getTokenType() == TKN_NAME) &&
			(nodeRight->getTokenType() == TKN_NUMBER)	)
		{
			if(!bindNameAndSlot((EsNameItNode*)nodeLeft))	{	return false;	}
			if(nodeLeft->getOpcodeType() == OP_ARGUMENTS)
			{
				f64 dSlotIdx = ((EsDoubleItNode*)nodeRight)->getDouble();
				if(TFW_F64_IS_INT(dSlotIdx))	// ⇒ JSDOUBLE_IS_INT
				{
					u32 iSlotIdx = (s32)dSlotIdx;
					if(iSlotIdx <= 0xffff)	// ⇒ (jsuint)slot < JS_BIT(16)
					{
						// arguments[xxx]() の対応．this オブジェクトの要求
						nodeLeft->setBytecodeOffset(bcofsTop);
						nodeRight->setBytecodeOffset(bcofsTop);
						if(!appendExInfoBc(nodeLeft, 0L))		{	return false;	}
						if(!append3Bc(OP_ARGSUB, TFW_U16_TO_U8_HI(iSlotIdx), TFW_U16_TO_U8_LO(iSlotIdx), 0L))	{	return false;	}	// EMIT_UINT16_IMM_OP(JSOP_ARGSUB, (jsatomid)slot);
						return true;
					}
				}
			}
		}

		if(!buildFromTree(nodeLeft))		{	return false;	}
	}

	// 右辺ノードの出力
	ASSERT((ot != OP_DESCENDANTS) || (nodeRight->getTokenType() != TKN_STRING) || (nodeRight->getOpcodeType() == OP_QNAMEPART));
	if(!buildFromTree(nodeRight))		{	return false;	}
	if(!appendExInfoBc(nodeRight, 0L))	{	return false;	}
	if(!append1Bc(ot, 0L))				{	return false;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	Pop バイトコードを追加する ⇒ FlushPops
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupPopsFlush(s32 popnum, EsItNode* nodeInfo, s32* bcofsLastOp)
{
	if(!appendExInfoBc(nodeInfo, bcofsLastOp))	{	return false;	}
	return append3Bc(OP_POPN, TFW_U16_TO_U8_HI(popnum), TFW_U16_TO_U8_LO(popnum), bcofsLastOp);	// ⇒ EMIT_UINT16_IMM_OP(JSOP_POPN, *npops);
}

/*---------------------------------------------------------------------*//**
	ブロックへ入るバイトコードを追加する ⇒ EmitEnterBlock
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupEnterBlock(EsItNode* node, s32* bcofsLastOp)
{
	ASSERT(node->getNodeType() == NT_NAME);
	ASSERT(node->getTokenType() == TKN_LEXICALSCOPE);

	EsObjectBox* objbox = ((EsNameItNode*)node)->getObjectBox();
	if(!putupObject(objbox, OP_ENTERBLOCK, node, bcofsLastOp))	{	return false;	}

	EsObject* objBlock = objbox->getBlockObject();
	s32 slotidxDepth = objBlock->getSlots()->getBlockDepth();	// ⇒ OBJ_BLOCK_DEPTH(cx, blockObj)
	slotidxDepth = adjustBlockSlotIndex(slotidxDepth);			// ⇒ jsint depth = AdjustBlockSlot(cx, cg, OBJ_BLOCK_DEPTH(cx, blockObj));
	if(slotidxDepth < 0)	{	ESCASSERT(false);	return false;	}

	s32 slotidxFree = ((const EsBlock::EsBlockClass*)EsSysClasses::getClass(EsSysClasses::CLASSTYPE_BLOCK))->getFreeSlotIndex();
	s32 slotidxWk = slotidxFree;
	s32 slotidxLimit = slotidxWk + objBlock->getScope()->getScopePropertyNum();	// ⇒ limit = slot + OBJ_BLOCK_COUNT(cx, blockObj);
	while(slotidxWk < slotidxLimit)
	{
		EsValue* v = objBlock->slots()->value(slotidxWk);
		if(!v->isVoid())
		{
			EsNameItNode* nodeDef = (EsNameItNode*)v->getPrivateData();
			ASSERT(TFW_IS_FLAG(nodeDef->getDefFlag(), EsNameItNode::DF_EXTYPE_DEFINE));
			ASSERT((nodeDef->getUpvarInfo()->getSlotIndex() + slotidxDepth) <= 0xffff);	// ⇒ JS_ASSERT(uintN(dn->frameSlot() + depth) < JS_BIT(16));
			nodeDef->upvarInfo()->setSlotIndex(nodeDef->getUpvarInfo()->getSlotIndex() + slotidxDepth);
		}
		slotidxWk++;
	}

	objBlock->getScope()->setFreeSlotIndex(slotidxFree);	// ⇒ OBJ_SCOPE(blockObj)->freeslot = JSSLOT_FREE(&js_BlockClass);
	objBlock->slots()->recreate(slotidxFree);				// ⇒ js_ReallocSlots(cx, blockObj, JSSLOT_FREE(&js_BlockClass), JS_TRUE);
	return true;
}

/*---------------------------------------------------------------------*//**
	停止バイトコードを追加する
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::putupStop()
{
	if(!appendExInfoBc(0L, 0L))	{	return false;	}
	return append1Bc(OP_STOP, 0L);
}

/*---------------------------------------------------------------------*//**
	スタックサイズを更新する ⇒ UpdateDepth
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::updateStackDepth(EsOpcodeType ot, u32 bcofs)
{
	FTRACE("{EsBytecodeBuilder::updateStackDepth} 000.\n");
	const EsOpcode::Spec* opspec = EsOpcode::getSpec(ot);	// ⇒ cs

	// ※※※ 実装保留 ※※※ ⇒ if ((cs->format & JOF_TMPSLOT_MASK) || extra)

	s32 numStackUses = EsOpcode::getStackUses(ot, bc(), bcofs);
	_depthStack -= numStackUses;
	ASSERTM(_depthStack >= 0, "{EsBytecodeBuilder::updateStackDepth} _depthStack >= 0");
	if(_depthStack < 0)
	{
		addCompileError(EsError::ID_STACK_UNDERFLOW, 0L);
		FTRACE("{EsBytecodeBuilder::updateStackDepth} -001. ot=OP_%s, bcofs=%d, _depthStack=%d, numStackUses=%d\n", opspec->getName(), bcofs, _depthStack, numStackUses);
	}

	s32 numStackDefs = opspec->_numStackDefs;
	if(numStackDefs < 0)
	{
		ASSERTM(ot == OP_ENTERBLOCK, "{EsBytecodeBuilder::updateStackDepth} ot == OP_ENTERBLOCK");
		ASSERTM(numStackUses == 0, "{EsBytecodeBuilder::updateStackDepth} numStackUses == 0");
		ASSERTM(_listObjbox->getCount() > 0, "{EsBytecodeBuilder::updateStackDepth} _listObjbox->getCount() > 0");
		EsObject* objBlock = _listObjbox->getTailObject()->getBlockObject();	// ⇒ blockObj = cg->objectList.lastbox->object;
		ASSERTM(objBlock != 0L, "{EsBytecodeBuilder::updateStackDepth} objBlock != 0L");
		ASSERTM(objBlock->getClass()->getClassType() == EsSysClasses::CLASSTYPE_BLOCK, "{EsBytecodeBuilder::updateStackDepth} objBlock->getClass()->getClassType() == EsSysClasses::CLASSTYPE_BLOCK");

		objBlock->slots()->setBlockDepth(_depthStack);				// ⇒  OBJ_SET_BLOCK_DEPTH(cx, blockObj, cg->stackDepth);
		numStackDefs = objBlock->getScope()->getScopePropertyNum();	// ⇒ ndefs = OBJ_BLOCK_COUNT(cx, blockObj);
	}
	_depthStack += numStackDefs;

	if(_depthStackMax < _depthStack)
	{
		_depthStackMax = _depthStack;
	}
	FTRACE("{EsBytecodeBuilder::updateStackDepth} 999.\n");
}

/*---------------------------------------------------------------------*//**
	隣接ノードが有効な操作かどうかを判定する
	⇒ CheckSideEffects
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::decideUsefulSideNodeOperation(bool* isUseful, EsItNode* nodeT)
{
	if((nodeT == 0L) || *isUseful)
	{
		return true;
	}

	bool isSucceeded = true;
	switch(nodeT->getNodeType())
	{
	case NT_UNARY:
		{
			EsItNode* nodeChild = nodeT->getChildNode();
			ASSERT(nodeChild != 0L);
			switch(nodeT->getTokenType())
			{
			case TKN_RP:
				isSucceeded = decideUsefulSideNodeOperation(isUseful, nodeChild);
				break;
			case TKN_DELETE:
				switch(nodeChild->getTokenType())
				{
				case TKN_NAME:
					if(!bindNameAndSlot((EsNameItNode*)nodeChild))	{	return false;	}
					if(TFW_IS_FLAG(((EsNameItNode*)nodeChild)->getDefFlag(), EsNameItNode::DF_CONST))
					{
						*isUseful = false;
					}
					else
					{
						*isUseful = true;
					}
					break;
				case TKN_DOT:
				case TKN_LP:
				case TKN_LB:
					*isUseful = true;
					break;
				default:
					isSucceeded = decideUsefulSideNodeOperation(isUseful, nodeChild);
					break;
				}
				break;
			case TKN_UNARYOP:
				if(nodeT->getOpcodeType() == OP_NOT)
				{
					isSucceeded = decideUsefulSideNodeOperation(isUseful, nodeChild);
				}
				else
				{
					*isUseful = true;
				}
				break;
			default:
				// TKN_INC、TKN_DEC、TKN_THROW、TKN_YIELD、TKN_DEFSHARP
				// は有効な操作があるといえる．
				// しかし、それ以外のオペランドは toString や valueOf を示すとは一概に言えない
				*isUseful = true;
				break;
			}
		}
		break;

	case NT_BINARY:
		if(nodeT->getTokenType() == TKN_ASSIGN)
		{
			// 代入は、左オペランドがセッタープロパティかもしれないので
			// 次の演算が効果を上書きするものであっても、有効と見なす．
			// 唯一の例外は、コンパイルされている現在の関数内で、
			// 定数定義に無効な値が代入されるときである．
			EsItNode* nodeL = ((EsBinaryItNode*)nodeT)->getLeftNode();
			if(nodeL->getTokenType() != TKN_NAME)
			{
				*isUseful = true;
			}
			else
			{
				if(!bindNameAndSlot((EsNameItNode*)nodeL))	{	return false;	}
				if(!decideUsefulSideNodeOperation(isUseful, ((EsBinaryItNode*)nodeT)->getRightNode()))	{	return false;	}

				if(!*isUseful)
				{
					if((nodeT->getOpcodeType() != OP_NOP) || !TFW_IS_FLAG(*((EsNameItNode*)nodeL)->defFlag(), EsNameItNode::DF_CONST))
					{
						*isUseful = true;
					}
				}
			}
		}
		else
		{
			switch(nodeT->getOpcodeType())
			{
			case OP_OR:
			case OP_AND:
			case OP_STRICTEQ:
			case OP_STRICTNE:
				// ||、&&、===、!== は、toString や valueOf コールを通ってオペランドを変換しない
				isSucceeded = 
					decideUsefulSideNodeOperation(isUseful, ((EsBinaryItNode*)nodeT)->getLeftNode()) &&
					decideUsefulSideNodeOperation(isUseful, ((EsBinaryItNode*)nodeT)->getRightNode());
				break;
			default:
				// どのオペランドも toString や valueOf を示すとは一概に言えない
				*isUseful = true;
				break;
			}
		}
		break;

	case NT_TERNARY:
		isSucceeded = 
			decideUsefulSideNodeOperation(isUseful, ((EsTernaryItNode*)nodeT)->getChildNode1()) &&
			decideUsefulSideNodeOperation(isUseful, ((EsTernaryItNode*)nodeT)->getChildNode2()) &&
			decideUsefulSideNodeOperation(isUseful, ((EsTernaryItNode*)nodeT)->getChildNode3());
		break;

	case NT_LIST:
		switch(nodeT->getOpcodeType())
		{
		case OP_NOP:
		case OP_OR:
		case OP_AND:
		case OP_STRICTEQ:
		case OP_STRICTNE:
			// ||、&&、===、!== に伴う NOP は、toString や valueOf を呼び出さない
			for(EsItNode* nodeWk = ((EsListItNode*)nodeT)->getHeadNode(); nodeWk != 0L; nodeWk = nodeWk->getNextNode())
			{
				if(!decideUsefulSideNodeOperation(isUseful, nodeWk))
				{
					isSucceeded = false;
				}
			}
			break;
		default:
			*isUseful = true;
			break;
		}
		break;

	case NT_NAME:
		{
			EsNameItNode* nodeNameT = (EsNameItNode*)nodeT;
			// ラベル名のバインドに注意（ラベル、for 文、アクセサ名はオブジェクトの初期化で OP_NOP になっている）
			if((nodeT->getTokenType() == TKN_NAME) && (nodeT->getOpcodeType() != OP_NOP))
			{
				if(!bindNameAndSlot(nodeNameT))	{	false;	}
				if((nodeT->getOpcodeType() != OP_ARGUMENTS) && (nodeT->getOpcodeType() != OP_CALLEE))
				{
					if(nodeNameT->getUpvarInfo()->isFree())
					{
						// ローカル変数や引数ではない
						*isUseful = true;
					}
				}
			}
			EsItNode* nodeExpr = nodeNameT->getExpressionNode();
			if(nodeT->getTokenType() == TKN_DOT)
			{
				ASSERT(nodeExpr != 0L);
				if(nodeExpr->getTokenType() == TKN_NAME)
				{
					if(!bindNameAndSlot((EsNameItNode*)nodeExpr))	{	return false;	}
				}

				// ドット演算子によるゲッタプロパティは基本的にはどんなものにも呼べるが、arguments.lengthは例外である
				if(!((nodeExpr->getOpcodeType() == OP_ARGUMENTS) && (EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom() == nodeNameT->getAtom())))	// ⇒ if (!(pn2->pn_op == JSOP_ARGUMENTS && pn->pn_atom == cx->runtime->atomState.lengthAtom))
				{
					*isUseful = true;
				}
			}
			isSucceeded = decideUsefulSideNodeOperation(isUseful, nodeExpr);
		}
		break;

	case NT_NAMESET:
		isSucceeded = decideUsefulSideNodeOperation(isUseful, ((EsNameSetItNode*)nodeT)->getSubTree());
		break;

	case NT_NULLARY:
	case NT_DOUBLE:
		if(nodeT->getTokenType() == TKN_DEBUGGER)
		{
			*isUseful = true;
		}
		break;
	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	以前追加したジャンプ命令のジャンプ位置を後で設定する
	⇒ js_SetJumpOffset
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::writeJumpOffsetLater(s32 bcosfJumpBcPos, s32 bcofsJump)
{
	ASSERT((0 <= (bcosfJumpBcPos + 1)) && ((bcosfJumpBcPos + 2) < bc()->getLength()));
	bc()->setAt(bcosfJumpBcPos + 1, TFW_U32_TO_U8_B3(bcofsJump));
	bc()->setAt(bcosfJumpBcPos + 2, TFW_U32_TO_U8_B4(bcofsJump));
	return true;
}

/*---------------------------------------------------------------------*//**
	以前追加したジャンプ命令のジャンプ位置を後で設定する（16bit 値のみ）
	⇒ SET_JUMP_OFFSET
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::writeJumpOffsetLater16(s32 bcosfJumpBcPos, u16 bcofsJump)
{
	ASSERT((0 <= (bcosfJumpBcPos + 1)) && ((bcosfJumpBcPos + 2) < bc()->getLength()));
	bc()->setAt(bcosfJumpBcPos + 1, TFW_U16_TO_U8_HI(bcofsJump));
	bc()->setAt(bcosfJumpBcPos + 2, TFW_U16_TO_U8_LO(bcofsJump));
	return true;
}

/*---------------------------------------------------------------------*//**
	以前追加した命令にインデックス値を後で設定する（16bit 値のみ）
	⇒ SET_INDEX
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::writeIndexLater16(s32 bcosfJumpBcPos, u16 index)
{
	ASSERT((0 <= (bcosfJumpBcPos + 1)) && ((bcosfJumpBcPos + 2) < bc()->getLength()));
	bc()->setAt(bcosfJumpBcPos + 1, TFW_U16_TO_U8_HI(index));
	bc()->setAt(bcosfJumpBcPos + 2, TFW_U16_TO_U8_LO(index));
	return true;
}

/*---------------------------------------------------------------------*//**
	ブロックスロットインデックスを調整する ⇒ AdjustBlockSlot
**//*---------------------------------------------------------------------*/
s32 EsBytecodeBuilder::adjustBlockSlotIndex(s32 slotidx)
{
	ASSERT(slotidx < _depthStackMax);
	if(TFW_IS_FLAG(_bflags, BF_IN_FUNCTION))
	{
		slotidx += _funcCur->getLocalVarNum(EsFunction::LVK_VAR);	// ⇒ slot += cg->fun->u.i.nvars;
		if(slotidx >= MAX_SLOTIDX)
		{
			addCompileError(EsError::ID_TOO_MANY_LOCALS, 0L);
			return -1;
		}
	}
	return slotidx;
}

/*---------------------------------------------------------------------*//**
	バイトコード出力時の定数検索 ⇒ LookupCompileTimeConstant
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::findConstant(EsValue* val, const EsAtom* atom)
{
	EsBytecodeBuilder* bcb = this;

	do
	{
		if(TFW_IS_FLAG(bcb->getBuildFlag(), BF_IN_FUNCTION | BF_OPT_NAME_REF_ON_SCOPE))	// ⇒ if (cg->flags & (TCF_IN_FUNCTION | TCF_COMPILE_N_GO))
		{
			if(findLexical(0L, 0L, atom, 0L))
			{
				return true;
			}

			EsAtomList::EsAtomListEntity* ale = _alistConst->find(atom);
			if(ale != 0L)
			{
				val->copy(ale->getValue());
				return true;
			}

			if(TFW_IS_FLAG(bcb->getBuildFlag(), BF_IN_FUNCTION))
			{
				if(_funcCur->findLocalVar(0L, 0L, atom))	// ⇒ if (js_LookupLocal(cx, cg->fun, atom, NULL) != JSLOCAL_NONE)
				{
					break;
				}
			}
			else	// BF_OPT_NAME_REF_ON_SCOPE
			{
				EsObject* objScopeChain = _objScopeChain;
				ASSERT(objScopeChain != 0L);
				EsScopeProperty* sprop;
				EsObject* objScopeFound = 0L;
				EsValue spropid(atom);
				if(!objScopeChain->findPropertyOv(&sprop, &objScopeFound, _ctxRef, &spropid))	// ⇒ OBJ_LOOKUP_PROPERTY
				{
					return false;
				}

				bool isSucceeded = true;
				if(objScopeFound == objScopeChain)
				{
					u8 atrbf = 0;
					if(objScopeChain->getPropertyAttributeFlagsOv(&atrbf, _ctxRef, &spropid, sprop))
					{
						// 定数プロパティの場合 ⇒ IS_CONSTANT_PROPERTY
						if(	TFW_IS_FLAG(atrbf, EsScopeProperty::ATRBF_READONLY) &&
							TFW_IS_FLAG(atrbf, EsScopeProperty::ATRBF_PERMANENT) &&
							!TFW_IS_FLAG(atrbf, EsScopeProperty::ATRBF_OBJ_GETTER) )
						{
							isSucceeded = objScopeChain->callGetPropertyOv(val, _ctxRef, &spropid);
						}
					}
				}

				if(!isSucceeded)	{	return false;	}
				if(sprop)			{	break;	}
			}
		}

		bcb = bcb->getParent();
	}
	while(bcb != 0L);
	return true;
}

/*---------------------------------------------------------------------*//**
	バイトコード出力時の定数定義 ⇒ js_DefineCompileTimeConstant
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::defineConstant(const EsAtom* atom, EsItNode* node)
{
	if(node->getTokenType() == TKN_NUMBER)
	{
		EsValue val;
		f64 fval = ((EsDoubleItNode*)node)->getDouble();
		if(TFW_F64_IS_INT(fval))
		{
			val.setValue((s32)fval);
		}
		else
		{
			const EsAtom* atomDouble = _ctxRef->makeDoubleAtom(&fval);	// ⇒ valueAtom = js_AtomizeDouble(cx, dval);
			if(atomDouble == 0L)	{	ESCASSERT(false);	return false;	}
			val.setValue(atomDouble);								// ⇒ v = ATOM_KEY(valueAtom);
		}
		EsAtomList::EsAtomListEntity* ale = _alistConst->add(atom);	// ⇒ ale = cg->constList.add(cg->compiler, atom);
		if(ale == 0L)	{	ESCASSERT(false);	return false;	}
		ale->setValue(&val);	// ⇒ ALE_SET_VALUE(ale, v);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステートメントを取り出し、ジャンプオフセットを書き込む
	⇒ js_PopStatementCG
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::popStatementAndWriteJumpOffset()
{
	EsStatement* stmt = _stmtTopEncl;

	if(true)	// ⇒ !STMT_IS_TRYING(stmt)
	{
		writeBackPathLater(stmt->getLastBreakBcOffset(), bcoffset(), OP_GOTO);			// ⇒ BackPatch(cx, cg, stmt->breaks, CG_NEXT(cg), JSOP_GOTO)
		writeBackPathLater(stmt->getLastContinueBcOffset(), stmt->getUpdateBcOffset(), OP_GOTO);	// ⇒ BackPatch(cx, cg, stmt->continues, CG_CODE(cg, stmt->update), JSOP_GOTO)
	}

	popStatement(false);
}

/*---------------------------------------------------------------------*//**
	ジャンプオフセットを書き込む ⇒ BackPatch
**//*---------------------------------------------------------------------*/
bool EsBytecodeBuilder::writeBackPathLater(s32 bcofsLast, s32 bcofsTrg, EsOpcodeType ot)
{
	if(bcofsLast > 0)
	{
		EsProgramCounter pcWk(bc());
		pcWk.advance(bcofsLast);
		s32 bcofsExi = _isAddExInfo ? EsBcExInfo::LEN_BC : 0;
		pcWk.advance(bcofsExi);
		while(true)
		{
			s32 delta = pcWk.getJumpOffset();					// ⇒ GetJumpOffset, GET_JUMP_OFFSET
			s32 diff = bcofsTrg - pcWk.getCurrentOffset();		// ⇒ PTRDIFF(target, pc, jsbytecode);
			if(!writeJumpOffsetLater(pcWk.getCurrentOffset(), diff))	{	return false;	}	// ⇒ CHECK_AND_SET_JUMP_OFFSET(cx, cg, pc, span);
			bc()->setAt(pcWk.getCurrentOffset(), ot);
			if(!pcWk.isSafeOffset(- delta - bcofsExi))
			{
				break;	// 終了
			}
			pcWk.advance(- delta);
		}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	編集バイトコードをメイン側に切り替える ⇒ CG_SWITCH_TO_MAIN
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::switchToMain()
{
	_bcCur = &_bcMain;
}

/*---------------------------------------------------------------------*//**
	編集バイトコードを非メイン側に切り替える ⇒ CG_SWITCH_TO_PROLOG
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::switchToProg()
{
	_bcCur = &_bcProg;
}

/*---------------------------------------------------------------------*//**
	コンパイルエラーを追加する
**//*---------------------------------------------------------------------*/
void EsBytecodeBuilder::addCompileError(EsError::Id id, VcStringArray* saParamDlg)
{
	_ctxRef->getError()->addCompileError(id, EsTokenPos(), saParamDlg);
}

//==========================================================================
// EsBytecodeBuilder::Bytecode メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsBytecodeBuilder::Bytecode::Bytecode()
	: _code(new VcString(256))
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsBytecodeBuilder::Bytecode::~Bytecode()
{
	delete _code;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
