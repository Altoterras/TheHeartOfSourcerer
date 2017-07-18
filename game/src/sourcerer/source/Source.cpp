/***********************************************************************//**
 *	Source.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Source.h"

// Friends
#include "FixedSourceDefTbl.h"
#include "../souma/script/SoumaScriptKeywords.h"

// External
#include "../../es/EsAtom.h"
#include "../../es/EsTokenGetter.h"
#include "../../es/kwset/JsKeywordSet.h"
#include "../../es/kwset/SsKeywordSet.h"
#include "../../tfw/serialize/Serializer.h"
#include "../../tfw/string/StringUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Source メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	未使用かどうかを調べる
**//*---------------------------------------------------------------------*/
bool Source::isEmpty() const
{
	return _code->getLength() == 0;
}

/*---------------------------------------------------------------------*//**
	有効なソースかどうかどうかを調べる
**//*---------------------------------------------------------------------*/
bool Source::isValid() const
{
	return (_name->getLength() > 0) && (_code->getLength() > 0);
}

/*---------------------------------------------------------------------*//**
	有効なユーザー作成ソースかを得る（バグがなく文法的に合っているかは別）
**//*---------------------------------------------------------------------*/
bool Source::isUserCustomized() const
{
	if(!isValid())					{	return false;	}	// 有効なソースではない
	if((_flags & F_KIND_MASK) == 0)	{	return false;	}	// 種別不明
	if(getFixedSourceDef() != 0L)	{	return false;	}	// 固定ソース定義あり
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	カスタムソースコードを設定する
**//*---------------------------------------------------------------------*/
bool Source::setCustomCode(const VcString* code)
{
	ASSERT((_name != 0L) && (_code != 0L));
	_code->copy(code);

	// ソース最大サイズでトリミング
	if(_code->getLength() > MAX_SOURCE_SIZE_REGULATION)
	{
		_code->remove(MAX_SOURCE_SIZE_REGULATION, _code->getLength() - MAX_SOURCE_SIZE_REGULATION);
	}

	// ソース種別を自動判別
	if(getSourceKind() == SourceDef::SRCKIND_NULL)
	{
		SourceDef::SourceKind srckindDetected = Source::detectSourceKind();
		if(srckindDetected != SourceDef::SRCKIND_NULL)
		{
			_flags = (srckindDetected & F_KIND_MASK) | (_flags & ~F_KIND_MASK);
		}
	}

	// 名称・説明を解析
	if(getSourceKind() == SourceDef::SRCKIND_NULL)
	{
		_flags = SourceDef::SRCKIND_SS | (_flags & ~F_KIND_MASK); // とりあえず SS 言語とする
		_name->set("");
		_explain->set("");
		return true;
	}
	else
	{
		return analyzeNameAndExplain(_name, _explain);
	}
}

/*---------------------------------------------------------------------*//**
	名前と説明文を解析する
**//*---------------------------------------------------------------------*/
bool Source::analyzeNameAndExplain(VcString* name, VcString* explain)
{
	name->empty();
	explain->empty();

	bool isSucceeded = false;
	bool isParseStrSign = (getSourceKind() == SourceDef::SRCKIND_SS) ? true : false;
	EsKeywordSet* kwset = SourceDef::getKeywordSet(getSourceKind());
	EsTokenGetter tg(kwset, 0L, _code, isParseStrSign, false);
	EsToken tknWk;
	EsTokenGetter::TokenHint th = EsTokenGetter::TH_NULL;
	while(true)
	{
		th = tg.nextToken(&tknWk);
		switch(th)
		{
		case EsTokenGetter::TH_WORD:
			if(tg.getTokenString()->equals("function"))
			{
				do
				{
					th = tg.nextToken(&tknWk);
					switch(th)
					{
					case EsTokenGetter::TH_NAME:
						// 名前取得
						name->copy(tg.getTokenString());
						isSucceeded = true;
						goto END_PARSE;	// 終了へ
					case EsTokenGetter::TH_SIGN:
					case EsTokenGetter::TH_EOF:
					case EsTokenGetter::TH_ERROR:
						goto END_PARSE;	// 終了へ
					}

				}
				while(th != EsTokenGetter::TH_NULL);
				goto END_PARSE;	// 終了へ
			}
			break;
		case EsTokenGetter::TH_COMMENT:
			{
				// 説明文字列を抽出する
				VcString strWk(tg.getTokenString());
				// コメント記号を消去
				for(int i = 0; i < kwset->getLineCommentNum(); i++)
				{
					strWk.replaceAll(kwset->getLineComment(i)->getName(), "");
				}
				for(int i = 0; i < kwset->getBlockCommentStartNum(); i++)
				{
					strWk.replaceAll(kwset->getBlockCommentStart(i)->getName(), "");
				}
				for(int i = 0; i < kwset->getBlockCommentEndNum(); i++)
				{
					strWk.replaceAll(kwset->getBlockCommentEnd(i)->getName(), "");
				}
				// 前後のスペースや改行を消去する
				StringUtils::trim(&strWk);
				// 最初の行のみを得る
				s32 idx = strWk.find('\r', 0);
				if(idx == -1)	{	idx = strWk.find('\n', 0);	}
				if(idx != -1)	{	strWk.remove(idx, strWk.getLength() - idx);	}
				// 説明文字列取得
				explain->copy(&strWk);
			}
			break;
		case EsTokenGetter::TH_EOF:
		case EsTokenGetter::TH_ERROR:
			goto END_PARSE;	// 終了へ
		}
	}
END_PARSE:

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	名前を書き換える
**//*---------------------------------------------------------------------*/
bool Source::rewriteName(const VcString* nameNew)
{
	VcString codeNew(_code);

	bool isSucceeded = false;
	bool isParseStrSign = (getSourceKind() == SourceDef::SRCKIND_SS) ? true : false;
	EsKeywordSet* kwset = SourceDef::getKeywordSet(getSourceKind());
	EsTokenGetter tg(kwset, 0L, _code, isParseStrSign, false);
	EsToken tknWk;
	EsTokenGetter::TokenHint th = EsTokenGetter::TH_NULL;
	while(true)
	{
		th = tg.nextToken(&tknWk);
		switch(th)
		{
		case EsTokenGetter::TH_WORD:
			if(tg.getTokenString()->equals("function"))
			{
				do
				{
					th = tg.nextToken(&tknWk);
					switch(th)
					{
					case EsTokenGetter::TH_NAME:
						// 名前書き換え
						codeNew.replace(tg.getTokenString(), nameNew, tg.getCurScriptIndex() - tg.getTokenString()->getLength(), 1);
						isSucceeded = true;
						goto END_PARSE;	// 終了へ
					case EsTokenGetter::TH_EOF:
					case EsTokenGetter::TH_ERROR:
						goto END_PARSE;	// 終了へ
					}

				}
				while(th != EsTokenGetter::TH_NULL);
				goto END_PARSE;	// 終了へ
			}
			break;
		case EsTokenGetter::TH_EOF:
		case EsTokenGetter::TH_ERROR:
			goto END_PARSE;	// 終了へ
		}
	}
END_PARSE:

	if(isSucceeded)
	{
		_name->copy(nameNew);
		_code->copy(&codeNew);
	}

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	言語種別判定
**//*---------------------------------------------------------------------*/
SourceDef::SourceKind Source::detectSourceKind() const
{
	VcString strBuf;
	const int NUM_CBUF = 4;
	char cbuf[NUM_CBUF] = { 0, 0, 0, 0 };
	s32 idxInLineComment = -1;
	s32 idxInBlockComment = -1;
	int jc;
	s32 score[SourceDef::NUM_SRCKIND];
	TFW_ZERO_MEMORY(score, sizeof(s32) * SourceDef::NUM_SRCKIND); 
	for(int i = 0; i < _code->getLength(); i++)
	{
		cbuf[3] = cbuf[2];
		cbuf[2] = cbuf[1];
		cbuf[1] = cbuf[0];
		cbuf[0] = _code->getAt(i);
		// コメント処理
		if((idxInLineComment == -1) && (idxInBlockComment == -1))
		{
			// コメント外
			strBuf.add(cbuf[0]);

			// 行コメントの始まりを検出
			for(int k = SourceDef::SRCKIND_NULL + 1; k < SourceDef::NUM_SRCKIND; k++)
			{
				const EsKeywordSet* kwset = SourceDef::getKeywordSet((SourceDef::SourceKind)k);
				if(kwset == 0L)	{	continue;	}
				for(int ic = 0; ic < kwset->getLineCommentNum(); ic++)
				{
					const VcString* cn = kwset->getLineComment(ic)->getName();
					int ncn = cn->getLength();	ASSERT(ncn <= NUM_CBUF);
					for(jc = 0; jc < ncn; jc++)
					{
						if(cn->getAt(jc) != cbuf[ncn - jc - 1])	{	break;	}
					}
					if(jc == ncn)
					{
						idxInLineComment = ic;
						strBuf.remove(strBuf.getLength() - ncn, ncn);
						SourceDef::SourceKind srckindDetected = detectSourceKindSub_checkPart(score, &strBuf);
						if(srckindDetected != SourceDef::SRCKIND_NULL)	{	return srckindDetected;	}
						break;
					}
				}
				if(idxInLineComment != -1)	{	break;	}
			}
			if(idxInLineComment != -1)	{	continue;	}
			// ブロックコメントの始まりを検出
			for(int k = SourceDef::SRCKIND_NULL + 1; k < SourceDef::NUM_SRCKIND; k++)
			{
				const EsKeywordSet* kwset = SourceDef::getKeywordSet((SourceDef::SourceKind)k);
				if(kwset == 0L)	{	continue;	}
				for(int ic = 0; ic < kwset->getBlockCommentStartNum(); ic++)
				{
					const VcString* cn = kwset->getBlockCommentStart(ic)->getName();
					int ncn = cn->getLength();	ASSERT(ncn <= NUM_CBUF);
					for(jc = 0; jc < ncn; jc++)
					{
						if(cn->getAt(jc) != cbuf[ncn - jc - 1])	{	break;	}
					}
					if(jc == ncn)
					{
						idxInBlockComment = ic;
						strBuf.remove(strBuf.getLength() - ncn, ncn);
						SourceDef::SourceKind srckindDetected = detectSourceKindSub_checkPart(score, &strBuf);
						if(srckindDetected != SourceDef::SRCKIND_NULL)	{	return srckindDetected;	}
						break;
					}
				}
				if(idxInBlockComment != -1)	{	break;	}
			}
			if(idxInBlockComment != -1)	{	continue;	}
		}
		else
		{
			// 行コメントの終わりを検出
			if(idxInLineComment != -1)
			{
				if(cbuf[0] == '\n')
				{
					idxInLineComment = -1;
					strBuf.emptyEasy();
					continue;
				}
			}
			// ブロックコメントの終わりを検出
			if(idxInBlockComment != -1)
			{
				for(int k = SourceDef::SRCKIND_NULL + 1; k < SourceDef::NUM_SRCKIND; k++)
				{
					const EsKeywordSet* kwset = SourceDef::getKeywordSet((SourceDef::SourceKind)k);
					if(kwset == 0L)	{	continue;	}
					const VcString* cn = kwset->getBlockCommentEnd(idxInBlockComment)->getName();
					int ncn = cn->getLength();	ASSERT(ncn <= NUM_CBUF);
					for(jc = 0; jc < ncn; jc++)
					{
						if(cn->getAt(jc) != cbuf[ncn - jc - 1])	{	break;	}
					}
					if(jc == ncn)
					{
						idxInBlockComment = -1;
						strBuf.emptyEasy();
						break;
					}
				}
				if(idxInBlockComment == -1)	{	continue;	}
			}
		}
	}
	return detectSourceKindSub_checkPart(score, &strBuf);
}

/*---------------------------------------------------------------------*//**
	言語種別判定サブルーチン - 部分判定
**//*---------------------------------------------------------------------*/
SourceDef::SourceKind Source::detectSourceKindSub_checkPart(s32 score[SourceDef::NUM_SRCKIND], const VcString* codePart) const
{
	// 記号の使用の仕方で判断する
	for(int k = SourceDef::SRCKIND_NULL + 1; k < SourceDef::NUM_SRCKIND; k++)
	{
		const EsKeywordSet* kwset = SourceDef::getKeywordSet((SourceDef::SourceKind)k);
		if(kwset == 0L)	{	continue;	}

		// 大括弧 {} 記号で判定する
		const VcString* cn = kwset->getSign(EsKeywordSet::S_SIGN_LC)->getName();	// {
		if(codePart->find(cn, 0) != -1)
		{
			score[k]++;
		}
		cn = kwset->getSign(EsKeywordSet::S_SIGN_RC)->getName();			// }
		if(codePart->find(cn, 0) != -1)
		{
			score[k]++;
		}
	}

	// 最大スコアのものを返す
	s32 scoreMax = 0;
	SourceDef::SourceKind srckindScoreMax = SourceDef::SRCKIND_NULL;
	for(int k = SourceDef::SRCKIND_NULL + 1; k < SourceDef::NUM_SRCKIND; k++)
	{
		if(scoreMax < score[k])
		{
			scoreMax = score[k];
			srckindScoreMax = (SourceDef::SourceKind)k;
		}
	}

	return srckindScoreMax;
}

/*---------------------------------------------------------------------*//**
	言語を JS へ変換
**//*---------------------------------------------------------------------*/
bool Source::convToJs()
{
	/*
	if(_fsrcdf != 0L)	// 固定ソースあり
	{
		_code->copy(_fsrcdf->getJsSource());
	}
	else
	*/
	{
		if(!convToJsCode(_code))	{	return false;	}
	}

	_flags = SourceDef::SRCKIND_JS | (_flags & ~F_KIND_MASK);
	return true;
}

/*---------------------------------------------------------------------*//**
	言語を SS へ変換
**//*---------------------------------------------------------------------*/
bool Source::convToSs()
{
	/*
	if(_fsrcdf != 0L)	// 固定ソースあり
	{
		_code->copy(_fsrcdf->getSsSource());
	}
	else
	*/
	{
		if(!convToSsCode(_code))	{	return false;	}
	}

	_flags = SourceDef::SRCKIND_SS | (_flags & ~F_KIND_MASK);
	return true;
}

/*---------------------------------------------------------------------*//**
	コードの言語を JS へ変換
**//*---------------------------------------------------------------------*/
bool Source::convToJsCode(VcString* code)
{
	VcString codeNew(*code);
	SsKeywordSet kwsetSs;
	JsKeywordSet kwsetJs;
	if(!convLanguageByKeyword(&codeNew, &kwsetSs, &kwsetJs, true))	{	return false;	}	// キーワード変換
	///adjustExtraSpaceOfCode(&codeNew, &kwsetJs, removeExtraSpaceOfJsCodePart);	// 余分なスペースの削除
	code->copy(&codeNew);
	return true;
}

/*---------------------------------------------------------------------*//**
	コードの言語を SS へ変換
**//*---------------------------------------------------------------------*/
bool Source::convToSsCode(VcString* code)
{
	VcString codeNew(*code);
	JsKeywordSet kwsetJs;
	SsKeywordSet kwsetSs;
	///adjustExtraSpaceOfCode(&codeNew, &kwsetJs, addExtraSpaceOfJsCodePart);	// 余剰スペースの追加
	if(!convLanguageByKeyword(&codeNew, &kwsetJs, &kwsetSs, false))	{	return false;	}	// キーワード変換
	code->copy(&codeNew);
	return true;
}

/*---------------------------------------------------------------------*//**
	言語変換
**//*---------------------------------------------------------------------*/
bool Source::convLanguageByKeyword(VcString* code, const EsKeywordSet* kwsetOld, const EsKeywordSet* kwsetNew, bool isOldLangSs)
{
	VcString codeNew;
	s32 idx;
	EsTokenGetter tg(kwsetOld, 0L, code, isOldLangSs ? true : false, false);
	EsToken tknWk;
	EsTokenGetter::TokenHint th = EsTokenGetter::TH_NULL;
	do
	{
		th = tg.nextToken(&tknWk);
		switch(th)
		{
		default:
			codeNew.add(tg.getTokenString());
			break;
		case EsTokenGetter::TH_STRING:
			{
				const EsKeyword* kwQuoteOld = tg.getTokenKeyword();
				const EsKeyword* kwQuoteNew = 0L;
				for(int i = 0; (i < kwsetOld->getStringQuoteNum()) && (i < kwsetNew->getStringQuoteNum()); i++)
				{
					if(kwQuoteOld == kwsetOld->getStringQuote(i))
					{
						kwQuoteNew = kwsetNew->getStringQuote(i);
						break;
					}
				}
				ASSERT(kwQuoteNew != 0L);
				if(kwQuoteNew != 0L)
				{
					const VcString* strQuote = kwQuoteNew->getName();
					bool isAlphabetQuote = (strQuote->getLength() > 0) ? EsTokenGetter::Cu::isAlphabet(strQuote->getAt(strQuote->getLength() - 1)) : false;
					convLanguageSub_writeStringKeyword(&codeNew, strQuote, isOldLangSs);
					if(isAlphabetQuote)	{	codeNew.add(' ');	}
					codeNew.add(tg.getTokenString());
					if(isAlphabetQuote)	{	codeNew.add(' ');	}
					codeNew.add(strQuote);
				}
			}
			break;
		case EsTokenGetter::TH_COMMENT:
			{
				VcString strWk(tg.getTokenString());
				for(int i = 0; (i < kwsetOld->getLineCommentNum()) && (i < kwsetNew->getLineCommentNum()); i++)
				{
					strWk.replaceAll(kwsetOld->getLineComment(i)->getName(), kwsetNew->getLineComment(i)->getName());
				}
				for(int i = 0; (i < kwsetOld->getBlockCommentStartNum()) && (i < kwsetNew->getBlockCommentStartNum()); i++)
				{
					strWk.replaceAll(kwsetOld->getBlockCommentStart(i)->getName(), kwsetNew->getBlockCommentStart(i)->getName());
				}
				for(int i = 0; (i < kwsetOld->getBlockCommentEndNum()) && (i < kwsetNew->getBlockCommentEndNum()); i++)
				{
					strWk.replaceAll(kwsetOld->getBlockCommentEnd(i)->getName(), kwsetNew->getBlockCommentEnd(i)->getName());
				}
				codeNew.add(&strWk);
			}
			break;
		case EsTokenGetter::TH_SIGN:
			idx = kwsetOld->findSignIndex(tg.getTokenString());
			if(idx != -1)
			{
				convLanguageSub_writeKeyword(&codeNew, kwsetNew->getSign(idx), isOldLangSs);
			}
			break;
		case EsTokenGetter::TH_WORD:
			// 言語キーワードを検索
			idx = kwsetOld->findWordIndex(tg.getTokenString());
			if(idx != -1)	// 言語キーワードである
			{
				convLanguageSub_writeKeyword(&codeNew, kwsetNew->getWord(idx), isOldLangSs);
			}
			break;
		case EsTokenGetter::TH_NAME:
			// ソウマキーワードを検索
			idx = -1;
			for(s32 i = 0; i < SoumaScriptKeywords::NUM_S; i++)
			{
				const EsAtom* kwd = SoumaScriptKeywords::getKeyword(i);
				if(kwd->getString()->equals(tg.getTokenString()))
				{
					idx = i;
					break;
				}
			}
			if(idx != -1)	// ソウマキーワードである
			{
				idx = (idx & 1) ? idx - 1 : idx + 1;	// 奇数ならば一つ前、偶数ならば一つ後
				convLanguageSub_writeStringKeyword(&codeNew, SoumaScriptKeywords::getKeyword(idx)->getString(), isOldLangSs);
			}
			else
			{
				convLanguageSub_writeStringKeyword(&codeNew, tg.getTokenString(), isOldLangSs);
			}
			break;
		case EsTokenGetter::TH_NUMBER:
			convLanguageSub_writeStringKeyword(&codeNew, tg.getTokenString(), isOldLangSs);
			break;
		case EsTokenGetter::TH_EOF:
		case EsTokenGetter::TH_ERROR:
			th = EsTokenGetter::TH_NULL;	// 終了へ
			break;
		case EsTokenGetter::TH_EOL:
			break;
		}
	}
	while(th != EsTokenGetter::TH_NULL);

	code->copy(&codeNew);
	return true;
}

/*---------------------------------------------------------------------*//**
	言語変換サブルーチン - キーワードの書込み
**//*---------------------------------------------------------------------*/
void Source::convLanguageSub_writeKeyword(VcString* codeNew, const EsKeyword* kwd, bool isOldLangSs)
{
#if 1
	ASSERT(kwd != 0L);
	convLanguageSub_writeStringKeyword(codeNew, kwd->getName(), isOldLangSs);
#else
	ASSERT(kwd != 0L);
	int nc = codeNew->getLength();						// 現在のコード長
	char cc = (nc >= 1) ? codeNew->getAt(nc - 1) : 0;	// 現在のコードの末尾文字
	char ck = kwd->getName()->getAt(0);					// キーワードの先頭文字
	if(EsTokenGetter::Cu::isAlphabet(ck))	// 追加キーワードはアルファベットである
	{
		if(EsTokenGetter::Cu::isAlphabet(cc) || EsTokenGetter::Cu::isDecimal(cc))	// コードの末尾文字もアルファベットまたは数字である
		{
			codeNew->add(' ');	// 単語同士がくっつかないようにスペースを追加する
		}
	}
	else	// 追加キーワードは記号である（数字のキーワードは無いとする）
	{
		if(cc == ' ')	// コードの末端文字がスペースである
		{
			bool isExtraSpace = true;
			// スペースがあっても自然なトークンかで判定
			switch(kwd->getTokenType())
			{
			case TKN_SEMI:		// ;
			case TKN_COMMA:		// ,
			case TKN_DOT:		// .
			case TKN_COLON:		// :
				break;
			case TKN_INC:		// ++
			case TKN_DEC:		// --
			case TKN_LB:		// [
			case TKN_RB:		// ]
			case TKN_LC:		// {
			case TKN_RC:		// }
			case TKN_LP:		// (
			case TKN_RP:		// )
			case TKN_DBLCOLON:	// ::
				if(nc >= 2)
				{
					cc = codeNew->getAt(nc - 2);	// 更に一つ前の文字を得る
					switch(cc)
					{
					case '=':	case '+':	case '-':	case '*':	case '/':	case '%':
					case '>':	case '<':	case '|':	case '&':	case '?':	case ':':
						isExtraSpace = false;	// 不要なスペースではない（必要ではないがおそらく意図的に入れている）
					}
				}
				break;					// スペース不要
			default:
				isExtraSpace = false;	// 不要なスペースではない
				break;					// スペース不要
			}
			// インデントのためのスペースかを調べる
			if(isExtraSpace)
			{
				for(int i = nc - 2; i >= 0; i--)
				{
					cc = codeNew->getAt(i);
					if(cc == ' ')
					{
						// ２つ以上の連続したスペースは何か意味があるものと考える
						isExtraSpace = false;	// 不要なスペースではない
						break;
					}
					else if((cc == '\n') || (i == 0))	// 行頭まで来た
					{
						// インデントのためのスペースである
						isExtraSpace = false;	// 不要なスペースではない
						break;
					}
					else
					{
						break;
					}
				}
			}
			// 不ペースは余分であると判断した場合は除去する
			if(isExtraSpace)
			{
				codeNew->remove(nc - 1, 1);	// 不要なスペースを削除する
			}
		}
	}

	codeNew->add(kwd->getName());
#endif
}

/*---------------------------------------------------------------------*//**
	言語変換サブルーチン - 文字列の書込み
**//*---------------------------------------------------------------------*/
void Source::convLanguageSub_writeStringKeyword(VcString* codeNew, const VcString* strKeyword, bool isOldLangSs)
{
	ASSERT(strKeyword != 0L);
	int nc = codeNew->getLength();	// 現在のコード長
	if(nc >= 1)
	{
		char ct1 = codeNew->getAt(nc - 1);										// 現在のコードの末尾文字
		char ck1 = strKeyword->getAt(0);										// 追加キーワードの先頭文字
		char ck2 = (strKeyword->getLength() >= 2) ? strKeyword->getAt(1) : 0;	// 追加キーワードの２文字目
		if(ct1 == ' ')	// １つ前がスペース（タブは含めない）
		{
			if((nc >= 2) && isOldLangSs)
			{
				char ct2 = codeNew->getAt(nc - 2);	// ２つ前の文字を得る
				if(EsTokenGetter::Cu::isAlphabet(ct2) || EsTokenGetter::Cu::isDecimal(ct2))	// ２つ前がアルファベットか数字
				{
					if(!EsTokenGetter::Cu::isAlphabet(ck1) && !EsTokenGetter::Cu::isDecimal(ck1) && !EsTokenGetter::Cu::isSpace(ck1))	// 記号
					{
						if(	((ck1 == '+') && (ck2 == '+'))	||	// ++
							((ck1 == '-') && (ck2 == '-'))	)	// --
						{
							codeNew->remove(nc - 1, 1);	// 不要なスペースを削除する
						}
						else switch(ck1)
						{
						case '=':	case '+':	case '-':	case '*':	case '/':	case '%':
						case '>':	case '<':	case '|':	case '&':	case '?':	case ':':	case ';':	case ',':
							break;
						default:
							codeNew->remove(nc - 1, 1);	// 不要なスペースを削除する
							break;
						}
					}
				}
				else if(!EsTokenGetter::Cu::isSpace(ct2))	// ２つ前が記号
				{
					char ct3 = (nc >= 3) ? codeNew->getAt(nc - 3) : 0;	// ３つ前の文字を得る
					if(	((ct2 == '+') && (ct2 == '+'))	||		// ++
						((ct3 == '-') && (ct3 == '-'))	)		// --
					{
						codeNew->remove(nc - 1, 1);	// 不要なスペースを削除する
					}
					else switch(ct2)
					{
					case '=':	case '+':	case '-':	case '*':	case '/':	case '%':
					case '>':	case '<':	case '|':	case '&':	case '?':	case ':':	case ';':	case ',':
						break;
					default:
						switch(ck1)
						{
						case '=':	case '+':	case '-':	case '*':	case '/':	case '%':
						case '>':	case '<':	case '|':	case '&':	case '?':	case ':':	case ';':	case ',':
							break;
						default:
							codeNew->remove(nc - 1, 1);	// 不要なスペースを削除する
							break;
						}
						break;
					}
				}
			}
		}
		else if(EsTokenGetter::Cu::isAlphabet(ck1) || EsTokenGetter::Cu::isDecimal(ck1))		// 追加キーワードはアルファベットまたは数字である
		{
			if(EsTokenGetter::Cu::isAlphabet(ct1) || EsTokenGetter::Cu::isDecimal(ct1) || (ct1 == '"') || (ct1 == '\''))	// 一つ前の文字がアルファベットまたは数字、文字列クォートである
			{
				codeNew->add(' ');	// 単語同士がくっつかないようにスペースを追加する
			}
		}
		else if((ck1 == '"') || (ck1 == '\''))	// 追加キーワードは文字列クォートである
		{
			if(EsTokenGetter::Cu::isAlphabet(ct1) || !EsTokenGetter::Cu::isDecimal(ct1) || !EsTokenGetter::Cu::isSpace(ct1))	// 一つ前が記号以外
			{
				codeNew->add(' ');	// 単語と記号がくっつかないようにスペースを追加する
			}
		}
	}

	codeNew->add(strKeyword);
}

#if 0
/*---------------------------------------------------------------------*//**
	コードを調整する
**//*---------------------------------------------------------------------*/
void Source::adjustExtraSpaceOfCode(VcString* code, const EsKeywordSet* kwset, void(* func)(VcString*))
{
	VcString codeNew;
	VcString strBuf;
	const int NUM_CBUF = 4;
	char cbuf[NUM_CBUF] = { 0, 0, 0, 0 };
	s32 idxInLineComment = -1;
	s32 idxInBlockComment = -1;
	int jc;
	for(int i = 0; i < code->getLength(); i++)
	{
		cbuf[3] = cbuf[2];
		cbuf[2] = cbuf[1];
		cbuf[1] = cbuf[0];
		cbuf[0] = code->getAt(i);
		strBuf.add(cbuf[0]);
		if((idxInLineComment == -1) && (idxInBlockComment == -1))
		{
			// 行コメントの始まりを検出
			for(int ic = 0; ic < kwset->getLineCommentNum(); ic++)
			{
				const VcString* cn = kwset->getLineComment(ic)->getName();
				int ncn = cn->getLength();	ASSERT(ncn <= NUM_CBUF);
				for(jc = 0; jc < ncn; jc++)
				{
					if(cn->getAt(jc) != cbuf[ncn - jc - 1])	{	break;	}
				}
				if(jc == ncn)
				{
					idxInLineComment = ic;
					strBuf.remove(strBuf.getLength() - ncn, ncn);
					func(&strBuf);
					codeNew.add(&strBuf);
					strBuf.copy(cn);
					break;
				}
			}
			if(idxInLineComment != -1)	{	continue;	}
			// ブロックコメントの始まりを検出
			for(int ic = 0; ic < kwset->getBlockCommentStartNum(); ic++)
			{
				const VcString* cn = kwset->getBlockCommentStart(ic)->getName();
				int ncn = cn->getLength();	ASSERT(ncn <= NUM_CBUF);
				for(jc = 0; jc < ncn; jc++)
				{
					if(cn->getAt(jc) != cbuf[ncn - jc - 1])	{	break;	}
				}
				if(jc == ncn)
				{
					idxInBlockComment = ic;
					strBuf.remove(strBuf.getLength() - ncn, ncn);
					func(&strBuf);
					codeNew.add(&strBuf);
					strBuf.copy(cn);
					break;
				}
			}
			if(idxInBlockComment != -1)	{	continue;	}
		}
		else
		{
			// 行コメントの終わりを検出
			if(idxInLineComment != -1)
			{
				if(cbuf[0] == '\n')
				{
					idxInLineComment = -1;
					codeNew.add(&strBuf);
					strBuf.emptyEasy();
					continue;
				}
			}
			// ブロックコメントの終わりを検出
			if(idxInBlockComment != -1)
			{
				const VcString* cn = kwset->getBlockCommentEnd(idxInBlockComment)->getName();
				int ncn = cn->getLength();	ASSERT(ncn <= NUM_CBUF);
				for(jc = 0; jc < ncn; jc++)
				{
					if(cn->getAt(jc) != cbuf[ncn - jc - 1])	{	break;	}
				}
				if(jc == ncn)
				{
					idxInBlockComment = -1;
					codeNew.add(&strBuf);
					strBuf.emptyEasy();
					continue;
				}
			}
		}
	}
	func(&strBuf);
	codeNew.add(&strBuf);
	code->copy(&codeNew);
}

/*---------------------------------------------------------------------*//**
	変換後の JS コードの余分なスペースを除去する
**//*---------------------------------------------------------------------*/
void Source::removeExtraSpaceOfJsCodePart(VcString* code)
{
	code->replaceAll(" (", "(");
	code->replaceAll("( ", "(");
	code->replaceAll(" )", ")");
	code->replaceAll(" {", "{");
	code->replaceAll("{ ", "{");
	code->replaceAll(" }", "}");
	code->replaceAll(" ++", "++");
	code->replaceAll(" --", "--");
	code->replaceAll("++ ", "++");
	code->replaceAll("-- ", "--");
}

/*---------------------------------------------------------------------*//**
	変換前の JS コードに余分なスペースを追加する
**//*---------------------------------------------------------------------*/
void Source::addExtraSpaceOfJsCodePart(VcString* code)
{
	VcString codeNew;
	char cNext = (code->getLength() > 0) ? code->getAt(0) : 0;
	char cCur = cNext;
	char cPrev = cCur;
	for(int i = 0; i < code->getLength(); i++)
	{
		cPrev = cCur;
		cCur = cNext;
		cNext = ((i + 1) < code->getLength()) ? code->getAt(i + 1) : 0;
		switch(cCur)
		{
		case '(':
		case ')':
		case '{':
		case '}':
			switch(cPrev)
			{
			case 0:
			case '\n':
			case '\t':
			case ' ':
				break;
			default:
				codeNew.add(' ');
				break;
			}
			codeNew.add(cCur);
			switch(cNext)
			{
			case 0:
			case '\r':
			case '\n':
			case '\t':
			case ' ':
			case ';':
				break;
			default:
				codeNew.add(' ');
				cCur = ' ';
				break;
			}
			break;
		default:
			codeNew.add(cCur);
			break;
		}
	}
	code->copy(&codeNew);
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Source::Source()
	: _name(new VcString())
	, _code(new VcString())
	, _explain(new VcString())
	, _fsrcdf(0L)
	, _flags(0)
{
}

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
**//*---------------------------------------------------------------------*/
Source::Source(const Source& src)
	: _name(new VcString(src._name))
	, _code(new VcString(src._code))
	, _explain(new VcString(src._explain))
	, _fsrcdf(src._fsrcdf)
	, _flags(src._flags)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Source::~Source()
{
	delete _name;
	delete _code;
	delete _explain;
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void Source::clear()
{
	_name->empty();
	_code->empty();
	_explain->empty();
	_fsrcdf = 0L;
	_flags = 0L;
	_eeNqlNecs.clear();
}

/*---------------------------------------------------------------------*//**
	固定ソース セットアップ
**//*---------------------------------------------------------------------*/
void Source::setup(const FixedSourceDef* fsrcdf, SourceDef::SourceKind srckind)
{
	ASSERT((_name != 0L) && (_code != 0L));
	clear();

	// フラグ設定
	_flags = srckind & F_KIND_MASK;

	if(fsrcdf != 0L)
	{
		// 固定ソース定義を保存
		_fsrcdf = fsrcdf;

		// 名前を設定
		_name->set(_fsrcdf->getName());

		// ソースコードを設定
		switch(srckind)
		{
		case SourceDef::SRCKIND_SS:	_code->set(_fsrcdf->getSsSource());		break;
		case SourceDef::SRCKIND_JS:	_code->set(_fsrcdf->getJsSource());		break;
		}

		// 説明を得る
		_explain->copy(_fsrcdf->getExplain());
	}

	// 必要原質解析
	analyzeOql();
}

/*---------------------------------------------------------------------*//**
	カスタムソース セットアップ
**//*---------------------------------------------------------------------*/
void Source::setup(const CStringBase* name, const CStringBase* code, const CStringBase* explain, SourceDef::SourceKind srckind)
{
	ASSERT((_name != 0L) && (_code != 0L));
	clear();

	// フラグ設定
	_flags = srckind & F_KIND_MASK;

	// 名前を設定
	_name->set(name);

	// ソースコードを設定
	_code->set(code);

	// 説明を得る
	_explain->set(explain);

	// 必要原質解析
	analyzeOql();
}

/*---------------------------------------------------------------------*//**
	コピー（完全複製）
**//*---------------------------------------------------------------------*/
void Source::copy(const Source* src)
{
	_name->copy(src->_name);
	_code->copy(src->_code);
	_explain->copy(src->_explain);
	_fsrcdf = src->_fsrcdf;
	_flags = src->_flags;
	_eeNqlNecs.copy(&_eeNqlNecs);
}

/*---------------------------------------------------------------------*//**
	ユーザーが編集できるソースに複製する
**//*---------------------------------------------------------------------*/
void Source::reproduce(const Source* src)
{
	_name->copy(src->_name);
	_code->copy(src->_code);
	_explain->copy(src->_explain);
	_fsrcdf = 0L;
	_flags = src->_flags;			// フラグはとりあえず継承
	_eeNqlNecs.copy(&src->_eeNqlNecs);
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool Source::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		if(!ser->store(&_flags)) { return false; }
		if(!ser->store(&_eeNqlNecs, sizeof(Elene))) { return false; }

		bool isFixSrc = (_fsrcdf != 0L);
		if(!ser->store(&isFixSrc)) { return false; }

		if(isFixSrc)
		{
			u16 fsrcdid = _fsrcdf->getFixedSourceDefId();
			if(!ser->store(&fsrcdid)) { return false; }
		}
		else
		{
			if(!ser->store(_name)) { return false; }
			if(!ser->store(_code)) { return false; }
		}
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	デシリアイズ
**//*---------------------------------------------------------------------*/
bool Source::deserialize(Serializer* ser, const FixedSourceDefTbl* fsrcdftbl)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		if(!ser->restore(&_flags)) { return false; }
		if(!ser->restore(&_eeNqlNecs, sizeof(Elene))) { return false; }

		bool isFixSrc = false;
		if(!ser->restore(&isFixSrc)) { return false; }

		if(isFixSrc)
		{
			u16 fsrcdid = 0;
			if(!ser->restore(&fsrcdid)) { return false; }
			_fsrcdf = fsrcdftbl->getDef(fsrcdid);
		}
		else
		{
			if(!ser->restore(_name)) { return false; }
			if(!ser->restore(_code)) { return false; }
		}
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャに保存する
**//*---------------------------------------------------------------------*/
bool Source::storeToStructure(SaveStructure* svst, VcString* strSave) const
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	// フラグ・必要原質量
	svst->_flags = _flags;
	TFW_COPY_MEMORY(&svst->_eeNqlNecs, &_eeNqlNecs, sizeof(Elene));

	if(_fsrcdf != 0L)
	{
		// 固定ソース定義 ID を書き込む
		svst->_sidxName = 0;
		svst->_sidxCode = - (s32)_fsrcdf->getFixedSourceDefId();
	}
	else
	{
		// 名前を末端文字列に追加
		svst->_sidxName = strSave->getLength();
		strSave->add(_name);
		strSave->add(SaveStructure::CHAR_SPRIT);
		// コードを末端文字列に追加
		svst->_sidxCode = strSave->getLength();
		strSave->add(_code);
		strSave->add(SaveStructure::CHAR_SPRIT);
		// 説明文を末端文字列に追加
		svst->_sidxExplain = strSave->getLength();
		strSave->add(_explain);
		strSave->add(SaveStructure::CHAR_SPRIT);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool Source::restoreFromStructure(const SaveStructure* svst, const VcString* strSave, const FixedSourceDefTbl* fsrcdftbl)
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	// フラグ・必要原質量
	u32 flags = svst->_flags;
	SourceDef::SourceKind srckind = (SourceDef::SourceKind)(flags & F_KIND_MASK);
	Elene eeNqlNes;
	TFW_COPY_MEMORY(&eeNqlNes, &svst->_eeNqlNecs, sizeof(Elene));

	if(svst->_sidxCode < 0)
	{
		// 固定ソース定義 ID を読み込む
		u16 fsrcdid = (u16)(- svst->_sidxCode);
		const FixedSourceDef* fsrcdf = fsrcdftbl->getDef(fsrcdid); ASSERT(fsrcdf != 0L);
		if(fsrcdf != 0L)
		{
			// 設定する
			setup(fsrcdf, srckind);
		}
	}
	else
	{
		// 名前を読み込む
		int idx = (svst->_sidxName >= 0) ? strSave->find(SaveStructure::CHAR_SPRIT, svst->_sidxName) : -1;
		VcString name = (idx > 0) ? strSave->substring(svst->_sidxName, idx) : "";
		// コードを読み込む
		idx = (svst->_sidxCode >= 0) ? strSave->find(SaveStructure::CHAR_SPRIT, svst->_sidxCode) : -1;
		VcString code = (idx > 0) ? strSave->substring(svst->_sidxCode, idx) : "";
		// 説明文を読み込む
		idx = (svst->_sidxExplain >= 0) ? strSave->find(SaveStructure::CHAR_SPRIT, svst->_sidxExplain) : -1;
		VcString explain = (idx > 0) ? strSave->substring(svst->_sidxExplain, idx) : "";
		// 設定する
		setup(&name, &code, &explain, srckind);
	}

	// セットアップ後の値上書き
	_flags = flags;
	TFW_COPY_MEMORY(&_eeNqlNecs, &eeNqlNes, sizeof(Elene));

	return true;
}

/*---------------------------------------------------------------------*//**
	値の入れ替え
**//*---------------------------------------------------------------------*/
void Source::swap(Source* source1, Source* source2)
{
	Source sourceTmp(*source1);
	source1->copy(source2);
	source2->copy(&sourceTmp);
}

/*---------------------------------------------------------------------*//**
	必要原質解析
**//*---------------------------------------------------------------------*/
void Source::analyzeOql()
{
	if(_code == 0L)	{	return;	}
	s32 len = _code->getLength();
	if(len <= 0)	{	return;	}

	s32 eeSo = 0;
	s32 eeWa = 0;
	s32 eeFi = 0;
	s32 eeWi = 0;
	s32 eeAe = 0;

	s32 idxFindBegin = 0;
	s32 idxFound = -1;
	s32 idx;
	const char* pc;
	if(getSourceKind() == SourceDef::SRCKIND_JS)
	{
///		ASSERT(false);	// 未実装

		const char* szEnluc = "enluc(";
		for(int safecnt = 0; safecnt < 100; safecnt++)
		{
			s32 eeSoWk = 0;
			s32 eeWaWk = 0;
			s32 eeFiWk = 0;
			s32 eeWiWk = 0;
			s32 eeAeWk = 0;

			idxFound = _code->find(szEnluc, idxFindBegin);
			if(idxFound < 0)	{	break;	}
			idxFound += (s32)::strlen(szEnluc);
			idxFindBegin = idxFound;
			if(idxFound >= len)	{	break;	}

			idx = idxFound;
			pc = _code->getRaw() + idx;

			if(!analyzeSub_skipSpace(&idx, &pc, len))				{	break;	}
			if(!analyzeSub_skipSign(&idx, &pc, len, '('))			{	break;	}
			if(!analyzeSub_getNumber(&eeAeWk, &idx, &pc, len, ','))	{	break;	}
			analyzeSub_skipSpace(&idx, &pc, len);
			if(!analyzeSub_getNumber(&eeWiWk, &idx, &pc, len, ','))	{	break;	}
			analyzeSub_skipSpace(&idx, &pc, len);
			if(!analyzeSub_getNumber(&eeFiWk, &idx, &pc, len, ','))	{	break;	}
			analyzeSub_skipSpace(&idx, &pc, len);
			if(!analyzeSub_getNumber(&eeWaWk, &idx, &pc, len, ','))	{	break;	}
			analyzeSub_skipSpace(&idx, &pc, len);
			if(!analyzeSub_getNumber(&eeSoWk, &idx, &pc, len, ')'))	{	break;	}

			if(eeAe < eeAeWk)	{	eeAe = eeAeWk;	}
			if(eeWi < eeWiWk)	{	eeWi = eeWiWk;	}
			if(eeFi < eeFiWk)	{	eeFi = eeFiWk;	}
			if(eeWa < eeWaWk)	{	eeWa = eeWaWk;	}
			if(eeSo < eeSoWk)	{	eeSo = eeSoWk;	}

			idxFindBegin += idx + 1;
		}
	}
	else if(getSourceKind() == SourceDef::SRCKIND_SS)
	{
		const char* szEnluc = "enluc";
		for(int safecnt = 0; safecnt < 100; safecnt++)
		{
			s32 eeSoWk = 0;
			s32 eeWaWk = 0;
			s32 eeFiWk = 0;
			s32 eeWiWk = 0;
			s32 eeAeWk = 0;

			idxFound = _code->find(szEnluc, idxFindBegin);
			if(idxFound < 0)	{	break;	}
			idxFound += (s32)::strlen(szEnluc);
			idxFindBegin = idxFound;
			if(idxFound >= len)	{	break;	}

			idx = idxFound;
			pc = _code->getRaw() + idx;

			if(!analyzeSub_skipSpace(&idx, &pc, len))				{	break;	}
			if(!analyzeSub_skipSign(&idx, &pc, len, 'a'))			{	break;	}
			if(!analyzeSub_getNumber(&eeAeWk, &idx, &pc, len, ','))	{	break;	}
			analyzeSub_skipSpace(&idx, &pc, len);
			if(!analyzeSub_getNumber(&eeWiWk, &idx, &pc, len, ','))	{	break;	}
			analyzeSub_skipSpace(&idx, &pc, len);
			if(!analyzeSub_getNumber(&eeFiWk, &idx, &pc, len, ','))	{	break;	}
			analyzeSub_skipSpace(&idx, &pc, len);
			if(!analyzeSub_getNumber(&eeWaWk, &idx, &pc, len, ','))	{	break;	}
			analyzeSub_skipSpace(&idx, &pc, len);
			if(!analyzeSub_getNumber(&eeSoWk, &idx, &pc, len, 'o')){	break;	}

			if(eeAe < eeAeWk)	{	eeAe = eeAeWk;	}
			if(eeWi < eeWiWk)	{	eeWi = eeWiWk;	}
			if(eeFi < eeFiWk)	{	eeFi = eeFiWk;	}
			if(eeWa < eeWaWk)	{	eeWa = eeWaWk;	}
			if(eeSo < eeSoWk)	{	eeSo = eeSoWk;	}

			idxFindBegin += idx + 1;
		}
	}

	_eeNqlNecs.setAethel(eeAe * Elene::ELENE_VALID_FIGURE);
	_eeNqlNecs.setWind(eeWi * Elene::ELENE_VALID_FIGURE);
	_eeNqlNecs.setFire(eeFi * Elene::ELENE_VALID_FIGURE);
	_eeNqlNecs.setWater(eeWa * Elene::ELENE_VALID_FIGURE);
	_eeNqlNecs.setSoil(eeSo * Elene::ELENE_VALID_FIGURE);
}

/*---------------------------------------------------------------------*//**
	必要原質解析 - スペースをスキップする
**//*---------------------------------------------------------------------*/
bool Source::analyzeSub_skipSpace(s32* idx, const char** pc, s32 len)
{
	bool isFindSpace = false;

	while(true)
	{
		if((*idx) >= len)
		{	
			return isFindSpace;
		}
		if(((**pc) != ' ') && ((**pc) != '\t'))
		{
			return isFindSpace;
		}

		isFindSpace = true;

		(*idx)++;
		(*pc)++;
	}

	return isFindSpace;
}

/*---------------------------------------------------------------------*//**
	必要原質解析 - 記号をスキップする
**//*---------------------------------------------------------------------*/
bool Source::analyzeSub_skipSign(s32* idx, const char** pc, s32 len, char cSign)
{
	bool isFindSign = false;

	while(true)
	{
		if((*idx) >= len)
		{
			return isFindSign;
		}
		if((**pc) != cSign)
		{
			(*idx)++;
			(*pc)++;
			return isFindSign;
		}

		isFindSign = true;

		(*idx)++;
		(*pc)++;
	}

	return isFindSign;
}

/*---------------------------------------------------------------------*//**
	必要原質解析 - 数値を取得する
**//*---------------------------------------------------------------------*/
bool Source::analyzeSub_getNumber(s32* number, s32* idx, const char** pc, s32 len, char splitter)
{
	*number = 0;
	while(true)
	{
		if((*idx) >= len)
		{
			return true;
		}
		if((**pc) == splitter)
		{
			(*idx)++;
			(*pc)++;
			return true;
		}

		if(('0' <= (**pc)) && ((**pc) <= '9'))
		{
			// 現状は 10 進数のみ対応
			*number = ((*number) * 10) + (**pc) - '0';
		}
		else
		{
			// スペース除去．スペース、スプリッタ以外は不正
			while(true)
			{
				if((*idx) >= len)						{	return true;	}
				if((**pc) == splitter)					{	return true;	}
				if(((**pc) != ' ') && ((**pc) != '\t'))	{	return false;	}
				(*idx)++;
				(*pc)++;
			}
		}

		(*idx)++;
		(*pc)++;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
