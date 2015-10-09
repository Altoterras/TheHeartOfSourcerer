/***********************************************************************//**
 *	SoumaScript.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SoumaScript.h"

// Friends
#include "SoumaScriptContext.h"
#include "SoumaScriptKeywords.h"
#include "SsBase.h"
#include "SsCluster.h"
#include "SsConsole.h"
#include "SsSouma.h"
#include "SsUnit.h"
#include "../Souma.h"
#include "../../debug/GameTest.h"
#include "../../body/Game.h"
#include "../../episode/char/pc/MySpirit.h"
#include "../../episode/party/MyParty.h"
#include "../../es/Es.h"
#include "../../es/EsContext.h"
#include "../../es/EsExternalExtension.h"
#include "../../es/EsRunStat.h"
#include "../../es/EsValue.h"
#include "../../es/kwset/JsKeywordSet.h"
#include "../../es/kwset/SsKeywordSet.h"
#include "../../magic/MagicCluster.h"
#include "../../magic/MagicSys.h"
#include "../../source/Source.h"
#include "../../source/SourceManager.h"
#include "../../spirit/Spirit.h"
#include "../../status/ActStat.h"
#include "../../status/CharStat.h"
#include "../../status/Elene.h"
#include "../../status/TransStat.h"
#include "../../unit/Unit.h"

// External
#include "../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define TEST_ES					(1) //&& (0)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SoumaScript 定数

const char* SoumaScript::_funcnames[SoumaScript::MAX_FUNC - 1] =
{
	// システム情報
	"sysinfo",			// FUNC_SYSINFO
	// デバッグコンソール出力
	"cout",				// FUNC_CONSOLE_OUT
};

//==========================================================================
// SoumaScript メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	スクリプトを解析する
**//*---------------------------------------------------------------------*/
bool SoumaScript::parse(SoumaScriptContext* ssctx, const VcString* sourcecode, SourceDef::SourceKind srckind)
{
	ASSERT((_es != 0L) && (_esextext != 0L));

	// ソース種別決定
	
	EsKeywordSet* kwset = SourceDef::getKeywordSet(srckind);
	if(kwset == 0L)		{	ASSERT(kwset != 0L);	return false;	}

	// スクリプトを解析・実行準備をする
	EsContext* ctx = ssctx->getEsContext();
	if(!_es->prepareParse(ctx, kwset, ((srckind == SourceDef::SRCKIND_SS) ? es::ES_MF_PARSE_STRING_SIGN : 0) | es::ES_MF_ADD_DEBUG_INFO, _esextext, ssctx))
	{
		return false;
	}
	if(!_es->parse(ctx, sourcecode))
	{
		return false;
	}
	if(!_es->prepareRun(ctx, ssctx))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	スクリプトをステップ実行する
**//*---------------------------------------------------------------------*/
bool SoumaScript::run(SoumaScriptContext* ssctx)
{
	return _es->runStep(ssctx->getEsContext(), ssctx->getEsRunStat());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SoumaScript::SoumaScript()
	: _es(0L)
	, _esextext(0L)
	, _srcmngRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SoumaScript::~SoumaScript()
{
	ASSERT(_es == 0L);
	ASSERT(_esextext == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SoumaScript::create()
{
	// 各種参照を取得
	_srcmngRef = Game::getGame()->getMyParty()->getSourceManager();	ASSERT(_srcmngRef != 0L);

	// Es プラグインを作成
	_esextext = new SoumaExtExt();
	#if defined(_DEBUG) && 0
		_esextext->addExtNativeFunc(new EsExternalExtension::EsExtNativeFunc(callbackFromEsStatic, "aiueo", EsExtParam(FUNC_DUMMY)));
	#endif
	for(s32 i = FUNCID_MYFUNC_BEGIN; i < SoumaScript::MAX_FUNC; i++)
	{
		_esextext->addExtNativeFunc(new EsExternalExtension::EsExtNativeFunc(callbackFromEsStatic, _funcnames[i - FUNCID_MYFUNC_BEGIN], EsExtParam(i)));
	}

	// Es の作成
	_es = new Es();

	// キーワードの初期化
	if(!SoumaScriptKeywords::init())	{	return false;	}

	// カスタムスクリプトクラスの初期化
	if(!SsBase::init())					{	return false;	}
	if(!SsCluster::init())				{	return false;	}
	if(!SsConsole::init())				{	return false;	}
	if(!SsSouma::init())				{	return false;	}
	if(!SsUnit::init())					{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SoumaScript::destroy()
{
	// Es の削除
	if(_es != 0L)
	{
		delete _es;
		_es = 0L;
	}
	if(_esextext != 0L)
	{
		delete _esextext;
		_esextext = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SoumaScript::exec(const ExecCtx* ec)
{
}

/*---------------------------------------------------------------------*//**
	未定義の関数の解決を試みる

	@param codeExtScriptOut	追加スクリプトコード（戻り値 false の場合有効）
	@retval true	解析を終了してよし
					※	外部拡張ネイティブ関数新規追加によって解決した場合も
						この戻り値でよい
	@retval false	解析を続ける
					※	codeExtScriptOut に該当関数のスクリプトコードを
						設定して、解決を試みる
**//*---------------------------------------------------------------------*/
bool SoumaScript::resolveUndefinedFunction(VcString* codeExtScriptOut, const VcString* funcnameUndefined)
{
	for(int i = 0; i < _srcmngRef->getSourceNum(); i++)
	{
		const Source* source = _srcmngRef->getSource(i);
		if(!source->isValid())	{	continue; }

		if(funcnameUndefined->equals(source->getName()))
		{
			VcString code;
			code.add(source->getCode());
			codeExtScriptOut->set(&code);
			return false;
		}
	}

	return true;
}

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	スクリプト テスト
**//*---------------------------------------------------------------------*/
void SoumaScript::_debug_testScript()
{
	SoumaScriptContext ssctx;
	ssctx.create(((MySpirit*)Game::getGame()->getMyParty()->getPcSpirit(0))->getSouma(), this);
	GameTest::testSoumaScript(&ssctx);
	ssctx.destroy();
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// コールバックと Es 関数

/*---------------------------------------------------------------------*//**
	Es コールバック（静的）
**//*---------------------------------------------------------------------*/
bool SoumaScript::callbackFromEsStatic(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	s32 funcid = exprm->_paramDef.getIntValue();
	///SoumaScriptContext* ssctx = (SoumaScriptContext*)exprm->_paramRun.getObject();
	///SoumaScript* ss = ssctx->getSoumaScript();

	switch(funcid)
	{
	case FUNC_SYSINFO:
		// 未実装
		break;
	case FUNC_CONSOLE_OUT:
		SsConsole::callOut(ctx, objThis, valCallee, valThis, vaArg, numArg, valRet, exprm);
		break;
	}

	return true;
}

//==========================================================================
// SoumaScript::SoumaExtExt メソッド

/*---------------------------------------------------------------------*//**
	未定義の関数の解決を試みる

	@param codeExtScriptOut	追加スクリプトコード（戻り値 false の場合有効）
	@param paramParse		解析汎用パラメータ
	@retval true	解析を終了してよし
					※	外部拡張ネイティブ関数新規追加によって解決した場合も
						この戻り値でよい
	@retval false	解析を続ける
					※	codeExtScriptOut に該当関数のスクリプトコードを
						設定して、解決を試みる
**//*---------------------------------------------------------------------*/
bool SoumaScript::SoumaExtExt::resolveUndefinedFunction(VcString* codeExtScriptOut, const VcString* funcnameUndefined, EsExtParam paramParse)
{
	SoumaScriptContext* ssctx = (SoumaScriptContext*)paramParse.getObject();
	return ssctx->getSoumaScript()->resolveUndefinedFunction(codeExtScriptOut, funcnameUndefined);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
