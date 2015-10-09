/***********************************************************************//**
 *	EsStackFrame.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_STACK_FRAME_H_
#define _ES_ES_STACK_FRAME_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class BitFlagArray;

TFW_END_NS

ES_BEGIN_NS

class EsClass;
class EsContext;
class EsFunction;
class EsInterlang;
class EsObject;
class EsProgramCounter;
class EsStackRegister;
class EsValue;

/*---------------------------------------------------------------------*//**
 *	スタックフレーム
 *
**//*---------------------------------------------------------------------*/
class EsStackFrame : public EsUnit
{
	//======================================================================
	// 定数
public:
	// スタックフレーム フラグ
	static const u16 F_CONSTRUCTING		= 0x0001;	// ⇒ JSFRAME_CONSTRUCTING
	static const u16 F_COMPUTED_THIS	= 0x0002;	// ⇒ JSFRAME_COMPUTED_THIS
	static const u16 F_ASSIGNING		= 0x0004;	// ⇒ JSFRAME_ASSIGNING
	static const u16 F_DEBUGGER			= 0x0008;	// ⇒ JSFRAME_DEBUGGER
	static const u16 F_EVAL				= 0x0010;	// ⇒ JSFRAME_EVAL
	static const u16 F_ROOTED_ARGV		= 0x0020;	// ⇒ JSFRAME_ROOTED_ARGV
	static const u16 F_YIELDING			= 0x0040;	// ⇒ JSFRAME_YIELDING
	static const u16 F_ITERATOR			= 0x0080;	// ⇒ JSFRAME_ITERATOR
	static const u16 F_GENERATOR		= 0x0100;	// ⇒ JSFRAME_GENERATOR
	static const u16 F_NEWSTACK			= 0x1000;	// スタックは新規に用意され自ら削除する必要がある

	static const s32 NUM_INITIAL_SLOT	= 5;		// 初期スロット数 ⇒ JS_INITIAL_NSLOTS

	//======================================================================
	// クラス
private:
	class ArgBitFlags : public EsUnit
	{
	public:
		BitFlagArray* _bfarr;

	public:
		ArgBitFlags();
		~ArgBitFlags();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 中間言語の取得
	inline EsInterlang* getInterlang() const { return _il; }
	// スタックレジスターを得る
	inline EsStackRegister*	getStackRegister() const { return _regs; }
	// フラグを得る
	inline const u16 getFlags() const { return _flags; }
	inline u16* flags() { return &_flags; }
	// オーバーライドフラグを得る
	inline u16 getOverrideFlags() const { return _bitsOvdPropsid; }
	inline u16* overrideFlags() { return &_bitsOvdPropsid; }
	// スロットを得る
	EsValue* getSlot(s32 slotidx) const;
	// スロット数を得る
	inline u16 getSlotNum() const { return _numSlot; }
	// スコープ チェイン オブジェクトを取得する
	inline EsObject* getScopeChainObject() const { return _objScopeChain; }
	// ブロック チェイン オブジェクトを取得する
	inline EsObject* getBlockChainObject() const { return _objBlockChain; }
	// this オブジェクトを取得する
	inline EsObject* getThisObject() const { return _objThis; }
	// 変数オブジェクトを取得する
	inline EsObject* getVarObject() const { return _objVar; }
	// 関数またスクリプトオブジェクトを取得する
	inline EsObject* getCalleeObject() const { return _objCallee; }
	// 現在呼ばれている関数を取得する
	inline EsFunction* getFunction() const { return _func; }
	// 関数の呼び出しオブジェクトを得る
	inline EsObject* getCallObject() const { return _objCall; }
	// 関数の呼び出しオブジェクト値を得る
	EsValue* getCalleeValue() const;
	// this オブジェクト値を得る
	EsValue* getThisValue() const;
	// 関数引数配列を得る
	EsValue* getArgArray() const;
	// 関数引数を得る
	EsValue* getArg(s32 index) const;
	// 関数引数数を得る
	inline u32 getArgNum() const { return _numArg; }
	// 関数の引数オブジェクトを得る
	inline EsObject* getArgsObject() const { return _objArgs; }
	// 関数リターン値を得る
	inline EsValue* getFunctionResultValue() const { return _valFuncRes; }
	// 下位のスタックフレームを得る
	inline EsStackFrame* getDownStackFrame() const { return _sfrmDown; }
	// 最後に実行した演算コードの長さを得る
	inline s32 getLastOpcodeLength() const { return _lenLastOpcode; }
	// オブジェクト初期化処理の深さを得る
	inline s32 getInitDepth() const { return _depthInit; }
	// シャープ変数スコープを得る
	inline EsObject* getSharpVarScope() const { return _objSharpVar; }
	// スタック基点を得る
	EsValue* getStackTop() const;
	// 一つ前のディスプレイスタックフレームを得る
	inline EsStackFrame* getPrevDisplayStackFrame() const { return _sfrmPrevDisp; }
	// 呼び出し元スタックレジスタを返す
	inline EsStackRegister* getCallerStackRegister() const { return _regsCallerRef; }

