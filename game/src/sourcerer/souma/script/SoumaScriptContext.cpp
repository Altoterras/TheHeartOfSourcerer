/***********************************************************************//**
 *	SoumaScriptContext.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/11/16.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SoumaScriptContext.h"

// Friends
#include "SoumaScript.h"
#include "../Souma.h"
#include "../../es/Es.h"
#include "../../es/EsContext.h"
#include "../../es/EsError.h"
#include "../../es/EsRunStat.h"
#include "../../es/EsValue.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SoumaScriptContext メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	エラーメッセージを設定する
**//*---------------------------------------------------------------------*/
void SoumaScriptContext::errorMessage(EsValue* valRet, s32 retcode)
{
	writeReturnValue(valRet, retcode);
	_soumaRef->doError(retcode);
}

/*---------------------------------------------------------------------*//**
	致命的エラーを設定する
**//*---------------------------------------------------------------------*/
void SoumaScriptContext::errorFatal(EsValue* valRet, s32 retcode)
{
	writeReturnValue(valRet, retcode);
	_soumaRef->doFatalError(retcode);
}

/*---------------------------------------------------------------------*//**
	ソウマ実行ミスを設定する
**//*---------------------------------------------------------------------*/
void SoumaScriptContext::errorMistake(EsValue* valRet, s32 retcode)
{
	writeReturnValue(valRet, retcode);
	_soumaRef->doMistake(retcode);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SoumaScriptContext::SoumaScriptContext()
	: _soumaRef(0L)
	, _esctx(0L)
	, _esrunstat(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SoumaScriptContext::~SoumaScriptContext()
{
	ASSERT(_esctx == 0L);
	ASSERT(_esrunstat == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SoumaScriptContext::create(Souma* soumaRef, SoumaScript* ssRef)
{
	_soumaRef = soumaRef;
	_ssRef = ssRef;
	_esrunstat = new EsRunStat();

#if 1
	if(!EsContext::create())
	{
		ASSERT(false);
		return false;
	}
#else
	_esctx = new EsContext();
	if(!_esctx->create())
	{
		delete _esctx;
		_esctx = 0L;
		ASSERT(false);
		return false;
	}
#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SoumaScriptContext::destroy()
{
#if 1
	EsContext::destroy();
#else
	if(_esctx != 0L)
	{
		_esctx->destroy();
		delete _esctx;
		_esctx = 0L;
	}
#endif
	if(_esrunstat != 0L)
	{
		delete _esrunstat;
		_esrunstat = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	返り値を設定する
**//*---------------------------------------------------------------------*/
void SoumaScriptContext::writeReturnValue(EsValue* valRet, s32 retcode)
{
	s32 esecode = 0;
	if((SoumaScript::RET_GG__END_ <= retcode) && (retcode <= SoumaScript::RET_GG__START_))
	{
		esecode = EsError::APP_ERROR_ID__START_;
	}
	else if((SoumaScript::RET_GL__END_ <= retcode) && (retcode <= SoumaScript::RET_GL__START_))
	{
		esecode = EsError::APP_ERROR_ID__START_;
	}
	else if((SoumaScript::RET_SC__END_ <= retcode) && (retcode <= SoumaScript::RET_SC__START_))
	{
		esecode = EsError::RUN_ERROR_ID__START_;
	}
	valRet->setErrorCode(esecode, retcode);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
