/***********************************************************************//**
 *	EsBytecodeBuilder.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_BYTE_CODE_BUILDER_H_
#define _ES_ES_BYTE_CODE_BUILDER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsError.h"
#include "EsOpcode.h"
#include "EsUpvarInfo.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

ES_BEGIN_NS

class EsAtom;
class EsAtomList;
class EsFunction;
class EsFunctionBox;
class EsItNode;
class EsNameItNode;
class EsObjectBox;
class EsStackFrame;
class EsStatement;

/*---------------------------------------------------------------------*//**
 *	バイトコード生成
 *		⇒ JSTreeContext + JSCodeGenerator
 *
**//*---------------------------------------------------------------------*/
class EsBytecodeBuilder
{
	//======================================================================
	// 定数
public:
	// 生成フラグ
	static const u32 BF_IN_FUNCTION				= 0x00000001;	// 関数内生成中 ⇒ TCF_IN_FUNCTION
	static const u32 BF_DONOT_WANT_RET_VAL		= 0x00000002;	// 返り値を求められていない ⇒ TCF_NO_SCRIPT_RVAL
	static const u32 BF_OPT_NAME_REF_ON_SCOPE	= 0x00000004;	// スコープチェイン上の名前参照を最適化する ⇒ TCF_COMPILE_N_GO
	static const u32 BF_AMONG_FOR_INIT			= 0x00000008;	// for 文内の初期化解析．ただし、for(xxx in xxx) を除く	⇒ TCF_IN_FOR_INIT
	static const u32 BF_CONTAIN_FUNC_STMT		= 0x00000010;	// function 文を含むブロックステートメント	⇒ TCF_HAS_FUNCTION_STMT
	static const u32 BF_RET_VOID				= 0x00000020;	// 関数は return; を含んでいる ⇒ TCF_RETURN_VOID
	static const u32 BF_RET_EXPR				= 0x00000040;	// 関数は return expr; を含んでいる ⇒ TCF_RETURN_EXPR
	static const u32 BF_IN_FOR_INIT				= 0x00000080;	// for 文の初期化部分である ⇒ TCF_IN_FOR_INIT
	static const u32 BF_GENEXP_LAMBDA			= 0x00000100;	// ラムダフラグ ⇒ TCF_GENEXP_LAMBDA
	static const u32 BF_FUNC_NEED_CALLOBJ		= 0x00010000;	// コールオブジェクトが必要な関数 ⇒ TCF_FUN_HEAVYWEIGHT
	static const u32 BF_FUNC_USES_ARGUMENTS		= 0x00020000;	// 関数は（パラメータ名を除いた）引数を使用する ⇒ TCF_FUN_USES_ARGUMENTS
	static const u32 BF_FUNC_SETS_OUTER_NAME	= 0x00040000;	// 関数は外部名が設定される ⇒ TCF_FUN_SETS_OUTER_NAME
	static const u32 BF_FUNC_IN_GENERATOR		= 0x00080000;	// 関数内の yield 文である ⇒ TCF_FUN_IS_GENERATOR

	static const u32 BF_FUNC_FLAGS	= BF_FUNC_NEED_CALLOBJ | BF_FUNC_USES_ARGUMENTS | BF_FUNC_SETS_OUTER_NAME | BF_FUNC_IN_GENERATOR;	// ⇒ TCF_FUN_FLAGS
	static const u32 BF_RET_FLAGS	= (BF_RET_VOID | BF_RET_EXPR);

public:
	static const u32 SIZE_JUMP_OFFSET_BC		= 2;			// ジャンプ オフセット値のバイトコード サイズ ⇒ JUMP_OFFSET_LEN
	static const u32 SIZE_JUMPX_OFFSET_BC		= 4;			// 拡張ジャンプ オフセット値のバイトコード サイズ ⇒ JUMPX_OFFSET_LEN
	static const u32 SIZE_INDEX_BC				= 2;			// インデックス値のバイトコード サイズ ⇒ INDEX_LEN

