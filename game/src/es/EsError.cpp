/***********************************************************************//**
 *	EsError.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsError.h"

// Friends

// External
#include "EsContext.h"
#include "EsBcExInfo.h"
#include "EsRunEnv.h"
#include "../tfw/collection/List.h"
#include "../tfw/string/VcStringArray.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsError メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsError::EsError()
	: _replistCompile(new List<Report*>(true))
	, _replistRun(new List<Report*>(true))
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsError::~EsError()
{
	delete _replistCompile;
	delete _replistRun;
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void EsError::clear()
{
	_replistCompile->removeAll();
	_replistRun->removeAll();
}

/*---------------------------------------------------------------------*//**
	コンパイルエラーの追加
**//*---------------------------------------------------------------------*/
void EsError::addCompileError(Id id, EsTokenPos pos, const VcStringArray* saParam)
{
	Report* report = new Report(id, pos, saParam, 0);
	_replistCompile->addTail(report);
}

/*---------------------------------------------------------------------*//**
	実行エラーの追加
**//*---------------------------------------------------------------------*/
void EsError::addRunError(EsContext* ctx, Id id, bool isWarning, bool isStrict)
{
	EsTokenPos pos;
	EsRunEnv* re = ctx->getRunEnv();
	if(re != 0L)
	{
		const EsBcExInfo* exi = re->getCurBcExInfo();
		if(exi != 0L)
		{
			pos.calcBackwardByIndex(ctx, exi->getBeginIndex());
		}
	}

	Report* report = new Report(id, pos, 0L, (isWarning ? Report::F_WARNING : 0) | (isStrict ? Report::F_STRICT : 0));
	_replistRun->addTail(report);
}

//==========================================================================
// EsError::Report メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsError::Report::Report(Id id, EsTokenPos pos, const VcStringArray* saParam, u8 flags)
	: _id(id)
	, _pos(pos)
	, _saParam(0L)
	, _flags(flags)
{
	_saParam = (saParam != 0L) ? new VcStringArray(*saParam) : new VcStringArray();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsError::Report::~Report()
{
	delete _saParam;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
