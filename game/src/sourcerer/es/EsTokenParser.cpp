/***********************************************************************//**
 *	EsTokenParser.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsTokenParser.h"

// Friends
#include "EsContext.h"
#include "EsAtom.h"
#include "EsTokenGetter.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsTokenParser メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	拡張スクリプトにより解析を再開する
**//*---------------------------------------------------------------------*/
void EsTokenParser::restartExtendScript()
{
	_tb->clear();
	_tg->reset();
}

/*---------------------------------------------------------------------*//**
	次のトークンを得る（ポインタは次へ進める）⇒ js_GetToken
**//*---------------------------------------------------------------------*/
EsTokenType	EsTokenParser::nextToken()
{
	// 解析済み保存トークンがある場合はそこから取り出す
	while(_tb->isSave())
	{
		EsToken* tkn = _tb->popSave();
		if((tkn->getType() != TKN_EOF) || TFW_IS_FLAG(getParseFlag(), PF_NEW_LINES))
		{
			return tkn->getType();
		}
	}

	// トークンを得る
	EsToken tknWk;
	EsTokenGetter::TokenHint th = _tg->nextToken(&tknWk);
	switch(th)
	{
	case EsTokenGetter::TH_NAME:
	case EsTokenGetter::TH_STRING:
	case EsTokenGetter::TH_WORD:
		{
			// アトムをセット
			const VcString* str = _ctxRef->newString(_tg->getTokenString());
			const EsAtom* atom = _ctxRef->makeStringAtom(str);
			tknWk.setAtomValue(atom);
		}
		break;
	}

	// トークン生成
	EsToken* tknNew = _tb->next();
	tknNew->copy(&tknWk);
	
	return tknNew->getType();
}

/*---------------------------------------------------------------------*//**
	次のトークンを得る（ポインタは次へ進めないでそのまま）
		次のトークンをのぞき見るときに使う
**//*---------------------------------------------------------------------*/
EsTokenType	EsTokenParser::peekToken()
{
	EsTokenType tt = TKN_NULL;

	if(_tb->isSave())
	{
		tt = _tb->curSave()->getType();
	}
	else
	{
		tt = nextToken();
		ungetToken();
	}

	return tt;
}

/*---------------------------------------------------------------------*//**
	同行の次のトークンを得る（ポインタは次へ進めないでそのまま）
		次のトークンをのぞき見るときに使う
**//*---------------------------------------------------------------------*/
EsTokenType EsTokenParser::peekTokenSameLine()
{
	EsTokenType tt = TKN_NULL;

	// 最後に解析したトークンが現在の行と同じで無かった場合は TKN_EOL を返す
	if(_tg->getCurScriptLineNo() != _tb->cur()->getPos()->getEndLineNo())
	{
		return TKN_EOL;
	}

	// 同行、もしくは TKN_EOL を返す
	setParseFlag(PF_NEW_LINES, true);
	tt = peekToken();
	setParseFlag(PF_NEW_LINES, false);
	return tt;
}

/*---------------------------------------------------------------------*//**
	トークン取得時のポインタを巻き戻す
**//*---------------------------------------------------------------------*/
void EsTokenParser::ungetToken()
{
	_tb->pushSave();
}

/*---------------------------------------------------------------------*//**
	トークンが一致するかどうか判定する
	（一致した場合はポインタを進め、一致しなかった場合は戻す）
	⇒ js_MatchToken
**//*---------------------------------------------------------------------*/
bool EsTokenParser::matchToken(EsTokenType tt)
{
	if(tt == nextToken())	{	return true;	}
	ungetToken();
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsTokenParser::EsTokenParser(EsContext* ctxRef, const VcString* codeRef, bool isStringSignMode)
	: _ctxRef(ctxRef)
	, _tg(new EsTokenGetter(ctxRef->getKeywordSet(), ctxRef->getError(), codeRef, isStringSignMode, true))
	, _tb(new EsTokenBuf())
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsTokenParser::~EsTokenParser()
{
	delete _tb;
	delete _tg;
}

//==========================================================================
// EsTokenParser::EsTokenBuf メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsTokenParser::EsTokenBuf::EsTokenBuf()
	: _idx(0)
	, _save(0)
{
}

/*---------------------------------------------------------------------*//**
	値のクリア
**//*---------------------------------------------------------------------*/
void EsTokenParser::EsTokenBuf::clear()
{
	_idx = 0;
	_save = 0;
}

/*---------------------------------------------------------------------*//**
	次のトークンを得る
**//*---------------------------------------------------------------------*/
EsToken* EsTokenParser::EsTokenBuf::next()
{
	_idx = (_idx + 1) & MASK_BUF;
	return cur();	// 進めた先のトークンを返す
}

/*---------------------------------------------------------------------*//**
	解析済み保存トークンの追加
**//*---------------------------------------------------------------------*/
EsToken* EsTokenParser::EsTokenBuf::pushSave()
{
	ASSERT(_save < MASK_BUF);
	_save++;
	_idx = (_idx - 1) & MASK_BUF;
	return cur();	// 戻した先のトークンを返す
}

/*---------------------------------------------------------------------*//**
	解析済み保存トークンの取り出し
**//*---------------------------------------------------------------------*/
EsToken* EsTokenParser::EsTokenBuf::popSave()
{
	ASSERT(_save > 0);
	_save--;
	_idx = (_idx + 1) & MASK_BUF;
	return cur();	// 進めた先のトークンを返す
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
