/***********************************************************************//**
 *	SsBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/11/09.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOUMA_SCRIPT_SS_BASE_H_
#define _SRCR_SOUMA_SCRIPT_SS_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

#include "SoumaScriptKeywords.h"

#include "../../../es/EsClass.h"
#include "../../../es/EsPrototype.h"
#include "../../../es/EsValue.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

class EsKeyword;
class EsValue;

ES_END_NS

SRCR_BEGIN_NS

class SoumaScriptContext;

using namespace es;

/*---------------------------------------------------------------------*//**
 *	ソウマ スクリプト クラス／プロトタイプ ベース
 *
**//*---------------------------------------------------------------------*/
class SsBase
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	汎用変数基底クラス
	 *
	**//*-----------------------------------------------------------------*/
	class BaseVariables
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// 汎用パラメータを得る
		inline void* getParam() const { return reinterpret_cast<void*>(_val->getInt64()); }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		BaseVariables(const EsValue* val) : _val(val) {}
		static void setVariables(EsValue* val, void* param) { val->setValue(reinterpret_cast<u64>(param)); }

		//==================================================================
		// 変数
	private:
		const EsValue* _val;
	};

	/*-----------------------------------------------------------------*//**
	 *	プロトタイプ基底クラス
	 *
	**//*-----------------------------------------------------------------*/
	class BasePrototype : public EsPrototype
	{
		//==================================================================
		// メソッド
	public:
		virtual EsObject* makeClassObject(EsContext* ctx, EsObject* obj) const = 0;
		bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const;
		BasePrototype(SoumaScriptKeywords::Keyword kwd);
	};

	/*-----------------------------------------------------------------*//**
	 *	スクリプト基底クラス
	 *
	**//*-----------------------------------------------------------------*/
	class BaseClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		virtual EsObject* makeClassObject(EsContext* ctx, EsObject* objParent, s32 skwdOfs) const = 0;
		BaseClass(SoumaScriptKeywords::Keyword kwd, u16 flags, u16 numDefaultReservedSlot);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool init();

	//======================================================================
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUMA_SCRIPT_SS_BASE_H_
