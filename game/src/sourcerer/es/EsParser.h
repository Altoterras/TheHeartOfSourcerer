/***********************************************************************//**
 *	EsParser.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_PARSER_H_
#define _ES_ES_PARSER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsError.h"
#include "EsItNode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

TFW_END_NS

ES_BEGIN_NS

class EsAtom;
class EsBytecodeBuilder;
class EsContext;
class EsFunction;
class EsFunctionBox;
class EsInterlang;
class EsObjectBox;
class EsStatement;
class EsTokenParser;

/*---------------------------------------------------------------------*//**
 *	ES パーサー（コンパイラ）
 *		⇒ JSCompiler
 *
**//*---------------------------------------------------------------------*/
class EsParser
{
	//======================================================================
	// 定数
protected:
	// 仮実装
	#define NUM_ARGC_MAX			0xffff
	#define NUM_ARRAY_INIT_MAX		0xfffff

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	EsParser();
	~EsParser();
	EsInterlang*		parse(EsContext* ctxRef, const VcString* strScript, u32 bflags);
	bool				parseFunctionParameters(EsContext* ctxRef, EsFunction* func, const VcString* strScript);
	bool				parseFunctionBody(EsContext* ctxRef, EsFunction* func, const VcString* strScript);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
private:
	void				addCompileError(EsError::Id id, VcStringArray* saParamDlg);
	EsUnaryItNode*		makeUnaryNode(EsTokenType tt, EsOpcodeType ot, const EsTokenPos* pos, EsItNode* nodeChild);
	EsBinaryItNode*		makeBinaryNode(EsTokenType tt, EsOpcodeType ot, const EsTokenPos* pos, EsItNode* nodeLeft, EsItNode* nodeRight);
	EsTernaryItNode*	makeTernaryNode(EsTokenType tt, EsOpcodeType ot, const EsTokenPos* pos, EsItNode* node1, EsItNode* node2, EsItNode* node3);
	EsNameItNode*		makeNameNode(EsOpcodeType ot, const EsTokenPos* pos, const EsAtom* atom, EsItNode* nodeExpr);
	EsNameItNode*		makePlaceholder(const EsAtom* atom);
	EsListItNode*		makeListNode(EsTokenType tt, EsOpcodeType ot, const EsTokenPos* pos, EsItNode* nodeListHead);
	EsNameItNode*		makeAssignmentNode(EsNameItNode* nodeName, EsItNode* nodeRight);
	EsDoubleItNode*		makeDoubleNode(const EsTokenPos* pos, f64 dval);
	EsNameItNode*		makeStringNode(EsTokenType tt, EsOpcodeType ot, const EsTokenPos* pos, const EsAtom* atom);
	EsNameItNode*		makeFuncNode(const EsTokenPos* pos);
	EsItNode*			makeNullaryNode();
	bool				matchOrInsertSemicolonToken();
	bool				mustMatchToken(EsTokenType tt, EsError::Id errid);
	bool				matchLabel(const EsAtom** atom);
	void				setLeftValueFlags(EsItNode* node, u32 defflag);
	EsItNode*			setLeftValueChild(EsItNode* nodeParent, EsItNode* nodeChild);
	EsOpcodeType		getIncrementOpcodeType(EsTokenType tt, EsItNode* nodeChild, bool isPreOrder);
	bool				setIncrementChild(EsItNode* nodeParent, EsItNode* nodeChild, bool isPreOrder);
	bool				bindVarOrConstData(EsNameItNode* node, EsOpcodeType otBind, bool* isFreshBind);
	bool				bindLetData(EsNameItNode* node, EsOpcodeType otBind, bool* isFreshBind);
	bool				rebindLetData(EsItNode* node);
	void				setLexDefNodeLink(EsNameItNode* node, EsNameItNode* nodeDef);
	void				unsetLexDefNodeLink(EsNameItNode* node);
	bool				setSetCall(EsItNode* node);
	bool				defineDeclaredName(EsNameItNode* node, const EsAtom* atom, bool isLet);
	bool				defineArg(EsNameItNode* nodeFunc, const EsAtom* atom, s32 indexArg);
	bool				defineFuncIntoLexDefNode(EsNameItNode* nodeDef, EsNameItNode* nodeFuncName, const EsAtom* atom);
	EsFunctionBox*		enterFunction(EsNameItNode* nodeFunc, EsBytecodeBuilder* bcbFunc, const EsAtom* atomFunc, bool isLambda);
	bool				leaveFunction(EsNameItNode* nodeFunc, EsBytecodeBuilder* bcbFunc, const EsAtom* atomFunc, bool isLambda);
	EsFunction*			makeFunction(const EsAtom* atomFunc, bool isLambda);
	EsFunctionBox*		makeFunctionBox(EsFunction* objFunc, EsNameItNode* nodeFunc);
	EsObjectBox*		makeObjectBox(EsObject* obj);
	EsNameItNode*		pushLexicalScope(EsStatement* stmt);
	void				setArgumentsFlags();
	EsItNode*			checkImmediatelyAppliedLambda(EsItNode* node);
	bool				checkBlockIdInScope(u32 blockid);

	bool				parseMain();
	EsItNode*			parseStatement();
	EsItNode*			parseStatementSub_if();
	EsItNode*			parseStatementSub_switch();
	EsItNode*			parseStatementSub_while();
	EsItNode*			parseStatementSub_do();
	EsItNode*			parseStatementSub_for();
	EsItNode*			parseStatementSub_break();
	EsItNode*			parseStatementSub_continue();
	EsItNode*			parseStatementSub_with();
	EsItNode*			parseStatementSub_var();
	EsItNode*			parseStatementSub_let();
	EsItNode*			parseStatementSub_return();
	EsItNode*			parseStatementSub_lc();
	EsItNode*			parseStatementSub_semi();
	EsItNode*			parseStatementSub_other();
	EsListItNode*		parseStatementBlock();
	EsItNode*			parseExpression();
	EsItNode*			parseAssignExpression();
	EsItNode*			parseFunctionDefine(bool isLambda);
	EsItNode*			parseFunctionBody();
	EsItNode*			parseCondition();
	EsItNode*			parseConditionExpression();
	EsItNode*			parseOrExpression();
	EsItNode*			parseAndExpression();
	EsItNode*			parseBitOrExpression();
	EsItNode*			parseBitXorExpression();
	EsItNode*			parseBitAndExpression();
	EsItNode*			parseEqualExpression();
	EsItNode*			parseInequalityExpression();
	EsItNode*			parseShiftExpression();
	EsItNode*			parseAddExpression();
	EsItNode*			parseMultiplyExpression();
	EsItNode*			parseUnaryExpression();
	EsItNode*			parseMemberExpression(bool isAllowCallSyntax);
	EsItNode*			parsePrimaryExpression(EsTokenType tt, bool isAfterDot);
	EsItNode*			parsePrimaryExpression_lb();
	EsItNode*			parsePrimaryExpression_lc();
	bool				parseArgumentList(EsListItNode* nodeList);
	EsListItNode*		parseVariables(bool isLetTop);
	EsItNode*			parseParenthesisExpression();
	EsItNode*			parseBracketExpression();
	EsItNode*			parseLet(bool isStatement);

	//======================================================================
	// 変数
private:
	EsContext* _ctxRef;				// コンテキストへの参照
	EsTokenParser* _tknprs;			// 現在のトークンパーサー
	EsBytecodeBuilder* _bcbCur;		// 現在のバイトコードビルダー
	u32 _cntFunc;					// 関数の数 ⇒ functionCount

	// ※解析エラーなどを保存するつもり
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_PARSER_H_
