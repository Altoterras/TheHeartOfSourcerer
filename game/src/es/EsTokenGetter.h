/***********************************************************************//**
 *	EsTokenGetter.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2012/01/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_TOKEN_GETTER_H_
#define _ES_ES_TOKEN_GETTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsToken.h"
#include "EsError.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

ES_BEGIN_NS

class EsError;
class EsKeyword;
class EsKeywordSet;

/*---------------------------------------------------------------------*//**
 *	トークン解析
 *
**//*---------------------------------------------------------------------*/
class EsTokenGetter
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

	enum TokenHint
	{
		TH_NULL,
		TH_COMMENT,
		TH_EOF,
		TH_EOL,
		TH_ERROR,
		TH_NAME,
		TH_NUMBER,
		TH_SIGN,
		TH_SPACE,
		TH_STRING,
		TH_WORD,
	};

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	文字バッファ
	 *
	**//*-----------------------------------------------------------------*/
	class EsCharBuf
	{
		//==================================================================
		// 定数
	private:
		static const int	NUM_UNGET		= 6;

		//==================================================================
		// メソッド
	public:
		EsCharBuf(const VcString* codeRef);

		s32				getNext();
		void			unget(s32 c);
		s32				getIndex() const	{	return _idxStr - _idxUnget;	}
		u32				getLineNo() const	{	return _lineno;				}
		EsTokenPos		getPos() const		{	return EsTokenPos(getIndex(), getLineNo(), getIndex(), getLineNo());	}

		//==================================================================
		// 変数
	private:
		const VcString*	_strRef;
		s32				_idxStr;
		s32				_cUnget[NUM_UNGET];
		s32				_idxUnget;
		u32				_lineno;
	};

public:
	/*-----------------------------------------------------------------*//**
	 *	文字ユーティリティ
	 *
	**//*-----------------------------------------------------------------*/
	class Cu
	{
	public:
		static inline bool	isCr(s32 c)	{	return c == '\r';	}
		static inline bool	isLf(s32 c)	{	return c == '\n';	}
		static bool	isSpace(s32 c);
		static bool	isReturnCode(s32 c);
		static bool	isAlphabet(s32 c);
		static bool	isDecimal(s32 c);
		static s32	toDecimal(s32 c);
		static bool isHexadecimal(s32 c);
		static s32	toHexadecimal(s32 c);
		static bool	isIdStart(s32 c);
		static bool	isIdEntity(s32 c);
		static bool isIdentifier(const VcString* str);
		static bool	isSameString(char* carr1, s32 len1, char* carr2, s32 len2);
		static s32	toLower(s32 c);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// トークンの文字列を得る
	inline const VcString*	getTokenString() const		{	return _strMkToken;			}
	// トークンキーワードを得る（無い場合は NULL）
	inline const EsKeyword*	getTokenKeyword() const		{	return _kwToken;			}
	// 現在のトークン位置を得る
	inline const EsTokenPos	getCurScriptPos() const		{	return _cb->getPos();		}
	// 現在のスクリプトインデックスを得る
	inline s32				getCurScriptIndex() const	{	return _cb->getIndex();		}
	// 現在のスクリプト行番号を得る
	inline u32				getCurScriptLineNo() const	{	return _cb->getLineNo();	}
	// 解析フラグを得る
	inline u32				getParseFlag() const		{	return _pflg;				}
	// ファイル終端かを得る
	inline bool				isEof()						{	return _isEof;				}

	// 解析フラグを設定する
	void					setParseFlag(u32 parseflag, bool enable);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	TokenHint				nextToken(EsToken* tkn);
	void					reset();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsTokenGetter(const EsKeywordSet* kwsetRef, EsError* errorRef, const VcString* codeRef, bool isStringSignMode, bool isParserMode);
	~EsTokenGetter();

private:
	s32						getTokenNextSub_FirstSkipSpace();
	TokenHint				getTokenNextSub_SkipComment(bool* isRetry, EsToken* tkn, s32 c, const EsTokenPos* posBegin);
	TokenHint				getTokenNextSub_Space(EsToken* tkn, s32 c, const EsTokenPos* posBegin);
	TokenHint				getTokenNextSub_Comment(EsToken* tkn, s32 c, const EsTokenPos* posBegin);
	TokenHint				getTokenNextSub_Eof(EsToken* tkn, s32 c, const EsTokenPos* posBegin);
	TokenHint				getTokenNextSub_Name(EsToken* tkn, s32 c, const EsTokenPos* posBegin);
	TokenHint				getTokenNextSub_Number(EsToken* tkn, s32 c, const EsTokenPos* posBegin);
	TokenHint				getTokenNextSub_String(EsToken* tkn, s32 c, const VcString* strFirst, const EsTokenPos* posBegin);
	TokenHint				getTokenNextSub_Sign(EsToken* tkn, s32 c, const VcString* strFirst, const EsTokenPos* posBegin);
	void					addCompileError(EsError::Id id, VcStringArray* saParamDlg);
	s32						getCharNext();
	s32						peekChar();
	bool					peekChars(s32* carr, s32 num);
	void					ungetChar(s32 c);
	bool					matchChars(const VcString* str, s32 cFirst, bool isStringSignMode);
	void					setupToken(EsToken* tkn, EsTokenType tt, const EsTokenPos* posBegin, const EsKeyword* kw);

	//======================================================================
	// 変数
private:
	const EsKeywordSet*		_kwsetRef;
	EsError*				_errorRef;
	const VcString*			_codeRef;
	EsCharBuf*				_cb;
	VcString*				_strMkToken;
	const EsKeyword*		_kwToken;
	u32						_pflg;
	bool					_isEof;
	bool					_isStringSignMode;
	bool					_isParserMode;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_TOKEN_GETTER_H_
