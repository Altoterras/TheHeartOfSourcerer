/***********************************************************************//**
 *	EsContext.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_CONTEXT_H_
#define _ES_ES_CONTEXT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsExtParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class HashTable;
template<class TYPE> class Array;
template<class TYPE> class List;

TFW_END_NS

ES_BEGIN_NS

class EsArray;
class EsAtom;
class EsAtomList;
class EsClass;
class EsError;
class EsExternalExtension;
class EsFunction;
class EsInterlang;
class EsIterator;
class EsItNode;
class EsItNodeFactory;
class EsKeywordSet;
class EsObject;
class EsResolver;
class EsRunEnv;
class EsStackFrame;
class EsUnit;
class EsValue;

/*---------------------------------------------------------------------*//**
 *	ES コンテキスト
 *		※	Es クラスと EsContext クラスの関係は、
 *			Es クラスがヴァーチャルマシン（VM）だとすると、
 *			EsContext はその上で走るプロセス、といったところか…。
 *			Es 環境上で、EsContext を切り替えると、
 *			別々のプログラムがそれぞれの環境（コンテキスト）で走る、
 *			というイメージである。
 *
**//*---------------------------------------------------------------------*/
class EsContext
{
	//======================================================================
	// 定数
public:
	static const s32 SIZE_SF_DISPLAY	= 16;	// スタックフレームディスプレイのサイズ ⇒ JS_DISPLAY_SIZE

	//======================================================================
	// 定数
private:
	class RunStack
	{
	public:
		RunStack(EsRunEnv* runenv);
		~RunStack();
		void clear();

	public:
		RunStack* _rsUp;
		EsRunEnv* _runenv;
		EsInterlang* _il;
		EsStackFrame* _sfrm;	// 現在のスタックフレーム ⇒ cx->fp
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// スクリプトコードの取得
	inline const VcString* getScriptCode() const { return _code; }
	// グローバルオブジェクトの取得
	inline EsObject* getGlobalObject() const { return _objGlobalRef; }
	// 動作モードフラグの取得
	inline u32 getBehaviorModeFlags() const { return _mflags; }

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// 解析・コンパイル アクセサ
public:
	// キーワードセットの取得
	inline const EsKeywordSet* getKeywordSet() const { return _kwsetRef; }
	// ノードファクトリの取得
	inline EsItNodeFactory* getItNodeFactory() const { return _nodefct; }
	// エラー処理の取得
	inline EsError* getError() const { return _err; }
	// 文字列アトムハッシュテーブルの取得
	inline HashTable* getStringAtomsHashTable() const { return _htblStringAtoms; }
	// リゾルバの取得
	inline EsResolver* getResolver() const { return _resolv; }

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// 実行環境 アクセサ
public:
	// 実行環境の取得
	inline EsRunEnv* getRunEnv() const { return _runenvCur; }
	// 中間言語の取得
	inline EsInterlang* getInterlang() const { return (_rsCur != 0L) ? _rsCur->_il : 0L; }
	// スタックフレームの取得 ⇒ js_GetTopStackFrame, cx->fp
	inline EsStackFrame* getTopStackFrame() const { return (_rsCur != 0L) ? _rsCur->_sfrm : 0L; }
	// ディスプレイスタックフレームの取得
	EsStackFrame* getDisplayStackFrame(s32 depth) const;
	// 実行時汎用パラメータを取得
	inline EsExtParam getRunParameter() const { return _paramRun; }
	// セカンダリ返答値を得る
	inline EsValue* get2ndRetValue() const { return _valRet2nd; }
	// セカンダリ返答値がセットされているかを得る
	inline bool is2ndRetValueSet() const { return _isRet2ndValSet; }
	// 停止しているかどうかを取得
	inline bool isStop() const { return _isStop; }

