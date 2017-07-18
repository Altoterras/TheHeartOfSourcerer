/***********************************************************************//**
 *	SoumaScriptContext.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/11/16.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_CONTEXT_H_
#define _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_CONTEXT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

#include "../../../es/EsContext.h"
#include "../../../es/EsExternalExtension.h"
#include "../../../es/EsExtParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class ExecCtx;

TFW_END_NS

namespace es
{
//	class EsContext;
	class EsRunStat;
	class EsValue;
}
using namespace es;

SRCR_BEGIN_NS

class Souma;
class SoumaScript;

/*---------------------------------------------------------------------*//**
 *	ソウマ スクリプト コンテキスト
 *
**//*---------------------------------------------------------------------*/
class SoumaScriptContext : public EsContext
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// コンテキストを得る
	inline EsContext* getEsContext() { return this; }	// EsContext を継承せずに委譲されていたときの名残り
	// 実行状態を得る
	inline EsRunStat* getEsRunStat() const { return _esrunstat; }
	// ソウマを得る
	inline Souma* getSouma() const { return _soumaRef; }
	// スクリプト環境を得る
	inline SoumaScript* getSoumaScript() const { return _ssRef; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void errorMessage(EsValue* valRet, s32 retcode);
	void errorFatal(EsValue* valRet, s32 retcode);
	void errorMistake(EsValue* valRet, s32 retcode);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SoumaScriptContext();
	~SoumaScriptContext();
	bool create(Souma* soumaRef, SoumaScript* ssRef);
	void destroy();

private:
	void writeReturnValue(EsValue* valRet, s32 retcode);

	//======================================================================
	// 変数
private:
	EsContext* _esctx;
	EsRunStat* _esrunstat;
	Souma* _soumaRef;
	SoumaScript* _ssRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_CONTEXT_H_
