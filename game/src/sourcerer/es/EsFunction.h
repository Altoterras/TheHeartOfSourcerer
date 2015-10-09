/***********************************************************************//**
 *	EsFunction.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_FUNCTION_H_
#define _ES_ES_FUNCTION_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsClass.h"
#include "EsPrototype.h"
#include "EsObject.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

ES_BEGIN_NS

class EsAtom;
class EsAtomList;
class EsClass;
class EsInterlang;
class EsStackFrame;

/*---------------------------------------------------------------------*//**
 *	関数
 *
**//*---------------------------------------------------------------------*/
class EsFunction : public EsObject
{
	//======================================================================
	// 定数
public:
	// 局所変数の種類 ⇒ JSLocalKind
	enum LocalVarKind
	{
		LVK_NULL,		// ⇒ JSLOCAL_NONE
		LVK_ARG,		// 引数 ⇒ nargs
		LVK_VAR,		// 関数内変数 ⇒ nvars
		LVK_CONST,		// 定数
		LVK_UPVAR,		// 上位ブロックの変数
		NUM_LVK,
	};

	// フラグ
	static const u32 F_KIND_INTERPRETED		= 0x00000001;	// スクリプト関数である ⇒ JSFUN_INTERPRETED
	static const u32 F_KIND_NATIVE			= 0x00000002;	// ネイティブ関数である
	static const u32 F_CLOSURE_NULL			= 0x00000010;	// （未調査）⇒ JSFUN_NULL_CLOSURE
	static const u32 F_CLOSURE_FLAT			= 0x00000020;	// （未調査）⇒ JSFUN_FLAT_CLOSURE
	static const u32 F_LAMBDA				= 0x00000100;	// ラムダ関数 ⇒ JSFUN_LAMBDA
	static const u32 F_FUNC_NEED_CALLOBJ	= 0x00000200;	// コールオブジェクトが必要な関数 ⇒ JSFUN_HEAVYWEIGHT
	static const u32 F_OBJ_GETTER			= 0x00001000;	// ゲッターを持つ ⇒ JSFUN_GETTER
	static const u32 F_OBJ_SETTER			= 0x00002000;	// セッターを持つ ⇒ JSFUN_SETTER
	static const u32 F_THISP_STRING			= 0x00010000;	// this ポインタはおそらく文字列である ⇒ JSFUN_THISP_STRING
	static const u32 F_THISP_NUMBER			= 0x00020000;	// this ポインタはおそらく値である ⇒ JSFUN_THISP_NUMBER
	static const u32 F_THISP_BOOLEAN		= 0x00040000;	// this ポインタはおそらくブーリアン値である ⇒ JSFUN_THISP_BOOLEAN
	static const u32 F_THISP_PRIMITIVE		= 0x00080000;	// this ポインタはおそらくいずれかのプリミティブ型である ⇒ JSFUN_THISP_PRIMITIVE
	static const u32 F_BOUND_METHOD			= 0x00100000;	// ⇒ JSFUN_BOUND_METHOD
	// ※※※ 未対応 ※※※ static const u32 F_FAST_NATIVE			= 0x00200000;	// 高速呼び出し ⇒ JSFUN_FAST_NATIVE

	// アクセサ ショート ID
	static const s16 PROPSID_CALL_ARGUMENTS	= -1;			// 事前定義されたローカル変数の引数 ⇒ CALL_ARGUMENTS
	static const s16 PROPSID_ARGS_LENGTH	= -2;			// 実引数の数 ⇒ ARGS_LENGTH
	static const s16 PROPSID_ARGS_CALLEE	= -3;			// 引数から関数オブジェクトへの参照 ⇒ ARGS_CALLEE
	static const s16 PROPSID_FUNC_ARITY		= -4;			// 仮引数の数 ⇒ FUN_ARITY
	static const s16 PROPSID_FUNC_NAME		= -5;			// 関数名（anonymous は ""） ⇒ FUN_NAME
	static const s16 PROPSID_FUNC_CALLER	= -6;			// function.prototype.caller ⇒ FUN_CALLER

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	関数プロトタイプ クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsFunctionPrototype : public EsPrototype
	{
		//==================================================================
		// メソッド
	public:
		bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const;
		static const EsFunctionPrototype* getInstance() { static EsFunctionPrototype instance; return &instance; }

	private:
		EsFunctionPrototype();
	};

