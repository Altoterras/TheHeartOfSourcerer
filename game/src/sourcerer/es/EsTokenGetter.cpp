/***********************************************************************//**
 *	EsTokenGetter.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2012/01/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsTokenGetter.h"

// Friends
#include "EsKeywordSet.h"

// External
#include "../../tfw/string/StringUtils.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsTokenGetter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	次のトークンを得る（ポインタは次へ進める）⇒ js_GetToken
**//*---------------------------------------------------------------------*/
EsTokenGetter::TokenHint EsTokenGetter::nextToken(EsToken* tkn)
{
	TokenHint th;
	s32 c;
	EsTokenPos posBegin;
	bool isRetry = false;
	do
	{
		if(_isParserMode)
		{
			// スペースをスキップする
			c = getTokenNextSub_FirstSkipSpace();

			// 現在の位置を得ておく
			posBegin.set(_cb->getIndex() - 1, _cb->getLineNo(), -1, 0);

			// コメントをスキップする
			isRetry = false;
			th = getTokenNextSub_SkipComment(&isRetry, tkn, c, &posBegin);
			if(isRetry)			{	continue;	}
			if(th != TH_NULL)	{	break;		}
		}
		else
		{
			// 現在の位置を得ておく
			posBegin.set(_cb->getIndex(), _cb->getLineNo(), -1, 0);

			c = getCharNext();	// 次の文字を得る

			// スペース解析
			th = getTokenNextSub_Space(tkn, c, &posBegin);
			if(th != TH_NULL)	{	break;		}

			// コメント解析
			th = getTokenNextSub_Comment(tkn, c, &posBegin);
			if(th != TH_NULL)	{	break;		}
		}

		// EOF トークン解析
		th = getTokenNextSub_Eof(tkn, c, &posBegin);
		if(th != TH_NULL)	{	break;		}

		// ID (TKN_NAME) 解析
		th = getTokenNextSub_Name(tkn, c, &posBegin);
		if(th != TH_NULL)	{	break;		}

		// 数値解析
		th = getTokenNextSub_Number(tkn, c, &posBegin);
		if(th != TH_NULL)	{	break;		}

		// 文字列解析
		th = getTokenNextSub_String(tkn, c, 0L, &posBegin);
		if(th != TH_NULL)	{	break;		}

		// 記号解析
		th = getTokenNextSub_Sign(tkn, c, 0L, &posBegin);
		if(th != TH_NULL)	{	break;		}

		// エラー
		addCompileError(EsError::ID_ILLEGAL_CHARACTER, 0L);
		this->setParseFlag(PF_ERROR, true);
		setupToken(tkn, TKN_ERROR, &posBegin, 0L);
		th = TH_ERROR;
	}
	while(isRetry);
	ASSERT(th != TH_NULL);

	// 行の途中フラグ
	if(th != TH_EOL)
	{
		setParseFlag(PF_DIRTY_LINE, true);
	}

	return th;
}

