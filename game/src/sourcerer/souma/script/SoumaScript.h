/***********************************************************************//**
 *	SoumaScript.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_H_
#define _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

#include "../../source/SourceDef.h"

#include "../../../es/EsExternalExtension.h"
#include "../../../es/EsExtParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class ExecCtx;

TFW_END_NS

namespace es
{
	class Es;
	class EsContext;
	class EsKeywordSet;
	class EsObject;
	class EsRunStat;
	class EsValue;
}
using namespace es;

SRCR_BEGIN_NS

class Game;
class Souma;
class SoumaScriptContext;
class SourceManager;

/*---------------------------------------------------------------------*//**
 *	ソウマ スクリプト
 *
**//*---------------------------------------------------------------------*/
class SoumaScript
{
	//======================================================================
	// 定数
public:
	const static s32 FUNCID_MYFUNC_BEGIN		= 1;
	const static s32 FUNCID_MAGICLIST_BEGIN		= 1001;

	enum GlobalFunc
	{
		// システム情報
		FUNC_SYSINFO = FUNCID_MYFUNC_BEGIN,
		// デバッグコンソール出力
		FUNC_CONSOLE_OUT,

		MAX_FUNC
	};

public:
	// ES 関数用の返り値
	enum SsReturnCode
	{
		RET_SYS_ERROR					= 0,		// システム的な失敗（スクリプト側での対応が不可能なバグや障害）

		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// 成功

		RET_OK							= 1,		// 成功
#if 0
		RET_ALREADY_ELENE_AMOUNT		= 2,		// 成功だが、既にその値の元素エネルギー量を持っている
		RET_SHORTAGE_ELENE_AMOUNT		= 3,		// 成功だが、その値の元素エネルギー量には達していない
#endif

		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// 失敗

		RET_GG__START_					= -1000,	// ＜開始＞ゲームシステム的なエラー
		RET_OTHER_PROBLEM				= -1000,	// その他の問題
		RET_NE_ENEG						= -1001,	// エネルギーが足りない
		RET_NE_CONCD					= -1002,	// 意識深さが足りない
		RET_NO_PREPARED					= -1003,	// 準備がされていない
		RET_NO_CLUSTER					= -1004,	// クラスターが存在しない
		RET_NO_TARGET					= -1005,	// 対象がみつからない
		RET_INVALID_HANDLE				= -1006,	// ハンドルが不正
		RET_ELENE_IMMATURE				= -1007,	// 元素過小（未成熟）
		RET_ELENE_OVERBREAK				= -1008,	// 元素過大（破裂）
		RET_NO_CLUSTER_SLOT				= -1009,	// 空きクラスタースロットが見つからない
		RET_ELENE_BALANCE_FAILURE		= -1010,	// 元素バランスが成立しなかった
		RET_GG__END_					= -1999,	// ＜終了＞ゲームシステム的なエラー

		RET_GL__START_					= -2000,	// ＜開始＞局所的なエラー
		RET_NE_ABILITY					= -2001,	// 作成能力が不足している
		RET_NE_ELENE_REST_AMOUNT		= -2002,	// 残り元素量が不足している
		RET_INVALID_HANDLE_STRING		= -2003,	// 不正なハンドル文字列
		RET_INVALID_FORM				= -2004,	// 形態が不正
		RET_GL__END_					= -2999,	// ＜終了＞局所的なエラー

		RET_SC__START_					= -10000,	// ＜開始＞スクリプトエラー
		RET_SC_SYS_ERROR				= -10001,	// スクリプトシステムエラー（スクリプトエンジンのバグ）
		RET_SC_SYNTAX_ERROR				= -10002,	// スクリプト構文エラー（スクリプトの記述上のミス）
		RET_SC_INVALID_ARG_NUM			= -10011,	// 引数の数が不正
		RET_SC_INVALID_ARG_TYPE			= -10012,	// 引数の型が不正
		RET_SC_INVALID_CALL				= -10013,	// 不正な呼び出し
		RET_SC__END_					= -19999,	// ＜終了＞スクリプトエラー
	};

private:
	/*-----------------------------------------------------------------*//**
	 *	ソウマ スクリプト 拡張
	 *
	**//*-----------------------------------------------------------------*/
	class SoumaExtExt : public EsExternalExtension
	{
	protected:
		virtual bool resolveUndefinedFunction(VcString* codeExtScriptOut, const VcString* funcnameUndefined, EsExtParam paramParse);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ES インスタンスを得る
	inline Es* getEs() const { return _es; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool parse(SoumaScriptContext* ssctx, const VcString* sourcecode, SourceDef::SourceKind srckind);
	bool run(SoumaScriptContext* ssctx);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SoumaScript();
	virtual ~SoumaScript();
	virtual bool create();
	virtual void destroy();
	void exec(const ExecCtx* ec);
	bool resolveUndefinedFunction(VcString* codeExtScriptOut, const VcString* funcnameUndefined);

#if defined(_DEBUG)
	void _debug_testScript();
#endif

private:
	static bool callbackFromEsStatic(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);

	//======================================================================
	// 変数
protected:
	Es* _es;
	EsExternalExtension* _esextext;
	SourceManager* _srcmngRef;

private:
	static const char* _funcnames[MAX_FUNC - 1];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_H_