	/*-----------------------------------------------------------------*//**
	 *	関数スクリプト クラス ⇒ js_FunctionClass
	 *
	**//*-----------------------------------------------------------------*/
	class EsFunctionClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		// 呼び出し関数スロットのインデックスを得る ⇒ JSSLOT_CALLEE
		inline u16 getCalleeSlotIndex() const { return getPrivateSlotIndex() + 1; }
		// 関数引数スロットのインデックスを得る ⇒ JSSLOT_CALL_ARGUMENTS
		inline u16 getCallArgumentsSlotIndex() const { return getPrivateSlotIndex() + 2; }
		// ダイナミックスロットの開始インデックスを得る
		virtual u16 getStartDynamicSlotIndex() const { return getPrivateSlotIndex() + 3; }

		bool getAdditionalReservedSlotNum(u16* slotnumOut, EsObject* obj) const;
		static const EsFunctionClass* getInstance() { static EsFunctionClass instance; return &instance; }
		static EsObject* makeClassObject(EsContext* ctx, EsObject* objParent); 

	private:
		EsFunctionClass();
		static bool callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
		static bool callEnumerate(EsContext* ctx, EsObject* obj);
		static bool callResolve(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv);
		static bool callConvert(EsValue* val, EsContext* ctx, EsObject* obj, EsLogicalType ltype);
		static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callApply(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callCall(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	};

private:
	class LocalVar
	{
	public:
		const EsAtom* _atom;
	public:
		LocalVar(const EsAtom* atom) : _atom(atom) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// フラグを得る
	inline u32 getFlags() const { return _flags; }
	inline u32* flags() { return &_flags; }
	// アトムを得る
	inline const EsAtom* getAtom() const { return _atomRef; }

	// スクリプト関数であるかを得る
	inline bool isInterpreted() const { return TFW_IS_FLAG(_flags, F_KIND_INTERPRETED); }
	// 引数の数を得る ⇒ nvars <- getLocalVarNum(LVK_VAR), nupvars <- getLocalVarNum(LVK_UPVAR), nargs <- getLocalVarNum(LVK_ARG)
	u32 getLocalVarNum(LocalVarKind lvk) const;
	// 生成済みの中間言語を得る
	inline EsInterlang* getInterlang() const { return _ilRef; }
	// スキップレベルを得る
	inline u16 getSkipDepth() const { return _depthSkipMin; }
	// ラッパークロージャが必要かどうかを得る ⇒ needsWrapper
	inline bool isNeedsWrapperClosure() const { return TFW_IS_FLAG(_flags, F_CLOSURE_NULL) && (_depthSkipMin != 0); }

	// 生成済みの中間言語を設定する
	inline void setInterlang(EsInterlang* il) { _ilRef = il; }
	// スキップレベルを設定する
	inline void setSkipDepth(u16 depthSkipMin) { _depthSkipMin = depthSkipMin; }

	// ネイティブ呼び出し関数を得る
	inline EsNativeCall getNativeCall() const { return _call; }
	// コンストラクタクラスを得る
	inline const EsClass* getConstructClass() const { return _classCtor; }
	// 定義パラメータを得る
	inline EsExtParam getDefParam() const { return _paramFuncDef; }

	// クラスを設定する
	inline void setConstructClass(const EsClass* cls) { _classCtor = cls; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool addLocalVar(const EsAtom* atomVar, LocalVarKind lvk);
	bool findLocalVar(LocalVarKind* lvkOut, u32* indexOut, const EsAtom* atomVar);
	bool enumLocalAtoms(EsAtomList* alist, LocalVarKind lvk);
	bool testPrimitiveThisValue(const EsValue* valThis) const;
	EsFunction* makeFlatClosure(EsContext* ctx, EsObject* objParent);
	static bool checkEscapingClosure(EsContext* ctx, EsObject* obj, EsValue* val);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsFunction();
	virtual ~EsFunction();
	bool createAsInterpreted(EsContext* ctx, const EsAtom* atomRef, EsObject* objParent, bool isLambda);
	bool createAsNative(EsContext* ctx, const EsAtom* atomRef, EsObject* objParent, EsNativeCall call, EsExtParam paramDef);
	EsFunction* clone(EsContext* ctx, EsObject* objParent);

private:
	EsObject* makeWrappedEscapingClosure(EsContext* ctx, EsStackFrame* sfrm, EsObject* objFunc);

	//======================================================================
	// 変数
private:
	u32 _flags;
	const EsAtom* _atomRef;

	// スクリプト関数用変数 ⇒ JSFunction::u.i
	List<LocalVar*>* _listLocalVar[NUM_LVK];	// ⇒ names
	EsInterlang* _ilRef;						// 関数の中間言語 ⇒ script
	u16 _depthSkipMin;							// ⇒ skipmin
	bool _isWrapper;							// ⇒ wrapper

	// ネイティブ関数用変数 ⇒ JSFunction::u.n
	EsNativeCall _call;							// ⇒ native
	const EsClass* _classCtor;					// ⇒ clasp
	EsExtParam _paramFuncDef;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_FUNCTION_H_
