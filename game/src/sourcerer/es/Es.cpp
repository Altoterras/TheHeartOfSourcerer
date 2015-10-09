/***********************************************************************//**
 *	Es.cpp
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
#include "Es.h"

// Friends
#include "EsBytecodeBuilder.h"
#include "EsClass.h"
#include "EsContext.h"
#include "EsSysClasses.h"
#include "EsInterlang.h"
#include "EsObject.h"
#include "EsParser.h"
#include "EsExternalExtension.h"
#include "EsRunEnv.h"
#include "EsStackFrame.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// 静的関数

/*---------------------------------------------------------------------*//**
	テスト関数 _debug_trace
**//*---------------------------------------------------------------------*/
#if defined(_DEBUG)
ES_END_NS
#include "EsValue.h"
ES_BEGIN_NS
static bool callDebugTrace(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	double dval = -1;
	const VcString* str = 0L;
	const EsValue* valArg = &vaArg[0];
	if(numArg > 0)
	{
		if(valArg->getType() == EsValue::VTYPE_INT)			{	dval = valArg->getInt();				}
		else if(valArg->getType() == EsValue::VTYPE_DOUBLE)	{	dval = valArg->getDouble();				}
		else if(valArg->getType() == EsValue::VTYPE_STRING)	{	str = valArg->getString();				}
		else if(valArg->getType() == EsValue::VTYPE_ATOM)	{	str = valArg->getAtom()->getString();	}
	}

	TRACE("{callDebugTrace} [_debug_trace] INT=%d, DOUBLE=%f, STRING=%s\n", (int)dval, dval, (str != 0L) ? str->getRaw() : "");

	valRet->setValue((s32)101);
	return true;
}
static VcString s_nameDebugTrace("_debug_trace");
#endif

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Es::Es()
	: _ilParseResultTmp(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Es::~Es()
{
}

/*---------------------------------------------------------------------*//**
	スクリプト解析準備 ⇒ main
**//*---------------------------------------------------------------------*/
bool Es::prepareParse(EsContext* ctx, const EsKeywordSet* kwsetRef, u32 mflags, EsExternalExtension* extextRef, EsExtParam paramParse)
{
	// コンテキストの解析準備
	ctx->prepareParse(kwsetRef, extextRef, mflags, paramParse);

	// グローバルオブジェクトの作成
	EsObject* objGlobal = ctx->newObject(EsSysClasses::getClass(EsSysClasses::CLASSTYPE_GLOBAL), 0L, 0L);	// ⇒ glob = JS_NewObject(cx, &global_class, NULL, NULL); @main
	ctx->setGlobalObject(objGlobal);
	// グローバルオブジェクトに外部拡張ネイティブ関数を追加する
	#if defined(_DEBUG)
		objGlobal->addFunction(ctx, &s_nameDebugTrace, callDebugTrace, 0, EsExtParam());	// テスト関数
	#endif
	if(extextRef != 0L)
	{
		// プラグイン関数を追加する
		for(int i = 0; i < extextRef->getExtNativeFuncNum(); i++)
		{
			const EsExternalExtension::EsExtNativeFunc* ef = extextRef->getExtNativeFunc(i);
			objGlobal->addFunction(ctx, ef->getName(), ef->getFunc(), 0, ef->getDefParam());
		}
	}
	
	// その他初期化
	_ilParseResultTmp = 0L;

	return true;
}

/*---------------------------------------------------------------------*//**
	メインスクリプト解析
**//*---------------------------------------------------------------------*/
bool Es::parse(EsContext* ctx, const VcString* strScript)
{
	return parse(ctx, strScript, 0L, EsBytecodeBuilder::BF_DONOT_WANT_RET_VAL | EsBytecodeBuilder::BF_OPT_NAME_REF_ON_SCOPE);
}

/*---------------------------------------------------------------------*//**
	スクリプト解析
**//*---------------------------------------------------------------------*/
bool Es::parse(EsContext* ctx, const VcString* strScript, EsStackFrame* sfrmCaller, u32 bflags)
{
	if(ctx == 0L)	{	return false;	}

	bool isMainScript = (sfrmCaller == 0L);

	// コンテキストにコードをセット
	if(isMainScript)	// メインスクリプトのみ
	{
		ctx->setCode(strScript);
		strScript = ctx->getScriptCode();
	}

	// 解析を行う
	EsParser parser;
	EsInterlang* il = parser.parse(ctx, strScript, bflags);
	if(il == 0L)
	{
		return false;
	}
	_ilParseResultTmp = il;	// 解析結果の一時保存

	return true;
}

/*---------------------------------------------------------------------*//**
	メインスクリプトの実行準備
**//*---------------------------------------------------------------------*/
bool Es::prepareRun(EsContext* ctx, EsExtParam paramRun)
{
	return prepareRun(ctx, 0L, 0, 0L, paramRun);
}

/*---------------------------------------------------------------------*//**
	実行準備
**//*---------------------------------------------------------------------*/
bool Es::prepareRun(EsContext* ctx, EsStackFrame* sfrmCaller, u16 flagsSfrm, EsValue* valRet, EsExtParam paramRun)
{
	if(ctx == 0L)				{	return false;	}
	if(_ilParseResultTmp == 0L)	{	return false;	}

	// コンテキストの実行準備
	return ctx->prepareRun(_ilParseResultTmp, sfrmCaller, flagsSfrm, valRet, paramRun);
}

/*---------------------------------------------------------------------*//**
	スクリプト実行
**//*---------------------------------------------------------------------*/
bool Es::runStep(EsContext* ctx, EsRunStat* stat)
{
	if(ctx == 0L)				{	return false;	}
	if(ctx->getRunEnv() == 0L)	{	return false;	}

	// ステップ実行
	return ctx->getRunEnv()->step(stat);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

////////////////////////////////////////////////////////////////////////////

ES_END_NS
