/***********************************************************************//**
 *	EsString.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/08/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsString.h"

// Friends
#include "EsArray.h"
#include "EsContext.h"
#include "EsError.h"
#include "EsSysClasses.h"
#include "EsKeywordSet.h"
#include "EsObject.h"
#include "EsResolver.h"
#include "EsRunEnv.h"
#include "EsScopeProperty.h"
#include "EsStackFrame.h"
#include "EsValue.h"

// External
#include "../../tfw/lib/TypeUtils.h"
#include "../../tfw/string/StringUtils.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsString::EsStringAtomHashOp メソッド

/*---------------------------------------------------------------------*//**
	エントリのサイズを返す
**//*---------------------------------------------------------------------*/
u32 EsString::EsStringAtomHashOp::getEntrySize()
{
	return sizeof(EsAtom::EsAtomHashEntry);
}

/*---------------------------------------------------------------------*//**
	エントリバッファを確保する
**//*---------------------------------------------------------------------*/
void* EsString::EsStringAtomHashOp::newEntryBuffer(u32 numEntry)
{
	EsAtom::EsAtomHashEntry* arr = new EsAtom::EsAtomHashEntry[numEntry];
	return arr;
}

/*---------------------------------------------------------------------*//**
	エントリバッファを削除する
**//*---------------------------------------------------------------------*/
void EsString::EsStringAtomHashOp::deleteEntryBuffer(void* entrybuf)
{
	EsAtom::EsAtomHashEntry* arr = (EsAtom::EsAtomHashEntry*)entrybuf;
	delete[] arr;
}

/*---------------------------------------------------------------------*//**
	ハッシュコードを算出する
**//*---------------------------------------------------------------------*/
u32 EsString::EsStringAtomHashOp::hashCode(const void* key)
{
	const VcString* str = (const VcString*)key;
	u32 h = 0;
	for(int i = 0; i < str->getLength(); i++)
	{
		h = TFW_BIT_ROT_L_32(h, 4) ^ str->getAt(i);
	}
	return h;
}

/*---------------------------------------------------------------------*//**
	エントリが同一かどうか判定する
**//*---------------------------------------------------------------------*/
bool EsString::EsStringAtomHashOp::matchEntry(HashEntry* entry, const void* key)
{
	const EsAtom* atom = ((EsAtom::EsAtomHashEntry*)entry)->getAtom();
	const VcString* str1 = atom->getString();
	const VcString* str2 = (VcString*)key;
	if(str1 == str2)					{	return true;	}
	if((str1 == 0L) && (str2 != 0L))	{	return false;	}
	if((str1 != 0L) && (str2 == 0L))	{	return false;	}
	return str1->equals(str2);
}

/*---------------------------------------------------------------------*//**
	エントリ初期化
**//*---------------------------------------------------------------------*/
bool EsString::EsStringAtomHashOp::initEntry(HashEntry* entry, const void* key)
{
	EsAtom::EsAtomHashEntry* ae = (EsAtom::EsAtomHashEntry*)entry;
	EsAtom* atom = new EsAtom((const VcString*)key);
	ae->setAtom(atom);
	return true;
}

/*---------------------------------------------------------------------*//**
	エントリ破棄
**//*---------------------------------------------------------------------*/
void EsString::EsStringAtomHashOp::cleanupEntry(HashEntry* entry)
{
	EsAtom::EsAtomHashEntry* ae = (EsAtom::EsAtomHashEntry*)entry;
	delete ae->getAtom();
	entry->clear();
}


//==========================================================================
// EsString::EsStringExtAtomHashOp メソッド

/*---------------------------------------------------------------------*//**
	エントリ初期化
**//*---------------------------------------------------------------------*/
bool EsString::EsStringRefAtomHashOp::initEntry(HashEntry* entry, const void* key)
{
	EsAtom::EsAtomHashEntry* ae = (EsAtom::EsAtomHashEntry*)entry;
	return true;
}

