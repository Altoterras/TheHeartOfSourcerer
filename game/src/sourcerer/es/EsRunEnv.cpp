/***********************************************************************//**
 *	EsRunEnv.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/04.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsRunEnv.h"

// Friends
#include "EsArray.h"
#include "EsAtom.h"
#include "EsBcExInfo.h"
#include "EsBlock.h"
#include "EsBytecodeBuilder.h"
#include "EsContext.h"
#include "EsError.h"
#include "EsSysClasses.h"
#include "EsSysPrototypes.h"
#include "EsFunction.h"
#include "EsFunctionBox.h"
#include "EsInterlang.h"
#include "EsIterator.h"
#include "EsKeywordSet.h"
#include "EsObject.h"
#include "EsObjectOp.h"
#include "EsOpcode.h"
#include "EsResolver.h"
#include "EsRunStat.h"
#include "EsScope.h"
#include "EsScopeProperty.h"
#include "EsProgramCounter.h"
#include "EsStackFrame.h"
#include "EsStackRegister.h"
#include "EsValue.h"

// External
#include "../../tfw/lib/TypeUtils.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// ステップ短縮の試作
#define SHORTEN_STEP		1

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	実行準備
**//*---------------------------------------------------------------------*/
bool EsRunEnv::begin(EsContext* ctx, s32 depthCall, EsValue* valResult)
{
	// 実行環境をコンテキストから得る
	_ctxRef = ctx; ASSERT(_ctxRef != 0L);
	_ilCur = _ctxRef->getInterlang(); ASSERT(_ilCur != 0L);				// ⇒ script
	_sfrmCur = _ctxRef->getTopStackFrame(); ASSERT(_sfrmCur != 0L);		// ⇒ fp
	_regsCur = _sfrmCur->getStackRegister(); ASSERT(_regsCur != 0L);	// ⇒ regs

	// 作業変数のリセット
	_indexBase = 0;
	_depthCall = depthCall;
	_otSpNextStep = OP_NULL;
	_valResult = valResult;

	// 拡張情報の準備
	delete _exiCurStep;
	_exiCurStep = 0L;
	delete _exiSpNextStep;
	_exiSpNextStep = 0L;
	if(_ilCur->isBcExInfoAdd())
	{
		_exiCurStep = new EsBcExInfo();
		_exiSpNextStep = new EsBcExInfo();
	}

	// 解決フラグの設定
	_ctxRef->getResolver()->setResolvFlags(EsResolver::RSLVF_INFER);	// ⇒ JSAutoResolveFlags rf(cx, JSRESOLVE_INFER); @js_Interpret

	// ディスプレイスタックフレームの設定
	u32 depthNest = _ilCur->getNestDepth();
	_sfrmCur->setPrevDisplayStackFrame(_ctxRef->getDisplayStackFrame(depthNest));
	_ctxRef->setDisplayStackFrame(depthNest, _sfrmCur);	// ⇒ *disp = fp;

	return true;
}

