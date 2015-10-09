/***********************************************************************//**
 *	SsCluster.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/01/24.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOUMA_SCRIPT_SS_CLUSTER_H_
#define _SRCR_SOUMA_SCRIPT_SS_CLUSTER_H_

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

class MagicCluster;

/*---------------------------------------------------------------------*//**
 *	ソウマ スクリプト クラス／プロトタイプ
 *
**//*---------------------------------------------------------------------*/
class SsCluster
{
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
		inline MagicCluster* cluster() const { return (MagicCluster*)getParam(); }
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
		NsPrototype() : BasePrototype(SoumaScriptKeywords::S_CLASS_NS_CLUSTER) {}
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
		EnPrototype() : BasePrototype(SoumaScriptKeywords::S_CLASS_EN_CLUSTER) {}
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
		NsClass() : FoundationClass(SoumaScriptKeywords::S_CLASS_NS_CLUSTER) {}
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
		EnClass() : FoundationClass(SoumaScriptKeywords::S_CLASS_EN_CLUSTER) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool init();
	static bool makeEsObject(EsObject** objMc, SoumaScriptContext* ssctx, MagicCluster* mc, s32 skwdOfs);
	static MagicCluster* getMagicCluster(EsObject* objMc);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
private:
	static s32 evokeElement(EsContext* ctx, Variables* v, s32 elTrg, s32 eeAdd);

public:
	static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callGetElementQuantity(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callEvokeElement(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callEvokeOriginalElements(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callEvokePresetElements(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callDissolve(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callMaterialize(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callSetFocusUnit(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callSynthesize(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);

	//======================================================================
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUMA_SCRIPT_SS_CLUSTER_H_