	static const s32 MAX_SLOTIDX				= 0xffffff;		// 最大スロットインデックス ⇒ SLOTNO_LIMIT

	//======================================================================
	// クラス
private:
	class Bytecode
	{
	public:
		const VcString* getCode() const { return _code; }
		VcString* code() { return _code; }
		s32 getOffset() const { return _code->getLength(); }

	public:
		Bytecode();
		~Bytecode();

	private:
		VcString* _code;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 親のバイトコードビルダーを取得する
	inline EsBytecodeBuilder* getParent() const { return _parent; }
	// 宣言済語彙アトムリストを取得する
	inline EsAtomList* getDeclaredNameAtomList() { return _alistDecl; }
	// 未解決語彙アトムリストを取得する
	inline EsAtomList* getUnresolvedNameAtomList() { return _alistUnrslv; }
	// 先頭のスコープのステートメント情報を取得する ⇒ topScopeStmt
	inline EsStatement* topScopeStatement() { return _stmtTopScope; }
	// 先頭の閉じたステートメント情報を取得する ⇒ topStmt
	inline EsStatement* topEnclosingStatement() { return _stmtTopEncl; }
	// スコープ チェイン オブジェクトを取得する
	inline EsObject* getScopeChainObject() const { return _objScopeChain; }
	// ブロック チェイン オブジェクトを取得する
	inline EsObject* getBlockChainObject() const { return _objBlockChain; }
	// 最大スタック深度を取得する
	inline s32 getMaxStackDepth() { return _depthStackMax; }
	// ネスト深度を得る
	inline u32 getNestDepth() const { return _depthNest; }
	// リテラルアトムリストを取得する
	inline const EsAtomList* getLiteralAtomList() const { return _alistLiteral; }
	inline EsAtomList* literalAtomList() const { return _alistLiteral; }
	// 最大大域変数数を得る
	inline s32 getGlobalVarsNum() const { return _numGlobalVars; }
	// 拡張情報付加を行うかを得る
	inline bool isExInfoAdd() const { return _isAddExInfo; }
	// 生成フラグを得る
	inline u32 getBuildFlag() const { return _bflags; }
	inline u32* buildFlag() { return &_bflags; }
	// ボディのブロック ID を得る
	inline u32 getBodyBlockId() const { return _blockidCur; }
	// ブロック ID を得る
	u32 getCurrentBlockId() const;
	// ブロックノードを得る
	inline EsItNode* getCurrentBlockNode() const { return _nodeBlockCur; }
	// 処理中の関数の取得
	inline EsFunction* getCurrentFunction() const { return _funcCur; }
	// 処理中の関数ボックスの取得
	inline EsFunctionBox* getCurrentFunctionBox() const { return _funcboxCur; }
	// 関数ボックスツリーの取得
	inline EsFunctionBox* getFunctionBoxTree() const { return _funcboxTree; }
	// メインエントリポイントのバイトコードを取得する
	inline const VcString* getMainBytecode() const { return _bcMain.getCode(); }
	// メイン外のバイトコードを取得する
	inline const VcString* getOtherBytecode() const { return _bcProg.getCode(); }
	// 生成オブジェクトボックスリストを取得する
	inline const List<EsObjectBox*>* getObjectBoxList() const { return _listObjbox; }
	// 呼び出し元のスタックフレームを取得する ⇒ callerFrame
	inline EsStackFrame* getCallerStackFrame() const { return _sfrmCaller; }