/*---------------------------------------------------------------------*//**
	実行終了
**//*---------------------------------------------------------------------*/
bool EsRunEnv::end(EsRunEnv* bk)
{
	clear();
	if(bk != 0L)
	{
		copy(bk);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行 ⇒ js_Interpret
**//*---------------------------------------------------------------------*/
bool EsRunEnv::step(EsRunStat* stat)
{
	ASSERT(_ctxRef != 0L);
	
	bool isOpOk = true;
	do
	{
		// 連続実行フラグのクリア
		_isContinueStep = false;
		// 特別に指定する Opcode の長さのクリア（-1 のままの場合はスペック情報から取得する）
		_lenOpcNextStep = -1;
		// 実行中にセット
		stat->setRunning(true, false);
		// この演算コードに対応したスタックフレームを記憶しておく
		EsStackFrame* sfrmOp = _sfrmCur;

		if(_otSpNextStep != OP_NULL)	// 指定命令コードがある場合
		{
			// 拡張情報の設定
			if(_ilCur->isBcExInfoAdd() && (_exiCurStep != 0L))
			{
				ASSERT(_exiSpNextStep != 0L);
				_exiCurStep->copy(_exiSpNextStep);
				stat->setCodeIndex(_exiCurStep->getBeginIndex(), _exiCurStep->getEndIndex());
			}

			// 命令コードを設定
			_otCurStep = _otSpNextStep;
			_otSpNextStep = OP_NULL;
		}
		else
		{
			// プログラムカウンタを進める
			_regsCur->advancePc(_sfrmCur->getLastOpcodeLength());

			// 拡張情報の取得と設定
			if(_ilCur->isBcExInfoAdd() && (_exiCurStep != 0L))
			{
				_exiCurStep->fromBytecode(_regsCur->pc());
				stat->setCodeIndex(_exiCurStep->getBeginIndex(), _exiCurStep->getEndIndex());

				// 拡張情報分のプログラムカウンタを進める
				_regsCur->advancePc(EsBcExInfo::LEN_BC);
			}

			// 命令コードを取得
			_otCurStep = (EsOpcodeType)_regsCur->pc()->get(0);
		}

		// 命令コード毎の処理
		EsOpcodeType ot = _otCurStep;
		//TRACELN("{EsRunEnv::step} OP_%s (%d-%d) - sp=[%d/%d]", EsOpcode::getSpec(ot)->getName(), _exiCurStep->getBeginIndex(), _exiCurStep->getEndIndex(), _regsCur->getCurSpIndex(), _regsCur->getSpNum());
		switch(ot)
		{
		case OP_DEFVAR:
		case OP_DEFCONST:		isOpOk = stepSub_defvar(ot);						break;
		case OP_DEFFUN:			isOpOk = stepSub_deffun();							break;
		case OP_DEFFUN_FC:		isOpOk = stepSub_deffunfc();						break;
		case OP_DEFLOCALFUN:	isOpOk = stepSub_deflocalfun();						break;
		case OP_DEFLOCALFUN_FC:	isOpOk = stepSub_deflocalfunfc();					break;
		case OP_LAMBDA:			isOpOk = stepSub_lambda();							break;
		case OP_LAMBDA_FC:		isOpOk = stepSub_lambdafc();						break;
		case OP_CALL:
		case OP_EVAL:
		case OP_APPLY:			isOpOk = stepSub_call(ot);							break;
		case OP_NEW:			isOpOk = stepSub_new();								break;
		case OP_RETURN:			isOpOk = stepSub_return(ot);						break;
		case OP_RETRVAL:
		case OP_STOP:			isOpOk = stepSub_retval(ot);						break;
		case OP_PUSH:			isOpOk = stepSub_push();							break;
		case OP_POP:			isOpOk = stepSub_pop();								break;
		case OP_POPN:			isOpOk = stepSub_popn();							break;
		case OP_POPV:
		case OP_SETRVAL:		isOpOk = stepSub_popv();							break;
		case OP_BINDNAME:		isOpOk = stepSub_bindname();						break;
			// GET, SET, CALL
		case OP_NAME:
		case OP_CALLNAME:		isOpOk = stepSub_getname(ot);						break;
		case OP_SETNAME:
		case OP_SETPROP:		isOpOk = stepSub_setname(ot);						break;
		case OP_GETELEM:		isOpOk = stepSub_getelem();							break;
		case OP_CALLELEM:		isOpOk = stepSub_callelem();						break;
		case OP_SETELEM:		isOpOk = stepSub_setelem();							break;
		case OP_ENUMELEM:		isOpOk = stepSub_enumelem();						break;
		case OP_GETPROP:
		case OP_GETXPROP:
		case OP_GETTHISPROP:
		case OP_GETARGPROP:
		case OP_GETLOCALPROP:
		case OP_LENGTH:			isOpOk = stepSub_getprop(ot);						break;
		case OP_CALLPROP:		isOpOk = stepSub_callprop();						break;
		case OP_GETGVAR:
		case OP_CALLGVAR:		isOpOk = stepSub_getgvar(ot);						break;
		case OP_SETGVAR:		isOpOk = stepSub_setgvar();							break;
		case OP_GETLOCAL:
		case OP_CALLLOCAL:		isOpOk = stepSub_getlocal(ot);						break;
		case OP_SETLOCAL:		isOpOk = stepSub_setlocal();						break;
		case OP_SETLOCALPOP:	isOpOk = stepSub_setlocalpop();						break;
		case OP_GETARG:
		case OP_CALLARG:		isOpOk = stepSub_getarg(ot);						break;
		case OP_SETARG:			isOpOk = stepSub_setarg();							break;
		case OP_GETUPVAR:
		case OP_CALLUPVAR:		isOpOk = stepSub_getupvar(ot);						break;
		case OP_GETDSLOT:
		case OP_CALLDSLOT:		isOpOk = stepSub_getdslot(ot);						break;
		case OP_SETCONST:		isOpOk = stepSub_setconst();						break;
		case OP_SETCALL:		isOpOk = stepSub_setcall();							break;
			// ARG
		case OP_ARGUMENTS:		isOpOk = stepSub_arguments();						break;
		case OP_ARGSUB:			isOpOk = stepSub_argsub();							break;
		case OP_ARGCNT:			isOpOk = stepSub_argcnt();							break;
			// FOR
		case OP_FORARG:			isOpOk = stepSub_forarg();							break;
		case OP_FORLOCAL:		isOpOk = stepSub_forlocal();						break;
		case OP_FORNAME:		isOpOk = stepSub_forname();							break;
		case OP_FORPROP:		isOpOk = stepSub_forprop();							break;
		case OP_FORELEM:		isOpOk = stepSub_forelem();							break;
			// DEL
		case OP_DELNAME:		isOpOk = stepSub_delname();							break;
		case OP_DELPROP:		isOpOk = stepSub_delprop();							break;
		case OP_DELELEM:		isOpOk = stepSub_delelem();							break;
			// 分岐
		case OP_GOTO:			isOpOk = stepSub_goto(false);						break;
		case OP_GOTOX:			isOpOk = stepSub_goto(true);						break;
		case OP_DEFAULT:		isOpOk = stepSub_default(false);					break;
		case OP_DEFAULTX:		isOpOk = stepSub_default(true);						break;
		case OP_IFEQ:
		case OP_IFNE:			isOpOk = stepSub_ifcond(ot, false);					break;
		case OP_IFEQX:
		case OP_IFNEX:			isOpOk = stepSub_ifcond(ot, true);					break;
		case OP_GOSUB:			isOpOk = stepSub_gosub(false);						break;
		case OP_GOSUBX:			isOpOk = stepSub_gosub(true);						break;
		case OP_RETSUB:			isOpOk = stepSub_retsub();							break;
		case OP_IN:				isOpOk = stepSub_in();								break;
			// 数値, オブジェクト
		case OP_UINT16:
		case OP_UINT24:
		case OP_INT8:
		case OP_INT32:
		case OP_ZERO:
		case OP_ONE:
		case OP_NULL:
		case OP_FALSE:
		case OP_TRUE:
		case OP_DOUBLE:
		case OP_STRING:
		case OP_OBJECT:
		case OP_CALLEE:
		case OP_HOLE:
		case OP_VOID:			isOpOk = stepSub_pushvalue(ot);						break;
		case OP_THIS:			isOpOk = stepSub_this();							break;
		case OP_PRIMTOP:		isOpOk = stepSub_primtop();							break;
		case OP_IFPRIMTOP:		isOpOk = stepSub_ifprimtop();						break;
		case OP_OBJTOP:			isOpOk = stepSub_objtop();							break;
		case OP_TYPEOF:
		case OP_TYPEOFEXPR:		isOpOk = stepSub_typeof();							break;
		case OP_INSTANCEOF:		isOpOk = stepSub_instanceof();						break;
			// 四則演算・ビット演算・論理演算
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
		case OP_MOD:			isOpOk = stepSub_arithop(ot);						break;
		case OP_BITOR:
		case OP_BITXOR:
		case OP_BITAND:			isOpOk = stepSub_bitop(ot);							break;
		case OP_LSFT:			
		case OP_RSFT:
		case OP_URSFT:			isOpOk = stepSub_bitshift(ot);						break;
		case OP_OR:
		case OP_AND:			isOpOk = stepSub_orand(ot, false);					break;
		case OP_ORX:
		case OP_ANDX:			isOpOk = stepSub_orand(ot, true);					break;
		case OP_NOT:			isOpOk = stepSub_not();								break;
		case OP_BITNOT:			isOpOk = stepSub_bitnot();							break;
		case OP_EQ:
		case OP_NE:
		case OP_LT:
		case OP_LE:
		case OP_GT:
		case OP_GE:				isOpOk = stepSub_compare(ot);						break;
		case OP_STRICTEQ:		isOpOk = stepSub_stricteq(true);					break;
		case OP_STRICTNE:		isOpOk = stepSub_stricteq(false);					break;
		case OP_CASE:			isOpOk = stepSub_case(false);						break;
		case OP_CASEX:			isOpOk = stepSub_case(true);						break;
		case OP_NEG:			isOpOk = stepSub_neg();								break;
		case OP_POS:			isOpOk = stepSub_pos();								break;
			// インクリメント・デクリメント
		case OP_NAMEINC:		isOpOk = stepSub_nameinc(ot, false, true);			break;
		case OP_INCNAME:		isOpOk = stepSub_nameinc(ot, false, false);			break;
		case OP_NAMEDEC:		isOpOk = stepSub_nameinc(ot, true, true);			break;
		case OP_DECNAME:		isOpOk = stepSub_nameinc(ot, true, false);			break;
		case OP_ELEMINC:		isOpOk = stepSub_nameinc(ot, false, true);			break;
		case OP_INCELEM:		isOpOk = stepSub_nameinc(ot, false, false);			break;
		case OP_ELEMDEC:		isOpOk = stepSub_nameinc(ot, true, true);			break;
		case OP_DECELEM:		isOpOk = stepSub_nameinc(ot, true, false);			break;
		case OP_PROPINC:		isOpOk = stepSub_nameinc(ot, false, true);			break;
		case OP_INCPROP:		isOpOk = stepSub_nameinc(ot, false, false);			break;
		case OP_PROPDEC:		isOpOk = stepSub_nameinc(ot, true, true);			break;
		case OP_DECPROP:		isOpOk = stepSub_nameinc(ot, true, false);			break;
		case OP_GVARINC:		isOpOk = stepSub_gvarinc(false, true, OP_NAMEINC);	break;
		case OP_INCGVAR:		isOpOk = stepSub_gvarinc(false, false, OP_INCNAME);	break;
		case OP_GVARDEC:		isOpOk = stepSub_gvarinc(true, true, OP_NAMEDEC);	break;
		case OP_DECGVAR:		isOpOk = stepSub_gvarinc(true, false, OP_DECNAME);	break;
		case OP_LOCALINC:		isOpOk = stepSub_localinc(false, false, true);		break;
		case OP_INCLOCAL:		isOpOk = stepSub_localinc(false, false, false);		break;
		case OP_LOCALDEC:		isOpOk = stepSub_localinc(false, true, true);		break;
		case OP_DECLOCAL:		isOpOk = stepSub_localinc(false, true, false);		break;
		case OP_ARGINC:			isOpOk = stepSub_localinc(true, false, true);		break;
		case OP_INCARG:			isOpOk = stepSub_localinc(true, false, false);		break;
		case OP_ARGDEC:			isOpOk = stepSub_localinc(true, true, true);		break;
		case OP_DECARG:			isOpOk = stepSub_localinc(true, true, false);		break;
			// インデックス
		case OP_RESETBASE:		isOpOk = stepSub_resetbase();						break;
		case OP_INDEXBASE:		isOpOk = stepSub_indexbase();						break;
			// DUP, SWAP
		case OP_DUP:
		case OP_DUP2:
		case OP_SWAP:			isOpOk = stepSub_dup(ot);							break;
		case OP_PICK:			isOpOk = stepSub_pick();							break;
			// ブロック
		case OP_ENTERBLOCK:		isOpOk = stepSub_enterblock();						break;
		case OP_LEAVEBLOCK:
		case OP_LEAVEBLOCKEXPR:	isOpOk = stepSub_leaveblock(ot);					break;
		case OP_ENTERWITH:		isOpOk = stepSub_enterwith();						break;
		case OP_LEAVEWITH:		isOpOk = stepSub_leavewith();						break;
			// イテレータ
		case OP_ITER:			isOpOk = stepSub_iter();							break;
		case OP_NEXTITER:		isOpOk = stepSub_nextiter();						break;
		case OP_ENDITER:		isOpOk = stepSub_enditer();							break;
			// new, 初期化
		case OP_NEWARRAY:		isOpOk = stepSub_newarray();						break;
		case OP_NEWINIT:		isOpOk = stepSub_newinit();							break;
		case OP_ENDINIT:		isOpOk = stepSub_endinit();							break;
		case OP_INITPROP:		isOpOk = stepSub_initprop();						break;
		case OP_INITELEM:		isOpOk = stepSub_initelem();						break;
			// switch
		case OP_LOOKUPSWITCH:	isOpOk = stepSub_lookupswitch(false);				break;
		case OP_LOOKUPSWITCHX:	isOpOk = stepSub_lookupswitch(true);				break;
		case OP_TABLESWITCH:	isOpOk = stepSub_tableswitch(false);				break;
		case OP_TABLESWITCHX:	isOpOk = stepSub_tableswitch(true);					break;

		case OP_NOP:
		case OP_LINENO:
		case OP_LOOP:
			_isContinueStep = true;	// 何もせず続けて次を実行
			break;

			// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
			// 未対応
		case OP_CONDSWITCH:
		case OP_TRY:
			_isContinueStep = true;	// 何もせず続けて次を実行
			break;

			// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
			// 来るはずのない不正な命令
		case OP_CALLBUILTIN:
		case OP_IMACOP:
			// 来るはずのない不正な命令
			ASSERT(false);
			break;

		default:
			ASSERT(false);
			stat->setRunning(false, true);
			break;

			// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
			// 対応予定なし
		case OP_ENUMCONSTELEM:
		case OP_REGEXP:
		case OP_GETUPVAR_DBG:
		case OP_CALLUPVAR_DBG:
		case OP_DEFFUN_DBGFC:
		case OP_DEFLOCALFUN_DBGFC:
		case OP_LAMBDA_DBGFC:
		case OP_GETTER:
		case OP_SETTER:
		case OP_DEFSHARP:
		case OP_USESHARP:
		case OP_DEBUGGER:
		case OP_RESETBASE0:
		case OP_INDEXBASE1:
		case OP_INDEXBASE2:
		case OP_INDEXBASE3:
		case OP_TRAP:
			// 例外（try, catch, finally, throw）
		case OP_EXCEPTION:
		case OP_FINALLY:
		case OP_THROWING:
		case OP_THROW:
			// XML
		case OP_DEFXMLNS:
		case OP_ANYNAME:
		case OP_QNAMEPART:
		case OP_QNAMECONST:
		case OP_QNAME:
		case OP_TOATTRNAME:
		case OP_TOATTRVAL:
		case OP_ADDATTRNAME:
		case OP_ADDATTRVAL:
		case OP_BINDXMLNAME:
		case OP_SETXMLNAME:
		case OP_CALLXMLNAME:
		case OP_XMLNAME:
		case OP_DESCENDANTS:
		case OP_DELDESC:
		case OP_FILTER:
		case OP_ENDFILTER:
		case OP_TOXML:
		case OP_TOXMLLIST:
		case OP_XMLTAGEXPR:
		case OP_XMLELTEXPR:
		case OP_XMLOBJECT:
		case OP_XMLCDATA:
		case OP_XMLCOMMENT:
		case OP_XMLPI:
		case OP_GETFUNNS:
			// Generator
		case OP_GENERATOR:
		case OP_YIELD:
		case OP_ARRAYPUSH:
			// ※※※ 実装しない ※※※
			ASSERT(false);
			stat->setRunning(false, true);
			break;
		}
		if(!isOpOk)
		{
			break;		// 失敗
		}

		// 次にプログラムカウンタを進めるように、現在の演算コードの長さを得ておく
		if(_lenOpcNextStep == -1)
		{
			_lenOpcNextStep = EsOpcode::getSpec(_otCurStep)->_length;	// 特にステップサブ内で指定がない場合
		}
		sfrmOp->setLastOpcodeLength(_lenOpcNextStep);

		// 停止セット
		if(_ctxRef->isStop())	{	stat->setRunning(false, false);	}
	}
	while(_isContinueStep && stat->isRunning());

	// 不要ユニットの削除
	_ctxRef->clearGarbageUnits();

	// 失敗時は停止する
	if(!isOpOk)
	{
		stop();
		stat->setRunning(false, true);
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	関数呼び出し ⇒ js_Invoke
**//*---------------------------------------------------------------------*/
bool EsRunEnv::invoke(EsStackFrame** sfrmInvoke, EsValue* vaStack, u32 numArg, u16 flagsSfrm)
{
	enum CallFunc
	{
		CFUNC_NULL,
		CFUNC_OBJ_CALL,
		CFUNC_OBJ_CONSTRUCT,
		CFUNC_NATIVE_CALL,
		CFUNC_SCRIPT,
	};
	
	bool isSucceeded = true;
	EsCallExtParam exprm;
	EsObject* objFunc;
	EsObject* objThis;
	EsValue* valParent;
	EsObject* objParent;
	const EsClass* classFunc;
	EsFunction* func;
	CallFunc cfunc;
	EsInterlang* ilScript;
	s32 numSlot;
	bool isHasFunc;
	bool isComputeThis;
	EsStackFrame* sfrmInvokeNew;
	
	if(vaStack[0].isPrimitive())
	{
		_ctxRef->addRunError(EsError::ID_NOT_FUNCTION);
		ESCASSERT(false);
		goto FAILED;
	}

	objFunc = vaStack[0].getObject();	// JSVAL_TO_OBJECT(v)
	objThis = vaStack[1].getObject();
	valParent = objFunc->slots()->value(objFunc->getSlots()->getParentIndex());
	objParent = valParent->getObject();
	classFunc = objFunc->getClass();
	func = 0L;
	cfunc = CFUNC_NULL;
	ilScript = 0L;
	numSlot = 0;
	isHasFunc = false;
	isComputeThis = true;

	if(classFunc->getClassType() == EsSysClasses::CLASSTYPE_FUNC)
	{
		isHasFunc = true;
	}
	else
	{
		do
		{
			// インライン関数
			EsValue val(vaStack[0]);
			ASSERT(classFunc->getConvertFunc() != 0L);
			if(!classFunc->getConvertFunc()(&val, _ctxRef, objFunc, LTYPE_VOID))
			{
				goto FAILED;
			}

			if(val.isFunctionObject())
			{
				vaStack[0].copy(&val);
				objFunc = val.getObject();
				objParent = objFunc->slots()->parentObject();

				isHasFunc = true;
				break;
			}

			// オブジェクトの callOv か constrcutOv を呼ぶ
			if(TFW_IS_FLAG(flagsSfrm, EsStackFrame::F_CONSTRUCTING))
			{
				if(!vaStack[1].isObject())
				{
					if(!_ctxRef->convPrimitiveValueToObject(&vaStack[1]))
					{
						ESCASSERT(false);
						goto FAILED;
					}
				}
				cfunc = CFUNC_OBJ_CONSTRUCT;
			}
			else
			{
				cfunc = CFUNC_OBJ_CALL;
			}
		}
		while(false);
	}

	if(isHasFunc)	// ⇒ have_fun:
	{
		func = (EsFunction*)objFunc;
		exprm._paramDef = func->getDefParam();

		numSlot = func->isInterpreted() ? func->getLocalVarNum(EsFunction::LVK_ARG) : 0;
		numSlot = (numSlot > (s32)numArg) ? numSlot - numArg : 0;

		if(func->isInterpreted())	// 内部スクリプト関数
		{
			cfunc = CFUNC_SCRIPT;
			ilScript = func->getInterlang();
		}
		else						// 外部ネイティブ関数
		{
			cfunc = CFUNC_NATIVE_CALL;
			// ※※※ 未実装 ※※※ ⇒ nslots += fun->u.n.extra;
		}

		if(TFW_IS_FLAG(func->getFlags(), EsFunction::F_BOUND_METHOD))
		{
			vaStack[1].setValue(objParent);
		}
		else if(!vaStack[1].isObject())
		{
			ASSERT(!TFW_IS_FLAG(flagsSfrm, EsStackFrame::F_CONSTRUCTING));
			if(func->testPrimitiveThisValue(&vaStack[1]))
			{
				isComputeThis = false;
			}
		}
	}

	if(isComputeThis)
	{
		if(TFW_IS_FLAG(flagsSfrm, EsStackFrame::F_CONSTRUCTING))
		{
			ASSERT(!vaStack[1].isPrimitive());
		}
		else	// ※※※ 実装なし ※※※ if((func == 0L) || !TFW_IS_FLAG(func->getFlags(), EsFunction::F_FAST_NATIVE))
		{
			if(getThisObject(false, &vaStack[0], &vaStack[1], &vaStack[2]) == 0L)
			{
				ESCASSERT(false);
				goto FAILED;
			}

			TFW_SET_FLAG(flagsSfrm, EsStackFrame::F_COMPUTED_THIS, true);
		}
	}

	// 関数用の新たなスタックフレームを用意する
	sfrmInvokeNew = _ctxRef->newStackFrame();
	if(sfrmInvokeNew == 0L)
	{
		_ctxRef->addRunError(EsError::ID_SYSTEM_ERROR);
		ESCASSERT(false);
		goto FAILED;
	}
	sfrmInvokeNew->prepareForInvoke(
		_ctxRef,
		func,
		ilScript,
		numSlot,
		flagsSfrm,
		vaStack,
		numArg,
		func,
		objThis,
		0L,
		_ctxRef->getTopStackFrame()
		);
	// スタックフレームを置き換える
	_sfrmCur = _sfrmInvoke = sfrmInvokeNew;
	_ctxRef->setTopStackFrame(_sfrmCur);

	switch(cfunc)
	{
	case CFUNC_OBJ_CALL:
	case CFUNC_OBJ_CONSTRUCT:
	case CFUNC_NATIVE_CALL:
		if(sfrmInvokeNew->getDownStackFrame() != 0L)	// ⇒ if (frame.down)
		{
			sfrmInvokeNew->setVarObject(sfrmInvokeNew->getDownStackFrame()->getVarObject());				// ⇒ frame.varobj = frame.down->varobj;
			sfrmInvokeNew->setScopeChainObject(sfrmInvokeNew->getDownStackFrame()->getScopeChainObject());	// ⇒  frame.scopeChain = frame.down->scopeChain;
		}
		if(sfrmInvokeNew->getScopeChainObject() == 0L)	// ⇒  if (!frame.scopeChain)
		{
			sfrmInvokeNew->setScopeChainObject(objParent);	// ⇒ frame.scopeChain = parent;
		}
		break;

	case CFUNC_SCRIPT:
		sfrmInvokeNew->setScopeChainObject(objParent);	// ⇒ frame.scopeChain = parent;
		if(TFW_IS_FLAG(func->getFlags(), EsFunction::F_FUNC_NEED_CALLOBJ))
		{
			if(!sfrmInvokeNew->makeCallObject(_ctxRef))
			{
				ESCASSERT(false);
				return false;
			}
		}
		// ※※※ 実装保留 ※※※ よくわかってないので… ⇒ frame.slots = sp - fun->u.i.nvars;
		break;
	}

	// 関数呼び出し ⇒ ok = native(cx, frame.thisp, argc, frame.argv, &frame.rval); / ok = js_Interpret(cx);
	switch(cfunc)
	{
	case CFUNC_OBJ_CALL:
	case CFUNC_OBJ_CONSTRUCT:
	case CFUNC_NATIVE_CALL:
		{
			_ctxRef->set2ndRetValueSet(false);	// ⇒ cx->rval2set = JS_FALSE;

			EsObject* objThis = (cfunc == CFUNC_NATIVE_CALL) ? sfrmInvokeNew->getThisObject() : 0L;
			EsValue* valCallee = sfrmInvokeNew->getCalleeValue();
			EsValue* valThis = sfrmInvokeNew->getThisValue();
			EsValue* vaArg = sfrmInvokeNew->getArgArray();
			u32 numArg = sfrmInvokeNew->getArgNum();
			EsValue* valRet = sfrmInvokeNew->getFunctionResultValue();
			exprm._paramRun = _ctxRef->getRunParameter();
			switch(cfunc)
			{
			case CFUNC_OBJ_CALL:
				if(!objThis->callOv(_ctxRef, valCallee, valThis, vaArg, numArg, valRet, &exprm)) { isSucceeded = false; }
				break;
			case CFUNC_OBJ_CONSTRUCT:
				if(!objThis->constructOv(_ctxRef, valCallee, valThis, vaArg, numArg, valRet, &exprm)) { isSucceeded = false; }
				break;
			case CFUNC_NATIVE_CALL:
				if(!func->getNativeCall()(_ctxRef, objThis, valCallee, valThis, vaArg, numArg, valRet, &exprm))	{ isSucceeded = false; }
				break;
			}
		}
		break;
	case CFUNC_SCRIPT:
		// ※※※ 未実装 ※※※
		ASSERT(false);
		break;
	}

	if(sfrmInvoke != 0L)	{	*sfrmInvoke = sfrmInvokeNew;	}

	if(isSucceeded)
	{
		// eval などで等しくならない → ASSERT((_sfrmInvoke == _sfrmCur) && (_sfrmCur == sfrmInvokeNew));	// 試験実装．常に _sfrmCur と等しいといえるならば、_sfrmInvoke は削除可能 【r-kishi 2012/02/08】
		ASSERT(_sfrmInvoke == sfrmInvokeNew);	// 試験実装
		if((cfunc != CFUNC_SCRIPT) && !_sfrmInvoke->getFunctionResultValue()->isUnfinished())
		{
			// 即座に終了した関数呼び出しの後処理
			invokeAfter(true);
		}
	}
	else
	{
		// エラーコードを設定する
		if(sfrmInvokeNew->getFunctionResultValue()->isErrorCode() && (_exiCurStep != 0L))
		{
			_ctxRef->addRunError((EsError::Id)sfrmInvokeNew->getFunctionResultValue()->getErrorCode());
		}

		// 関数呼出し語の後処理
		invokeAfter(false);
	}

	return isSucceeded;

FAILED:
	if(TFW_IS_FLAG(flagsSfrm, EsStackFrame::F_NEWSTACK))	{	delete[] vaStack;	}
	return false;
}

/*---------------------------------------------------------------------*//**
	関数呼び出し後処理
**//*---------------------------------------------------------------------*/
bool EsRunEnv::invokeAfter(bool isSucceeded)
{
	ASSERT(_sfrmInvoke == _sfrmCur);	// 試験実装．常に _sfrmCur と等しいといえるならば、_sfrmInvoke は削除可能 【r-kishi 2012/02/08】
	if(!_sfrmCur->postprocForInvoke(_ctxRef, isSucceeded))	{	isSucceeded = false;	}

	// スタックフレームを戻す
	_sfrmCur->becomeGarbage();	// 削除可能
	_sfrmCur = _sfrmCur->getDownStackFrame();
	_ctxRef->setTopStackFrame(_sfrmCur);
	_sfrmInvoke = _sfrmCur;

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	内部関数呼び出し ⇒ js_InternalInvoke
**//*---------------------------------------------------------------------*/
bool EsRunEnv::invokeInternal(EsValue* valReturn, EsObject* obj, const EsValue* valFunc, EsValue* vaStack, u32 numArg, u16 flagsSfrm)
{
	EsValue* varrStackNew = new EsValue[numArg + 2];
	if(varrStackNew == 0L)	{	return false;	}

	varrStackNew[0].copy(valFunc);
	varrStackNew[1].setValue(obj);
	TFW_COPY_MEMORY(&varrStackNew[2], vaStack, (numArg) * sizeof(EsValue));

	EsStackFrame* sfrmInvoke;
	if(!invoke(&sfrmInvoke, varrStackNew, numArg, flagsSfrm | EsStackFrame::F_NEWSTACK))
	{
		return false;
	}
	ASSERT(!sfrmInvoke->getFunctionResultValue()->isUnfinished());	// 想定外
	valReturn->copy(sfrmInvoke->getFunctionResultValue());	// ⇒ *rval = *invokevp;

	return true;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ呼び出し ⇒ js_InvokeConstructor
**//*---------------------------------------------------------------------*/
bool EsRunEnv::invokeConstructor(EsValue* vaStack, u32 numArg, u16 flagsSfrm, bool isClampReturn)
{
	EsObject* objFunc = vaStack[0].isObject() ? vaStack[0].getObject() : 0L;
	EsFunction* func = 0L;
	if(	(objFunc == 0L) ||
		(objFunc->getClass()->getClassType() == EsSysClasses::CLASSTYPE_FUNC) ||
		(objFunc->getObjectOp()->getConstructFunc() == 0L) )
	{
		if(!_ctxRef->convValueToFunction(&func, &vaStack[0], true))	// ⇒ fun = js_ValueToFunction(cx, vp, JSV2F_CONSTRUCT);
		{
			return false;
		}
	}

	// クラス、プロトタイプオブジェクト、親オブジェクトを得る
	const EsClass* cls = EsSysClasses::getClass(EsSysClasses::CLASSTYPE_OBJECT);
	EsObject* objParent = 0L;
	EsObject* objPtype = 0L;
	if(objFunc != 0L)
	{
		EsValue valWk;
		EsValue valPtypeAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROTOTYPE)->getAtom());
		if(!objFunc->callGetPropertyOv(&valWk, _ctxRef, &valPtypeAtom))
		{
			return false;
		}

		if(valWk.isObject())	{	objPtype = valWk.getObject();	}
		objParent = objFunc->slots()->parentObject();

		if(objFunc->getClass()->getClassType() == EsSysClasses::CLASSTYPE_FUNC)
		{
			EsFunction* funcWk = (EsFunction*)objFunc->slots()->privateData();
			if(!funcWk->isInterpreted() && (funcWk->getClass() != 0L))
			{
				cls = funcWk->getConstructClass();
			}
		}
	}

	// 新しいオブジェクトを作成する
	EsObject* objNew = _ctxRef->newObject(cls, objPtype, objParent);
	if(objNew == 0L)
	{
		return false;
	}

	// コンストラクタを実行する
	vaStack[1].setValue(objNew);
	EsStackFrame* sfrmInvoke;
	if(!invoke(&sfrmInvoke, vaStack, numArg, flagsSfrm | EsStackFrame::F_CONSTRUCTING))
	{
		return false;
	}

	// コンストラクタの実行結果を得る
	ASSERT(!sfrmInvoke->getFunctionResultValue()->isUnfinished());	// 想定外
	EsValue valWk(*sfrmInvoke->getFunctionResultValue());
	if(valWk.isPrimitive())
	{
		if(func == 0L)
		{
			_ctxRef->addRunError(EsError::ID_BAD_NEW_RESULT);
			ESCASSERT(false);
			return false;
		}
		vaStack[0].setValue(objNew);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	インライン関数呼び出し ⇒ inline_call:
**//*---------------------------------------------------------------------*/
bool EsRunEnv::callInline(EsFunction* func, EsValue* vaStack, u32 numArg, u16 flagsSfrm)
{
	EsInterlang* ilBody = func->getInterlang();
	EsObject* objThis = vaStack[1].getObject();
	EsObject* objParent = func->slots()->parentObject();

	_ctxRef->prepareCall(
		func,
		ilBody,
		flagsSfrm,
		vaStack,
		numArg,
		func,
		objThis,
		objParent,
		_sfrmCur	);

	return true;
}

/*---------------------------------------------------------------------*//**
	関数定義（試験実装）
**//*---------------------------------------------------------------------*/
bool EsRunEnv::defineFunction(EsFunction* func)
{
	// スコープオブジェクトの作成
	EsObject* objScope = 0L;
	if(TFW_IS_FLAG(func->getFlags(), EsFunction::F_CLOSURE_NULL))	// ⇒ FUN_NULL_CLOSURE(fun)
	{
		objScope = _sfrmCur->getScopeChainObject();	// ⇒ obj2 = fp->scopeChain;
	}
	else
	{
		ASSERT(!TFW_IS_FLAG(func->getFlags(), EsFunction::F_CLOSURE_FLAT));

		if(_sfrmCur->getBlockChainObject() == 0L)
		{
			objScope = _sfrmCur->getScopeChainObject();		// ⇒ obj2 = fp->scopeChain;
		}
		else
		{
			objScope = _sfrmCur->makeScopeChainObject(_ctxRef);	// ⇒ obj2 = js_GetScopeChain(cx, fp);
			if(objScope == 0L)	{	return false;	}
		}
	}

	// クロージャオブジェクトの作成
	if(func->getSlots()->getParentObject() != objScope)
	{
		func = func->clone(_ctxRef, objScope);
		if(func == 0)	{	return false;	}
	}
	_sfrmCur->setScopeChainObject(func);

	EsValue valFunc(func);	// ⇒ rval = OBJECT_TO_JSVAL(obj);

	// Eval コードに入ったとき、関数は一時的（Permanent でない）として定義される必要がある（ECMA 規定）
	u8 spatrbf = TFW_IS_FLAG(_sfrmCur->getFlags(), EsStackFrame::F_EVAL) ? EsScopeProperty::ATRBF_ENUMERATE : EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_PERMANENT;

	// ゲッター、セッターの用意
	EsPropertyGetterFunc funcGetter = g_funcDummyPropertyGetter;
	EsPropertySetterFunc funcSetter = g_funcDummyPropertySetter;
	bool isObjGetter = TFW_IS_FLAG(func->getFlags(), EsFunction::F_OBJ_GETTER);
	bool isObjSetter = TFW_IS_FLAG(func->getFlags(), EsFunction::F_OBJ_SETTER);
	if(isObjGetter || isObjSetter)
	{
		valFunc.setVoid();
		TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_OBJ_GETTER | EsScopeProperty::ATRBF_SHARED, true);
		if(isObjGetter)
		{
			TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_OBJ_GETTER, true);
			funcGetter = reinterpret_cast<EsPropertyGetterFunc>(func);
		}
		if(isObjSetter)
		{
			TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_OBJ_SETTER, true);
			funcSetter = reinterpret_cast<EsPropertySetterFunc>(func);
		}
	}

	EsObject* objParent = _sfrmCur->getVarObject();
	ASSERT(objParent != 0L);

	const EsAtom* atomFunc = func->getAtom();
	EsValue funcid(atomFunc);
	// 多重定義チェック
	EsScopeProperty* spropFound = 0L;
	EsObject* objScopeFound = 0L;
	if(!checkRedeclaration(&spropFound, &objScopeFound, objParent, &funcid, spatrbf, false))
	{
		_sfrmCur->setScopeChainObject(objScope);
		return false;
	}

	bool isSet = (spatrbf == EsScopeProperty::ATRBF_ENUMERATE);
	if(spropFound != 0L)
	{
		if((objParent == objScopeFound) && (objParent->getClass()->getClassType() == EsSysClasses::CLASSTYPE_CALL))
		{
			u8 spatrbfOld = spropFound->getAtrbFlags();
			if(	!TFW_IS_FLAG(spatrbfOld, EsScopeProperty::ATRBF_OBJ_GETTER) &&
				!TFW_IS_FLAG(spatrbfOld, EsScopeProperty::ATRBF_OBJ_SETTER) &&
				((spatrbfOld & (EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_PERMANENT)) == spatrbf)	)
			{
				isSet = true;
			}
		}
	}

	bool isSucceeded;
	if(isSet)
	{
		isSucceeded = objParent->callSetPropertyOv(_ctxRef, &funcid, &valFunc);
	}
	else
	{
		isSucceeded = objParent->definePropertyOv(0L, _ctxRef, &funcid, &valFunc, funcGetter, funcSetter, spatrbf);
	}

	_sfrmCur->setScopeChainObject(objScope);

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	クラスインスタンスの作成（試験実装）
		OP_NAME と OP_NEW の組み合わせ
**//*---------------------------------------------------------------------*/
bool EsRunEnv::makeClassInstance(EsObject** objOut, const EsAtom* atomClassName, EsValue* vaArg, u32 numArg)
{
	// クラスインスタンス作成
	EsScopeProperty* sprop = 0L;
	EsObject* objScope;
	EsObject* objScopeParent;
	EsValue id(atomClassName);
	{
		EsResolver::AutoFlags raf(_ctxRef->getResolver(), EsResolver::RSLVF_QUALIFIED);
		if(!_ctxRef->findPropertyFromScopeChain(&sprop, &objScope, &objScopeParent, &id))
		{
			return false;
		}
	}
	ASSERT(objScope != 0L);
	ASSERT(objScopeParent != 0L);
	if((objScope == 0L) || (objScopeParent == 0L))
	{
		return false;
	}

	// ゲッタの呼び出し
	EsValue valNameVal;
	if(!objScope->isNative() || !objScopeParent->isNative())
	{
		if(!objScope->callGetPropertyOv(&valNameVal, _ctxRef, &id))
		{
			return false;
		}
	}
	else
	{
		if(!callGetPropertyObjself(&valNameVal, _ctxRef, objScope, objScope, sprop))
		{
			return false;
		}
	}

	// コンストラクタ呼び出しのためのスタックを用意する
	EsValue* vaStack = new EsValue[2 + numArg];
	vaStack[0].copy(&valNameVal);
	vaStack[1].setValue(objScope);
	for(u32 i = 0; i < numArg; i++)
	{
		vaStack[2 + i].copy(&vaArg[i]);
	}

	// コンストラクタ呼び出し
	if(!invokeConstructor(vaStack, numArg, EsStackFrame::F_NEWSTACK, false))
	{
		return false;
	}

	*objOut = vaStack[1].isObject() ? vaStack[1].getObject() : 0L;

	return true;
}

/*---------------------------------------------------------------------*//**
	実行停止
**//*---------------------------------------------------------------------*/
void EsRunEnv::stop()
{
	// 解決フラグの解除
	_ctxRef->getResolver()->setResolvFlags(0);	// ⇒ ~JSAutoResolveFlags

	// 終了フラグの設定
	_ctxRef->setStop(true);

	// 終了処理
	end(0L);
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_DEFVAR, OP_DEFCONST
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_defvar(EsOpcodeType ot)
{
	u32 index = getIndex(0);	// ⇒ index = GET_INDEX(regs.pc);
	index += _indexBase;		// インデックスの絶対値化 ⇒ index += atoms - script->atomMap.vector;
	const EsAtom* atom = _ilCur->getAtom(index);
	EsValue spropid(atom);

	EsObject* obj = _sfrmCur->getVarObject();

	// 属性フラグの決定
	u8 spatrbf = EsScopeProperty::ATRBF_ENUMERATE;
	if(!TFW_IS_FLAG(_sfrmCur->getFlags(), EsStackFrame::F_EVAL))
	{
		TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_PERMANENT, true);
	}
	if(ot == OP_DEFCONST)
	{
		TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_READONLY, true);
	}

	// 多重定義チェック
	EsScopeProperty* sprop = 0L;
	EsObject* objScope = 0L;
	if(!checkRedeclaration(&sprop, &objScope, obj, &spropid, spatrbf, false))
	{
		ESCASSERT(false);
		return false;
	}

	// まだ定義されてない場合は定義する
	if(sprop == 0L)
	{
		EsValue valVoid;
		if(!obj->definePropertyOv(&sprop, _ctxRef, &spropid, &valVoid, g_funcDummyPropertyGetter, g_funcDummyPropertySetter, spatrbf))	// ⇒ OBJ_DEFINE_PROPERTY
		{
			return false;
		}
		objScope = obj;
	}

	if(	(_sfrmCur->getFunction() == 0L) &&
		(index < (u32)_sfrmCur->getInterlang()->getFixedSlotNum()) &&	// ⇒ index < GlobalVarCount(fp)
		(obj == objScope) &&
		obj->isNative()	)
	{
		if(	TFW_IS_FLAG(sprop->getAtrbFlags(), EsScopeProperty::ATRBF_PERMANENT) &&
			(sprop->getSlotIndex() < obj->getScope()->getFreeSlotIndex()) &&		// ⇒ SPROP_HAS_VALID_SLOT(sprop, OBJ_SCOPE(obj))
			(sprop->getGetterFunc() == 0L) &&	// ⇒ SPROP_HAS_STUB_GETTER(sprop)
			(sprop->getSetterFunc() == 0L) )
		{
			EsValue valSlotIndex((s32)sprop->getSlotIndex());
			_sfrmCur->setSlot(index, &valSlotIndex);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_DEFFUN
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_deffun()
{
	// 関数を取得する
	EsFunction* func = getFunction(0);

	// スコープオブジェクトの作成
	EsObject* objScope = 0L;
	if(TFW_IS_FLAG(func->getFlags(), EsFunction::F_CLOSURE_NULL))	// ⇒ FUN_NULL_CLOSURE(fun)
	{
		objScope = _sfrmCur->getScopeChainObject();	// ⇒ obj2 = fp->scopeChain;
	}
	else
	{
		ASSERT(!TFW_IS_FLAG(func->getFlags(), EsFunction::F_CLOSURE_FLAT));

		if(_sfrmCur->getBlockChainObject() == 0L)
		{
			objScope = _sfrmCur->getScopeChainObject();		// ⇒ obj2 = fp->scopeChain;
		}
		else
		{
			objScope = _sfrmCur->makeScopeChainObject(_ctxRef);	// ⇒ obj2 = js_GetScopeChain(cx, fp);
			if(objScope == 0L)	{	return false;	}
		}
	}

	// クロージャオブジェクトの作成
	if(func->getSlots()->getParentObject() != objScope)
	{
		func = func->clone(_ctxRef, objScope);
		if(func == 0)	{	return false;	}
	}
	_sfrmCur->setScopeChainObject(func);

	EsValue valFunc(func);	// ⇒ rval = OBJECT_TO_JSVAL(obj);

	// Eval コードに入ったとき、関数は一時的（Permanent でない）として定義される必要がある（ECMA 規定）
	u8 spatrbf = TFW_IS_FLAG(_sfrmCur->getFlags(), EsStackFrame::F_EVAL) ? EsScopeProperty::ATRBF_ENUMERATE : EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_PERMANENT;

	// ゲッター、セッターの用意
	EsPropertyGetterFunc funcGetter = g_funcDummyPropertyGetter;
	EsPropertySetterFunc funcSetter = g_funcDummyPropertySetter;
	bool isObjGetter = TFW_IS_FLAG(func->getFlags(), EsFunction::F_OBJ_GETTER);
	bool isObjSetter = TFW_IS_FLAG(func->getFlags(), EsFunction::F_OBJ_SETTER);
	if(isObjGetter || isObjSetter)
	{
		valFunc.setVoid();
		TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_OBJ_GETTER | EsScopeProperty::ATRBF_SHARED, true);
		if(isObjGetter)
		{
			TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_OBJ_GETTER, true);
			funcGetter = reinterpret_cast<EsPropertyGetterFunc>(func);
		}
		if(isObjSetter)
		{
			TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_OBJ_SETTER, true);
			funcSetter = reinterpret_cast<EsPropertySetterFunc>(func);
		}
	}

	EsObject* objParent = _sfrmCur->getVarObject();
	ASSERT(objParent != 0L);

	const EsAtom* atomFunc = func->getAtom();
	EsValue funcid(atomFunc);
	// 多重定義チェック
	EsScopeProperty* spropFound = 0L;
	EsObject* objScopeFound = 0L;
	if(!checkRedeclaration(&spropFound, &objScopeFound, objParent, &funcid, spatrbf, false))
	{
		_sfrmCur->setScopeChainObject(objScope);
		return false;
	}

	bool isSet = (spatrbf == EsScopeProperty::ATRBF_ENUMERATE);
	if(spropFound != 0L)
	{
		if((objParent == objScopeFound) && (objParent->getClass()->getClassType() == EsSysClasses::CLASSTYPE_CALL))
		{
			u8 spatrbfOld = spropFound->getAtrbFlags();
			if(	!TFW_IS_FLAG(spatrbfOld, EsScopeProperty::ATRBF_OBJ_GETTER) &&
				!TFW_IS_FLAG(spatrbfOld, EsScopeProperty::ATRBF_OBJ_SETTER) &&
				((spatrbfOld & (EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_PERMANENT)) == spatrbf)	)
			{
				isSet = true;
			}
		}
	}

	bool isSucceeded;
	if(isSet)
	{
		isSucceeded = objParent->callSetPropertyOv(_ctxRef, &funcid, &valFunc);
	}
	else
	{
		isSucceeded = objParent->definePropertyOv(0L, _ctxRef, &funcid, &valFunc, funcGetter, funcSetter, spatrbf);
	}

	_sfrmCur->setScopeChainObject(objScope);

	_isContinueStep = true;	// 続けて次を実行
	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_DEFFUN_FC
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_deffunfc()
{
	// 関数を取得する
	EsFunction* func = getFunction(0);

	// クロージャオブジェクトの作成
	EsObject* objFuncClosure = func->makeFlatClosure(_ctxRef, _ctxRef->getTopStackFrame()->getScopeChainObject());
	if(objFuncClosure == 0L)	{	return false;	}

	EsValue valFunc(func);	// ⇒ rval = OBJECT_TO_JSVAL(obj);

	// Eval コードに入ったとき、関数は一時的（Permanent でない）として定義される必要がある（ECMA 規定）
	u8 spatrbf = TFW_IS_FLAG(_sfrmCur->getFlags(), EsStackFrame::F_EVAL) ? EsScopeProperty::ATRBF_ENUMERATE : EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_PERMANENT;

	// ゲッター、セッターによる判定
	bool isObjGetter = TFW_IS_FLAG(func->getFlags(), EsFunction::F_OBJ_GETTER);
	bool isObjSetter = TFW_IS_FLAG(func->getFlags(), EsFunction::F_OBJ_SETTER);
	if(isObjGetter || isObjSetter)
	{
		TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_OBJ_GETTER, isObjGetter);
		TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_OBJ_SETTER, isObjSetter);
		TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_SHARED, true);
		valFunc.setVoid();
	}

	EsObject* objParent = _sfrmCur->getVarObject();
	ASSERT(objParent != 0L);

	const EsAtom* atomFunc = func->getAtom();
	EsValue funcid(atomFunc);
	// 多重定義チェック
	EsScopeProperty* spropFound = 0L;
	EsObject* objScopeFound = 0L;
	if(!checkRedeclaration(&spropFound, &objScopeFound, objParent, &funcid, spatrbf, false))
	{
		return false;
	}

	if(spatrbf == EsScopeProperty::ATRBF_ENUMERATE)
	{
		if(!objParent->callSetPropertyOv(
			_ctxRef,
			&funcid,
			&valFunc	))
		{
			return false;
		}
	}
	else
	{
		EsScopeProperty* sprop;
		if(!objParent->definePropertyOv(
			&sprop,
			_ctxRef,
			&funcid,
			&valFunc,
			isObjGetter ? (EsPropertyGetterFunc)func : g_funcDummyPropertyGetter,
			isObjSetter ? (EsPropertySetterFunc)func : g_funcDummyPropertySetter,
			spatrbf	))
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_DEFLOCALFUN
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_deflocalfun()
{
	// 関数を取得する
	EsFunction* func = getFunction(2);	// ⇒ LOAD_FUNCTION(SLOTNO_LEN);
	ASSERT(!TFW_IS_FLAG(func->getFlags(), EsFunction::F_CLOSURE_FLAT));

	// クロージャオブジェクトの作成
	if(TFW_IS_FLAG(func->getFlags(), EsFunction::F_CLOSURE_NULL))
	{
		func = func->clone(_ctxRef, _sfrmCur->getScopeChainObject());
		if(func == 0)	{	return false;	}
	}
	else
	{
		EsObject* objParent = _sfrmCur->makeScopeChainObject(_ctxRef);	// ⇒ parent = js_GetScopeChain(cx, fp);
		if(objParent == 0L)	{	return false;	}

		if(func->getSlots()->getParentObject() != objParent)
		{
			func = func->clone(_ctxRef, objParent);
			if(func == 0)	{	return false;	}
		}
	}

	// スタックフレームのスロットにクロージャオブジェクトを格納
	u32 slotidx = getIndex(0);	// ⇒ slot = GET_SLOTNO(regs.pc);
	EsValue valFunc(func);
	_sfrmCur->setSlot(slotidx, &valFunc);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_DEFLOCALFUN_FC
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_deflocalfunfc()
{
	// 関数を取得する
	EsFunction* func = getFunction(0);

	// クロージャオブジェクトの作成
	EsObject* objFuncClosure = func->makeFlatClosure(_ctxRef, _ctxRef->getTopStackFrame()->getScopeChainObject());
	if(objFuncClosure == 0L)	{	return false;	}

	// スタックフレームのスロットにクロージャオブジェクトを格納
	u32 slotidx = getIndex(0);	// ⇒ slot = GET_SLOTNO(regs.pc);
	EsValue valFunc(func);
	_sfrmCur->setSlot(slotidx, &valFunc);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_LAMBDA
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_lambda()
{
	// 関数を取得する
	EsFunction* func = getFunction(0);

	// クロージャオブジェクトの作成
	if(TFW_IS_FLAG(func->getFlags(), EsFunction::F_CLOSURE_NULL))	// ⇒ FUN_NULL_CLOSURE(fun)
	{
		func = func->clone(_ctxRef, _sfrmCur->getScopeChainObject());
		if(func == 0)	{	return false;	}
	}
	else
	{
		EsObject* objParent = _sfrmCur->makeScopeChainObject(_ctxRef);	// ⇒ parent = js_GetScopeChain(cx, fp);
		if(objParent == 0L)	{	return false;	}

		if(func->getSlots()->getParentObject() != objParent)
		{
			func = func->clone(_ctxRef, objParent);
			if(func == 0)	{	return false;	}
		}
	}

	EsValue valFunc(func);
	_regsCur->pushOperand(&valFunc);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_LAMBDA_FC
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_lambdafc()
{
	EsFunction* func = getFunction(0);
	func = func->makeFlatClosure(_ctxRef, _ctxRef->getTopStackFrame()->getScopeChainObject());

	EsValue valFunc(func);
	_regsCur->pushOperand(&valFunc);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_CALL, OP_EVAL, OP_APPLY
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_call(EsOpcodeType ot)
{
	s32 numArg = (s32)_regsCur->pc()->getU16(1);
	EsValue* vaStack = (EsValue*)(_regsCur->sp(- (numArg + 2)));

	// インライン実行
	if(vaStack[0].isFunctionObject())	// ⇒ if (VALUE_IS_FUNCTION(cx, lval)) {
	{
		EsObject* obj = vaStack[0].getObject();
		EsFunction* func = (EsFunction*)obj->slots()->privateData();
		if(func->isInterpreted())
		{
			return callInline(func, vaStack, numArg, 0);
		}
	}

	// 関数呼び出し
	EsStackFrame* sfrmInvoke;
	bool isSucceeded = invoke(&sfrmInvoke, vaStack, (u32)numArg, 0);

	// 未終了のメソッドの場合、そのまま抜ける
	if(isSucceeded && sfrmInvoke->getFunctionResultValue()->isUnfinished())
	{
		return true;
	}

	// スタックポインタを巻き戻す
	_regsCur->advanceSp(- (numArg + 2) + 1);	// ⇒ regs.sp = vp + 1;

	if(!isSucceeded)
	{
		return false;
	}

	return stepSub_callAfter(ot);
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_CALL, OP_EVAL, OP_APPLY 後処理
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_callAfter(EsOpcodeType ot)
{
	// 戻り値の処理
	if(_ctxRef->is2ndRetValueSet())
	{
		_regsCur->pushOperand(_ctxRef->get2ndRetValue());

		// ⇒ ELEMENT_OP(-1, OBJ_GET_PROPERTY(cx, obj, id, &rval));
		EsObject* obj;
		if(!_regsCur->fetchObject(&obj, -2)) { return false; }
		EsValue spropid;
		if(!_regsCur->fetchId(&spropid, -1)) { return false; }
		EsValue valWk;
		if(!obj->callGetPropertyOv(&valWk, _ctxRef, &spropid)) { return false; }

		_regsCur->advanceSp(- 1);				// ⇒ regs.sp--;
		_regsCur->storeOperand(-1, &valWk);

		_ctxRef->set2ndRetValueSet(false);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_NEW
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_new()
{
	s32 numArg = (s32)_regsCur->pc()->getU16(1);		// ⇒ argc = GET_ARGC(regs.pc);
	EsValue* vaStack = (EsValue*)(_regsCur->sp(- (numArg + 2)));

	// コンストラクタ実行
	if(vaStack[0].isFunctionObject())	// ⇒ if (VALUE_IS_FUNCTION(cx, lval)) {
	{
		EsFunction* func = (EsFunction*)vaStack[0].getObject();

		if(func->isInterpreted())
		{
			EsValue valPtypeAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROTOTYPE)->getAtom());
			if(!func->callGetPropertyOv(&vaStack[1], _ctxRef, &valPtypeAtom))	// ⇒ OBJ_GET_PROPERTY(cx, obj, ATOM_TO_JSID(cx->runtime->atomState.classPrototypeAtom), &vp[1])
			{
				return false;
			}

			EsObject* objNew = _ctxRef->newObject(
				EsSysClasses::getClass(EsSysClasses::CLASSTYPE_OBJECT),
				vaStack[1].isObject() ? vaStack[1].getObject() : 0L,
				func->slots()->parentObject()	);
			if(objNew == 0L)
			{
				return false;
			}

			vaStack[1].setValue(objNew);
			return callInline(func, vaStack, numArg, EsStackFrame::F_CONSTRUCTING);
		}
	}
	if(!invokeConstructor(vaStack, numArg, 0, false))	// ⇒ js_InvokeConstructor(cx, argc, JS_FALSE, vp)
	{
		return false;
	}

	_regsCur->advanceSp(- (numArg + 2) + 1);	// ⇒ regs.sp = vp + 1;
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_RETURN
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_return(EsOpcodeType ot)
{
	*_sfrmCur->getFunctionResultValue() = _regsCur->popOperand();	// ⇒ fp->rval = POP_OPND()
	return stepSub_retval(ot);
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_RETRVAL, OP_STOP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_retval(EsOpcodeType ot)
{
	// ※※※ 実装保留 ※※※	⇒ if (fp->imacpc)

	if(TFW_IS_FLAG(_sfrmCur->getFlags(), EsStackFrame::F_CONSTRUCTING) && _sfrmCur->getFunctionResultValue()->isPrimitive())
	{
		if(_sfrmCur->getFunction() == 0L)
		{
			_ctxRef->addRunError(EsError::ID_BAD_NEW_RESULT);
			return false;
		}
		EsValue valThisObject(_sfrmCur->getThisObject());
		_sfrmCur->getFunctionResultValue()->copy(&valThisObject);	// ⇒ fp->rval = OBJECT_TO_JSVAL(fp->thisp);
	}

	if(_depthCall > 0)	// 関数停止の場合
	{
		if(_sfrmCur->getCallerStackRegister() == 0L)	// invoke の場合（インライン関数（call）ではない場合）
		{
			///ASSERT(_sfrmInvoke == _sfrmCur);	// eval などで等しくならない → 試験実装．常に _sfrmCur と等しいといえるならば、_sfrmInvoke は削除可能 【r-kishi 2012/02/08】
			EsStackFrame* sfrmInvoke = _sfrmCur;
			EsValue* valResult = _valResult;

			// ディスプレイスタックフレームを戻す
			_ctxRef->setDisplayStackFrame(_ilCur->getNestDepth(), _sfrmCur->getPrevDisplayStackFrame());	// ⇒ cx->display[script->staticLevel] = fp->displaySave;

			// 後処理
			_ctxRef->disposeCall();

			// 返答値のコピー
			if(valResult != 0L)
			{
				valResult->copy(sfrmInvoke->getFunctionResultValue());	// ⇒ *result = frame.rval;
			}

			// 関数呼び出しの後処理
			invokeAfter(true);

			// スタックポインタを巻き戻す
			_regsCur->advanceSp(- ((s32)sfrmInvoke->getArgNum() + 2) + 1);	// ⇒ regs.sp = vp + 1;

			// OP_CALL, OP_EVAL, OP_APPLY, OP_SETCALL 後処理
			switch(_otCurStep)
			{
			case OP_CALL:
			case OP_EVAL:
			case OP_APPLY:
				if(!stepSub_callAfter(_otCurStep))
				{
					return false;
				}
				break;
			case OP_SETCALL:
				if(!stepSub_setcallAfter())
				{
					return false;
				}
				break;
			}
		}
		else	// インライン関数終了の場合 ⇒ inline_return
		{
			EsStackFrame* sfrmCall = _sfrmCur;
			_ctxRef->setDisplayStackFrame(_ilCur->getNestDepth(), _sfrmCur->getPrevDisplayStackFrame());	// ⇒ cx->display[script->staticLevel] = fp->displaySave;

			// 後処理
			_ctxRef->disposeCall();	// ⇒ cx->fp = fp = fp->down;

			// 戻り値をスタックレジスタに格納する
			EsStackRegister* regsCaller = sfrmCall->getCallerStackRegister();
			regsCaller->advanceSp(- 1 - sfrmCall->getArgNum());					// ⇒ regs.sp -= 1 + (size_t) ifp->frame.argc;
			regsCaller->storeOperand(-1, sfrmCall->getFunctionResultValue());	// ⇒ regs.sp[-1] = fp->rval;
		}
	}
	else	// main の終了
	{
		_ctxRef->setStop(true);	// 停止セット
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_PUSH
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_push()
{
	EsValue valVoid;
	_regsCur->pushOperand(&valVoid);	// ⇒ PUSH_OPND(JSVAL_VOID);

	_isContinueStep = true;	// 続けて次を実行
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_POP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_pop()
{
	_regsCur->advanceSp(-1);			// ⇒ _regsCur.sp--;

	_isContinueStep = true;	// 続けて次を実行
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_POPN
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_popn()
{
	_regsCur->advanceSp(- (s32)_regsCur->pc()->getU16(1));	// regs.sp -= GET_UINT16(regs.pc);

	_isContinueStep = true;	// 続けて次を実行
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_POPV
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_popv()
{
	*_sfrmCur->getFunctionResultValue() = _regsCur->popOperand();	// ⇒ fp->rval = POP_OPND()

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_BINDNAME
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_bindname()
{
	EsObject* objWk = _sfrmCur->getScopeChainObject();
	if(objWk->getSlots()->getParentObject() == 0L)
	{
		EsValue valObjWk(objWk);
		_regsCur->pushOperand(&valObjWk);
		return true;
	}

	// ※※※ 実装保留 ※※※ プロパティキャッシュ ⇒ if (JS_LIKELY(OBJ_IS_NATIVE(obj))) ～ PROPERTY_CACHE_TEST

	EsValue valAtom(getAtom(0));
	objWk = _ctxRef->findIdentifierBase(_sfrmCur->getScopeChainObject(), &valAtom);
	if(objWk == 0L)
	{
		return false;
	}
	
	EsValue valObjWk(objWk);
	_regsCur->pushOperand(&valObjWk);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_NAME, OP_CALLNAME
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_getname(EsOpcodeType ot)
{
	// ※※※ 実装保留 ※※※ キャッシュ機構 ⇒ JSPropertyCache

	// プロパティを探す
	const EsAtom* atom = getAtom(0);	// アトム取得
	EsValue propid(atom);
	EsScopeProperty* sprop = 0L;
	EsObject* objScope;
	EsObject* objScopeParent;
	if(!_ctxRef->findPropertyFromScopeChain(&sprop, &objScope, &objScopeParent, &propid))
	{
		return false;
	}
	if(sprop == 0L)
	{
		EsOpcodeType otNext;
		if(_regsCur->pc()->peekNextOpcode(&otNext, 0L, _ctxRef, OP_NAME))	// 次の命令コードを取得する ⇒ op2 = js_GetOpcode(cx, script, regs.pc + JSOP_NAME_LENGTH)
		{
			if(otNext == OP_TYPEOF)
			{
				EsValue valVoid;
				_regsCur->pushOperand(&valVoid);
				return true;
			}
		}
		_ctxRef->addRunError(EsError::ID_NO_PROPERTY);
		ESCASSERT(false);
		return false;
	}

	// ゲッタの呼び出し
	EsValue valNameVal;
	if(!objScope->isNative() || !objScopeParent->isNative())
	{
		objScopeParent->dropPropertyOv(_ctxRef, sprop);
		if(!objScope->callGetPropertyOv(&valNameVal, _ctxRef, &propid))
		{
			return false;
		}
	}
	else
	{
		if(!callGetPropertyObjself(&valNameVal, _ctxRef, objScope, objScopeParent, sprop))	// ⇒ NATIVE_GET(cx, obj, obj2, sprop, &rval);
		{
			return false;
		}
		objScopeParent->dropPropertyOv(_ctxRef, sprop);
	}

	// レジスタにプッシュする
	_regsCur->pushOperand(&valNameVal);
	if(ot == OP_CALLNAME)
	{
		EsValue valScopeObj(objScope);
		_regsCur->pushOperand(&valScopeObj);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_SETNAME, OP_SETPROP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_setname(EsOpcodeType ot)
{
	// レジスタからオブジェクトを取得
	EsValue valNameVal = _regsCur->fetchOperand(-1);
	EsValue valNameObj = _regsCur->fetchOperand(-2);
	EsObject* objName;
	_regsCur->convValueToObject(&objName, -2, &valNameObj);

	// ※※※ 実装保留 ※※※ キャッシュ機構 ⇒ JSPropertyCache

	// セッタの呼び出し
	const EsAtom* atom = getAtom(0);	// アトム取得
	EsValue spropid(atom);
	if(!objName->callSetPropertyOv(_ctxRef, &spropid, &valNameVal))
	{
		return false;
	}

	// 結果の保存 ⇒ END_SET_CASE_STORE_RVAL
	const s32 spofsDecrement = 2;
	if(skipPopAfterSet(OP_SETPROP, spofsDecrement))	// ⇒ SKIP_POP_AFTER_SET(OP##_LENGTH, spdec);
	{
		return true;
	}
	valNameVal = _regsCur->fetchOperand(-1);
	_regsCur->advanceSp(-(spofsDecrement - 1));
	_regsCur->storeOperand(-1, &valNameVal);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_GETELEM
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_getelem()
{
	EsValue valObj = _regsCur->fetchOperand(-2);
	EsValue valRet = _regsCur->fetchOperand(-1);

	// 文字列の１文字抽出（string[index] など）
	if(valObj.isString() && valRet.isInt())
	{
		const VcString* str = valObj.getString();
		s32 ival = valRet.getInt();
		if(ival < str->getLength())
		{
			VcString strRetInt(str->getAt(ival));
			str = _ctxRef->newString(&strRetInt);	// ⇒ str = js_GetUnitString(cx, str, (size_t)i);
			if(str == 0L)	{	return false;	}
			_regsCur->advancePc(-1);
			EsValue valStr(str);
			_regsCur->storeOperand(-1, &valStr);
			return true;
		}
	}

	EsObject* obj;
	EsValue id;
	_regsCur->convValueToObject(&obj, -2, &valObj);
	if(valRet.isInt())
	{
		if(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ARRAY)
		{
			ASSERT(obj->getObjType() == OBJT_ARRAY);
			EsArray* arr = (EsArray*)obj;
			if(arr->isIntIndexedArray())	// ⇒ if (OBJ_IS_DENSE_ARRAY(cx, obj))
			{
				// 配列インデックスを得る
				s32 index = valRet.getInt();
				// 配列要素ゲット
				if(!arr->getElementForRunOp(&valRet, index))
				{
					return false;
				}

				// ホール以外であったら終了へ
				if(!valRet.isHole())
				{
					goto END;
				}

				// 再読み込みへ
				valRet = _regsCur->fetchOperand(-1);
			}
		}
		id.copy(&valRet);
	}
	else
	{
		// 文字列 ID 化
		if(!_ctxRef->convValueToStringId(&id, &valRet))	// ⇒ if (!js_InternNonIntElementId(cx, obj, rval, &id))
		{
			return false;
		}
	}

	// ID からアクセサ値を取得
	if(!obj->callGetPropertyOv(&valRet, _ctxRef, &id))
	{
		return false;
	}

END:
	_regsCur->advanceSp(-1);
	_regsCur->storeOperand(-1, &valRet);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_CALLELEM
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_callelem()
{
	// ⇒ ELEMENT_OP(-1, js_GetMethod(cx, obj, id, false, &rval));
	EsObject* obj;
	if(!_regsCur->fetchObject(&obj, -2)) { return false; }
	EsValue spropid;
	if(!_regsCur->fetchId(&spropid, -1)) { return false; }
	EsValue valMethod;
	if(!obj->getMethod(&valMethod, _ctxRef, &spropid)) { return false; }

	_regsCur->storeOperand(-2, &valMethod);
	EsValue valObj(obj);
	_regsCur->storeOperand(-1, &valObj);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_SETELEM
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_setelem()
{
	EsValue valRet = _regsCur->fetchOperand(-1);
	EsValue id;
	if(!_regsCur->fetchId(&id, -2)) { return false; }	// ⇒ FETCH_ELEMENT_ID(obj, -2, id);
	EsObject* obj;
	if(!_regsCur->fetchObject(&obj, -3)) { return false; }

	bool isDoSet = false;
	if(id.isInt())	// ⇒ JSID_IS_INT(id)
	{
		if(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ARRAY)
		{
			// 配列の場合
			ASSERT(obj->getObjType() == OBJT_ARRAY);
			EsArray* arr = (EsArray*)obj;
			if(arr->isIntIndexedArray())	// ⇒ OBJ_IS_DENSE_ARRAY(cx, obj)
			{
				// 配列インデックスを得る
				s32 index = id.getInt();
				// 配列要素セット
				isDoSet = arr->setElementForRunOp(index, &valRet);
			}
		}
	}
	if(!isDoSet)
	{
		obj->callSetPropertyOv(_ctxRef, &id, &valRet);
	}

	// 結果の保存 ⇒ END_SET_CASE_STORE_RVAL
	const s32 spofsDecrement = 3;
	if(skipPopAfterSet(OP_SETPROP, spofsDecrement))	// ⇒ SKIP_POP_AFTER_SET(OP##_LENGTH, spdec);
	{
		return true;
	}
	valRet = _regsCur->fetchOperand(-1);
	_regsCur->advanceSp(-(spofsDecrement - 1));
	_regsCur->storeOperand(-1, &valRet);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_ENUMELEM
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_enumelem()
{
	EsValue valRes = _regsCur->fetchOperand(-3);	// ⇒ rval = FETCH_OPND(-3);
	EsObject* obj;
	if(!_regsCur->fetchObject(&obj, -2))	{	return false;	}	// ⇒ FETCH_OBJECT(cx, -2, lval, obj);
	EsValue id;
	if(!_regsCur->fetchId(&id, -1))			{	return false;	}	// ⇒ FETCH_ELEMENT_ID(obj, -1, id);

	if(!obj->callSetPropertyOv(_ctxRef, &valRes, &id))
	{
		return false;
	}

	_regsCur->advanceSp(-3);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン -
		OP_GETPROP, OP_GETXPROP,
		OP_GETTHISPROP, OP_GETARGPROP, OP_GETLOCALPROP, OP_LENGTH
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_getprop(EsOpcodeType ot)
{
	s32 idxAtom;
	EsValue valProp;
	EsObject* objProp;
	switch(ot)
	{
	case OP_GETPROP:
	case OP_GETXPROP:
		idxAtom = 0;
		valProp = _regsCur->fetchOperand(-1);
		_regsCur->convValueToObject(&objProp, -1, &valProp);
		break;

	case OP_GETTHISPROP:
		idxAtom = 0;
		objProp = getThisObjectForFrame();
		if(objProp == 0L)
		{
			return false;
		}
		{
			EsValue valNull(EsValue::VTYPE_NULL);
			_regsCur->push(&valNull);
		}
		break;

	case OP_GETARGPROP:
		idxAtom = 2;	// ⇒ i = ARGNO_LEN
		{
			u32 slotidx = getIndex(0);		// ⇒ slot = GET_ARGNO(regs.pc)
			ASSERT(slotidx < _sfrmCur->getFunction()->getLocalVarNum(EsFunction::LVK_ARG));	// ⇒ JS_ASSERT(slot < fp->fun->nargs)
			_regsCur->pushOperand(_sfrmCur->getArg(slotidx));	// ⇒ PUSH_OPND(fp->argv[slot])
		}
		valProp = _regsCur->fetchOperand(-1);
		_regsCur->convValueToObject(&objProp, -1, &valProp);
		break;

	case OP_GETLOCALPROP:
		idxAtom = 2;	// ⇒ i = SLOTNO_LEN
		{
			u32 slotidx = getIndex(0);		// ⇒ slot = GET_SLOTNO(regs.pc)
			ASSERT(slotidx < _ilCur->getSlotNum());		// ⇒ JS_ASSERT(slot < script->nslots)
			_regsCur->pushOperand(_sfrmCur->getSlot(slotidx));		// ⇒ PUSH_OPND(fp->slots[slot])
		}
		valProp = _regsCur->fetchOperand(-1);
		_regsCur->convValueToObject(&objProp, -1, &valProp);
		break;

	case OP_LENGTH:
		valProp = _regsCur->fetchOperand(- 1);
		if(valProp.isString())
		{
			// 文字列の長さを保存して終了
			const VcString* str = valProp.getString();
			EsValue valStrLen(str->getLength());
			_regsCur->storeOperand(-1, &valStrLen);
			return true;
		}
		else
		{
			if(valProp.isObject())
			{
				objProp = valProp.getObject();
				if(objProp->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ARRAY)	// ⇒ OBJ_IS_ARRAY
				{
					// 配列の長さを保存して終了
					EsValue* valLength;
					((EsArray*)objProp)->getLengthValueFromSlot(&valLength);
					_regsCur->storeOperand(-1, valLength);
					return true;
				}
			}

			_regsCur->convValueToObject(&objProp, -1, &valProp);
		}
		break;
	}
	ASSERT(objProp != 0L);
	if(objProp == 0L)	{	return false;	}

	const EsAtom* atom;
	if(ot == OP_LENGTH)
	{
		atom = EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom();
	}
	else
	{
		atom = getAtom(idxAtom);
	}
	ASSERT(atom != 0L);
	EsValue spropid(atom);

	if(!objProp->callGetPropertyOv(&valProp, _ctxRef, &spropid))
	{
		ESCASSERT(false);
		return false;
	}
	_regsCur->storeOperand(-1, &valProp);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_CALLPROP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_callprop()
{
	EsObject* objL;
	EsValue valL = _regsCur->fetchOperand(-1);
	s32 ival;
	if(valL.isPrimitive())
	{
		if(valL.isString())
		{
			ival = EsSysPrototypes::PTYPEKIND_STRING;	// ⇒ i = JSProto_String;
		}
		else if(valL.isNumber())
		{
			ival = EsSysPrototypes::PTYPEKIND_NUMBER;	// ⇒ i = JSProto_Number;
		}
		else if(valL.isBoolean())
		{
			ival = EsSysPrototypes::PTYPEKIND_BOOLEAN;	// ⇒ i = JSProto_Boolean;
		}
		else
		{
			_ctxRef->addRunError(EsError::ID_NULL_OR_UNDEFINED);	// ⇒ js_ReportIsNullOrUndefined
			return false;
		}

		EsValue valPtypeKind(ival);
		if(!EsObject::getClassPrototype(&objL, _ctxRef, 0L, &valPtypeKind))
		{
			return false;
		}
	}
	else if(valL.isObject())
	{
		objL = valL.getObject();
	}
	else
	{
		ASSERT(false);
		return false;
	}
	
	// ※※※ 実装保留 ※※※ キャッシュ機構？ ⇒ js_GetProtoIfDenseArray, PROPERTY_CACHE_TEST ...

	const EsAtom* atom = getAtom(0);	// ⇒ LOAD_ATOM(0);
	EsValue id(atom);
	EsValue valNull(EsValue::VTYPE_NULL);
	_regsCur->push(&valNull);		// ⇒ PUSH(JSVAL_NULL);
	EsValue valR;
	if(!valL.isPrimitive())
	{
		if(!objL->getMethod(&valR, _ctxRef, &id))
		{
			return false;
		}
		if(valR.isVoid())	// メソッドが見つからなかった場合
		{
			EsOpcodeType otNext;
			if(_regsCur->pc()->peekNextOpcode(&otNext, 0L, _ctxRef, OP_CALLPROP) && (otNext == OP_CALL))	// 次の命令コードを取得する ⇒ op2 = js_GetOpcode(cx, script, regs.pc + JSOP_NAME_LENGTH)
			{
				//_ctxRef->addRunError(EsError::ID_NO_PROPERTY);
				TRACE("not found property: name=%s\n", (atom->getType() == EsAtom::ATYPE_STRING) ? atom->getString()->getRaw() : "");
			}
		}
		EsValue valObjL(objL);
		_regsCur->storeOperand(-1, &valObjL);		// ⇒ STORE_OPND(-1, OBJECT_TO_JSVAL(objL));
		_regsCur->storeOperand(-2, &valR);
	}
	else
	{
		if(!objL->callGetPropertyBase(&valR, _ctxRef, &id))
		{
			return false;
		}
		_regsCur->storeOperand(-1, &valL);
		_regsCur->storeOperand(-2, &valR);
	}

	// 必要に応じて、プリミティブ値の valL をオブジェクトに包み込む
	if(valL.isPrimitive())
	{
		EsObject* objR = valR.isObject() ? valR.getObject() : 0L;
		const EsFunction* funcR = (objR != 0L) ? (const EsFunction*)objR->getSlots()->getPrivateData() : 0L;
		if((!valR.isFunctionObject()) || !funcR->testPrimitiveThisValue(&valL))
		{
			if(!_ctxRef->convPrimitiveValueToObject(_regsCur->sp(-1)))	// ⇒ if (!js_PrimitiveToObject(cx, &regs.sp[-1]))
			{
				return false;
			}
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_GETGVAR, OP_CALLGVAR
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_getgvar(EsOpcodeType ot)
{
	u32 slotidx = getIndex(0);
	ASSERT(slotidx < _sfrmCur->getSlotNum());

	const EsValue* valGvarIdx = _sfrmCur->getSlot(slotidx);
	ASSERT(valGvarIdx != 0L);
	if(valGvarIdx->isNull())
	{
		restep((ot == OP_GETGVAR) ? OP_NAME : OP_CALLNAME, _exiCurStep);
	}
	slotidx = valGvarIdx->getInt();
	EsObject* objGlobal = _sfrmCur->getVarObject();
	const EsValue* valGvar = objGlobal->getSlots()->getValue(slotidx);
	_regsCur->pushOperand(valGvar);
	if(ot == OP_CALLGVAR)
	{
		EsValue valGlobalObj(objGlobal);
		_regsCur->pushOperand(&valGlobalObj);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_SETGVAR
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_setgvar()
{
	u32 slotidx = getIndex(0);
	ASSERT(slotidx < _sfrmCur->getSlotNum());

	EsValue valGvar = _regsCur->fetchOperand(- 1);
	EsObject* objGlobal = _sfrmCur->getVarObject();
	EsValue* valGvarIdx = _sfrmCur->getSlot(slotidx);
	ASSERT(valGvarIdx != 0L);
	if(valGvarIdx->isNull())
	{
		EsValue valAtom(getAtom(0));
		if(!objGlobal->callSetPropertyOv(_ctxRef, &valAtom, &valGvar))
		{
			return false;
		}
	}
	else
	{
		slotidx = valGvarIdx->getInt();
		ES_LOCK_OBJ(ctx, obj);
		objGlobal->writeSlotValueLockedBarrier(slotidx, &valGvar);
		ES_UNLOCK_OBJ(ctx, obj);
	}
	skipPopAfterSet(OP_SETGVAR, 1);					// ⇒ SKIP_POP_AFTER_SET(OP##_LENGTH, 1);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_GETLOCAL, OP_CALLLOCAL
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_getlocal(EsOpcodeType ot)
{
	u32 slotidx = getIndex(0);							// ⇒ slot = GET_SLOTNO(regs.pc);
	ASSERT(slotidx < _ilCur->getSlotNum());				// ⇒ JS_ASSERT(slot < script->nslots);

	const EsValue* val = _sfrmCur->getSlot(slotidx);
	_regsCur->pushOperand(val);							// ⇒ PUSH_OPND(fp->slots[slot]);
	if(ot == OP_CALLLOCAL)
	{
		EsValue valNull(EsValue::VTYPE_NULL);
		_regsCur->pushOperand(&valNull);	// ⇒ PUSH_OPND(JSVAL_NULL);
	}

	#if SHORTEN_STEP
		_isContinueStep = true;	// 続けて次を実行
	#endif
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_SETLOCAL
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_setlocal()
{
	u32 slotidx = getIndex(0);							// ⇒ slot = GET_SLOTNO(regs.pc);
	ASSERT(slotidx < _ilCur->getSlotNum());				// ⇒ JS_ASSERT(slot < script->nslots);

	EsValue* valLocal = _sfrmCur->getSlot(slotidx);		// ⇒ vp = &fp->slots[slot];
	*valLocal = _regsCur->fetchOperand(-1);				// ⇒ *vp = FETCH_OPND(-1);
	skipPopAfterSet(OP_SETLOCAL, 1);					// ⇒ SKIP_POP_AFTER_SET(OP##_LENGTH, 1);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_SETLOCALPOP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_setlocalpop()
{
	u32 slotidx = getIndex(0);								// ⇒ slot = GET_UINT16(regs.pc);
	ASSERT((slotidx + 1) < _ilCur->getSlotNum());			// ⇒ JS_ASSERT(slot + 1 < script->nslots);

	EsValue val = _regsCur->popOperand();
	_sfrmCur->setSlot(slotidx, &val);						// ⇒ fp->slots[slot] = POP_OPND();

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_GETARG, OP_CALLARG
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_getarg(EsOpcodeType ot)
{
	u32 slotidx = getIndex(0);		// ⇒ slot = GET_ARGNO(regs.pc);
	ASSERT(slotidx < _sfrmCur->getFunction()->getLocalVarNum(EsFunction::LVK_ARG));	// ⇒ JS_ASSERT(slot < fp->fun->nargs);

	_regsCur->pushOperand(_sfrmCur->getArg(slotidx));	// ⇒ PUSH_OPND(fp->argv[slot]);
	if(ot == OP_CALLARG)
	{
		EsValue valNull(EsValue::VTYPE_NULL);
		_regsCur->pushOperand(&valNull);	// ⇒ PUSH_OPND(JSVAL_NULL);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_SETARG
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_setarg()
{
	u32 slotidx = getIndex(0);		// ⇒ slot = GET_ARGNO(regs.pc);
	ASSERT(slotidx < _sfrmCur->getFunction()->getLocalVarNum(EsFunction::LVK_ARG));	// ⇒ JS_ASSERT(slot < fp->fun->nargs);

	EsValue* valArg = _sfrmCur->getArg(slotidx);		// ⇒ vp = &fp->argv[slot];
	*valArg = _regsCur->fetchOperand(-1);				// ⇒ *vp = FETCH_OPND(-1);
	skipPopAfterSet(OP_SETARG, 1);						// ⇒ SKIP_POP_AFTER_SET(OP##_LENGTH, 1);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_GETUPVAR, OP_CALLUPVAR
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_getupvar(EsOpcodeType ot)
{
	s32 upvaridx = getIndex(0);		// ⇒ index = GET_UINT16(regs.pc);
	const EsUpvarInfo* upvari = _ilCur->getUpvarInfo(upvaridx);	// ⇒ JSUpvarArray *uva = JS_SCRIPT_UPVARS(script), uva->vector[index]
	const EsValue* valUpvar = getUpvar(_ilCur->getNestDepth(), upvari);
	_regsCur->pushOperand(valUpvar);

	if(ot == OP_CALLUPVAR)
	{
		EsValue valNull(EsValue::VTYPE_NULL);
		_regsCur->pushOperand(&valNull);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_GETDSLOT, OP_CALLDSLOT
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_getdslot(EsOpcodeType ot)
{
	EsObject* obj = _sfrmCur->getCalleeObject();
	ASSERT(obj != 0L);
	s32 dslotidx = getIndex(0);		// ⇒ index = GET_UINT16(regs.pc);
	_regsCur->pushOperand(obj->getSlots()->getDvalue(dslotidx));

	if(ot == OP_CALLDSLOT)
	{
		EsValue valNull(EsValue::VTYPE_NULL);
		_regsCur->pushOperand(&valNull);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_SETCONST
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_setconst()
{
	const EsAtom* atom = getAtom(0);
	EsObject* obj = _sfrmCur->getVarObject();	// ⇒ obj = fp->varobj;
	EsValue val = _regsCur->fetchOperand(-1);

	EsScopeProperty* sprop;
	EsValue valAtom(atom);
	return obj->definePropertyOv(
		&sprop,
		_ctxRef,
		&valAtom,
		&val,
		g_funcDummyPropertyGetter,
		g_funcDummyPropertySetter,
		EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_READONLY);
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_SETCALL
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_setcall()
{
	s32 numArg = (s32)_regsCur->pc()->getU16(1);
	EsValue* vaStack = _regsCur->sp(- (numArg + 2));

	// 関数呼び出し
	EsStackFrame* sfrmInvoke;
	bool isSucceeded = invoke(&sfrmInvoke, vaStack, (u32)numArg, 0);

	// 未終了のメソッドの場合、そのまま抜ける
	if(isSucceeded && sfrmInvoke->getFunctionResultValue()->isUnfinished())
	{
		return true;
	}

	// スタックポインタを巻き戻す
	_regsCur->advanceSp(- (numArg + 2) + 1);	// ⇒ regs.sp = vp + 1;

	if(!isSucceeded)
	{
		return false;
	}

	return stepSub_setcallAfter();
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_SETCALL 後処理
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_setcallAfter()
{
	// 戻り値の処理
	if(_ctxRef->is2ndRetValueSet())
	{
		_regsCur->pushOperand(_ctxRef->get2ndRetValue());
		_ctxRef->set2ndRetValueSet(false);
	}
	else
	{
		s32 pcofsNext;
		EsOpcodeType otNext;
		if(_regsCur->pc()->peekNextOpcode(&otNext, &pcofsNext, _ctxRef, OP_SETCALL))
		{
			if(otNext != OP_DELELEM)
			{
				_ctxRef->addRunError(EsError::ID_BAD_LEFTSIDE_OF_ASSIGN);
				return false;
			}

			// 返り値に true を設定
			EsValue* vaStack = _regsCur->sp(-1);	// ⇒ vp = regs.sp - argc - 2; regs.sp = vp + 1;
			vaStack[0].setValue(true);

			// バイトコードの送り量を設定
			pcofsNext += EsOpcode::getSpec(OP_DELELEM)->_length;
			if(_ilCur->isBcExInfoAdd())	{	pcofsNext += EsBcExInfo::LEN_BC;	}	// 拡張情報分を加算
			_lenOpcNextStep = pcofsNext;

			return true;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_ARGUMENTS
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_arguments()
{
	EsValue valWk;
	if(!_sfrmCur->getArgsValue(&valWk, _ctxRef))
	{
		return false;
	}

	_regsCur->pushOperand(&valWk);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_ARGSUB
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_argsub()
{
	EsValue spropid((s32)getIndex(0));	// ⇒ id = INT_TO_JSID(GET_ARGNO(regs.pc));
	EsValue valWk;
	if(!_sfrmCur->getArgsProperty(&valWk, _ctxRef, &spropid))
	{
		return false;
	}

	_regsCur->pushOperand(&valWk);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_ARGCNT
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_argcnt()
{
	EsValue spropidLength(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());	// ⇒ id = ATOM_TO_JSID(rt->atomState.lengthAtom);
	EsValue valWk;
	if(!_sfrmCur->getArgsProperty(&valWk, _ctxRef, &spropidLength))
	{
		return false;
	}

	_regsCur->pushOperand(&valWk);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_FORNAME
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_forname()
{
	ASSERT(_regsCur->sp(-2) >= _sfrmCur->getStackTop());

	// プロパティを探す
	const EsAtom* atom = getAtom(0);	// アトム取得
	EsValue propid(atom);
	EsScopeProperty* sprop = 0L;
	EsObject* objName;
	EsObject* objScope;
	if(!_ctxRef->findPropertyFromScopeChain(&sprop, &objName, &objScope, &propid))
	{
		return false;
	}

	if(sprop != 0L)
	{
		objScope->dropPropertyOv(_ctxRef, sprop);
	}
	return objName->callSetPropertyOv(_ctxRef, &propid, _regsCur->sp(-1));
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_FORLOCAL
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_forlocal()
{
	ASSERT((_regsCur->sp(0) - 2) > _sfrmCur->getStackTop());

	u32 slotidx = getIndex(0);							// ⇒ slot = GET_SLOTNO(regs.pc);
	ASSERT(slotidx < _ilCur->getSlotNum());				// ⇒ JS_ASSERT(slot < script->nslots);

	_sfrmCur->getSlot(slotidx)->copy(_regsCur->sp(-1));	// ⇒ vp = &fp->slots[slot]; *vp = regs.sp[-1];

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_FORARG
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_forarg()
{
	ASSERT((_regsCur->sp(0) - 2) > _sfrmCur->getStackTop());

	u32 slotidx = getIndex(0);		// ⇒ slot = GET_ARGNO(regs.pc);
	ASSERT(slotidx < _sfrmCur->getFunction()->getLocalVarNum(EsFunction::LVK_ARG));	// ⇒ JS_ASSERT(slot < fp->fun->nargs);

	_sfrmCur->getArg(slotidx)->copy(_regsCur->sp(-1));	// ⇒ fp->argv[slot] = regs.sp[-1];

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_FORPROP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_forprop()
{
	ASSERT((_regsCur->sp(0) - 2) > _sfrmCur->getStackTop());

	const EsAtom* atom = getAtom(0);	// アトム取得
	EsValue propid(atom);
	EsObject* obj;
	if(!_regsCur->fetchObject(&obj, -1))	{	return false;	}
	if(!obj->callSetPropertyOv(_ctxRef, &propid, _regsCur->sp(-2)))	{	return false;	}

	_regsCur->advanceSp(-1);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_FORELEM
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_forelem()
{
	ASSERT((_regsCur->sp(0) - 2) > _sfrmCur->getStackTop());

	EsValue val = _regsCur->fetchOperand(-1);
	_regsCur->push(&val);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_DELNAME
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_delname()
{
	const EsAtom* atom = getAtom(0);
	EsValue propid(atom);

	EsScopeProperty* sprop;
	EsObject* objScope;
	EsObject* obj;
	if(!_ctxRef->findPropertyFromScopeChain(&sprop, &obj, &objScope, &propid))
	{
		return false;
	}

	EsValue valTrue(true);
	_regsCur->pushOperand(&valTrue);	// ⇒ PUSH_OPND(JSVAL_TRUE);
	if(sprop != 0L)
	{
		objScope->dropPropertyOv(_ctxRef, sprop);
		if(!obj->deletePropertyOv(_regsCur->sp(-1), _ctxRef, &propid))	// ⇒ if (!OBJ_DELETE_PROPERTY(cx, obj, id, &regs.sp[-1]))
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_DELPROP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_delprop()
{
	const EsAtom* atom = getAtom(0);
	EsValue id(atom);

	// ⇒ PROPERTY_OP ～
	EsObject* obj;
	if(!_regsCur->fetchObject(&obj, -1)) { return false; }
	EsValue valWk;
	if(!obj->deletePropertyOv(&valWk, _ctxRef, &id)) { return false; }

	_regsCur->storeOperand(-1, &valWk);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_DELELEM
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_delelem()
{
	// ⇒ ELEMENT_OP(-1, OBJ_DELETE_PROPERTY(cx, obj, id, &rval));
	EsObject* obj;
	if(!_regsCur->fetchObject(&obj, -2)) { return false; }
	EsValue spropid;
	if(!_regsCur->fetchId(&spropid, -1)) { return false; }
	EsValue valWk;
	if(!obj->deletePropertyOv(&valWk, _ctxRef, &spropid)) { return false; }

	_regsCur->advanceSp(-1);
	_regsCur->storeOperand(-1, &valWk);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_GOTO, OP_GOTOX
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_goto(bool isXofs)
{
	s32 len = isXofs ? _regsCur->pc()->getJumpXOffset() : _regsCur->pc()->getJumpOffset();	// ⇒ GET_JUMPX_OFFSET(_regsCur.pc), GET_JUMP_OFFSET(_regsCur.pc)
	jumpBranch(len);	// ⇒ BRANCH(len);

	_isContinueStep = true;	// 続けて次を実行
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_DEFAULT, OP_DEFAULTX
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_default(bool isXofs)
{
	_regsCur->pop();	// ⇒ (void) POP();

	s32 len = isXofs ? _regsCur->pc()->getJumpXOffset() : _regsCur->pc()->getJumpOffset();
	jumpBranch(len);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_IFEQ, OP_IFNE, OP_IFEQX, OP_IFNEX
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_ifcond(EsOpcodeType ot, bool isXofs)
{
	EsValue val;
	bool cond = _regsCur->popBoolean(&val);			// ⇒ POP_BOOLEAN(cx, rval, cond);
	if(	((ot == OP_IFEQ) && (cond == false)) ||
		((ot == OP_IFNE) && (cond == true))	)
	{
		s32 len = isXofs ? _regsCur->pc()->getJumpXOffset() : _regsCur->pc()->getJumpOffset();	// ⇒ GET_JUMPX_OFFSET(_regsCur.pc), GET_JUMP_OFFSET(_regsCur.pc)
		jumpBranch(len);				// ⇒ BRANCH(len);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_GOSUB, OP_GOSUBX
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_gosub(bool isXofs)
{
	EsValue valFalse(false);
	_regsCur->push(&valFalse);

	s32 offset = _regsCur->pc()->getCurrentOffset() + EsOpcode::getSpec(isXofs ? OP_GOSUBX : OP_GOSUB)->_length;
	EsValue valOffset(offset);

	if(isXofs)
	{
		_lenOpcNextStep = _regsCur->pc()->getJumpXOffset();
		_regsCur->push(&valOffset);
	}
	else
	{
		_regsCur->push(&valOffset);	
		_lenOpcNextStep = _regsCur->pc()->getJumpOffset();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_RETSUB
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_retsub()
{
	EsValue valR = _regsCur->popOperand();
	EsValue valL = _regsCur->popOperand();
	ASSERT(valL.isBoolean());
	if(valL.getBoolean() == true)
	{
		return false;
	}
	ASSERT(valR.isInt());
	_lenOpcNextStep = valR.getInt();
	_regsCur->pc()->reset();	// ⇒ regs.pc = script->main;
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_IN
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_in()
{
	EsValue val = _regsCur->fetchOperand(-1);
	if(val.isPrimitive())
	{
		_ctxRef->addRunError(EsError::ID_IN_NOT_OBJECT);
		return false;
	}
	ASSERT(val.isObject());
	EsObject* obj = val.getObject();

	EsValue id;
	_regsCur->fetchId(&id, -2);

	EsScopeProperty* sprop;
	EsObject* objParent;
	if(obj->findPropertyOv(&sprop, &objParent, _ctxRef, &id))
	{
		return false;
	}
	bool cond = (sprop != 0L);
	if(cond)
	{
		objParent->dropPropertyOv(_ctxRef, sprop);
	}

	jumpIfCondition(OP_IN, cond, 2);	// ⇒ TRY_BRANCH_AFTER_COND(cond, 2);

	_regsCur->advanceSp(-1);				// ⇒ regs.sp--;
	EsValue valCond(cond);
	_regsCur->storeOperand(-1, &valCond);	// ⇒ STORE_OPND(-1, BOOLEAN_TO_JSVAL(cond));

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - 値のプッシュ
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_pushvalue(EsOpcodeType ot)
{
	EsValue val;

	switch(ot)
	{
	case OP_UINT16:					val.setValue((s32)_regsCur->pc()->getU16(1));	break;	// ⇒ i = (jsint) GET_UINT16(regs.pc); rval = INT_TO_JSVAL(i);
	case OP_UINT24:					val.setValue((s32)_regsCur->pc()->getU24(1));	break;	// ⇒ i = (jsint) GET_UINT24(regs.pc);
	case OP_INT8:					val.setValue((s32)_regsCur->pc()->get(1));		break;	// ⇒ i = GET_INT8(regs.pc);
	case OP_INT32:					val.setValue((s32)_regsCur->pc()->getS32(1));	break;	// ⇒ i = GET_INT32(regs.pc);
	case OP_ZERO:					val.setValue((s32)0);							break;	// ⇒ JSVAL_ZERO
	case OP_ONE:					val.setValue((s32)1);							break;	// ⇒ JSVAL_ONE
	case OP_NULL:					val.setType(EsValue::VTYPE_NULL);				break;	// ⇒ JSVAL_NULL
	case OP_FALSE:					val.setValue(false);							break;	// ⇒ JSVAL_FALSE
	case OP_TRUE:					val.setValue(true);								break;	// ⇒ JSVAL_TRUE
	case OP_DOUBLE:	case OP_STRING:	val.setValue(getAtom(0));						break;	// ⇒ LOAD_ATOM(0), ATOM_KEY(atom)
	case OP_OBJECT:					val.setValue(getObject(0));						break;
	case OP_CALLEE:					val.setValue(_sfrmCur->getCalleeObject());		break;
	case OP_HOLE:					val.setHole();									break;
	case OP_VOID:																	break;
	}

	_regsCur->pushOperand(&val);

	#if SHORTEN_STEP
		_isContinueStep = true;	// 続けて次を実行
	#endif
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_THIS
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_this()
{
	EsObject* objThis = getThisObjectForFrame();
	if(objThis == 0L)
	{
		return false;
	}
	EsValue valThisObj(objThis);
	_regsCur->pushOperand(&valThisObj);

	#if SHORTEN_STEP
		_isContinueStep = true;	// 続けて次を実行
	#endif
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_PRIMTOP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_primtop()
{
	EsValue valWk = _regsCur->fetchOperand(-1);
	if(!valWk.isPrimitive())
	{
		_ctxRef->addRunError(EsError::ID_BAD_PRIM_TYPE);
		return false;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_IFPRIMTOP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_ifprimtop()
{
	EsValue valWk = _regsCur->fetchOperand(-1);
	if(valWk.isPrimitive())
	{
		s32 offset = _regsCur->pc()->getJumpOffset();	// ⇒ GET_JUMP_OFFSET(regs.pc);
		jumpBranch(offset);			// ⇒ BRANCH(len);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_OBJTOP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_objtop()
{
	EsValue val = _regsCur->fetchOperand(-1);
	if(val.isPrimitive())
	{
		_ctxRef->addRunError(EsError::ID_BAD_OBJ_TYPE);
		return false;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_TYPEOF, OP_TYPEOFEXPR
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_typeof()
{
	EsValue valWk = _regsCur->fetchOperand(-1);
	EsLogicalType ltype = valWk.getLogicalType();
	const EsAtom* atom = _ctxRef->getKeywordSet()->getLogicalType(ltype)->getAtom();
	ASSERT(atom != 0L);
	EsValue valAtom(atom);
	_regsCur->storeOperand(-1, &valAtom);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_INSTANCEOF
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_instanceof()
{
	EsValue valR = _regsCur->fetchOperand(-1);
	if(valR.isPrimitive())
	{
		_ctxRef->addRunError(EsError::ID_BAD_INSTANCEOF_RHS);
		return false;
	}

	EsValue valL = _regsCur->fetchOperand(-2);

	EsObject* obj = valR.getObject();
	EsValue valCond;
	if(!obj->hasInstanceOv(&valCond, _ctxRef, obj, &valL))
	{
		return false;
	}

	_regsCur->advanceSp(-1);
	_regsCur->storeOperand(-1, &valCond);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_ADD, OP_SUB, OP_MUL, OP_DIV
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_arithop(EsOpcodeType ot)
{
	switch(ot)
	{
	case OP_ADD:
		{
			EsValue valL = _regsCur->fetchOperand(-2);
			EsValue valR = _regsCur->fetchOperand(-1);
			if(!valL.isPrimitive())
			{
				ASSERT(valL.isObject());
				if(!valL.getObject()->convDefaultValueOv(_ctxRef, _regsCur->sp(-2), LTYPE_VOID))
				{
					return false;
				}
				valL.copy(_regsCur->sp(-2));
			}
			if(!valR.isPrimitive())
			{
				ASSERT(valR.isObject());
				if(!valR.getObject()->convDefaultValueOv(_ctxRef, _regsCur->sp(-1), LTYPE_VOID))
				{
					return false;
				}
				valR.copy(_regsCur->sp(-1));
			}

			if(valL.isString() || valR.isString())
			{
				// 文字列の結合
				const VcString* strL;
				const VcString* strR;
				if(valL.isString() && valR.isString())
				{
					strL = valL.getString();
					strR = valR.getString();
				}
				else if(valL.isString())
				{
					strL = valL.getString();
					VcString strRwk;
					if(!valR.toString(&strRwk, _ctxRef))	{	return false;	}
					strR = _ctxRef->newString(&strRwk);
					if(strR == 0L)							{	return false;	}
					EsValue valStrR(strR);
					_regsCur->storeOperand(-1, &valStrR);
				}
				else
				{
					VcString strLwk;
					if(!valL.toString(&strLwk, _ctxRef))	{	return false;	}
					strL = _ctxRef->newString(&strLwk);
					if(strL == 0L)							{	return false;	}
					EsValue valStrL(strL);
					_regsCur->storeOperand(-2, &valStrL);
					strR = valR.getString();
				}
				VcString strAwk = *strL + *strR;
				const VcString* strA = _ctxRef->newString(&strAwk);
				if(strA == 0L)							{	return false;	}
				_regsCur->advanceSp(-1);				// ⇒ regs.sp--;
				EsValue valStrA(strA);
				_regsCur->storeOperand(-1, &valStrA);	// ⇒ STORE_NUMBER(cx, -1, d);
			}
			else
			{
				f64 fvalL = _regsCur->fetchF64(-2);
				f64 fvalR = _regsCur->fetchF64(-1);
				fvalL += fvalR;
				_regsCur->advanceSp(-1);			// ⇒ regs.sp--;
				_regsCur->storeF64(-1, fvalL);		// ⇒ STORE_NUMBER(cx, -1, d);
			}
		}
		break;
	case OP_SUB:
		{
			f64 fvalL = _regsCur->fetchF64(-2);	// ⇒ FETCH_NUMBER
			f64 fvalR = _regsCur->fetchF64(-1);
			fvalL -= fvalR;
			_regsCur->advanceSp(-1);
			_regsCur->storeF64(-1, fvalL);
		}
		break;
	case OP_MUL:
		{
			f64 fvalL = _regsCur->fetchF64(-2);
			f64 fvalR = _regsCur->fetchF64(-1);
			fvalL *= fvalR;
			_regsCur->advanceSp(-1);
			_regsCur->storeF64(-1, fvalL);
		}
		break;
	case OP_DIV:
		{
			f64 fvalL = _regsCur->fetchF64(-2);	// ⇒ d
			f64 fvalR = _regsCur->fetchF64(-1);	// ⇒ d2
			if(fvalR == 0.0f)	// ゼロディバイド
			{
#if defined(WIN32)
				if(TFW_F64_IS_NAN(fvalR))	// ⇒ if (JSDOUBLE_IS_NaN(d2))
				{
					fvalL = TypeUtils::getF64NaN();
				}
				else				
#endif
				if((fvalL == 0.0) || TFW_F64_IS_NAN(fvalL))
				{
					fvalL = TypeUtils::getF64NaN();
				}
				else if((TFW_F64_TO_RAWU32_HI(fvalL) ^ TFW_F64_TO_RAWU32_HI(fvalR)) >> 0x1f)	// ⇒ else if ((JSDOUBLE_HI32(d) ^ JSDOUBLE_HI32(d2)) >> 31)
				{
					fvalL = TypeUtils::getF64NegativeInfinity();
				}
				else
				{
					fvalL = TypeUtils::getF64PositiveInfinity();
				}
				_regsCur->advanceSp(-1);
				_regsCur->storeF64(-1, fvalL);
			}
			else
			{
				fvalL /= fvalR;
				_regsCur->advanceSp(-1);
				_regsCur->storeF64(-1, fvalL);
			}
		}
		break;
	case OP_MOD:
		{
			f64 fvalL = _regsCur->fetchF64(-2);
			f64 fvalR = _regsCur->fetchF64(-1);
			if(fvalR == 0.0)	// ゼロディバイド
			{
				fvalL = TypeUtils::getF64NaN();	// ⇒ rt->jsNaN
				_regsCur->advanceSp(-1);
				_regsCur->storeF64(-1, fvalL);
			}
			else
			{
				#if defined(WIN32)
					if(!(TFW_F64_IS_FINITE(fvalL) && TFW_F64_IS_INF(fvalR)))	// ⇒ if (!(JSDOUBLE_IS_FINITE(d) && JSDOUBLE_IS_INFINITE(d2)))
					{
						fvalL = ::fmod(fvalL, fvalR);
					}
				#else
					fvalL = ::fmod(fvalL, fvalR);
				#endif
				_regsCur->advanceSp(-1);
				_regsCur->storeF64(-1, fvalL);
			}
		}
		break;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_BITOR, OP_BITXOR, OP_BITAND
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_bitop(EsOpcodeType ot)
{
	s32 ivalL = _regsCur->fetchS32(-2);
	s32 ivalR = _regsCur->fetchS32(-1);

	switch(ot)
	{
	case OP_BITOR:	ivalL = ivalL | ivalR;	break;
	case OP_BITXOR:	ivalL = ivalL ^ ivalR;	break;
	case OP_BITAND:	ivalL = ivalL & ivalR;	break;
	}

	_regsCur->advanceSp(-1);	// ⇒ regs.sp--;
	_regsCur->storeS32(-1, ivalL);	// ⇒ STORE_INT(cx, -1, i);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_LSFT, OP_RSFT, OP_URSFT
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_bitshift(EsOpcodeType ot)
{
	s32 ivalL = _regsCur->fetchS32(-2);
	s32 ivalR = _regsCur->fetchS32(-1);

	switch(ot)
	{
	case OP_LSFT:	ivalL = ivalL << (ivalR & 0x1f);	break;
	case OP_RSFT:	ivalL = ivalL >> (ivalR & 0x1f);	break;
	case OP_URSFT:	ivalL >>= (ivalR & 0x1f);		break;
	}

	_regsCur->advanceSp(-1);	// ⇒ regs.sp--;
	_regsCur->storeS32(-1, ivalL);	// ⇒ STORE_INT(cx, -1, i);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_OR, OP_AND, OP_ORX, OP_ANDX
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_orand(EsOpcodeType ot, bool isXofs)
{
	EsValue val;
	bool cond = _regsCur->popBoolean(&val);	// ⇒ POP_BOOLEAN(cx, rval, cond);
	if(	((ot == OP_OR) && (cond == true)) ||
		((ot == OP_AND) && (cond == false)) )
	{
		s32 len = isXofs ? _regsCur->pc()->getJumpXOffset() : _regsCur->pc()->getJumpOffset();	// ⇒ GET_JUMPX_OFFSET(_regsCur.pc), GET_JUMP_OFFSET(_regsCur.pc)
		_regsCur->pushOperand(&val);
		_lenOpcNextStep = len;						// ⇒ DO_NEXT_OP(len);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_NOT
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_not()
{
	EsValue val;
	bool cond = _regsCur->popBoolean(&val);	// ⇒ POP_BOOLEAN(cx, rval, cond);
	EsValue valCond(!cond);
	_regsCur->pushOperand(&valCond);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_BITNOT
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_bitnot()
{
	s32 ival = _regsCur->fetchS32(-1);
	ival = ~ival;
	_regsCur->storeS32(-1, ival);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_compare(EsOpcodeType ot)
{
	EsValue valL = _regsCur->fetchOperand(-2);
	EsValue valR = _regsCur->fetchOperand(-1);

	bool cond;
	switch(ot)
	{
	case OP_EQ:	cond = valL.compareEq(_ctxRef, &valR);	break;	// == 比較	⇒ EQUALITY_OP(==, JS_FALSE);
	case OP_NE:	cond = valL.compareNe(_ctxRef, &valR);	break;	// != 比較	⇒ EQUALITY_OP(!=, JS_TRUE);
	case OP_LT:	cond = valL.compareLt(_ctxRef, &valR);	break;	// < 比較	⇒ RELATIONAL_OP(<);
	case OP_LE:	cond = valL.compareLe(_ctxRef, &valR);	break;	// <= 比較	⇒ RELATIONAL_OP(<=);
	case OP_GT:	cond = valL.compareGt(_ctxRef, &valR);	break;	// > 比較	⇒ RELATIONAL_OP(>);
	case OP_GE:	cond = valL.compareGe(_ctxRef, &valR);	break;	// >= 比較	⇒ RELATIONAL_OP(>=);
	}

	jumpIfCondition(ot, cond, 2);	// ⇒ TRY_BRANCH_AFTER_COND(cond, 2);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_STRICTEQ, OP_STRICTNE
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_stricteq(bool isEquals)
{
	EsValue valL = _regsCur->fetchOperand(-2);
	EsValue valR = _regsCur->fetchOperand(-1);
	bool cond = valL.equalsStrict(&valR);
	if(!isEquals)	{	cond = !cond;	}
	_regsCur->advanceSp(-1);	// ⇒ regs.sp--;
	EsValue valCond(cond);
	_regsCur->storeOperand(-1, &valCond);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_CASE, OP_CASEX
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_case(bool isXofs)
{
	EsValue valL = _regsCur->fetchOperand(-2);
	EsValue valR = _regsCur->fetchOperand(-1);
	bool cond = valL.equalsStrict(&valR);
	_regsCur->advanceSp(-1);	// ⇒ regs.sp--;
	EsValue valCond(cond);
	_regsCur->storeOperand(-1, &valCond);

	_regsCur->pop();	// ⇒ (void) POP();

	if(cond)
	{
		s32 len = isXofs ? _regsCur->pc()->getJumpXOffset() : _regsCur->pc()->getJumpOffset();	// ⇒ GET_JUMPX_OFFSET(_regsCur.pc), GET_JUMP_OFFSET(_regsCur.pc)
		jumpBranch(len);	// ⇒ BRANCH(len);
		return true;
	}

	_regsCur->push(&valL);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_POS
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_pos()
{
	EsValue val = _regsCur->fetchOperand(-1);
	if(!val.isNumber())
	{
		f64 fval;
		val.toNumber(&fval, &val, _ctxRef);
		if(val.isNull())
		{
			return false;
		}
		ASSERT(val.isNumber());

		_regsCur->sp(-1)->setNumber(fval);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_NEG
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_neg()
{
	EsValue val = _regsCur->fetchOperand(-1);
	if(val.isInt() && (val.getInt() != 0))
	{
		s32 ival = val.getInt();
		ival = - ival;
		EsValue valInt(ival);
		_regsCur->sp(-1)->copy(&valInt);
	}
	else
	{
		f64 fval;
		if(val.isDouble())
		{
			fval = val.getDouble();
		}
		else
		{
			val.toNumber(&fval, &val, _ctxRef);
			if(val.isNull())
			{
				return false;
			}
			ASSERT(val.isNumber());
		}

		fval = - fval;

		_regsCur->sp(-1)->setNumber(fval);	// ⇒ js_NewNumberInRootedValue(cx, d, &regs.sp[-1])
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン -
		OP_NAMEINC, OP_INCNAME, OP_NAMEDEC, OP_DECNAME
		OP_ELEMINC, OP_INCELEM, OP_ELEMDEC, OP_DECELEM
		OP_PROPINC, OP_INCPROP, OP_PROPDEC, OP_DECPROP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_nameinc(EsOpcodeType ot, bool isDec, bool isPost)
{
	const EsOpcode::Spec* opspec = EsOpcode::getSpec(ot);
	const EsAtom* atom = 0L;
	EsObject* objScope = 0L;
	switch(ot)
	{
	case OP_NAMEINC: case OP_INCNAME: case OP_NAMEDEC: case OP_DECNAME:
		{
			objScope = _sfrmCur->getScopeChainObject();	// ⇒ obj = fp->scopeChain;

			// ※※※ 実装保留 ※※※ プロパティキャッシュ ⇒ if (JS_LIKELY(OBJ_IS_NATIVE(obj))) ～ PROPERTY_CACHE_TEST

			atom = getAtom(0);
			ASSERT(atom != 0L);
			EsValue valAtom(atom);
			EsScopeProperty* sprop = 0L;
			EsObject* objScopeParent = 0L;
			if(!_ctxRef->findPropertyFromScopeChain(&sprop, &objScope, &objScopeParent, &valAtom) || (sprop == 0L))
			{
				return false;
			}
			objScopeParent->dropPropertyOv(_ctxRef, sprop);
		}
		break;
	case OP_ELEMINC: case OP_INCELEM: case OP_ELEMDEC: case OP_DECELEM:
		{
			if(!_regsCur->fetchObject(&objScope, -2))	{	return false;	}	// ⇒ FETCH_OBJECT(cx, i, lval, obj);
			if(!_regsCur->fetchAtom(&atom, -1))			{	return false;	}
		}
		break;
	case OP_PROPINC: case OP_INCPROP: case OP_PROPDEC: case OP_DECPROP:
		{
			atom = getAtom(0);
			if(!_regsCur->fetchObject(&objScope, -1))	{	return false;	}	// ⇒ FETCH_OBJECT(cx, i, lval, obj);
		}
		break;
	}
	EsValue spropid(atom);

	EsValue valNull(EsValue::VTYPE_NULL);
	_regsCur->pushOperand(&valNull);
	EsValue* valVar = _regsCur->sp(-1);
	objScope->callGetPropertyOv(valVar, _ctxRef, &spropid); 

	EsValue valRes = *valVar;
	if(valRes.isFastIncrement())
	{
		s32 incrSlot = isDec ? -1 : 1;									// 加算値 ⇒ incr =  2; ※ MozillaJs の jsval は左に１ビットシフトしているため、値が倍になっている
		if(isPost)
		{
			valVar->setValue(valRes.getInt() + incrSlot);
		}
		else
		{
			valRes.setValue(valRes.getInt() + incrSlot);
			valVar->copy(&valRes);
		}

		TFW_SET_FLAG(*_sfrmCur->flags(), EsStackFrame::F_ASSIGNING, true);
		bool isSucceeded = objScope->callSetPropertyOv(_ctxRef, &spropid, valVar); 
		TFW_SET_FLAG(*_sfrmCur->flags(), EsStackFrame::F_ASSIGNING, false);
		if(!isSucceeded)	{	return false;	}
	}
	else
	{
		_regsCur->pushOperand(&valNull);
		EsValue* valStackVar = _regsCur->sp(-2);
		EsValue* valStackRes = _regsCur->sp(-1);
		if(!valStackVar->incdec(valStackRes, _ctxRef, isDec, isPost))
		{
			return false;
		}

		TFW_SET_FLAG(*_sfrmCur->flags(), EsStackFrame::F_ASSIGNING, true);
		bool isSucceeded = objScope->callSetPropertyOv(_ctxRef, &spropid, valVar); 
		TFW_SET_FLAG(*_sfrmCur->flags(), EsStackFrame::F_ASSIGNING, false);
		if(!isSucceeded)	{	return false;	}

		_regsCur->advanceSp(-1);
	}

	if(opspec->_numStackUses != 0)
	{
		EsValue valTmp = *valVar;
		_regsCur->advanceSp(- opspec->_numStackUses);
		*_regsCur->sp(-1) = valTmp;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン -
		OP_GVARINC, OP_INCGVAR, OP_GVARDEC, OP_DECGVAR
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_gvarinc(bool isDec, bool isPost, EsOpcodeType otSubst)
{
	u32 slotidx = (s32)_regsCur->pc()->getU16(1);					// ⇒ slot = GET_SLOTNO(regs.pc);
	ASSERT(slotidx < _sfrmCur->getSlotNum());	// ⇒ JS_ASSERT(slot < GlobalVarCount(fp));
	EsValue* valIdxSlot = _sfrmCur->getSlot(slotidx);
	if(valIdxSlot->isNull())
	{
		// OP_NAME* 系に変換して再実行
		restep(otSubst, _exiCurStep);
		return true;
	}
	slotidx = valIdxSlot->getInt();
	EsObject* obj = _sfrmCur->getVarObject();
	EsValue valRes = *obj->getSlots()->getValue(slotidx);
	if(valRes.isFastIncrement())
	{
		s32 incrSlot = isDec ? -1 : 1;									// 加算値 ⇒ incr =  2; ※ MozillaJs の jsval は左に１ビットシフトしているため、値が倍になっている
		if(isPost)
		{
			_regsCur->pushOperand(&valRes);
			valRes.setValue(valRes.getInt() + incrSlot);
		}
		else
		{
			valRes.setValue(valRes.getInt() + incrSlot);
			_regsCur->pushOperand(&valRes);
		}
	}
	else
	{
		_regsCur->pushOperand(&valRes);
		EsValue valNull(EsValue::VTYPE_NULL);
		_regsCur->pushOperand(&valNull);
		EsValue* valStackVar = _regsCur->sp(-2);
		EsValue* valStackRes = _regsCur->sp(-1);
		if(!valStackVar->incdec(valStackRes, _ctxRef, isDec, isPost))
		{
			return false;
		}

		valRes.copy(valStackRes);
		_regsCur->advanceSp(-1);
	}
	obj->slots()->setValue(slotidx, &valRes);	// ⇒ OBJ_SET_SLOT(cx, fp->varobj, slot, rval);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン -
		OP_LOCALINC, OP_INCLOCAL, OP_LOCALDEC, OP_DECLOCAL,
		OP_ARGINC, OP_INCARG, OP_ARGDEC, OP_DECARG,
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_localinc(bool isArg, bool isDec, bool isPost)
{
	EsValue* valSlot = 0L;

	if(isArg)	// ⇒ do_arg_incop
	{
		u32 slotidx = getIndex(0);						// ⇒ slot = GET_ARGNO(regs.pc);
		ASSERT(slotidx < _sfrmCur->getFunction()->getLocalVarNum(EsFunction::LVK_ARG));	// ⇒  JS_ASSERT(slot < fp->fun->nargs);
		ASSERT(slotidx < _sfrmCur->getArgNum());
		valSlot = _sfrmCur->getArg(slotidx);
	}
	else		// ⇒ do_local_incop
	{
		u32 slotidx = getIndex(0);						// ⇒ slot = GET_SLOTNO(regs.pc);
		ASSERT(slotidx < _ilCur->getSlotNum());			// ⇒ JS_ASSERT(slot < script->nslots);
		valSlot = _sfrmCur->getSlot(slotidx);			// ⇒ vp = fp->slots + slot;
	}

	if(valSlot->isFastIncrement())
	{
		s32 incrSlot = isDec ? -1 : 1;							// 加算値 ⇒ incr =  2; ※ MozillaJs の jsval は左に１ビットシフトしているため、値が倍になっている
		EsValue valStack = *valSlot;							// 計算前の値を保存しておく ⇒ rval = *vp;
		valSlot->setValue(valSlot->getInt() + incrSlot);		// 計算処理 ⇒ *vp = rval + incr;
		if(skipPopAfterSet(OP_INCARG, 0))
		{
			return true;	// POP スキップ ⇒ SKIP_POP_AFTER_SET(JSOP_INCARG_LENGTH, 0);
		}
		if(isPost)
		{
			valStack.setValue(valStack.getInt() + incrSlot);	// 後置の場合は返答値に対してインクリメント／デクリメントしない ⇒ rval + incr2
		}
		_regsCur->pushOperand(&valStack);
	}
	else
	{
		_regsCur->pushOperand(valSlot);
		EsValue* valStack = _regsCur->sp(-1);
		if(!valStack->incdec(valSlot, _ctxRef, isDec, isPost))
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_RESETBASE, OP_RESETBASE0
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_resetbase()
{
	_indexBase = 0;	// 上位バイトインデックスをリセットする ⇒ atoms = script->atomMap.vector;

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_INDEXBASE
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_indexbase()
{
	_indexBase += ((s32)_regsCur->pc()->getU16(1)) << 16;	// 上位バイトインデックスを設定する ⇒ GET_INDEXBASE(_regsCur.pc);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_DUP, OP_DUP2, OP_SWAP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_dup(EsOpcodeType ot)
{
	switch(ot)
	{
	case OP_DUP:
		{
			ASSERT(_regsCur->sp(0) > _sfrmCur->getStackTop());
			EsValue valR = _regsCur->fetchOperand(-1);
			_regsCur->push(&valR);
		}
		break;
	case OP_DUP2:
		{
			ASSERT((_regsCur->sp(0) - 2) > _sfrmCur->getStackTop());
			EsValue valL = _regsCur->fetchOperand(-2);
			EsValue valR = _regsCur->fetchOperand(-1);
			_regsCur->push(&valL);
			_regsCur->push(&valR);
		}
		break;
	case OP_SWAP:
		{
			ASSERT((_regsCur->sp(0) - 2) > _sfrmCur->getStackTop());
			EsValue valL = _regsCur->fetchOperand(-2);
			EsValue valR = _regsCur->fetchOperand(-1);
			_regsCur->storeOperand(-1, &valL);
			_regsCur->storeOperand(-2, &valR);
		}
		break;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_PICK
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_pick()
{
	s32 ival = _regsCur->pc()->get(1);	// ⇒ i = regs.pc[1];
	ASSERT((_regsCur->sp(0) - (ival + 1)) > _sfrmCur->getStackTop());
	EsValue valL = *_regsCur->sp(- (ival + 1));
	::memmove(_regsCur->sp(- (ival + 1)), _regsCur->sp(- ival), sizeof(EsValue) * ival);
	_regsCur->sp(- (ival + 1))->copy(&valL);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_ENTERBLOCK
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_enterblock()
{
	EsObject* obj = getObject(0);
	ASSERT(!((EsBlock*)obj)->isCloned());	// ⇒ JS_ASSERT(!OBJ_IS_CLONED_BLOCK(obj));
	ASSERT((_sfrmCur->getStackTop() + obj->getSlots()->getBlockDepth()) == _regsCur->sp(0));

	s32 numBlock = obj->getScope()->getScopePropertyNum();
	ASSERT((_regsCur->sp(0) + numBlock) <= (_sfrmCur->getSlot(0) + _ilCur->getSlotNum()));	// ⇒ JS_ASSERT(vp <= fp->slots + script->nslots);
	for(int i = 0; i < numBlock; i++)
	{
		EsValue valVoid;
		_regsCur->storeOperand(0, &valVoid);
		_regsCur->advanceSp(1);
	}

	_sfrmCur->setBlockChainObject(obj);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_LEAVEBLOCK, OP_LEAVEBLOCKEXPR
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_leaveblock(EsOpcodeType ot)
{
	EsObject* obj = _sfrmCur->getScopeChainObject();
	if(obj->getSlots()->getPrototypeObject() == _sfrmCur->getBlockChainObject())
	{
		ASSERT(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_BLOCK);
		if(!_sfrmCur->putBlockObject(_ctxRef, true))
		{
			return false;
		}
	}

	_sfrmCur->setBlockChainObject(_sfrmCur->getBlockChainObject()->slots()->parentObject());

	EsValue valWk;
	if(ot == OP_LEAVEBLOCKEXPR)
	{
		valWk = _regsCur->fetchOperand(-1);
	}

	_regsCur->advanceSp(- (s32)_regsCur->pc()->getU16(1));

	if(ot == OP_LEAVEBLOCKEXPR)
	{
		_regsCur->storeOperand(-1, &valWk);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_ENTERWITH
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_enterwith()
{
	if(!_ctxRef->getTopStackFrame()->enterWithBlock(_ctxRef, -1))	{	return false;	}
	_regsCur->sp(-1)->setValue(_sfrmCur->getScopeChainObject());
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_LEAVEWITH
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_leavewith()
{
	ASSERT(_regsCur->sp(-1)->isObject() && (_regsCur->sp(-1)->getObject() == _sfrmCur->getScopeChainObject()));
	_regsCur->advanceSp(-1);
	_ctxRef->getTopStackFrame()->leaveWithBlock(_ctxRef);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_ITER
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_iter()
{
	u8 flagsIter = _regsCur->pc()->get(1);	// ⇒ flags = regs.pc[1];
	EsIterator* iter;
	if(!_ctxRef->convValueToIterator(&iter, _regsCur->sp(-1), flagsIter))
	{
		return false;
	}
	ASSERT(!_regsCur->sp(-1)->isPrimitive());
	EsValue valVoid;
	_regsCur->push(&valVoid);		// ⇒ PUSH(JSVAL_VOID);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_NEXTITER
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_nextiter()
{
	EsValue* valIterObj = _regsCur->sp(-2);
	ASSERT(!valIterObj->isPrimitive());
	ASSERT(valIterObj->isObject());
	if(!valIterObj->isObject())	{	return false;	}
	EsObject* objIter = valIterObj->getObject();
	if(!EsIterator::next(_regsCur->sp(-1), objIter, _ctxRef))	// ⇒ js_CallIteratorNext(cx, JSVAL_TO_OBJECT(regs.sp[-2]), &regs.sp[-1])
	{
		return false;
	}
	EsValue valRes(!_regsCur->sp(-1)->isHole());
	_regsCur->push(&valRes);		// ⇒ PUSH(rval);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_ENDITER
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_enditer()
{
	EsValue* valIterObj = _regsCur->sp(-2);
	ASSERT(!valIterObj->isPrimitive());
	ASSERT(valIterObj->isObject());
	if(!valIterObj->isObject())	{	return false;	}
	EsObject* objIter = valIterObj->getObject();
	bool isSucceeded = EsIterator::close(objIter, _ctxRef);	// ⇒ ok = js_CloseIterator(cx, regs.sp[-2]);
	_regsCur->advanceSp(-2);
	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_NEWARRAY
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_newarray()
{
	s32 length = _regsCur->pc()->getU16(1);	// ⇒ len = GET_UINT16(regs.pc);
	EsArray* arr = _ctxRef->newArray(_regsCur->sp(- length), length);	// ⇒ obj = js_NewArrayObject(cx, len, regs.sp - len, JS_TRUE);
	if(arr == 0L)	{	return false;	}
	_regsCur->advanceSp(- length + 1);
	EsValue valArr(arr);
	_regsCur->storeOperand(-1, &valArr);

	_isContinueStep = true;	// 続けて次も実行
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_NEWINIT
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_newinit()
{
	EsObject* objWk = 0L;

	s32 ptypekind = (s32)_regsCur->pc()->get(1);	// ⇒ i = GET_INT8(regs.pc);
	switch(ptypekind)
	{
	default:
		objWk = _ctxRef->newObject(EsSysClasses::getClass(EsSysClasses::CLASSTYPE_OBJECT), 0L, 0L);
		break;
	case EsSysPrototypes::PTYPEKIND_ARRAY:
		objWk = _ctxRef->newArray(0L, 0);
		break;
	}
	if(objWk == 0L)
	{
		return false;
	}

	EsValue valObjWk(objWk);
	_regsCur->pushOperand(&valObjWk);
	_sfrmCur->setInitDepth(_sfrmCur->getInitDepth() + 1);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_ENDINIT
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_endinit()
{
	_sfrmCur->setInitDepth(_sfrmCur->getInitDepth() - 1);
	if(_sfrmCur->getInitDepth() == 0)
	{
		_sfrmCur->setSharpVarScope(0L);
	}

	EsValue valWk = _regsCur->fetchOperand(-1);
	ASSERT(valWk.isObject());
	// ※※※ 実装保留 ※※※ ⇒ cx->weakRoots.newborn[GCX_OBJECT] = JSVAL_TO_GCTHING(lval);

	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_INITPROP
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_initprop()
{
	// アクセサ初期化値を読み込む
	EsValue valR = _regsCur->fetchOperand(-1);

	// 初期化されるオブジェクトを読み込む
	EsValue valL = _regsCur->fetchOperand(-2);
	EsObject* obj = valL.getObject();
	ASSERT(obj->isNative());
	ASSERT(obj->getClass()->getDefaultReservedSlotNum() == 0);
	ASSERT(!TFW_IS_FLAG(obj->getClass()->getFlags(), EsClass::F_SHARE_ALL_PROPS));

	// ※※※ 実装保留 ※※※ プロパティキャッシュ ⇒ propertyCache

	// ID を得る
	const EsAtom* atom = getAtom(0);	// ⇒ LOAD_ATOM(0);
	EsValue id(atom);

	// 多重定義チェック
	EsScopeProperty* sprop = 0L;
	EsObject* objParent = 0L;
	if(!checkRedeclaration(&sprop, &objParent, obj, &id, 0, true))
	{
		return false;
	}

	// プロパティを設定する
	if(atom == EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROTO)->getAtom())
	{
		if(!obj->callSetPropertyBase(_ctxRef, &id, &valR))
		{
			return false;
		}
	}
	else
	{
		if(!obj->definePropertyNative(_ctxRef, &id, &valR, 0L, 0L, EsScopeProperty::ATRBF_ENUMERATE, 0, 0, false))
		{
			return false;
		}
	}

	_regsCur->advanceSp(-1);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_INITELEM
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_initelem()
{
	// アクセサ初期化値を読み込む
	EsValue valR = _regsCur->fetchOperand(-1);

	// 初期化されるオブジェクトを読み込む
	EsValue valL = _regsCur->fetchOperand(-3);
	ASSERT(!valL.isPrimitive());
	EsObject* obj = valL.getObject();

	// ID を得る
	EsValue id;
	if(!_regsCur->fetchId(&id, -2)) { return false; }	// ⇒ FETCH_ELEMENT_ID(obj, -2, id);

	// 多重定義チェック
	if(!checkRedeclaration(0L, 0L, obj, &id, 0, true))
	{
		return false;
	}

	// プロパティセット
	if(valR.isHole())
	{
		// 配列の長さプロパティを設定する
		ASSERT(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ARRAY);
		EsArray* arr = (EsArray*)obj;
		ASSERT(id.isInt());
		EsOpcodeType otNext;
		if(_regsCur->pc()->peekNextOpcode(&otNext, 0L, _ctxRef, OP_INITELEM))	// 次の命令コードを取得する ⇒ js_GetOpcode(cx, script, regs.pc + JSOP_INITELEM_LENGTH)
		{
			if(otNext == OP_ENDINIT)
			{
				if(!arr->setArrayLengthProperty(_ctxRef, id.getInt() + 1))
				{
					return false;
				}
			}
		}
	}
	else
	{
		if(!obj->definePropertyOv(0L, _ctxRef, &id, &valR, 0L, 0L, EsScopeProperty::ATRBF_ENUMERATE))
		{
			return false;
		}
	}

	_regsCur->advanceSp(-2);
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_LOOKUPSWITCH, OP_LOOKUPSWITCHX
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_lookupswitch(bool isXofs)
{
	s32 bcofs = isXofs ? EsBytecodeBuilder::SIZE_JUMPX_OFFSET_BC : EsBytecodeBuilder::SIZE_JUMP_OFFSET_BC;
	EsProgramCounter pcWk(*_regsCur->pc());
	EsValue valL = _regsCur->popOperand();

	if(!valL.isNumber() && !valL.isString() && !valL.isBoolean())
	{
		s32 len = isXofs ? pcWk.getJumpXOffset() : pcWk.getJumpOffset();	// ⇒ len = (op == JSOP_LOOKUPSWITCH) ? GET_JUMP_OFFSET(pc2) : GET_JUMPX_OFFSET(pc2)
		_lenOpcNextStep = len;
		return true;
	}

	pcWk.advance(bcofs);
	s32 numPairs = (s32)pcWk.getU16(1);	// ⇒ npairs = (jsint) GET_UINT16(pc2);
	pcWk.advance(2);					// ⇒ pc2 += UINT16_LEN;
	ASSERT(numPairs > 0);				// 空の switch は OP_TABLESWITCH を用いる

	while(true)
	{
		s32 indexAtom = pcWk.getU16(1);
		ASSERT(indexAtom < _ilCur->getAtomNum());
		const EsAtom* atomR = _ilCur->getAtom(indexAtom);	// ⇒ atom = atoms[GET_INDEX(pc2)]; rval = ATOM_KEY(atom);
		bool match = atomR->equals(&valL);
		pcWk.advance(EsBytecodeBuilder::SIZE_INDEX_BC);
		if(match)	{	break;	}
		pcWk.advance(bcofs);
		if(--numPairs == 0)
		{
			pcWk.copy(_regsCur->pc());
			break;
		}
	}

	s32 len = isXofs ? pcWk.getJumpXOffset() : pcWk.getJumpOffset();	// ⇒ len = (op == JSOP_LOOKUPSWITCH) ? GET_JUMP_OFFSET(pc2) : GET_JUMPX_OFFSET(pc2)
	_lenOpcNextStep = len;
	return true;
}

/*---------------------------------------------------------------------*//**
	ステップ実行サブルーチン - OP_TABLESWITCH, OP_TABLESWITCHX
**//*---------------------------------------------------------------------*/
bool EsRunEnv::stepSub_tableswitch(bool isXofs)
{
	EsProgramCounter pcWk(*_regsCur->pc());
	s32 len = isXofs ? pcWk.getJumpXOffset() : pcWk.getJumpOffset();	// ⇒ len = (op == JSOP_LOOKUPSWITCH) ? GET_JUMP_OFFSET(pc2) : GET_JUMPX_OFFSET(pc2)

	s32 n;
	EsValue valR = _regsCur->popOperand();
	if(valR.isInt())
	{
		n = valR.getInt();
	}
	else if(valR.isDouble() && (valR.getDouble() == 0.0))
	{
		n = 0;
	}
	else
	{
		_lenOpcNextStep = len;
		return true;
	}

	s32 bclenJump = isXofs ? EsBytecodeBuilder::SIZE_JUMPX_OFFSET_BC : EsBytecodeBuilder::SIZE_JUMP_OFFSET_BC;
	pcWk.advance(bclenJump);
	s32 jofsLow = isXofs ? pcWk.getJumpXOffset() : pcWk.getJumpOffset();
	pcWk.advance(bclenJump);
	s32 jofsHigh = isXofs ? pcWk.getJumpXOffset() : pcWk.getJumpOffset();

	n -= jofsLow;
	if((u32)n < (u32)(jofsHigh - jofsLow + 1))
	{
		pcWk.advance(bclenJump + (bclenJump * n));
		s32 jofsWk = isXofs ? pcWk.getJumpXOffset() : pcWk.getJumpOffset();
		if(jofsWk > 0)
		{
			len = jofsWk;
		}
	}

	_lenOpcNextStep = len;
	return true;
}


/*---------------------------------------------------------------------*//**
	ステップ再実行 ⇒ DO_OP
	
	@param otSubst 再実行時の命令コードタイプ
**//*---------------------------------------------------------------------*/
void EsRunEnv::restep(EsOpcodeType otSubst, const EsBcExInfo* exi)
{
	_otSpNextStep = otSubst;

	if((_exiSpNextStep != 0L) && (exi != 0L))
	{
		_exiSpNextStep->copy(exi);
	}

	_isContinueStep = true;
}

/*---------------------------------------------------------------------*//**
	インデックスを取得する ⇒ GET_INDEX, GET_SLOTNO
**//*---------------------------------------------------------------------*/
u32 EsRunEnv::getIndex(s32 offsetPc) const
{
	return (u32)_regsCur->pc()->getU16(offsetPc + 1);	// +1 は命令コード（EsOpcodeType）分
}

/*---------------------------------------------------------------------*//**
	16 bit を超える（かもしれない）インデックスを取得する
	⇒ GET_FULL_INDEX
**//*---------------------------------------------------------------------*/
u32 EsRunEnv::getFullIndex(s32 offsetPc) const
{
	return _indexBase + (u32)_regsCur->pc()->getU16(offsetPc + 1);	// +1 は命令コード（EsOpcodeType）分
}

/*---------------------------------------------------------------------*//**
	アトムを取得する ⇒ LOAD_ATOM, JS_GET_SCRIPT_ATOM
**//*---------------------------------------------------------------------*/
const EsAtom* EsRunEnv::getAtom(s32 offsetPc) const
{
	u32 index = getIndex(offsetPc);

	// ※※※ 実装なし ※※※ ⇒ imacpc

	return _ilCur->getAtom(index);
}

/*---------------------------------------------------------------------*//**
	バイトコードからアトムを取得する ⇒ GET_ATOM_FROM_BYTECODE
**//*---------------------------------------------------------------------*/
const EsAtom* EsRunEnv::getAtomFromBytecode(s32 offsetPc) const
{
	s32 index = _regsCur->pc()->getCompleteIndex(_ctxRef, offsetPc);

	// ※※※ 実装なし ※※※ ⇒ imacpc

	return _ilCur->getAtom(index);
}

/*---------------------------------------------------------------------*//**
	オブジェクトを取得する ⇒ LOAD_OBJECT, JS_GET_SCRIPT_OBJECT
**//*---------------------------------------------------------------------*/
EsObject* EsRunEnv::getObject(s32 offsetPc) const
{
	s32 index = getFullIndex(offsetPc);
	return _ilCur->getObject(index);
}

/*---------------------------------------------------------------------*//**
	関数を取得する ⇒ LOAD_FUNCTION, JS_GET_SCRIPT_FUNCTION
**//*---------------------------------------------------------------------*/
EsFunction* EsRunEnv::getFunction(s32 offsetPc) const
{
	EsObject* objFunc = getObject(offsetPc);
	ASSERT(objFunc->isFunction());
	ASSERT(objFunc == objFunc->getSlots()->getPrivateData());
	EsFunction* func = (EsFunction*)objFunc;
	ASSERT(func->isInterpreted());
	return func;
}

/*---------------------------------------------------------------------*//**
	上域変数を取得する ⇒ js_GetUpvar
**//*---------------------------------------------------------------------*/
const EsValue* EsRunEnv::getUpvar(u32 depthNest, const EsUpvarInfo* upvari) const
{
	depthNest -= upvari->getSkipDepth();
	ASSERT(depthNest < EsContext::SIZE_SF_DISPLAY);
	EsStackFrame* sfrm = _ctxRef->getDisplayStackFrame(depthNest);
	ASSERT(sfrm->getInterlang() != 0L);

	const EsValue* val;
	if(sfrm->getFunction() == 0L)
	{
		val = sfrm->getSlot(sfrm->getInterlang()->getFixedSlotNum() + upvari->getSlotIndex());
	}
	else if(upvari->getSlotIndex() < sfrm->getFunction()->getLocalVarNum(EsFunction::LVK_ARG))
	{
		val = sfrm->getArg(upvari->getSlotIndex());
	}
	else
	{
		u32 slotidx = upvari->getSlotIndex() - sfrm->getFunction()->getLocalVarNum(EsFunction::LVK_ARG);
		ASSERT(slotidx < sfrm->getInterlang()->getSlotNum());
		val = sfrm->getSlot(slotidx);
	}
	return val;
}

/*---------------------------------------------------------------------*//**
	this オブジェクトを取得する ⇒ JS_ComputeThis, js_ComputeThis
**//*---------------------------------------------------------------------*/
EsObject* EsRunEnv::getThisObject(bool isLazy, EsValue* valCallee, EsValue* valThis, EsValue* vaArg)
{
	if(valThis->isNull())
	{
		return getGlobalThisObject(isLazy, valCallee, valThis, vaArg);
	}

	ASSERT(!valThis->isNull());
	if(valThis->isObject())
	{
		EsObject* objThis = valThis->getObject();
		if(objThis == 0L)
		{
			return 0L;
		}
		switch(objThis->getClass()->getClassType())
		{
		case EsSysClasses::CLASSTYPE_CALL:
		case EsSysClasses::CLASSTYPE_BLOCK:
			return getGlobalThisObject(isLazy, valCallee, valThis, vaArg);
		}

		objThis = objThis->getThisObjectOv(_ctxRef);
		if(objThis == 0L)
		{
			return 0L;
		}

		valThis->setValue(objThis);
		return objThis;
	}
	else
	{
		if(!_ctxRef->convPrimitiveValueToObject(valThis))
		{
			return 0L;
		}
		return valThis->getObject();
	}
}

/*---------------------------------------------------------------------*//**
	グローバル this オブジェクトを取得する ⇒ js_ComputeGlobalThis
**//*---------------------------------------------------------------------*/
EsObject* EsRunEnv::getGlobalThisObject(bool isLazy, EsValue* valCallee, EsValue* valThis, EsValue* vaArg)
{
	EsObject* objFunc = valCallee->isObject() ? valCallee->getObject() : 0L;
	EsObject* objThis = 0L;

	if(valCallee->isPrimitive() || ((objFunc != 0L) && (objFunc->getSlots()->getParentObject() == 0L)))
	{
		objThis = _ctxRef->getGlobalObject();
	}
	else if(valCallee->isObject())
	{
		EsStackFrame* sfrmTop = _ctxRef->getTopStackFrame();
		if(isLazy)
		{
			// ※※※ 実装なし ※※※ ⇒ fp->dormantNext = cx->dormantFrameChain;
			// ※※※ 実装なし ※※※ ⇒ cx->dormantFrameChain = fp;
			_ctxRef->setTopStackFrame(_sfrmCur->getDownStackFrame());
			_sfrmCur->setDownStackFrame(0L);
		}

		objThis = objFunc;
		const EsAtom* atom = EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PARENT)->getAtom();
		EsValue spropid(atom);
		u8 spatrbf;
		EsValue val;
		bool isSucceeded = objThis->checkAccessOv(&spatrbf, &val, _ctxRef, &spropid, ACSF_PARENT);

		if(isLazy)
		{
			// ※※※ 実装なし ※※※ ⇒ cx->dormantFrameChain = fp->dormantNext;
			// ※※※ 実装なし ※※※ ⇒ fp->dormantNext = NULL;
			_sfrmCur->setDownStackFrame(_ctxRef->getTopStackFrame());
			_ctxRef->setTopStackFrame(_sfrmCur);
		}

		if(!isSucceeded)
		{
			return 0L;
		}

		if(val.isVoid())
		{
			objThis = objThis->slots()->parentObject();
		}
		else
		{
			ASSERT(val.isObject());
			objThis = val.getObject();
		}

		while(true)
		{
			EsObject* objParent = objThis->slots()->parentObject();
			if(objParent == 0L)	{	break;	}
			objThis = objParent;
		}
	}
	else
	{
		ASSERT(false);
		return 0L;
	}

	objThis = objThis->getThisObjectOv(_ctxRef);
	if(objThis == 0L)
	{
		return 0L;
	}
	valThis->setValue(objThis);
	return objThis;
}

/*---------------------------------------------------------------------*//**
	スタックフレームの this オブジェクトを取得する ⇒ js_ComputeThisForFrame
**//*---------------------------------------------------------------------*/
EsObject* EsRunEnv::getThisObjectForFrame()
{
	if(TFW_IS_FLAG(*_sfrmCur->flags(), EsStackFrame::F_COMPUTED_THIS))
	{
		return _sfrmCur->getThisObject();
	}

	EsObject* objThis = getThisObject(true, _sfrmCur->getCalleeValue(), _sfrmCur->getThisValue(), _sfrmCur->getArgArray());
	if(objThis == 0L)
	{
		return 0L;
	}
	_sfrmCur->setThisObject(objThis);
	TFW_SET_FLAG(*_sfrmCur->flags(), EsStackFrame::F_COMPUTED_THIS, true);

	return objThis;
}

/*---------------------------------------------------------------------*//**
	プリミティブ型の this オブジェクトを取得する ⇒ js_GetPrimitiveThis
**//*---------------------------------------------------------------------*/
bool EsRunEnv::getPrimitiveThisObject(EsValue* valThisObjOut, const EsClass* cls, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg)
{
	if(valThis->isObject())
	{
		EsObject* obj = getThisObject(false, valCallee, valThis, vaArg);
		if(!_ctxRef->instanceOf(obj, cls, vaArg))
		{
			return false;
		}
		valThisObjOut->copy(obj->getSlots()->getPrivateValue());
		return true;
	}
	valThisObjOut->copy(valThis);
	return true;
}

/*---------------------------------------------------------------------*//**
	再定義チェック ⇒ js_CheckRedeclaration
**//*---------------------------------------------------------------------*/
bool EsRunEnv::checkRedeclaration(EsScopeProperty** spropOut, EsObject** objScopeOwnerOut, EsObject* obj, const EsValue* spropid, u8 spatrbfNew, bool isInitializer) const
{
	EsScopeProperty* spropWk = 0L;
	EsObject* objScopeOwnerWk = 0L;
	if(!obj->findPropertyOv(&spropWk, &objScopeOwnerWk, _ctxRef, spropid))
	{
		return false;
	}
	if(spropWk == 0L)
	{
		return true;
	}
	ASSERT(objScopeOwnerWk != 0L);

	u8 spatrbfOld;
	if(!objScopeOwnerWk->getPropertyAttributeFlagsOv(&spatrbfOld, _ctxRef, spropid, spropWk))
	{
		objScopeOwnerWk->dropPropertyOv(_ctxRef, spropWk);
		return false;
	}

	if(spropOut == 0L)
	{
		objScopeOwnerWk->dropPropertyOv(_ctxRef, spropWk);
		spropWk = 0L;
	}
	else
	{
		*objScopeOwnerOut = objScopeOwnerWk;
		*spropOut = spropWk;
	}

	bool isFunction = false;
	if(isInitializer)
	{
		// 新規オブジェクトのオーバーライドされたプロパティは許可する
		if(objScopeOwnerWk != obj)
		{
			return true;
		}
	}
	else
	{
		// 再定義の際に読み込み専用属性はいくつかのケースで許可する
		if(!TFW_IS_FLAG(spatrbfOld, EsScopeProperty::ATRBF_READONLY) && !TFW_IS_FLAG(spatrbfNew, EsScopeProperty::ATRBF_READONLY))
		{
			// 変数と関数の再定義は許可する
			if(!TFW_IS_FLAG(spatrbfNew, EsScopeProperty::ATRBF_OBJ_GETTER) &&
				!TFW_IS_FLAG(spatrbfNew, EsScopeProperty::ATRBF_OBJ_SETTER) )
			{
				return true;
			}

			// ？ゲッタからセッタへの変更は許可する？
			if((~(spatrbfOld ^ spatrbfNew) & (EsScopeProperty::ATRBF_OBJ_GETTER | EsScopeProperty::ATRBF_OBJ_SETTER)) == 0)
			{
				return true;
			}

			// 非永久プロパティは許可する
			if(!TFW_IS_FLAG(spatrbfOld, EsScopeProperty::ATRBF_PERMANENT))
			{
				return true;
			}
		}

		isFunction = TFW_IS_FLAG(spatrbfOld, EsScopeProperty::ATRBF_OBJ_GETTER) || TFW_IS_FLAG(spatrbfOld, EsScopeProperty::ATRBF_OBJ_SETTER);
		if(!isFunction)
		{
			EsValue val;
			if(!obj->callGetPropertyOv(&val, _ctxRef, spropid))
			{
				return false;
			}
			isFunction = val.isFunctionObject();
		}
	}

	_ctxRef->addRunError(EsError::ID_REDECLARED_VAR);
	return false;
}

/*---------------------------------------------------------------------*//**
	if 条件文によるジャンプ ⇒ TRY_BRANCH_AFTER_COND
**//*---------------------------------------------------------------------*/
void EsRunEnv::jumpIfCondition(EsOpcodeType ot, bool cond, s32 spdec)
{
	// 次の命令コードを取得する
	EsOpcodeType otNext;
	s32 pcofsNext;
	if(!_regsCur->pc()->peekNextOpcode(&otNext, &pcofsNext, _ctxRef, ot))	{	return;	}

	if((otNext == OP_IFEQ) || (otNext == OP_IFNE))
	{
		_regsCur->advanceSp(- spdec);
		if((cond && (otNext == OP_IFNE)) || (!cond && (otNext == OP_IFEQ)))
		{
			_regsCur->advancePc(pcofsNext);
			s32 offset = _regsCur->pc()->getJumpOffset();	// ⇒ GET_JUMP_OFFSET(regs.pc);
			jumpBranch(offset);			// ⇒ BRANCH(len);
		}
		else
		{
			#define OP_IFEQ_LENGTH	 EsOpcode::getSpec(OP_IFEQ)->_length	// ⇒ JSOP_IFEQ_LENGTH
			_lenOpcNextStep = pcofsNext + OP_IFEQ_LENGTH;
		}
	}
	else
	{
		_regsCur->advanceSp(-1);
		EsValue valCond(cond);
		_regsCur->storeOperand(-1, &valCond);
	}
}

/*---------------------------------------------------------------------*//**
	分岐ジャンプ ⇒ BRANCH
**//*---------------------------------------------------------------------*/
void EsRunEnv::jumpBranch(s32 offsetJump)
{
	_regsCur->advancePc(offsetJump);

	EsBcExInfo* exi = 0L;
	EsBcExInfo exiWk;
	if(_ilCur->isBcExInfoAdd())
	{
		exi = &exiWk;
		exiWk.fromBytecode(_regsCur->pc());
		_regsCur->advancePc(EsBcExInfo::LEN_BC);	// 拡張情報分のプログラムカウンタを進める
	}

	EsOpcodeType ot = (EsOpcodeType)_regsCur->pc()->get(0);
	if(offsetJump <= 0)
	{
		if(ot == OP_NOP)
		{
			_regsCur->advancePc(1);	// 1 == EsOpcode::getSpec(OP_NOP)->_length
			if(_ilCur->isBcExInfoAdd())
			{
				exiWk.fromBytecode(_regsCur->pc());
				_regsCur->advancePc(EsBcExInfo::LEN_BC);	// 拡張情報分のプログラムカウンタを進める
			}
			ot = (EsOpcodeType)_regsCur->pc()->get(0);
		}
	}

	restep(ot, exi);

	_lenOpcNextStep = 0;	// 手動でプログラムカウンタを送ったので自動をオフに
}

/*---------------------------------------------------------------------*//**
	セット後の OP_POP をスキップする ⇒ SKIP_POP_AFTER_SET
**//*---------------------------------------------------------------------*/
bool EsRunEnv::skipPopAfterSet(EsOpcodeType ot, s32 spofsDecrement)
{
	// 次の命令コードを取得する
	EsOpcodeType otNext;
	s32 pcofsNext;
	if(!_regsCur->pc()->peekNextOpcode(&otNext, &pcofsNext, _ctxRef, ot))	{	return false;	}
	
	// OP_POP だったらスキップ処理
	if(otNext != OP_POP)
	{
		return false;	// OP_POP ではなかった
	}
	_regsCur->advanceSp(- spofsDecrement);

	// POP の省略．次はこの分（pcofsNext, OP_POP）のプログラムカウンタを進めて再開
	_lenOpcNextStep = pcofsNext + EsOpcode::getSpec(OP_POP)->_length;	// ⇒ regs.pc += oplen + JSOP_POP_LENGTH;

	return true;	// OP_POP であった
}

/*---------------------------------------------------------------------*//**
	アクセサ取得 ⇒ NATIVE_GET
**//*---------------------------------------------------------------------*/
bool EsRunEnv::callGetPropertyObjself(EsValue* val, EsContext* ctx, EsObject* obj, EsObject* objScopeOwn, EsScopeProperty* sprop)
{
	if(sprop->getGetterFunc() == 0L)
	{
		s32 slotidx = sprop->getSlotIndex();
		ASSERT((slotidx != EsScopeProperty::SLOTIDX_INVALUD) || (sprop->getSetterFunc() != 0L));
		if(slotidx != EsScopeProperty::SLOTIDX_INVALUD)
		{
			ASSERT((0 <= slotidx) && (slotidx < objScopeOwn->getSlots()->getNum()));
			val->copy(objScopeOwn->getSlots()->getValue(slotidx));
		}
		else
		{
			val->setVoid();
		}
		return true;
	}
	else
	{
		return obj->callGetPropertyNative(val, ctx, objScopeOwn, sprop);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsRunEnv::EsRunEnv()
	: _exiCurStep(0L)
	, _exiSpNextStep(0L)
{
	clear();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsRunEnv::~EsRunEnv()
{
	delete _exiCurStep;
	delete _exiSpNextStep;
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void EsRunEnv::clear()
{
	_ctxRef = 0L;
	_ilCur = 0L;
	_sfrmCur = 0L;
	_regsCur = 0L;
	_indexBase = 0;
	_depthCall = 0;
	_sfrmInvoke = 0L;
	_valResult = 0L;
	_isContinueStep = false;
	_lenOpcNextStep = 0;
	_otCurStep = OP_NULL;
	_otSpNextStep = OP_NULL;

	delete _exiCurStep;
	_exiCurStep = 0L;
	delete _exiSpNextStep;
	_exiSpNextStep = 0L;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void EsRunEnv::copy(const EsRunEnv* src)
{
	_ctxRef = src->_ctxRef;
	_ilCur = src->_ilCur;
	_sfrmCur = src->_sfrmCur;
	_regsCur = src->_regsCur;
	_indexBase = src->_indexBase;
	_depthCall = src->_depthCall;
	_sfrmInvoke = src->_sfrmInvoke;
	_valResult = src->_valResult;
	_isContinueStep = src->_isContinueStep;
	_lenOpcNextStep = src->_lenOpcNextStep;
	_otCurStep = src->_otCurStep;
	_otSpNextStep = src->_otSpNextStep;

	delete _exiCurStep;
	_exiCurStep = 0L;
	if(src->_exiCurStep != 0L)
	{
		_exiCurStep = new EsBcExInfo(*src->_exiCurStep);
	}

	delete _exiSpNextStep;
	_exiSpNextStep = 0L;
	if(src->_exiSpNextStep != 0L)
	{
		_exiSpNextStep = new EsBcExInfo(*src->_exiSpNextStep);
	}
}

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	スタックポインタ デバッグ
**//*---------------------------------------------------------------------*/
void EsRunEnv::_debug_Sp()
{
	static int iii = 0;
	if((++iii) >= 10)
	{
		TRACE("![%d]! sp=[%d/%d]", iii, _regsCur->getCurSpIndex(), _regsCur->getSpNum());
	}
}
#endif

////////////////////////////////////////////////////////////////////////////

ES_END_NS
