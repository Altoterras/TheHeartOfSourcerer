/***********************************************************************//**
 *	SsSouma.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/10/27.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOUMA_SCRIPT_SS_SOUMA_H_
#define _SRCR_SOUMA_SCRIPT_SS_SOUMA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "SsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

ES_BEGIN_NS

class EsArray;

ES_END_NS

SRCR_BEGIN_NS

class Elene;
class Souma;
class Unit;

/*---------------------------------------------------------------------*//**
 *	ソウマ スクリプト クラス／プロトタイプ
 *
**//*---------------------------------------------------------------------*/
class SsSouma
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	変数
	 *
	**//*-----------------------------------------------------------------*/
	class Variables : public SsBase::BaseVariables
	{
		//==================================================================
		// メソッド
	public:
		inline Souma* souma() const { return (Souma*)getParam(); }
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
		NsPrototype() : BasePrototype(SoumaScriptKeywords::S_CLASS_NS_SOUMA) {}
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
		EnPrototype() : BasePrototype(SoumaScriptKeywords::S_CLASS_EN_SOUMA) {}
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
		NsClass() : FoundationClass(SoumaScriptKeywords::S_CLASS_NS_SOUMA) {}
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
		EnClass() : FoundationClass(SoumaScriptKeywords::S_CLASS_EN_SOUMA) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool init();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
private:
	static s32 getKeywordLangOffset(const EsObject* objThis);
	static bool makeUnitEsArray(EsArray** objArray, const List<Unit*>* listUnits, EsContext* ctx, EsObject* objThis);

public:
	static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callCentering(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callMeditate(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callGetConsDepth(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callGetConsole(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callCreateCluster(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callCreateIntent(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callGetFocusUnits(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callSetFocusUnit(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callGetMasterUnit(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callEnumUnitsByName(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callEnumAroundUnits(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callEnumRecognizedUnits(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callEnumSpiritKnowsUnits(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callWait(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callSlowChant(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
//	static bool callPreset(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
//	static bool computeEleneBalance(Elene* eleneNql, Elene* eleneCql, u32 flagFunc, s32 eneTotalSpec);

	//======================================================================
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUMA_SCRIPT_SS_SOUMA_H_