	// 親のバイトコードビルダーを設定する
	inline void setParent(EsBytecodeBuilder* bcbParent) { _parent = bcbParent; }
	// 先頭のスコープのステートメント情報を設定する ⇒ topScopeStmt
	inline void setTopScopeStatement(EsStatement* stmt) { _stmtTopScope = stmt; }
	// 先頭の閉じたステートメント情報を設定する ⇒ topStmt
	inline void setTopEnclosingStatement(EsStatement* stmt) { _stmtTopEncl = stmt; }
	// スコープ チェイン オブジェクトを設定する
	inline void setScopeChainObject(EsObject* obj) { _objScopeChain = obj; }
	// ブロック チェイン オブジェクトを設定する
	inline void setBlockChainObject(EsObject* obj) { _objBlockChain = obj; }
	// 拡張情報付加を行うかを設定する
	inline void setExInfoAdd(bool isAdd) { _isAddExInfo = isAdd; }
	// 最大大域変数数を設定する
	inline void setGlobalVarsNum(s32 num) { _numGlobalVars = num; }
	// ネスト深度を設定する
	inline void setNestDepth(u32 depth) { _depthNest = depth; }
	// 現在のブロック ID を設定する
	inline void setCurrentBlockId(u32 blockid) { _blockidCur = blockid; }
	// 現在のブロックノードを設定する
	inline void setCurrentBlockNode(EsItNode* node) { _nodeBlockCur = node; }
	// 処理中の関数の設定
	inline void setCurrentFunction(EsFunction* func) { _funcCur = func; }
	// 処理中の関数ボックスの設定
	inline void setCurrentFunctionBox(EsFunctionBox* funcbox) { _funcboxCur = funcbox; }
	// 関数ボックスツリーの設定
	inline void setFunctionBoxTree(EsFunctionBox* funcbox) { _funcboxTree = funcbox; }

