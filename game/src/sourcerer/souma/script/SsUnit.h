/***********************************************************************//**
 *	SsUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/11/09.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOUMA_SCRIPT_SS_UNIT_H_
#define _SRCR_SOUMA_SCRIPT_SS_UNIT_H_

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
 *	ユニット クラス／プロトタイプ
 *
**//*---------------------------------------------------------------------*/
class SsUnit
{
	//======================================================================
	// 定数

	// 装備位置
	enum EquipPlaceConst
	{
		EQPC_NULL,		// （未定義）
		EQPC_LHAND,		// 左手
		EQPC_RHAND,		// 右手
		EQPC_ACCSR,		// アクセサリー
		NUM_EQPC
	};

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
		NsPrototype() : SsBase::BasePrototype(SoumaScriptKeywords::S_CLASS_NS_UNIT) {}
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
		EnPrototype() : SsBase::BasePrototype(SoumaScriptKeywords::S_CLASS_EN_UNIT) {}
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
		NsClass() : FoundationClass(SoumaScriptKeywords::S_CLASS_NS_UNIT) {}
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
		EnClass() : FoundationClass(SoumaScriptKeywords::S_CLASS_EN_UNIT) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool init();
	static bool makeEsObject(EsObject** objUnit, SoumaScriptContext* ssctx, Unit* unit, s32 skwdOfs);
	static Unit* getUnit(EsObject* objUnit);
	static s32 getKeywordLangOffset(const EsObject* objThis);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callAccessory(EsValue* valRet, EsContext* ctx, EsObject* objThis, const EsValue* id);
	static bool callLeftHand(EsValue* valRet, EsContext* ctx, EsObject* objThis, const EsValue* id);
	static bool callRightHand(EsValue* valRet, EsContext* ctx, EsObject* objThis, const EsValue* id);
	static bool callGetConsdepth(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callGetLifeEnergy(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callGetMaxLifeEnergy(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callGetEnergyDamage(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callGetPhysicalDamage(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callIsEnemy(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callIsFriend(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callIsItem(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callIsPutItem(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callIsMaster(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool callGetEquipedUnit(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);

	//======================================================================
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUMA_SCRIPT_SS_UNIT_H_
