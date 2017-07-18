/***********************************************************************//**
 *	SsConsole.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/21.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SsConsole.h"

// Friends
#include "SoumaScript.h"
#include "SoumaScriptContext.h"
#include "SoumaScriptKeywords.h"
#include "../../body/Conductor.h"
#include "../../body/Game.h"

// External
#include "../../../es/EsContext.h"
#include "../../../es/EsKeyword.h"
#include "../../../es/EsObject.h"
#include "../../../es/EsRunEnv.h"
#include "../../../es/EsSysClasses.h"
#include "../../../es/EsSysPrototypes.h"
#include "../../../tfw/lib/Color.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SsConsole::UnitClass メソッド

/*---------------------------------------------------------------------*//**
	ソウマスクリプトクラスの作成
**//*---------------------------------------------------------------------*/
EsObject* SsConsole::FoundationClass::makeClassObject(EsContext* ctx, EsObject* objParent, s32 skwdOfs) const
{
	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, this, 0L, SsConsole::callConstructor);	// ⇒ proto = JS_InitClass(cx, obj, NULL, &js_FunctionClass, Function, 1, function_props, function_methods, NULL, NULL);
	if(objPtype == 0L)
	{
		return 0L;
	}
	// 非静的プロパティを定義する
	///objPtype->defineClassProperty(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_CONSDEPTH + skwdOfs), SsConsole::callGetConsdepth, g_funcDummyPropertySetter, -1, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED);
	// 非静的関数を定義する
///	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_IN + skwdOfs), callIn, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_OUT + skwdOfs), callOut, 0);
	// 静的プロパティを定義する
	///objCtor->defineClassProperty(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_CONSTP_NS_ACCESSORY + skwdOfs), callAccessory, g_funcDummyPropertySetter, -1, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED);

	return objPtype;
}

/*---------------------------------------------------------------------*//**
	ソウマスクリプトクラスのコンストラクタ
**//*---------------------------------------------------------------------*/
SsConsole::FoundationClass::FoundationClass(SoumaScriptKeywords::Keyword kwd)
	: SsBase::BaseClass(kwd, EsClass::F_HAS_PRIVATE, 0)
{
	_funcConstruct = SsConsole::callConstructor;
}


//==========================================================================
// SsConsole メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool SsConsole::init()
{
	EsSysPrototypes::registerExternalPrototype(NsPrototype::getInstance());
	EsSysPrototypes::registerExternalPrototype(EnPrototype::getInstance());
	EsSysClasses::registerExternalClass(NsClass::getInstance(), true);
	EsSysClasses::registerExternalClass(EnClass::getInstance(), true);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es オブジェクトの作成
**//*---------------------------------------------------------------------*/
bool SsConsole::makeEsObject(EsObject** objConsole, SoumaScriptContext* ssctx, s32 skwdOfs)
{
	const EsAtom* atom = SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_CLASS_NS_CONSOLE + skwdOfs);	// アトム取得
	EsValue valArg;
	SsBase::BaseVariables::setVariables(&valArg, 0L);
	if(!ssctx->getEsContext()->getRunEnv()->makeClassInstance(objConsole, atom, &valArg, 1))
	{
		ASSERT(false);
		return false;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	キーワード言語オフセットを得る
**//*---------------------------------------------------------------------*/
s32 SsConsole::getKeywordLangOffset(const EsObject* objThis)
{
	return (objThis->getClass()->getClassType() == EnClass::getInstance()->getClassType()) ? 1 : 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	Es 関数 - コンストラクタ
**//*---------------------------------------------------------------------*/
bool SsConsole::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis->slots()->setPrivateValue((numArg > 0) ? &vaArg[0] : 0L);

	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - アウトプット
		de / out
**//*---------------------------------------------------------------------*/
bool SsConsole::callIn(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	///（未実装）

	// 戻り値の設定
	valRet->setValue((s32)SoumaScript::RET_OK);	// <OK> 成功
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - インプット
		im / in
**//*---------------------------------------------------------------------*/
bool SsConsole::callOut(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	VcString msg;
	if(numArg > 0)
	{
		const EsValue* valArg = &vaArg[0];
		if(valArg->isInt())				{	msg.set(VcString::format("%d", valArg->getInt()));		}
		else if(valArg->isBoolean())	{	msg.set(valArg->getBoolean() ? "true" : "false");		}
		else if(valArg->isDouble())		{	msg.set(VcString::format("%f", valArg->getDouble()));	}
		else if(valArg->isString())		{	msg.set(valArg->getString());							}
		else							{	valArg->toString(&msg, ctx);							}
	}

	// アクションウインドウにデバッグ出力
	ColorU8 col;
	Conductor::getUserSsDebugColor(&col);
	Conductor* conductor = Game::getGame()->getConductor();
	conductor->on(&msg, &col, 0);

	// 戻り値の設定
	valRet->setValue((s32)SoumaScript::RET_OK);	// <OK> 成功
	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