	// var オブジェクトを設定する
	inline void setVarObject(EsObject* objVar) { _objVar = objVar; }
	// スコープ チェイン オブジェクトを設定する
	inline void setScopeChainObject(EsObject* objScopeChain) { _objScopeChain = objScopeChain; }
	// ブロック チェイン オブジェクトを設定する
	inline void setBlockChainObject(EsObject* objBlockChain) { _objBlockChain = objBlockChain; }
	// スロットに値を設定する
	bool setSlot(s32 slotidx, const EsValue* val);
	// （遅延作成された）関数引数を設定する
	bool setArg(s32 index, const EsValue* val);
	// this オブジェクトを設定する
	inline void setThisObject(EsObject* objThis) { _objThis = objThis; }
	// （遅延作成された）呼び出しオブジェクトを設定する
	inline void setCallObj(EsObject* objCall) { _objCall = objCall; }
	// 下位のスタックフレームを得る
	inline void setDownStackFrame(EsStackFrame* sfrmDown) { _sfrmDown = sfrmDown; }
	// 最後に実行した演算コードの長さを設定する
	inline void setLastOpcodeLength(s32 lenOpcode) { _lenLastOpcode = lenOpcode; }
	// オブジェクト初期化処理の深さを設定する
	inline void setInitDepth(s32 depth) { _depthInit = depth; }
	// シャープ変数スコープを設定する
	inline void setSharpVarScope(EsObject* obj) { _objSharpVar = obj; }
	// 一つ前のディスプレイスタックフレームを設定
	inline void setPrevDisplayStackFrame(EsStackFrame* sfrm) { _sfrmPrevDisp = sfrm; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool prepareForRun(EsContext* ctx, EsInterlang* il, EsObject* objScope, EsStackFrame* sfrmDown, u16 flags);
	bool prepareForCall(EsContext* ctx, EsFunction* func, EsInterlang* il, u16 flags, EsValue* vaStack, s32 numArg, EsObject* objCallee, EsObject* objThis, EsObject* objScopeChain, EsStackFrame* sfrmCaller);
	bool prepareForInvoke(EsContext* ctx, EsFunction* func, EsInterlang* il, s32 numSlot, u16 flags, EsValue* vaStack, s32 numArg, EsObject* objCallee, EsObject* objThis, EsObject* objScopeChain, EsStackFrame* sfrmCaller);
	bool postprocForInvoke(EsContext* ctx, bool isSucceededCall);

	EsObject* makeScopeChainObject(EsContext* ctx);
	EsObject* makeCallObject(EsContext* ctx);
	EsObject* makeArgsObject(EsContext* ctx);
	bool putCallObject(EsContext* ctx);
	bool getArgsProperty(EsValue* val, EsContext* ctx, const EsValue* spropid);
	bool getArgsValue(EsValue* val, EsContext* ctx);
	bool putArgsObject(EsContext* ctx);
	bool enterWithBlock(EsContext* ctx, s32 indexStack);
	void leaveWithBlock(EsContext* ctx);
	const EsClass* getActiveBlockClass(EsContext* ctx, const EsObject* obj, s32 depthStack);
	bool unwindScope(EsContext* ctx, s32 depthStack, bool isNormalUnwind);
	bool putBlockObject(EsContext* ctx, bool isNormalUnwind);
	bool isArgDeleted(EsContext* ctx, u16 slotidx);
	bool markArgDeleted(EsContext* ctx, u16 slotidx);

	static u16 convPropsidToBits(s16 propsid);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsStackFrame();
	~EsStackFrame();

	EsValue* makeSlotArray(EsContext* ctx, s32 numFuncArgs, s32 numFuncVars, s32 numSlot, EsValue* vaStack, s32 numActualArg);

	//======================================================================
	// 変数
private:
	// 作業変種
	EsInterlang*		_il;				// 中間言語 ⇒ script
	EsProgramCounter*	_pc;				// プログラムカウンタ
	EsStackRegister*	_regs;				// スタックレジスタ ⇒ regs
	u16					_flags;				// フラグ ⇒ flags
	u16					_bitsOvdPropsid;	// オーバーライドフラグ ⇒ JSFRAME_OVERRIDE_SHIFT, JSFRAME_OVERRIDE_BITS
	u16					_numSlot;			// スロットの数
	EsValue*			_varrSlots;			// スロット配列 ⇒ slots
	EsValue*			_varrSlotsAlloc;	// 自らで確保したスロット配列
	EsObject*			_objScopeChain;		// スコープ チェイン オブジェクト ⇒ scopeChain
	EsObject*			_objBlockChain;		// ブロック チェイン オブジェクト ⇒ blockChain
	EsObject*			_objThis;			// メソッド内の this ポインタ ⇒ thisp
	EsObject*			_objVar;			// 変数オブジェクト ⇒ varobj
	EsObject*			_objCallee;			// 関数またはスクリプトオブジェクト ⇒ callee
	EsFunction*			_func;				// 関数 ⇒ fun
	EsObject*			_objCall;			// 関数の呼び出しオブジェクト ⇒ callobj
	EsValue*			_valarrStack;		// 関数の引数などのスタック ⇒ argv - 2
	EsValue*			_valarrStackOrg;	// 外部から与えられたオリジナルのスタック
	EsValue*			_valarrStackAlloc;	// 外部で確保したスタック
	u32					_numArg;			// 関数の引数の数 ⇒ argc
	EsObject*			_objArgs;			// 関数の引数オブジェクト ⇒ argsobj
	EsValue*			_valFuncRes;		// 関数の戻り値 ⇒ rval
	EsStackFrame*		_sfrmDown;			// 上位ブロックのスタックフレーム ⇒ down
	s32					_lenLastOpcode;		// 最終実行演算コードの長さ ⇒ js_Interpret 内の len
	s32					_depthInit;			// オブジェクト初期化処理の深さ ⇒ sharpDepth
	EsObject*			_objSharpVar;		// シャープ変数スコープ ⇒ sharpArray
	EsStackFrame*		_sfrmPrevDisp;		// 一つ前のディスプレイスタックフレーム ⇒ displaySave
	// ※※※ 実装なし ※※※ ⇒ imacro

	// 関数呼び出し時の作業変数 ⇒ JSInlineFrame
	EsStackRegister*	_regsCallerRef;		// 呼び出し元のスタックレジスタへのポインタ
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_STACK_FRAME_H_