	// トップレベルステートメントかどうかを得る ⇒ atTopLevel
	bool isTopLevelStatement() const;

private:
	// 編集用のバイトコードポインタを得る ⇒ CG_BASE(cg)
	inline VcString* bc() { return _bcCur->code(); }
	// 編集用のバイトコードオフセットを得る ⇒ CG_OFFSET(cg)
	inline s32 bcoffset() const { return _bcCur->getOffset(); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool buildFromTree(EsItNode* nodeT);
	bool buildFunctionBody(EsItNode* nodeFunc);
	bool finalize();

	bool findLexical(EsStatement** stmtOut, s32* slotidxOut, const EsAtom* atom, EsStatement* stmtStart);
	void pushStatement(EsStatement* stmt, s32 bcofsTop);
	void popStatement(bool isDeleteScopeDecl);
	inline void popStatement() { popStatement(true); }
	bool isInStatement(s32 statementtype);
	void pushBlockScope(EsStatement* stmt, EsObject* objBlock, s32 bcofsTop);
	u32 makeBlockId();

	void inheritForChildBlockParseStart(EsBytecodeBuilder* parent);
	void inheritForChildBlockParseEnd(EsBytecodeBuilder* child);

	void inheritForChildBlockBuildStart(EsBytecodeBuilder* parent, EsFunctionBox* funcbox);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsBytecodeBuilder(EsContext* ctxRef);
	~EsBytecodeBuilder();

private:
	bool buildSub_function(EsItNode* nodeT);
	bool buildSub_functionNop();
	bool buildSub_lexicalscope(EsItNode* nodeT);
	bool buildSub_lp(EsItNode* nodeT);
	bool buildSub_rp(EsItNode* nodeT);
	bool buildSub_lc(EsItNode* nodeT);
	bool buildSub_rc(EsItNode* nodeT);
	bool buildSub_lb(EsItNode* nodeT);
	bool buildSub_rb(EsItNode* nodeT);
	bool buildSub_seq(EsItNode* nodeT);
	bool buildSub_semi(EsItNode* nodeT);
	bool buildSub_var(EsItNode* nodeT, bool isInLetHead);
	bool buildSub_let(EsItNode* nodeT);
	bool buildSub_upvars(EsItNode* nodeT);
	bool buildSub_return(EsItNode* nodeT);
	bool buildSub_number(EsItNode* nodeT);
	bool buildSub_name(EsItNode* nodeT);
	bool buildSub_comma(EsItNode* nodeT);
	bool buildSub_assign(EsItNode* nodeT);
	bool buildSub_dot(EsItNode* nodeT);
	bool buildSub_unaryop(EsItNode* nodeT);
	bool buildSub_incdec(EsItNode* nodeT);
	bool buildSub_andor(EsItNode* nodeT);
	bool buildSub_binary(EsItNode* nodeT);
	bool buildSub_while(EsItNode* nodeT);
	bool buildSub_do(EsItNode* nodeT);
	bool buildSub_for(EsItNode* nodeT);
	bool buildSub_if(EsItNode* nodeT);
	bool buildSub_hook(EsItNode* nodeT);
	bool buildSub_switch(EsItNode* nodeT);
	bool buildSub_break(EsItNode* nodeT);
	bool buildSub_continue(EsItNode* nodeT);
	bool buildSub_colon(EsItNode* nodeT);
	bool buildSub_with(EsItNode* nodeT);
	bool buildSub_argsbody(EsItNode* nodeT);
	bool buildSub_primary(EsItNode* nodeT);

	bool append1Bc(EsOpcodeType ot, s32* bcofsLastOp);
	bool append2Bc(EsOpcodeType ot, u8 bc1, s32* bcofsLastOp);
	bool append3Bc(EsOpcodeType ot, u8 bc1, u8 bc2, s32* bcofsLastOp);
	bool append4Bc(EsOpcodeType ot, u8 bc1, u8 bc2, u8 bc3, s32* bcofsLastOp);
	bool append5Bc(EsOpcodeType ot, u8 bc1, u8 bc2, u8 bc3, u8 bc4, s32* bcofsLastOp);
	bool appendManyBc(EsOpcodeType ot, u8* bcarr, s32 num, s32* bcofsLastOp);
	bool appendExInfoBc(EsItNode* node, s32* bcofsLastOp);

	bool putupIndex(EsOpcodeType ot, u32 index, EsItNode* nodeInfo, s32* bcofsLastOp);
	bool putupSlotIndex(EsOpcodeType ot, u32 slotidx, u32 index, EsItNode* nodeInfo, s32* bcofsLastOp);
	bool putupName(EsNameItNode* node, bool isCallO, s32* bcofsLastOp);
	bool putupAtom(EsNameItNode* node, EsOpcodeType ot, s32* bcofsLastOp);
	bool putupObject(EsObjectBox* objbox, EsOpcodeType ot, EsItNode* nodeInfo, s32* bcofsLastOp);
	bool putupVarDecl(EsNameItNode* node, EsOpcodeType ot, s32* bcofsLastOp, u32* atomidxOut);
	bool putupJump(EsOpcodeType ot, s32 bcofsJump, EsItNode* nodeInfo, s32* bcofsLastOp);
	bool putupNonLocalJump(EsStatement* stmtTo, EsItNode* nodeInfo, s32* bcofsLastOp);
	bool putupGoto(EsStatement* stmtTo, s32* bcofsLastJump, EsItNode* nodeInfo, s32* bcofsLastOp);
	bool putupBackPatch(EsOpcodeType ot, s32* bcofsLastJump, EsItNode* nodeInfo, s32* bcofsLastOp);
	bool putupNumber(f64 dval, EsItNode* nodeInfo, s32* bcofsLastOp);
	bool putupProperty(EsNameItNode* node, EsOpcodeType ot, bool isCallContext, s32* bcofsLastOp);
	bool putupElement(EsItNode* node, EsOpcodeType ot, s32* bcofsLastOp);
	bool putupPopsFlush(s32 popnum, EsItNode* nodeInfo, s32* bcofsLastOp);
	bool putupEnterBlock(EsItNode* node, s32* bcofsLastOp);
	bool putupStop();

private:
	void updateStackDepth(EsOpcodeType ot, u32 bcofs);
	bool bindNameAndSlot(EsNameItNode* node);
	bool changeUpvarForEvalNameNode(EsNameItNode* node);
	bool decideUsefulSideNodeOperation(bool* isUseful, EsItNode* nodeT);
	bool writeJumpOffsetLater(s32 bcosfJumpBcPos, s32 bcofsJump);
	bool writeJumpOffsetLater16(s32 bcosfJumpBcPos, u16 bcofsJump);
	bool writeIndexLater16(s32 bcosfJumpBcPos, u16 index);
	s32 adjustBlockSlotIndex(s32 slotidx);
	bool findConstant(EsValue* val, const EsAtom* atom);
	bool defineConstant(const EsAtom* atom, EsItNode* node);
	void popStatementAndWriteJumpOffset();
	bool writeBackPathLater(s32 bcofsLast, s32 bcofsTrg, EsOpcodeType ot);

	void switchToMain();
	void switchToProg();

	void addCompileError(EsError::Id id, VcStringArray* saParamDlg);

	//======================================================================
	// 変数
private:
	EsContext* _ctxRef;								// コンテキストへの参照
	EsBytecodeBuilder* _parent;						// 親のバイトコードビルダー
	EsAtomList* _alistDecl;							// 宣言済語彙アトムリスト ⇒ decls
	EsAtomList* _alistUnrslv;						// 未解決語彙語彙アトムリスト ⇒ lexdeps
	EsStatement* _stmtTopEncl;						// 先頭の閉じたステートメントの情報 ⇒ topStmt
	EsStatement* _stmtTopScope;						// 先頭のスコープのステートメント情報 ⇒ topScopeStmt
	EsObject* _objScopeChain;						// スコープ チェイン オブジェクト ⇒ scopeChain
	EsObject* _objBlockChain;						// ブロック チェイン オブジェクト ⇒ blockChain

	bool _isAddExInfo;								// デバッグなどの拡張情報を追加するかどうか
	u32 _bflags;									// 生成フラグ
	Bytecode* _bcCur;								// 現在生成中のバイトコード ⇒ current
	EsAtomList* _alistLiteral;						// リテラルアトムリスト ⇒ atomList
	EsAtomList* _alistConst;						// 定数アトムリスト ⇒ constList
	EsAtomList* _alistUpvar;						// 上域変数アトムリスト ⇒ upvarList
	InsArray<EsUpvarInfo>* _arrUpvarMap;				// 上域変数情報配列 ⇒ upvarMap
	s32 _depthStack;								// スタックの深さ ⇒ stackDepth
	s32 _depthStackMax;								// スタックの最大深さ ⇒ maxStackDepth
	s32 _numGlobalVars;								// 大域変数の最大数 ⇒ ngvars
	u32 _depthNest;									// ブロックの深さ ⇒ staticLevel
	
	u32 _blockidGenCnt;								// ブロック ID 生成カウンタ ⇒ blockidGen
	u32 _blockidCur;								// 現在のブロック ID ⇒ bodyid
	EsItNode* _nodeBlockCur;						// 現在のブロックノード ⇒ blockNode

	Bytecode _bcMain;								// メインエントリーポイントのバイトコード ⇒ main
	Bytecode _bcProg;								// メイン外のバイトコード ⇒ prolog

	EsFunction* _funcCur;							// 現在処理中の関数 ⇒ fun
	EsFunctionBox* _funcboxCur;						// 現在処理中の関数ボックス ⇒ funbox
	EsFunctionBox* _funcboxTree;					// 現在の関数ボックスツリー要素 ⇒ functionList
	List<EsObjectBox*>* _listObjbox;				// 生成オブジェクトボックスリスト ⇒ objectList

	EsStackFrame* _sfrmCaller;						// 呼び出し元のスタックフレーム ⇒ callerFrame
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_BYTE_CODE_BUILDER_H_
