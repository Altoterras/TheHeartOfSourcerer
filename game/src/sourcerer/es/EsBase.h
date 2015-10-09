/***********************************************************************//**
 *	EsBase.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_BASE_H_
#define _ES_ES_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../tfw/Base.h"
#include "../../tfw/string/VcString.h"
#include "../debug/Debug.h"

#include "EsExtParam.h"
#include "EsDef.h"

////////////////////////////////////////////////////////////////////////////
// ネームスペース

namespace tfw {}
using namespace tfw;

////////////////////////////////////////////////////////////////////////////
// マクロ

#define ES_BEGIN_NS namespace es {
#define ES_END_NS }

#define ES_LOCK_OBJ(ctx_, obj_)			//
#define ES_UNLOCK_OBJ(ctx_, obj_)		//

////////////////////////////////////////////////////////////////////////////
// 定数定義

ES_BEGIN_NS

// 動作モードフラグ
static const u32 ES_MF_ADD_DEBUG_INFO		= 0x00000001;	// デバッグ情報を追加モード
static const u32 ES_MF_PARSE_STRING_SIGN	= 0x00000002;	// 文字列記号解析モード

ES_END_NS

////////////////////////////////////////////////////////////////////////////
// 型定義

ES_BEGIN_NS

class EsContext;
class EsObject;
class EsScopeProperty;
class EsValue;

class EsCallExtParam
{
public:
	EsExtParam _paramDef;
	EsExtParam _paramRun;
};

typedef bool (* EsNativeCall)(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);	// ⇒ JSNative
typedef bool (* EsPropertyGetterFunc)(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
typedef bool (* EsPropertySetterFunc)(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val);
typedef bool (* EsHasInstanceFunc)(EsValue* valRes, EsContext* ctx, EsObject* obj, const EsValue* valInst);
typedef bool (* EsCheckAccessFunc)(u8* spatrbfOut, EsValue* valOut, EsContext* ctx, EsObject* obj, const EsValue* spropid, u8 acsflags);
typedef bool (* EsEnumerateFunc)(EsContext* ctx, EsObject* obj);
typedef bool (* EsEnumerateOutFunc)(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop);
typedef bool (* EsResolveFunc)(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv);
typedef bool (* EsConvertFunc)(EsValue* val, EsContext* ctx, EsObject* obj, EsLogicalType ltype);

ES_END_NS

////////////////////////////////////////////////////////////////////////////
// グローバル関数

ES_BEGIN_NS

bool g_funcDummyPropertyGetter(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
bool g_funcDummyPropertySetter(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val);
bool g_funcDummyEnumerate(EsContext* ctx, EsObject* obj);
bool g_funcDummyEnumerateOut(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop);
bool g_funcDummyResolve(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv);


ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_BASE_H_