	// スタックフレームを設定する
	inline void setTopStackFrame(EsStackFrame* sfrm) { ASSERT(_rsCur != 0L); _rsCur->_sfrm = sfrm; }
	// ディスプレイスタックフレームを設定する
	void setDisplayStackFrame(s32 depth, EsStackFrame* sfrm);
	// セカンダリ返答値がセットされているかを設定する
	inline void set2ndRetValueSet(bool isSet) { _isRet2ndValSet = isSet; }
	// 停止しているかどうかを設定する
	inline void setStop(bool isStop) { _isStop = isStop; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void prepareParse(const EsKeywordSet* kwsetRef, EsExternalExtension* extextRef, u32 mflags, EsExtParam paramParse);
	void setCode(const VcString* code);
	bool checkParseEnd();
	bool prepareRun(EsInterlang* il, EsStackFrame* sfrmCaller, u16 flagsSfrm, EsValue* valRet, EsExtParam paramRun);
	bool prepareCall(EsFunction* func, EsInterlang* il, u16 flagsSfrm, EsValue* vpStack, s32 numArg, EsObject* objCallee, EsObject* objThis, EsObject* objScopeChain, EsStackFrame* sfrmCaller);
	bool disposeCall();

	bool findPropertyFromScopeChain(EsScopeProperty** spropOut, EsObject** objScopeOut, EsObject** objScopeParentOut, const EsValue* spropid);
	EsObject* findIdentifierBase(EsObject* objScopeChain, const EsValue* id);
	void setGlobalObject(EsObject* objGlobalRef);
	void addParsedFunctionInfoForExternalExtension(const EsAtom* atom, bool isDef);
	EsStackFrame* getScriptedCallerFrame() const;

	void addLeakNode(EsItNode* node);
	void removeLeakNode(EsItNode* node);
	void addDeleteCollectionNode(EsItNode* node);
	void clearDeleteCollectionNodes();

	EsInterlang* newInterlang();
	EsStackFrame* newStackFrame();
	EsObject* newObject(const EsClass* cls, EsObject* objPrototype, EsObject* objParent);
	EsObject* newObjectWithGivenPrototype(const EsClass* cls, EsObject* objPrototype, EsObject* objParent);
	EsFunction* newFunction(const EsAtom* atom, EsObject* objPrototype, EsObject* objParent, bool isLambda);
	EsFunction* newFunction(const EsAtom* atom, EsNativeCall call, EsObject* objParent, EsExtParam paramDef);
	EsArray* newArray(EsValue* valarr, s32 lenArr);
	EsIterator* newIterator(EsObject* objParent, u8 flagsIter);
	const VcString* newString(const VcString* str);
	void registerCustomUnit(EsUnit* unit);
	void deleteRegisteredCustomUnit(EsUnit* unit);
	void clearDeleteCollectionUnits(bool isAfterParse);
	void clearGarbageUnits();
	
	const EsAtom* findMadeStringAtom(const VcString* str) const;
	const EsAtom* makeStringAtom(const VcString* str);
	const EsAtom* makeDoubleAtom(const f64* fval);
	const EsAtom* makeValueAtom(const EsValue* val);
	const EsAtom* makePrimitiveValueAtom(const EsValue* val);
	bool convValueToObject(EsObject** objOut, const EsValue* val);
	bool convValueToFunction(EsFunction** funcOut, const EsValue* val, bool isConstructor);
	bool convValueToStringId(EsValue* id, const EsValue* val);
	bool convValueToId(EsValue* id, const EsValue* val);
	bool convValueToIterator(EsIterator** iter, EsValue* val, u8 flagsIter);
	bool convPrimitiveValueToObject(EsValue* val);
	u32 makeUniqueShapeId();
	bool instanceOf(EsObject* obj, const EsClass* cls, const EsValue* val);

	void addRunError(s32 id) { return addRunError(id, false, false); }
	void addRunError(s32 id, bool isWarning, bool isStrict);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsContext();
	~EsContext();
	bool create();
	void destroy();
	void reset();

	//======================================================================
	// 変数
private:
	// 解析・コンパイル変数
	EsItNodeFactory* _nodefct;
	EsError* _err;
	VcString* _code;
	HashTable* _htblStringAtoms;
	HashTable* _htblDoubleAtoms;
	HashTable* _htblValueAtoms;
	EsResolver*	_resolv;					// ⇒ resolvingTable
	u32 _mflags;							// 動作モードフラグ
	EsExtParam _paramParse;

	// 削除用リスト（ガベージコレクションのようなもの）
	List<EsUnit*>* _listDelUnit;
	List<EsItNode*>* _listDelNode;
	List<VcString*>* _listDelString;

	// 外部オブジェクト
	const EsKeywordSet* _kwsetRef;
	EsObject* _objGlobalRef;
	EsExternalExtension* _extextRef;

	// 実行変数
	EsRunEnv* _runenvCur;
	RunStack* _rsCur;
	Array<EsStackFrame*>* _arrSfrmDisp;		// スタックフレーム ディスプレイ ⇒ cx->display
	EsExtParam _paramRun;
	EsValue* _valRet2nd;					// 左辺用セカンダリ返答値 ⇒ rval2
	bool _isRet2ndValSet;					// 左辺用セカンダリ返答値がセットされているかどうか ⇒ rval2set
	bool _isStop;
	u32 _gencntShape;						// ⇒ shapeGen
	// ※※※ 実装なし ※※※ ⇒ dormantFrameChain
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_CONTEXT_H_
