/***********************************************************************//**
 *	Es.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_H_
#define _ES_ES_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsContext.h"
#include "EsExtParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

TFW_END_NS

ES_BEGIN_NS

class EsExternalExtension;
class EsInterlang;
class EsStackFrame;
class EsRunEnv;
class EsRunStat;

/*---------------------------------------------------------------------*//**
 *	Enlight ECMA スクリプト エンジン
 *		※　基本的に将来的に Java に移植することを考えて、
 *			struct, union, ビットフィールド などは積極的に使わない。
 *			また、ゲームでの利用上、速度重視ではないので、
 *			効率的には最善な手法はとらない。
 *
**//*---------------------------------------------------------------------*/
class Es
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	Es();
	~Es();
	bool prepareParse(EsContext* ctx, const EsKeywordSet* kwsetRef, u32 mflags, EsExternalExtension* extextRef, EsExtParam paramParse);
	bool parse(EsContext* ctx, const VcString* strScript);
	bool parse(EsContext* ctx, const VcString* strScript, EsStackFrame* sfrmCaller, u32 bflags);
	bool prepareRun(EsContext* ctx, EsExtParam paramRun);
	bool prepareRun(EsContext* ctx, EsStackFrame* sfrmCaller, u16 flagsSfrm, EsValue* valRet, EsExtParam paramRun);
	bool runStep(EsContext* ctx, EsRunStat* stat);

	//======================================================================
	// 変数
private:
	EsInterlang* _ilParseResultTmp;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_H_
