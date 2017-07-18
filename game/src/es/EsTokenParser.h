/***********************************************************************//**
 *	EsTokenParser.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_TOKEN_PARSER_H_
#define _ES_ES_TOKEN_PARSER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsError.h"
#include "EsToken.h"
#include "EsTokenGetter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

ES_BEGIN_NS

class EsContext;

/*---------------------------------------------------------------------*//**
 *	トークン解析
 *
**//*---------------------------------------------------------------------*/
class EsTokenParser
{
	//======================================================================
	// 定数
public:
	// 解析フラグ
	static const u32 PF_ERROR			= 0x00000001;	// エラー ⇒ TSF_ERROR
	static const u32 PF_NEW_LINES		= 0x00000010;	// 新しい行である ⇒ TSF_NEWLINES
	static const u32 PF_DIRTY_LINE		= 0x00000020;	// 行の途中である ⇒ TSF_DIRTYLINE
	static const u32 PF_KEYWD_IS_NAME	= 0x00000100;	// キーワードを NAME として解析 ⇒ TSF_KEYWORD_IS_NAME
	// 不要かも static const u32 PF_OPERAND			= 0x00000200;	// オペランド（演算の対象となる数値）を探し、オペレーター（演算子）は探さない ⇒ TSF_OPERAND

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	トークンソース
	 *
	**//*-----------------------------------------------------------------*/
	class EsTokenBuf
	{
		//==================================================================
		// 定数
	private:
		static const int	NUM_BUF		= 4;
		static const int	MASK_BUF	= 3;

		//==================================================================
		// メソッド
	public:
		EsTokenBuf();
		void clear();

		inline EsToken*	cur()				{	return &_tBuf[ _idx ];							}
		inline EsToken*	next();
		inline EsToken*	curSave()			{	return &_tBuf[ (_idx + _save) & MASK_BUF ];		}
		EsToken*		pushSave();
		EsToken*		popSave();
		inline bool		isSave() const		{	return _save != 0;								}

		//==================================================================
		// 変数
	private:
		EsToken		_tBuf[NUM_BUF];
		s32			_idx;
		s32			_save;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在のトークンを得る
	inline const EsToken*	getCurToken() const		{	return _tb->cur();				}
	inline EsToken*			curToken() const		{	return _tb->cur();				}

	// 現在のトークン位置を得る
	inline const EsTokenPos	getCurScriptPos() const	{	return _tg->getCurScriptPos();	}

	// 解析フラグを得る
	inline u32				getParseFlag() const	{	return _tg->getParseFlag();		}

	// ファイル終端かを得る
	inline bool				isEof()					{	return _tg->isEof();			}

	// 解析フラグを設定する
	void					setParseFlag(u32 parseflag, bool enable)	{	_tg->setParseFlag(parseflag, enable);	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void					restartExtendScript();
	EsTokenType				nextToken();
	EsTokenType				peekToken();
	EsTokenType				peekTokenSameLine();
	void					ungetToken();
	bool					matchToken(EsTokenType tt);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsTokenParser(EsContext* ctxRef, const VcString* codeRef, bool isStringSignMode);
	~EsTokenParser();

	//======================================================================
	// 変数
private:
	EsContext*				_ctxRef;
	EsTokenGetter*			_tg;
	EsTokenBuf*				_tb;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_TOKEN_PARSER_H_