/*---------------------------------------------------------------------*//**
	エントリ破棄
**//*---------------------------------------------------------------------*/
void EsString::EsStringRefAtomHashOp::cleanupEntry(HashEntry* entry)
{
	EsAtom::EsAtomHashEntry* ae = (EsAtom::EsAtomHashEntry*)entry;
	entry->clear();
}


//==========================================================================
// EsString::EsStringPrototype メソッド

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool EsString::EsStringPrototype::initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const
{
	EsObject* objPtype = EsString::EsStringClass::makeClassObject(ctx, obj);
	*objPrototypeOut = objPtype;
	return objPtype != 0L;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsString::EsStringPrototype::EsStringPrototype()
	: EsPrototype(EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_STRING)->getAtom(), EsSysPrototypes::PTYPEKIND_STRING)
{
}


//==========================================================================
// EsString::EsStringClass メソッド

/*---------------------------------------------------------------------*//**
	文字列スクリプトクラスの作成 ⇒ js_InitStringClass
**//*---------------------------------------------------------------------*/
EsObject* EsString::EsStringClass::makeClassObject(EsContext* ctx, EsObject* objParent)
{
	// グローバルオブジェクトに URI 関連の関数、escape, unescape, decodeURI, encodeURI を定義する
	// URI は当面取り扱わない… ※※※ 実装不要 ※※※ ⇒ string_functions

	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, getInstance(), 0L, &callConstructor);	// ⇒ proto = JS_InitClass(cx, obj, NULL, &js_ArrayClass, js_Array, 1, array_props, array_methods, NULL, NULL);
	if((objPtype == 0L) || (objCtor == 0L))
	{
		return 0L;
	}
	// 非静的プロパティを定義する
	///objPtype->defineClassProperty();
	// 非静的関数を定義する ⇒ string_methods
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOSTRING)->getAtom(), callToString, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_SUBSTRING)->getAtom(), callSubstring, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOLOWERCASE)->getAtom(), callToLowerCase, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOUPPERCASE)->getAtom(), callToUpperCase, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CHARAT)->getAtom(), callCharAt, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CHARCODEAT)->getAtom(), callCharCodeAt, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_INDEXOF)->getAtom(), callIndexOf, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LASTINDEXOF)->getAtom(), callLastIndexOf, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TRIM)->getAtom(), callTrim, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TRIMLEFT)->getAtom(), callTrimLeft, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TRIMRIGHT)->getAtom(), callTrimRight, 0);
	// ※※※ 未実装 ※※※ 正規表現 objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_MATCH)->getAtom(), callMatch, 0);
	// ※※※ 未実装 ※※※ 正規表現 objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_SEARCH)->getAtom(), callSearch, 0);
	// ※※※ 未実装 ※※※ 正規表現 objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_REPLACE)->getAtom(), callReplace, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_SPLIT)->getAtom(), callSplit, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CONCAT)->getAtom(), callConcat, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_SLICE)->getAtom(), callSlice, 0);
	///objPtype->defineClassFunction();
	// 静的プロパティを定義する
	///objCtor->defineClassProperty();
	// 静的関数を定義する ⇒ string_static_methods
	objCtor->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_FROMCHARCODE)->getAtom(), callFromCharCode, 0);

	// オブジェクト初期化
	EsValue valEmptyString(TypeUtils::getEmptyString());
	objPtype->slots()->setPrivateValue(&valEmptyString);	// ⇒ proto->fslots[JSSLOT_PRIVATE] = STRING_TO_JSVAL(cx->runtime->emptyString);
	EsValue valLengthAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
	EsValue valVoid;
	if(!objPtype->definePropertyNative(ctx, &valLengthAtom, &valVoid, 0L, 0L, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED, 0, 0, false))
	{
		return 0L;
	}
	return objPtype;
}

/*---------------------------------------------------------------------*//**
	文字列オブジェクトクラスのコンストラクタ ⇒ js_StringClass
**//*---------------------------------------------------------------------*/
EsString::EsStringClass::EsStringClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_STRING)->getAtom()
		, EsSysClasses::CLASSTYPE_STRING
		, EsClass::F_RESOLVE_OUT_PARAM | EsClass::F_HAS_PRIVATE
		, EsSysPrototypes::PTYPEKIND_STRING
		, 0 )
{
	_funcEnumerate = callEnumerate;
	_funcResolv = callResolve;
	_funcConstruct = callConstructor;
}

