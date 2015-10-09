/***********************************************************************//**
 *	SsConsole.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/21.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOUMA_SCRIPT_SS_CONSOLE_H_
#define _SRCR_SOUMA_SCRIPT_SS_CONSOLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "SsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

ES_END_NS

SRCR_BEGIN_NS

class SoumaScriptContext;
class Unit;

/*---------------------------------------------------------------------*//**
 *	コンソール クラス／プロトタイプ
 *
**//*---------------------------------------------------------------------*/
class SsConsole
{
	//======================================================================
	// 定数

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	変数
	 *
	**//*-----------------------------------------------------------------*/
	class Variables : public SsBase::BaseVariables
	{
		//==================================================================
		// メソッド
	public:
		inline Unit* unit() const { return (Unit*)getParam(); }
		Variables(const EsValue* val) : BaseVariables(val) {}
	};

	/*-----------------------------------------------------------------*//**
	 *	プロトタイプ クラス - ネイティブ ソウマ語ベース
	 *
	**//*-----------------------------------------------------------------*/
	class NsPrototype : public SsBase::BasePrototype
	{
		//==================================================================
		// メソッド
	public:
		static const NsPrototype* getInstance() { static NsPrototype instance; return &instance; }
		EsObject* makeClassObject(EsContext* ctx, EsObject* objParent) const { return NsClass::getInstance()->makeClassObject(ctx, objParent, 0); }
		NsPrototype() : SsBase::BasePrototype(SoumaScriptKeywords::S_CLASS_NS_CONSOLE) {}
	};

	/*-----------------------------------------------------------------*//**
	 *	プロトタイプ クラス - 英語ベース
	 *
	**//*-----------------------------------------------------------------*/
	class EnPrototype : public SsBase::BasePrototype
	{
		//==================================================================
		// メソッド
	public:
		static const EnPrototype* getInstance() { static EnPrototype instance; return &instance; }
		EsObject* makeClassObject(EsContext* ctx, EsObject* objParent) const { return EnClass::getInstance()->makeClassObject(ctx, objParent, 1); }
		EnPrototype() : SsBase::BasePrototype(SoumaScriptKeywords::S_CLASS_EN_CONSOLE) {}
	};

	/*-----------------------------------------------------------------*//**
	 *	スクリプト基底クラス
	 *
	**//*-----------------------------------------------------------------*/
	class FoundationClass : public SsBase::BaseClass
	{
		//==================================================================
		// メソッド
	public:
		EsObject* makeClassObject(EsContext* ctx, EsObject* objParent, s32 skwdOfs) const;
		FoundationClass(SoumaScriptKeywords::Keyword kwd);
	};

	/*-----------------------------------------------------------------*//**
	 *	スクリプト クラス - ネイティブ ソウマ語ベース
	 *
	**//*-----------------------------------------------------------------*/
	class NsClass : public FoundationClass
	{
		//==================================================================
		// メソッド
	public:
		static const NsClass* getInstance() { static NsClass instance; return &instance; }
		NsClass() : FoundationClass(SoumaScriptKeywords::S_CLASS_NS_CONSOLE) {}
	};

	/*-----------------------------------------------------------------*//**
	 *	スクリプト クラス - 英語ベース
	 *
	**//*-----------------------------------------------------------------*/
	class EnClass : public FoundationClass
	{
		//==================================================================
		// メソッド
	public:
		static const EnClass* getInstance() { static EnClass instance; return &instance; }
		EnClass() : FoundationClass(SoumaScriptKeywords::S_CLASS_EN_CONSOLE) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool init();
	static bool makeEsObject(EsObject** objConsole, SoumaScriptContext* ssctx, s32 skwdOfs);
	static s32 getKeywordLangOffset(const EsObject* objThis);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callIn(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callOut(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);

	//======================================================================
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUMA_SCRIPT_SS_CONSOLE_H_
