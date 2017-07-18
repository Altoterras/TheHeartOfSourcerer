/***********************************************************************//**
 *	EsRunEnv.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/04.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_RUN_ENV_H_
#define _ES_ES_RUN_ENV_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsOpcodeType.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsAtom;
class EsBcExInfo;
class EsClass;
class EsContext;
class EsFunction;
class EsInterlang;
class EsRunStat;
class EsObject;
class EsStackFrame;
class EsStackRegister;
class EsUpvarInfo;

/*---------------------------------------------------------------------*//**
 *	スクリプト実行環境
 *
**//*---------------------------------------------------------------------*/
class EsRunEnv
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 関数呼び出しの深さを得る
	inline s32 getCallDepth() const { return _depthCall; }
	// 現在の拡張情報を得る
	inline const EsBcExInfo* getCurBcExInfo() const { return _exiCurStep; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool begin(EsContext* ctx, s32 depthCall, EsValue* valResult);
	bool end(EsRunEnv* bk);
	bool step(EsRunStat* stat);

	bool invoke(EsStackFrame** sfrmInvoke, EsValue* vpStack, u32 numArg, u16 flagsSfrm);
	bool invokeInternal(EsValue* valReturn, EsObject* obj, const EsValue* valFunc, EsValue* vpStack, u32 numArg, u16 flagsSfrm);

private:
	bool invokeConstructor(EsValue* vaStack, u32 numArg, u16 flagsSfrm, bool isClampReturn);
	bool callInline(EsFunction* func, EsValue* vpStack, u32 numArg, u16 flagsSfrm);
	bool invokeAfter(bool isSucceeded);

public:	
	bool defineFunction(EsFunction* func);
	bool makeClassInstance(EsObject** objOut, const EsAtom* atomClassName, EsValue* vaArg, u32 numArg);

private:
	void stop();

private:
	bool stepSub_defvar(EsOpcodeType ot);
	bool stepSub_deffun();
	bool stepSub_deffunfc();
	bool stepSub_deflocalfun();
	bool stepSub_deflocalfunfc();
	bool stepSub_lambda();
	bool stepSub_lambdafc();
	bool stepSub_call(EsOpcodeType ot);
	bool stepSub_callAfter(EsOpcodeType ot);
	bool stepSub_new();
	bool stepSub_return(EsOpcodeType ot);
	bool stepSub_retval(EsOpcodeType ot);
	bool stepSub_push();
	bool stepSub_pop();
	bool stepSub_popn();
	bool stepSub_popv();
	bool stepSub_bindname();
	bool stepSub_getname(EsOpcodeType ot);
	bool stepSub_setname(EsOpcodeType ot);
	bool stepSub_getelem();
	bool stepSub_callelem();
	bool stepSub_setelem();
	bool stepSub_enumelem();
	bool stepSub_getprop(EsOpcodeType ot);
	bool stepSub_callprop();
	bool stepSub_getgvar(EsOpcodeType ot);
	bool stepSub_setgvar();
	bool stepSub_getlocal(EsOpcodeType ot);
	bool stepSub_setlocal();
	bool stepSub_setlocalpop();
	bool stepSub_getarg(EsOpcodeType ot);
	bool stepSub_setarg();
	bool stepSub_getupvar(EsOpcodeType ot);
	bool stepSub_getdslot(EsOpcodeType ot);
	bool stepSub_setconst();
	bool stepSub_setcall();
	bool stepSub_setcallAfter();
	bool stepSub_arguments();
	bool stepSub_argsub();
	bool stepSub_argcnt();
	bool stepSub_forname();
	bool stepSub_forlocal();
	bool stepSub_forarg();
	bool stepSub_forprop();
	bool stepSub_forelem();
	bool stepSub_delname();
	bool stepSub_delprop();
	bool stepSub_delelem();
	bool stepSub_goto(bool isXofs);
	bool stepSub_default(bool isXofs);
	bool stepSub_ifcond(EsOpcodeType ot, bool isXofs);
	bool stepSub_gosub(bool isXofs);
	bool stepSub_retsub();
	bool stepSub_in();
	bool stepSub_pushvalue(EsOpcodeType ot);
	bool stepSub_this();
	bool stepSub_primtop();
	bool stepSub_ifprimtop();
	bool stepSub_objtop();
	bool stepSub_typeof();
	bool stepSub_instanceof();
	bool stepSub_arithop(EsOpcodeType ot);
	bool stepSub_bitop(EsOpcodeType ot);
	bool stepSub_bitshift(EsOpcodeType ot);
	bool stepSub_orand(EsOpcodeType ot, bool isXofs);
	bool stepSub_not();
	bool stepSub_bitnot();
	bool stepSub_compare(EsOpcodeType ot);
	bool stepSub_stricteq(bool isEquals);
	bool stepSub_case(bool isXofs);
	bool stepSub_pos();
	bool stepSub_neg();
	bool stepSub_nameinc(EsOpcodeType ot, bool isDec, bool isPost);
	bool stepSub_gvarinc(bool isDec, bool isPost, EsOpcodeType otSubst);
	bool stepSub_localinc(bool isArg, bool isDec, bool isPost);
	bool stepSub_resetbase();
	bool stepSub_indexbase();
	bool stepSub_dup(EsOpcodeType ot);
	bool stepSub_pick();
	bool stepSub_enterblock();
	bool stepSub_leaveblock(EsOpcodeType ot);
	bool stepSub_enterwith();
	bool stepSub_leavewith();
	bool stepSub_iter();
	bool stepSub_nextiter();
	bool stepSub_enditer();
	bool stepSub_newarray();
	bool stepSub_newinit();
	bool stepSub_endinit();
	bool stepSub_initprop();
	bool stepSub_initelem();
	bool stepSub_lookupswitch(bool isXofs);
	bool stepSub_tableswitch(bool isXofs);
	
	void restep(EsOpcodeType otSubst, const EsBcExInfo* exi);

public:
	u32 getIndex(s32 offsetPc) const;
	u32 getFullIndex(s32 offsetPc) const;
	const EsAtom* getAtom(s32 offsetPc) const;
	const EsAtom* getAtomFromBytecode(s32 offsetPc) const;
	EsObject* getObject(s32 offsetPc) const;
	EsFunction* getFunction(s32 offsetPc) const;
	const EsValue* getUpvar(u32 depthNest, const EsUpvarInfo* upvari) const;
	EsObject* getThisObject(bool isLazy, EsValue* valCallee, EsValue* valThis, EsValue* vaArg);
	EsObject* getGlobalThisObject(bool isLazy, EsValue* valCallee, EsValue* valThis, EsValue* vaArg);
	EsObject* getThisObjectForFrame();
	bool getPrimitiveThisObject(EsValue* valThisObjOut, const EsClass* cls, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg);

private:
	bool checkRedeclaration(EsScopeProperty** spropOut, EsObject** objScopeOwnerOut, EsObject* obj, const EsValue* spropid, u8 spatrbfNew, bool isInitializer) const;
	void jumpIfCondition(EsOpcodeType ot, bool cond, s32 spdec);
	void jumpBranch(s32 offsetJump);
	bool skipPopAfterSet(EsOpcodeType ot, s32 spofsDecrement);
	bool callGetPropertyObjself(EsValue* val, EsContext* ctx, EsObject* obj, EsObject* objScopeOwn, EsScopeProperty* sprop);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsRunEnv();
	~EsRunEnv();
	void clear();
	void copy(const EsRunEnv* src);

#if defined(_DEBUG)
	void _debug_Sp();
#endif

	//======================================================================
	// 変数
private:
	EsContext* _ctxRef;
	EsInterlang* _ilCur;		// 現在の中間言語 ⇒ script
	EsStackFrame* _sfrmCur;		// 現在のスタックフレーム ⇒ fp
	EsStackRegister* _regsCur;	// 現在のスタックレジスタ ⇒ regs
	u32 _indexBase;				// 大きいインデックスを扱うための上位ビットインデックス ⇒ atoms
	s32 _depthCall;				// 関数呼び出しの深さ ⇒ inlineCallCount
	EsStackFrame* _sfrmInvoke;	// invoke 中のスタックフレーム
	EsValue* _valResult;		// 返答値を受け取る値のポインタ

	// ステップ変数
	bool _isContinueStep;		// 続けて次を実行
	s32 _lenOpcNextStep;		// バイトコードの長さ（次への送り量）
	EsOpcodeType _otCurStep;	// 現在の命令コード
	EsBcExInfo* _exiCurStep;	// 現在の拡張情報
	EsOpcodeType _otSpNextStep;	// 次の指定命令コード
	EsBcExInfo* _exiSpNextStep;	// 次の指定拡張情報
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_RUN_ENV_H_