/*---------------------------------------------------------------------*//**
	列挙 ⇒ str_enumerate
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callEnumerate(EsContext* ctx, EsObject* obj)
{
	EsValue val(*obj->getSlots()->getPrivateValue());
	ASSERT(val.isString());
	const VcString* cstr = val.getString();

	for(int idx = 0; idx < cstr->getLength(); idx++)
	{
		VcString strSub(cstr[idx]);
		const VcString* cstrSub = ctx->newString(&strSub);
		if(cstrSub == 0L)
		{
			return false;
		}
		EsValue valIndex(idx);
		EsValue valSubConstStr(cstrSub);
		if(!obj->definePropertyOv(
			0L,
			ctx,
			&valIndex,
			&valSubConstStr,
			0L,
			0L,
			EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_READONLY	))
		{
			return false;
		}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	解決 ⇒ str_resolve
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callResolve(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv)
{
	if(!id->isInt() || TFW_IS_FLAG(flagsRslv, EsResolver::RSLVF_ASSIGNING))
	{
		return true;
	}

	EsValue val(*obj->getSlots()->getPrivateValue());
	ASSERT(val.isString());
	const VcString* cstr = val.getString();

	s32 idx = id->getInt();
	if((0 <= idx) && (idx < cstr->getLength()))
	{
		VcString strSub(cstr[idx]);
		const VcString* cstrSub = ctx->newString(&strSub);
		if(cstrSub == 0L)
		{
			return false;
		}
		EsValue valIndex(idx);
		EsValue valSubConstStr(cstrSub);
		if(!obj->definePropertyOv(
			0L,
			ctx,
			&valIndex,
			&valSubConstStr,
			0L,
			0L,
			EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_READONLY	))
		{
			return false;
		}
		*objOut = obj;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	文字列スクリプトクラスのコンストラクタ ⇒ js_String
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	// 値取得
	const VcString* cstr;
	if(numArg > 0)
	{
		if(!vaArg[0].toString(&cstr, ctx))
		{
			return false;
		}
		vaArg[0].setValue(cstr);
	}
	else
	{
		cstr = TypeUtils::getEmptyString();
	}

	// コンストラクタの場合は返答値に値をセットして返る
	if(!TFW_IS_FLAG(ctx->getTopStackFrame()->getFlags(), EsStackFrame::F_CONSTRUCTING))	// ⇒ JS_IsConstructing(cx)
	{
		valRet->setValue(cstr);
		return true;
	}

	// 値保存
	EsValue valConstStr(cstr);
	objThis->slots()->setPrivateValue(&valConstStr);	// ⇒ obj->fslots[JSSLOT_PRIVATE] = STRING_TO_JSVAL(str);
	return true;
}

/*---------------------------------------------------------------------*//**
	文字列化 ⇒ str_toString
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	return ctx->getRunEnv()->getPrimitiveThisObject(valRet, getInstance(), valCallee, valThis, vaArg, numArg);
}

/*---------------------------------------------------------------------*//**
	部分文字抽出 ⇒ str_substring
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callSubstring(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(numArg != 0L)
	{
		// 開始インデックス
		s32 idxBegin;
		f64 fval;
		if(!vaArg[0].toNumber(&fval, 0L))
		{
			return false;
		}
		idxBegin = EsValue::convF64ToS32(fval);
		// 終了インデックス
		s32 idxEnd;
		if(numArg == 1)
		{
			idxEnd = cstr->getLength();
		}
		else
		{
			if(!vaArg[1].toNumber(&fval, 0L))
			{
				return false;
			}
			idxEnd = EsValue::convF64ToS32(fval);
			// 負の値の場合は 0 とする
			if(idxEnd < 0)
			{
				idxEnd = 0;
			}
		}

		// 文字列処理
		VcString strWk = cstr->substring(idxBegin, idxEnd);
		cstr = ctx->newString(&strWk);
	}

	// 固定文字列を返答値に設定
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	小文字化 ⇒ str_toLowerCase
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callToLowerCase(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr == 0L)	{	return false;	}

	// 文字列処理
	VcString strWk = cstr->toLower();
	cstr = ctx->newString(&strWk);

	// 固定文字列を返答値に設定
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	大文字化 ⇒ str_toUpperCase
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callToUpperCase(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr == 0L)	{	return false;	}

	// 文字列処理
	VcString strWk = cstr->toUpper();
	cstr = ctx->newString(&strWk);

	// 固定文字列を返答値に設定
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	文字抽出 ⇒ str_charAt
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callCharAt(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr == 0L)	{	return false;	}

	// 文字インデックスを得る
	s32 index = 0;
	if(numArg > 0)
	{
		if(vaArg[0].isInt())
		{
			index = vaArg[0].getInt();
		}
		else
		{
			f64 fval;
			vaArg[0].toNumber(&fval, &vaArg[0], ctx);
			if(vaArg[0].isNull())	{	return false;	}
			index = (s32)fval;
		}

		if((index < 0) || (cstr->getLength() <= index))
		{
			valRet->setValue(TypeUtils::getEmptyString());
			return true;
		}
	}

	// 文字列処理
	VcString strWk(cstr->getAt(index));
	cstr = ctx->newString(&strWk);

	// 固定文字列を返答値に設定
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	文字コード抽出 ⇒ str_charCodeAt
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callCharCodeAt(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr == 0L)	{	return false;	}

	// 文字インデックスを得る
	s32 index = 0;
	if(numArg > 0)
	{
		if(vaArg[0].isInt())
		{
			index = vaArg[0].getInt();
		}
		else
		{
			f64 fval;
			vaArg[0].toNumber(&fval, &vaArg[0], ctx);
			if(vaArg[0].isNull())	{	return false;	}
			index = EsValue::convF64ToS32(fval);
		}

		if((index < 0) || (cstr->getLength() <= index))
		{
			valRet->setValue(TypeUtils::getEmptyString());
			return true;
		}
	}

	// 文字列処理
	char c = cstr->getAt(index);

	// 文字コードを返答値に設定
	valRet->setValue((s32)c);
	return true;
}

/*---------------------------------------------------------------------*//**
	指定文字が出現する場所を得る ⇒ str_indexOf
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callIndexOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr1 = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr1 == 0L)	{	return false;	}
	const VcString* cstr2 = convArgString(ctx, vaArg, numArg, 0);
	if(cstr2 == 0L)	{	return false;	}

	// 開始文字インデックスを得る
	s32 indexBegin = 0;
	if(numArg > 1)
	{
		if(vaArg[1].isInt())
		{
			indexBegin = vaArg[1].getInt();
		}
		else
		{
			f64 fval;
			vaArg[1].toNumber(&fval, &vaArg[1], ctx);
			if(vaArg[1].isNull())	{	return false;	}
			indexBegin = EsValue::convF64ToS32(fval);
		}
	}

	// 文字列処理
	s32 index = cstr1->find(cstr2, indexBegin);

	// 文字インデックスを返答値に設定
	valRet->setValue(indexBegin);
	return true;
}

/*---------------------------------------------------------------------*//**
	指定文字が最後に出現する場所を得る ⇒ str_lastIndexOf
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callLastIndexOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr1 = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr1 == 0L)	{	return false;	}
	const VcString* cstr2 = convArgString(ctx, vaArg, numArg, 0);
	if(cstr2 == 0L)	{	return false;	}

	// 開始文字インデックスを得る
	s32 indexBegin = 0;
	if(numArg > 1)
	{
		if(vaArg[1].isInt())
		{
			indexBegin = vaArg[1].getInt();
		}
		else
		{
			f64 fval;
			vaArg[1].toNumber(&fval, &vaArg[1], ctx);
			if(vaArg[1].isNull())	{	return false;	}
			indexBegin = EsValue::convF64ToS32(fval);
		}
	}

	// 文字列処理
	s32 index = cstr1->findReverse(cstr2);
	if(index < indexBegin)
	{
		index = -1;
	}

	// 文字インデックスを返答値に設定
	valRet->setValue(indexBegin);
	return true;
}

/*---------------------------------------------------------------------*//**
	空白削除 ⇒ str_trim
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callTrim(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr == 0L)	{	return false;	}

	// 文字列処理
	VcString strWk(cstr);
	StringUtils::trim(&strWk);
	cstr = ctx->newString(&strWk);

	// 固定文字列を返答値に設定
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	左空白削除 ⇒ str_trimLeft
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callTrimLeft(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr == 0L)	{	return false;	}

	// 文字列処理
	VcString strWk(cstr);
	StringUtils::trimLeft(&strWk);
	cstr = ctx->newString(&strWk);

	// 固定文字列を返答値に設定
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	右空白削除 ⇒ str_trimRight
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callTrimRight(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr == 0L)	{	return false;	}

	// 文字列処理
	VcString strWk(cstr);
	StringUtils::trimRight(&strWk);
	cstr = ctx->newString(&strWk);

	// 固定文字列を返答値に設定
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	パターンマッチ ⇒ str_match
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callMatch(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	// ※※※ 未実装 ※※※ 正規表現
	ASSERT(false);
	return false;
}

/*---------------------------------------------------------------------*//**
	パターン検索 ⇒ str_search
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callSearch(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	// ※※※ 未実装 ※※※ 正規表現
	ASSERT(false);
	return false;
}

/*---------------------------------------------------------------------*//**
	パターン置換 ⇒ str_replace
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callReplace(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	// ※※※ 未実装 ※※※ 正規表現
	ASSERT(false);
	return false;
}

/*---------------------------------------------------------------------*//**
	分割 ⇒ str_split
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callSplit(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr == 0L)	{	return false;	}

	// 結果を格納する配列を作成する
	EsArray* arr = ctx->newArray(0L, 0);
	if(arr == 0L)	{	return false;	}

	if(numArg == 0)
	{
		// 引数無しの場合は文字列をそのまま設定して返る
		EsValue valZero((s32)0);
		EsValue valStr(cstr);
		if(!arr->callSetPropertyOv(ctx, &valZero, &valStr))
		{
			return false;
		}
	}
	else
	{
		// ※※※ 未実装 ※※※ 正規表現 ⇒ VALUE_IS_REGEXP(cx, vp[2]

		// スプリッタ文字を得る
		const VcString* cstrSplitter = 0;
		if(!vaArg[0].toString(&cstrSplitter, ctx))
		{
			return false;
		}
		vaArg[0].setValue(cstrSplitter);

		// 限界数を得る
		s32 limit = 0;
		if((numArg >= 2) && !vaArg[1].isVoid())
		{
			f64 fvalWk;
			if(!vaArg[1].toNumber(&fvalWk, &vaArg[1], ctx) || vaArg[1].isNull())
			{
				return false;
			}
			limit = EsValue::convF64ToS32(fvalWk);
			if(limit > cstr->getLength())
			{
				limit = cstr->getLength() + 1;
			}
		}

		// 文字列処理
		VcStringArray strarrWk = StringUtils::split(cstr, cstrSplitter);
		for(int i = 0; (i < strarrWk.getCount()) && (i < limit); i++)
		{
			const VcString* cstrElm = ctx->newString(strarrWk.get(i));
			EsValue valElmStr(cstrElm);
			if(!arr->setElement(ctx, i, &valElmStr))
			{
				return false;
			}
		}
	}

	// 配列を返答値に設定
	valRet->setValue((EsObject*)arr);
	return true;
}

/*---------------------------------------------------------------------*//**
	分割 ⇒ str_concat
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callConcat(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(cstr == 0L)	{	return false;	}

	// 引数無しの場合は文字列をそのまま設定して返る
	if(numArg == 0)
	{
		valRet->setValue(cstr);
		return true;
	}

	// 文字列処理
	VcString strWk;
	for(u32 i = 0; i < numArg; i++)
	{
		const VcString* cstrAdd = 0;
		if(!vaArg[i].toString(&cstrAdd, ctx) || (cstrAdd == 0L))
		{
			return false;
		}
		strWk.add(cstrAdd);
	}
	cstr = ctx->newString(&strWk);

	// 固定文字列を返答値に設定
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	部分文字列 ⇒ str_slice
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callSlice(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	const VcString* cstr;

	if((numArg == 1) && valThis->isString() && vaArg[0].isInt())
	{
		cstr = valThis->getString();
		s32 idxBegin = vaArg[0].getInt();
		s32 idxEnd = cstr->getLength();
		if(idxBegin <= idxEnd)
		{
			s32 length = idxEnd - idxBegin;
			if(length == 0)
			{
				cstr = TypeUtils::getEmptyString();
			}
			else
			{
				VcString strSub(cstr->substring(idxBegin, idxEnd));
				cstr = ctx->newString(&strSub);	// ⇒ str = (length == 1) ? js_GetUnitString(cx, str, begin) : js_NewDependentString(cx, str, begin, length);
				if(cstr == 0L)
				{
					return false;
				}
			}
			valRet->setValue(cstr);
			return true;
		}
	}

	cstr = convThisString(ctx, valCallee, valThis, vaArg);
	if(numArg != 0)
	{
		// 開始インデックス
		s32 idxBegin;
		f64 fval;
		if(!vaArg[0].toNumber(&fval, 0L))
		{
			return false;
		}
		idxBegin = EsValue::convF64ToS32(fval);
		// 終了インデックス
		s32 idxEnd;
		if(numArg == 1)
		{
			idxEnd = cstr->getLength();
		}
		else
		{
			if(!vaArg[1].toNumber(&fval, 0L))
			{
				return false;
			}
			idxEnd = EsValue::convF64ToS32(fval);
			// 負の値の場合は後ろからのカウントになる
			if(idxEnd < 0)
			{
				idxEnd = cstr->getLength() + idxEnd;
			}
		}

		// 文字列処理
		VcString strWk = cstr->substring(idxBegin, idxEnd);
		cstr = ctx->newString(&strWk);
	}

	// 固定文字列を返答値に設定
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	文字列化 ⇒ str_fromCharCode
**//*---------------------------------------------------------------------*/
bool EsString::EsStringClass::callFromCharCode(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	// 引数から取得
	VcString strWk;
	for(u32 i = 0; i < numArg; i++)
	{
		u32 uiwk;
		if(!vaArg[i].toUint32(&uiwk, ctx))	// ⇒ js_ValueToUint16(cx, &argv[i]);
		{
			return false;
		}
		strWk.add((char)uiwk);
	}

	// 固定文字列を返答値に設定
	const VcString* cstr = ctx->newString(&strWk);
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	引数から文字列を得る ⇒ NormalizeThis, NORMALIZE_THIS
**//*---------------------------------------------------------------------*/
const VcString* EsString::EsStringClass::convThisString(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg)
{
	if(valThis->isString())
	{
		return valThis->getString();
	}

	if(valThis->isNull())
	{
		return 0L;
	}
	ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ JS_THIS(cx, vp)
	if(valThis->isNull())
	{
		return 0L;
	}

	const VcString* cstr = 0L;
	if(!valThis->toString(&cstr, ctx) || (cstr == 0L))
	{
		return 0L;
	}
	valThis->setValue(cstr);
	return cstr;
}

/*---------------------------------------------------------------------*//**
	引数から指定インデックスの文字列を得る ⇒ ArgToRootedString
**//*---------------------------------------------------------------------*/
const VcString* EsString::EsStringClass::convArgString(EsContext* ctx, EsValue* vaArg, u32 numArg, u32 index)
{
	if(numArg <= index)
	{
		return ctx->getKeywordSet()->getLogicalType(LTYPE_VOID)->getName();	// ⇒ return ATOM_TO_STRING(cx->runtime->atomState.typeAtoms[JSTYPE_VOID]);
	}

	const VcString* cstr;
	if(!vaArg[index].toString(&cstr, ctx))
	{
		return 0L;
	}
	return cstr;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