/*---------------------------------------------------------------------*//**
	拡張スクリプトにより解析を再開する
**//*---------------------------------------------------------------------*/
void EsTokenGetter::reset()
{
	_isEof = false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsTokenGetter::EsTokenGetter(const EsKeywordSet* kwsetRef, EsError* errorRef, const VcString* codeRef, bool isStringSignMode, bool isParserMode)
	: _kwsetRef(kwsetRef)
	, _errorRef(errorRef)
	, _codeRef(codeRef)
	, _cb(new EsCharBuf(codeRef))
	, _strMkToken(new VcString())
	, _pflg(0)
	, _isEof(false)
	, _isStringSignMode(isStringSignMode)
	, _isParserMode(isParserMode)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsTokenGetter::~EsTokenGetter()
{
	delete _strMkToken;
	delete _cb;
}

/*---------------------------------------------------------------------*//**
	nextToken サブルーチン - スペーススキップ
**//*---------------------------------------------------------------------*/
s32 EsTokenGetter::getTokenNextSub_FirstSkipSpace()
{
	s32 c;
	while(true)
	{
		c = getCharNext();	// 次の文字を得る

		if(c == '\n')
		{
			setParseFlag(PF_DIRTY_LINE, false);	// 行の途中ではない
			if(TFW_IS_FLAG(_pflg, PF_NEW_LINES))
			{
				break;
			}
		}

		if(!Cu::isSpace(c) && !Cu::isCr(c) && !Cu::isLf(c))
		{
			break;
		}
	}

	return c;
}

/*---------------------------------------------------------------------*//**
	nextToken サブルーチン - コメントスキップ
**//*---------------------------------------------------------------------*/
EsTokenGetter::TokenHint EsTokenGetter::getTokenNextSub_SkipComment(bool* isRetry, EsToken* tkn, s32 c, const EsTokenPos* posBegin)
{
	// 行コメント開始文字列の一致判定
	s32 idxMatch = -1;
	for(s32 i = 0; i < _kwsetRef->getLineCommentNum(); i++)
	{
		if(matchChars(_kwsetRef->getLineComment(i)->getName(), c, false))
		{
			idxMatch = i;
			break;
		}
	}
	// 一致した場合
	if(idxMatch != -1)
	{
		while(true)
		{
			c = getCharNext();
			if(c == '\n')
			{
				s32 cn = peekChar();
				if(cn == EOF)
				{
					c = cn;
				}
				else
				{
					ungetChar(c);
					*isRetry = true;
					setupToken(tkn, TKN_EOL, posBegin, 0L);
					return TH_EOL;
				}
			}
			if(c == EOF)
			{
				ungetChar(c);
				*isRetry = true;
				setupToken(tkn, TKN_EOF, posBegin, 0L);
				return TH_EOF;
			}
		}
	}
	
	// ブロックコメント開始文字列の一致判定
	idxMatch = -1;
	for(s32 i = 0; i < _kwsetRef->getBlockCommentStartNum(); i++)
	{
		if(matchChars(_kwsetRef->getBlockCommentStart(i)->getName(), c, false))
		{
			idxMatch = i;
			break;
		}
	}
	// 一致した場合
	if(idxMatch != -1)
	{
		u32 linenoBk = _cb->getLineNo();
		while(true)
		{
			c = getCharNext();
			if(c == EOF)
			{
				// エラー
				addCompileError(EsError::ID_UNTERMINATED_COMMENT, 0L);
				this->setParseFlag(PF_ERROR, true);
				setupToken(tkn, TKN_ERROR, posBegin, 0L);
				return TH_ERROR;
			}
			// コメント終端文字列に一致したら抜ける
			if(matchChars(_kwsetRef->getBlockCommentEnd(idxMatch)->getName(), c, false))
			{
				break;
			}
			// 行が新しくなった
			if(TFW_IS_FLAG(_pflg, PF_NEW_LINES) && linenoBk != _cb->getLineNo())
			{
				setParseFlag(PF_DIRTY_LINE, false);	// 行の途中ではない
				setupToken(tkn, TKN_EOL, posBegin, 0L);
				return TH_EOL;
			}
		}
		*isRetry = true;
	}

	return TH_NULL;
}

/*---------------------------------------------------------------------*//**
	nextToken サブルーチン - スペース
**//*---------------------------------------------------------------------*/
EsTokenGetter::TokenHint EsTokenGetter::getTokenNextSub_Space(EsToken* tkn, s32 c, const EsTokenPos* posBegin)
{
	if(!Cu::isSpace(c) && !Cu::isCr(c) && !Cu::isLf(c))	{	return TH_NULL;	}

	_strMkToken->emptyEasy();	// トークン作成文字列をクリア

	while(true)
	{
		_strMkToken->add(c);	// トークン作成文字列に文字追加

		c = getCharNext();	// 次の文字を得る

		/*
		if(c == '\n')
		{
			setParseFlag(PF_DIRTY_LINE, false);	// 行の途中ではない
			if(TFW_IS_FLAG(_pflg, PF_NEW_LINES))
			{
				break;
			}
		}
		*/

		if(!Cu::isSpace(c) && !Cu::isCr(c) && !Cu::isLf(c))
		{
			//break;
			ungetChar(c);
			return TH_SPACE;
		}
	}

	return TH_NULL;
}

/*---------------------------------------------------------------------*//**
	nextToken サブルーチン - コメント
**//*---------------------------------------------------------------------*/
EsTokenGetter::TokenHint EsTokenGetter::getTokenNextSub_Comment(EsToken* tkn, s32 c, const EsTokenPos* posBegin)
{
	// 行コメント開始文字列の一致判定
	s32 idxMatch = -1;
	for(s32 i = 0; i < _kwsetRef->getLineCommentNum(); i++)
	{
		if(matchChars(_kwsetRef->getLineComment(i)->getName(), c, false))
		{
			idxMatch = i;
			_strMkToken->copy(_kwsetRef->getLineComment(idxMatch)->getName());	// トークン作成文字列に文字設定
			break;
		}
	}
	// 一致した場合
	if(idxMatch != -1)
	{
		while(true)
		{
			c = getCharNext();
			if(c == '\n')
			{
				s32 cn = peekChar();
				if(cn == EOF)
				{
					c = cn;
				}
				else
				{
					ungetChar(c);
					return TH_COMMENT;
				}
			}
			if(c == EOF)
			{
				ungetChar(c);
				return TH_COMMENT;
			}

			_strMkToken->add(c);	// トークン作成文字列に文字追加
		}
	}
	
	// ブロックコメント開始文字列の一致判定
	idxMatch = -1;
	for(s32 i = 0; i < _kwsetRef->getBlockCommentStartNum(); i++)
	{
		if(matchChars(_kwsetRef->getBlockCommentStart(i)->getName(), c, false))
		{
			idxMatch = i;
			_strMkToken->copy(_kwsetRef->getBlockCommentStart(idxMatch)->getName());	// トークン作成文字列に文字追加
			break;
		}
	}
	// 一致した場合
	if(idxMatch != -1)
	{
		u32 linenoBk = _cb->getLineNo();
		while(true)
		{
			c = getCharNext();
			if(c == EOF)
			{
				// エラー
				addCompileError(EsError::ID_UNTERMINATED_COMMENT, 0L);
				this->setParseFlag(PF_ERROR, true);
				setupToken(tkn, TKN_ERROR, posBegin, 0L);
				return TH_ERROR;
			}
			// コメント終端文字列に一致したら抜ける
			if(matchChars(_kwsetRef->getBlockCommentEnd(idxMatch)->getName(), c, false))
			{
				_strMkToken->add(_kwsetRef->getBlockCommentEnd(idxMatch)->getName());	// トークン作成文字列に文字追加
				return TH_COMMENT;
			}
			/*
			// 行が新しくなった
			if(TFW_IS_FLAG(_pflg, PF_NEW_LINES) && linenoBk != _cb->getLineNo())
			{
				setParseFlag(PF_DIRTY_LINE, false);	// 行の途中ではない
				return setupToken(tkn, TKN_EOL, posBegin, 0L);
			}
			*/

			_strMkToken->add(c);	// トークン作成文字列に文字追加
		}
	}

	return TH_NULL;
}

/*---------------------------------------------------------------------*//**
	nextToken サブルーチン - EOF トークン解析
**//*---------------------------------------------------------------------*/
EsTokenGetter::TokenHint EsTokenGetter::getTokenNextSub_Eof(EsToken* tkn, s32 c, const EsTokenPos* posBegin)
{
	if(c != EOF)	{	return TH_NULL;	}	// 改行でない場合は抜ける

	// 新しいトークンを作成
	setupToken(tkn, TKN_EOF, posBegin, 0L);
	return TH_EOF;
}

/*---------------------------------------------------------------------*//**
	nextToken サブルーチン - NAME トークン解析
**//*---------------------------------------------------------------------*/
EsTokenGetter::TokenHint EsTokenGetter::getTokenNextSub_Name(EsToken* tkn, s32 c, const EsTokenPos* posBegin)
{
	if(!Cu::isIdStart(c))	{	return TH_NULL;	}	// NAME トークン開始でない場合は抜ける ⇒ if (JS_ISIDSTART(c) || @jsscan.cpp

	_strMkToken->emptyEasy();	// トークン作成文字列をクリア
	s32 cWk = c;
	while(true)
	{
		_strMkToken->add((char)cWk);	// トークン作成文字列に文字追加
		cWk = getCharNext();	// 次の文字を得る
		if(!Cu::isIdEntity(cWk))
		{
			// １文字戻して抜ける
			ungetChar(cWk);
			break;
		}
	}

	TokenHint th = TH_NAME;
	EsTokenType tt = TKN_NAME;
	EsOpcodeType ot = OP_NAME;
	const EsKeyword* kw = 0L;

	if(	!TFW_IS_FLAG(_pflg, PF_KEYWD_IS_NAME) 
		&& (_strMkToken->getLength() > 0)	)
	{
		kw = _kwsetRef->findWord(_strMkToken);
		if(kw != 0L)	// 言語キーワードの場合
		{
			// 予約済みキーワードの場合はエラーで返す
			if(kw->getTokenType() == TKN_RESERVED)
			{
				// エラー
				addCompileError(EsError::ID_RESERVED_NAME, 0L);
				this->setParseFlag(PF_ERROR, true);
				setupToken(tkn, TKN_ERROR, posBegin, 0L);
				return TH_ERROR;
			}

			th = TH_WORD;
			tt = kw->getTokenType();
			ot = kw->getOpcodeType();
		}
		else if(_isStringSignMode)	// 文字列記号モード
		{
			TokenHint thWk = getTokenNextSub_Sign(tkn, c, _strMkToken, posBegin);
			if(thWk != TH_NULL)	{	return thWk;	}
			thWk = getTokenNextSub_String(tkn, c, _strMkToken, posBegin);
			if(thWk != TH_NULL)	{	return thWk;	}
		}
	}

	// 新しいトークンを作成
	EsTokenPos pos(posBegin->getBeginIndex(), posBegin->getBeginLineNo(), _cb->getIndex(), _cb->getLineNo());
	setupToken(tkn, tt, &pos, kw);
	tkn->setOpcodeTypeValue(ot);
	return th;
}

/*---------------------------------------------------------------------*//**
	nextToken サブルーチン - 数値トークン解析
**//*---------------------------------------------------------------------*/
EsTokenGetter::TokenHint EsTokenGetter::getTokenNextSub_Number(EsToken* tkn, s32 c, const EsTokenPos* posBegin)
{
	if(!Cu::isDecimal(c) && !((c == '.') && Cu::isDecimal( peekChar() )))	{	return TH_NULL;	}	// 数字、もしくは少数点から始まる数字でない場合は抜ける

	_strMkToken->emptyEasy();	// トークン作成文字列をクリア

	// 何進数かを得る
	s32	radix = 10;
	while(c == '0')
	{
		_strMkToken->add((char)c);	// トークン作成文字列に文字追加
		c = getCharNext();	// 次の文字を得る
		if(Cu::toLower(c) == 'x')
		{
			_strMkToken->add((char)c);	// トークン作成文字列に文字追加
			c = getCharNext();	// 次の文字を得る
			radix = 16;
		}
		else if(Cu::isDecimal(c))
		{
			radix = 8;
		}
	}

	// 整数部を集める
	while(Cu::isHexadecimal(c))
	{
		if(radix < 16)	// 16 進以外の場合
		{
			if(Cu::isAlphabet(c))	// アルファベットの場合
			{
				break;
			}

			// “08”や“09”は 10 進数としてみなす
			if((radix == 8) && (c >= '8'))
			{
				// 警告発生
				addCompileError(EsError::ID_BAD_OCTAL, new VcStringArray(new VcString(c == '8' ? "08" : "09")));
				// 10 進数に強制変換
				radix = 10;
			}
		}

		_strMkToken->add((char)c);	// トークン作成文字列に文字追加
		c = getCharNext();	// 次の文字を得る
	}

	// 小数部を集める
	if(radix == 10 && ((c == '.') || Cu::toLower(c) == 'e'))
	{
		if(c == '.')
		{
			do
			{
				_strMkToken->add((char)c);	// トークン作成文字列に文字追加
				c = getCharNext();	// 次の文字を得る
			}
			while(Cu::isDecimal(c));
		}
		if(Cu::toLower(c) == 'e')
		{
			_strMkToken->add((char)c);	// トークン作成文字列に文字追加
			c = getCharNext();	// 次の文字を得る
			if((c == '+') || (c == '-'))
			{
				_strMkToken->add((char)c);	// トークン作成文字列に文字追加
				c = getCharNext();	// 次の文字を得る
			}
			if(!Cu::isDecimal(c))
			{
				// エラー
				addCompileError(EsError::ID_MISSING_EXPONENT, 0L);
				this->setParseFlag(PF_ERROR, true);
				setupToken(tkn, TKN_ERROR, posBegin, 0L);
				return TH_ERROR;
			}
			do
			{
				_strMkToken->add((char)c);	// トークン作成文字列に文字追加
				c = getCharNext();	// 次の文字を得る
			}
			while(Cu::isDecimal(c));
		}
	}
	ungetChar(c);	// １文字戻す

	// トークン文字列が正常か確認
	if(_strMkToken->getLength() <= 0)
	{
		// エラー
		setParseFlag(PF_ERROR, true);
		setupToken(tkn, TKN_ERROR, posBegin, 0L);
		return TH_ERROR;
	}
	if(_isStringSignMode && (_strMkToken->getAt(_strMkToken->getLength() - 1) == '.'))	// 文字列記号モード時は . で終わる数値は認めない
	{
		_strMkToken->remove(_strMkToken->getLength() - 1, 1);
		ungetChar('.');	// １文字戻す
	}

	// 新しいトークンを作成
	EsTokenPos pos(posBegin->getBeginIndex(), posBegin->getBeginLineNo(), _cb->getIndex(), _cb->getLineNo());
	setupToken(tkn, TKN_NUMBER, &pos, 0L);
	// 数値をセット
	f64 dval = 0.0;
	if(radix == 10)
	{
		dval = StringUtils::toDouble(_strMkToken, _kwsetRef->getInfinity()->getName());
	}
	else
	{
		dval = StringUtils::toDouble(_strMkToken, radix, _kwsetRef->getInfinity()->getName());
	}
	tkn->setDoubleValue(dval);

	return TH_NUMBER;
}

/*---------------------------------------------------------------------*//**
	nextToken サブルーチン - 文字列トークン解析
**//*---------------------------------------------------------------------*/
EsTokenGetter::TokenHint EsTokenGetter::getTokenNextSub_String(EsToken* tkn, s32 c, const VcString* strFirst, const EsTokenPos* posBegin)
{
	// 文字列開始引用符の一致判定	⇒ if (c == '"' || c == '\'') { @ jsscan.cpp
	char cTerminate = EOF;			// 引用符の文字による終端
	VcString strTerminate;			// 引用符の文字列による終端
	const EsKeyword* kwQuote = 0L;	// 引用符のキーワード
	if(strFirst != 0L)
	{
		kwQuote = _kwsetRef->findStringQuote(strFirst);
		if(kwQuote == 0L)		{	return TH_NULL;	}
		strTerminate.set(kwQuote->getName());
		if(kwQuote->getName()->getLength() == 1)	{	cTerminate = kwQuote->getName()->getAt(0);	}
		else										{	strTerminate.set(kwQuote->getName());		}
	}
	else
	{
		s32 idxMatch = -1;
		for(s32 i = 0; i < _kwsetRef->getStringQuoteNum(); i++)
		{
			const EsKeyword* kw = _kwsetRef->getStringQuote(i);
			if(matchChars(kw->getName(), c, false))
			{	
				kwQuote = kw;
				idxMatch = i;
				break;
			}
		}
		if(idxMatch == -1)	{	return TH_NULL;	}
		if(kwQuote->getName()->getLength() == 1)	{	cTerminate = kwQuote->getName()->getAt(0);	}
		else										{	strTerminate.set(kwQuote->getName());		}
	}

	// 文字列を集める
	_strMkToken->emptyEasy();	// トークン作成文字列をクリア
	while(true)
	{
		c = getCharNext();	// 次の文字を得る
		if((c == '\n') || (c == EOF))	// 改行、もしくはファイル終端
		{
			ungetChar(c);
			// エラー
			addCompileError(EsError::ID_UNTERMINATED_STRING, 0L);
			this->setParseFlag(PF_ERROR, true);
			setupToken(tkn, TKN_ERROR, posBegin, 0L);
			return TH_ERROR;
		}
		else if(c == '\\')	// エスケープシーケンス
		{
			c = getCharNext();	// 次の文字を得る
			if(c == 'b')		{	c = '\b';	}
			else if(c == 'f')	{	c = '\f';	}
			else if(c == 'n')	{	c = '\n';	}
			else if(c == 'r')	{	c = '\r';	}
			else if(c == 't')	{	c = '\t';	}
			else if(c == 'v')	{	c = '\v';	}
			else if(('0' <= c) && (c <= '7'))
			{
				// 8 進数数値
				s32 val = 0;
				for(int i = 0; i < 2; i++)
				{
					s32 valNew = (val * 8) + Cu::toDecimal(c);
					if(valNew > 0377)	// ここで定義できる最大値は 377
					{
						break;
					}
					val = valNew;
					// 次の文字
					c = peekChar();
					if((c < '0') || ('7' < c))
					{
						break;
					}
					getCharNext();
				}
				c = val;
			}
			else if(c == 'x')
			{
				// 16 進数数値
				s32 cw[2] = { 0, 0 };
				if(peekChars(cw, 2))
				{
					if(Cu::isHexadecimal(cw[0]) && Cu::isHexadecimal(cw[1]))
					{
						s32 val = 0;
						for(int i = 0; i < 2; i++)
						{
							val = (val * 16) + Cu::toHexadecimal(cw[i]);
							getCharNext();
						}
						c = val;
					}
				}
			}
			else if(c == 'u')
			{
				// 16 進数数値によるユニコード文字
				s32 cw[4] = { 0, 0, 0, 0 };
				if(peekChars(cw, 4))
				{
					if(Cu::isHexadecimal(cw[0]) && Cu::isHexadecimal(cw[1]) && Cu::isHexadecimal(cw[2]) && Cu::isHexadecimal(cw[3]))
					{
						s32 val = 0;
						for(int i = 0; i < 4; i++)
						{
							val = (val * 16) + Cu::toHexadecimal(cw[i]);
							getCharNext();
						}
						c = val;
					}
				}
			}
			else if(c == '\n')
			{
				// 改行のエスケープ
				continue;
			}
		}
		else if(cTerminate == c)	// " か '
		{
			break;
		}
		_strMkToken->add((char)c);	// トークン作成文字列に文字追加

		// 文字列による終端の場合
		if(strTerminate.getLength() > 0)
		{
			if(strTerminate.getLength() < _strMkToken->getLength())
			{
				s32 idxSpritter = _strMkToken->getLength() - strTerminate.getLength() - 1;
				ASSERT((0 <= idxSpritter) && (idxSpritter < _strMkToken->getLength()));
				char cWk = _strMkToken->getAt(idxSpritter);
				if(Cu::isSpace(cWk))	// スプリットされている
				{
					idxSpritter++;
					VcString word = _strMkToken->substring(idxSpritter);
					if(word.equals(strTerminate))	// 終端引用符判定
					{
						// 終端された
						ASSERT(_strMkToken->getLength() > 0);
						// 終端引用符を取り除く
						word = _strMkToken->substring(0, idxSpritter);
						// 前後の不要なスペース、前後それぞれ１つ分を取り除く
						s32 idxStart = 0;
						cWk = word.getAt(idxStart);
						if(Cu::isSpace(cWk))
						{
							idxStart++;
						}
						s32 idxEnd = word.getLength();
						cWk = word.getAt(idxEnd - 1);
						if(Cu::isSpace(cWk))
						{
							idxEnd--;
						}
						if(idxStart < idxEnd)
						{
							VcString wordSub(word.substring(idxStart, idxEnd));
							_strMkToken->set(&wordSub);
						}
						else
						{
							_strMkToken->emptyEasy();
						}
						break;
					}
				}
			}
		}
	}

	// 新しいトークンを作成
	EsTokenPos pos(posBegin->getBeginIndex(), posBegin->getBeginLineNo(), _cb->getIndex(), _cb->getLineNo());
	setupToken(tkn, TKN_STRING, &pos, kwQuote);
	// ＃＃＃ 未対応 ＃＃＃ ⇒  tp->pos.end.lineno = (uint16)ts->lineno;
	tkn->setOpcodeTypeValue(OP_STRING);
	return TH_STRING;
}

/*---------------------------------------------------------------------*//**
	nextToken サブルーチン - 記号トークン解析
**//*---------------------------------------------------------------------*/
EsTokenGetter::TokenHint EsTokenGetter::getTokenNextSub_Sign(EsToken* tkn, s32 c, const VcString* strFirst, const EsTokenPos* posBegin)
{
	if(strFirst != 0)
	{
		const EsKeyword* kw = _kwsetRef->findSign(strFirst);
		if(kw != 0L)
		{
			if(!_isParserMode)	{	_strMkToken->copy(kw->getName());		}	// トークン作成文字列に文字設定
			// 新しいトークンを作成
			setupToken(tkn, kw->getTokenType(), posBegin, kw);
			tkn->setOpcodeTypeValue(kw->getOpcodeType());
			return TH_SIGN;
		}
	}
	else
	{
		for(s32 i = 0; i < _kwsetRef->getSignNum(); i++)
		{
			const EsKeyword* kw = _kwsetRef->getSign(i);
			if(matchChars(kw->getName(), c, _isStringSignMode))
			{
				if(!_isParserMode)	{	_strMkToken->copy(kw->getName());	}	// トークン作成文字列に文字設定
				// 新しいトークンを作成
				setupToken(tkn, kw->getTokenType(), posBegin, kw);
				tkn->setOpcodeTypeValue(kw->getOpcodeType());
				return TH_SIGN;
			}
		}
	}

	return TH_NULL;
}

/*---------------------------------------------------------------------*//**
	コンパイルエラーを追加する
**//*---------------------------------------------------------------------*/
void EsTokenGetter::addCompileError(EsError::Id id, VcStringArray* saParamDlg)
{
	if(_errorRef != 0L)	{	_errorRef->addCompileError(id, _cb->getPos(), saParamDlg);	}
}

/*---------------------------------------------------------------------*//**
	次の解析文字を得る（ポインタを進める）
**//*---------------------------------------------------------------------*/
s32 EsTokenGetter::getCharNext()
{
	s32 c = _cb->getNext();
	if(c == EOF)	{	_isEof = true;	}	// EOF フラグを立てる
	return c;
}

/*---------------------------------------------------------------------*//**
	次の解析文字を得る（ポインタを進めない）
		次の文字をのぞき見るときに使う
**//*---------------------------------------------------------------------*/
s32 EsTokenGetter::peekChar()
{
	s32 c = getCharNext();
	ungetChar(c);
	return c;
}

/*---------------------------------------------------------------------*//**
	次の複数解析文字を得る（ポインタを進めない）
		次の文字をのぞき見るときに使う
**//*---------------------------------------------------------------------*/
bool EsTokenGetter::peekChars(s32* carr, s32 num)
{
	s32 c;

	s32 cnt = 0;
	while(cnt < num)
	{
		c = getCharNext();
		if(c == EOF)
		{
			break;
		}
		if(c == '\n')
		{
			ungetChar(c);
			break;
		}
		carr[cnt] = c;
		cnt++;
	}

	for(int i = cnt - 1; i >= 0; i--)
	{
		ungetChar(carr[i]);
	}

	return cnt == num;
}

/*---------------------------------------------------------------------*//**
	解析文字のポインタを戻す
**//*---------------------------------------------------------------------*/
void EsTokenGetter::ungetChar(s32 c)
{
	_cb->unget(c);
}

/*---------------------------------------------------------------------*//**
	解析文字列が一致するか判定する
	（一致した場合はポインタを進め、一致しなかった場合は戻す）
**//*---------------------------------------------------------------------*/
bool EsTokenGetter::matchChars(const VcString* str, s32 cFirst, bool isStringSignMode)
{
	s32 c = cFirst;
	const char* cs = str->getRaw();
	s32 num = str->getLength();
	char buf[16];
	char* carr = (num > 16) ? new char[num] : buf;
	carr[0] = c;
	s32 cnt = 0;
	while(cnt < num)
	{
		if(c == EOF)
		{
			break;
		}
		if(c == '\n')
		{
			if(cnt >= 1)	{	ungetChar(c);	}
			break;
		}
		if(c != cs[cnt])
		{
			break;
		}

		cnt++;

		// 全て一致
		if(cnt == num)
		{
			if(isStringSignMode)	// 文字列記号モード
			{
				// 比較対象の文字がそこで終わっているかどうか判定する
				if((num > 1) || Cu::isAlphabet(cs[0]) || Cu::isDecimal(cs[0]))	// １文字記号ではない場合
				{
					c = peekChar();
					if((c != EOF) && (c != '\r') && (c != '\n') && !Cu::isSpace(c))
					{
						// NG
						break;
					}
				}
			}

			if(carr != buf)	{	delete[] carr;	}
			return true;
		}

		c = getCharNext();
		carr[cnt] = c;
	}

	// 一致していない場合は unget する
	for(int i = cnt - 1; i >= 0; i--)
	{
		ungetChar(carr[i + 1]);
	}
	if(carr != buf)	{	delete[] carr;	}
	return false;
}

/*---------------------------------------------------------------------*//**
	解析フラグ設定
**//*---------------------------------------------------------------------*/
void EsTokenGetter::setParseFlag(u32 parseflag, bool enable)
{
	if(enable)
	{
		_pflg |= parseflag;
	}
	else
	{
		_pflg &= ~parseflag;
	}
}

/*---------------------------------------------------------------------*//**
	新しいトークンを得る
**//*---------------------------------------------------------------------*/
void EsTokenGetter::setupToken(EsToken* tkn, EsTokenType tt, const EsTokenPos* posBegin, const EsKeyword* kw)
{
	tkn->setType(tt);
	EsTokenPos pos(posBegin->getBeginIndex(), posBegin->getBeginLineNo(), _cb->getIndex(), _cb->getLineNo());
	tkn->setPos(&pos);
	_kwToken = kw;
}


//==========================================================================
// EsTokenGetter::EsCharBuf メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsTokenGetter::EsCharBuf::EsCharBuf(const VcString* codeRef)
	: _strRef(codeRef)
	, _idxStr(0)
	, _idxUnget(0)
	, _lineno(0)
{
	memset(_cUnget, 0, sizeof(_cUnget));
}

/*---------------------------------------------------------------------*//**
	次の文字を得る
**//*---------------------------------------------------------------------*/
s32 EsTokenGetter::EsCharBuf::getNext()
{
	// Unget バッファに保存されている場合はそちらから取り出す
	if(_idxUnget > 0)
	{
		_idxUnget--;
		s32 c = _cUnget[_idxUnget];
		if(c == '\n')	{	_lineno++;	}	// 改行コードの場合は行番号を１つ増やす
		return c;
	}

	// 終端判定
	if(_idxStr >= _strRef->getLength())
	{
		return EOF;
	}

	// １文字取り出す
	s32 c = _strRef->getAt(_idxStr);
	_idxStr++;
	if(c == '\n')	{	_lineno++;	}	// 改行コードの場合は行番号を１つ増やす

	return c;
}

/*---------------------------------------------------------------------*//**
	文字の進行を戻す
**//*---------------------------------------------------------------------*/
void EsTokenGetter::EsCharBuf::unget(s32 c)
{
	if(c == EOF)	{	return;		}
	ASSERT(_idxUnget < NUM_UNGET);

	if(c == '\n')	{	_lineno--;	}	// 改行コードの場合は行番号を１つ減らす

	// Unget バッファに保存
	_cUnget[_idxUnget] = c;
	_idxUnget++;
}


//==========================================================================
// EsTokenGetter::Cu メソッド

/*---------------------------------------------------------------------*//**
	文字がスペースであるかどうか
**//*---------------------------------------------------------------------*/
bool EsTokenGetter::Cu::isSpace(s32 c)
{
	return (c == ' ') || (c == '\t');
}

/*---------------------------------------------------------------------*//**
	文字がアルファベットであるかどうか
**//*---------------------------------------------------------------------*/
bool EsTokenGetter::Cu::isAlphabet(s32 c)
{
	return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
}

/*---------------------------------------------------------------------*//**
	文字が（10 進数の）数字であるかどうか ⇒ JS7_ISDEC
**//*---------------------------------------------------------------------*/
bool EsTokenGetter::Cu::isDecimal(s32 c)
{
	return ('0' <= c) && (c <= '9');
}

/*---------------------------------------------------------------------*//**
	文字を（10 進数の）数字に変換する ⇒ JS7_UNDEC
**//*---------------------------------------------------------------------*/
s32 EsTokenGetter::Cu::toDecimal(s32 c)
{
	return c - '0';
}

/*---------------------------------------------------------------------*//**
	文字が（16 進数の）数字であるかどうか
**//*---------------------------------------------------------------------*/
bool EsTokenGetter::Cu::isHexadecimal(s32 c)
{
	return (('0' <= c) && (c <= '9')) || (('a' <= c) && (c <= 'f')) || (('A' <= c) && (c <= 'F'));
}

/*---------------------------------------------------------------------*//**
	文字を（16 進数の）数字に変換する
**//*---------------------------------------------------------------------*/
s32 EsTokenGetter::Cu::toHexadecimal(s32 c)
{
	if(('a' <= c) && (c <= 'f'))	{	return c - 'a' + 10;	}
	if(('A' <= c) && (c <= 'F'))	{	return c - 'F' + 10;	}
	return c - '0';
}

/*---------------------------------------------------------------------*//**
	ID の開始文字であるかどうか ⇒ JS_ISIDSTART
**//*---------------------------------------------------------------------*/
bool EsTokenGetter::Cu::isIdStart(s32 c)
{
	return isAlphabet(c) || (c == '_') || (c == '$');
}

/*---------------------------------------------------------------------*//**
	ID の構成文字であるかどうか ⇒ JS_ISIDENT
**//*---------------------------------------------------------------------*/
bool EsTokenGetter::Cu::isIdEntity(s32 c)
{
	return isAlphabet(c) || isDecimal(c) || (c == '_') || (c == '$');
}

/*---------------------------------------------------------------------*//**
	文字列が ID であるかどうか ⇒ js_IsIdentifier
**//*---------------------------------------------------------------------*/
bool EsTokenGetter::Cu::isIdentifier(const VcString* str)
{
	s32 length = str->getLength();
	if(length <= 0)			{	return false;	}

	s32 c = str->getAt(0);
	if(!isIdStart(c))		{	return false;	}

	for(int i = 1; i < length; i++)
	{
		c = str->getAt(1);
		if(!isIdEntity(c))
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	同じ文字列かどうか
**//*---------------------------------------------------------------------*/
bool EsTokenGetter::Cu::isSameString(char* carr1, s32 len1, char* carr2, s32 len2)
{
	if(len1 != len2)			{	return false;	}
	for(int i = 0; i < len1; i++)
	{
		if(carr1[i] != carr2[i])	{	return false;	}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	小文字に変換する
**//*---------------------------------------------------------------------*/
s32 EsTokenGetter::Cu::toLower(s32 c)
{
	return (('A' <= c) && (c <= 'Z')) ? (c - 'A' + 'a') : c;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
