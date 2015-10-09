/***********************************************************************//**
 *	SsKeywordSet.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2010/09/14.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SsKeywordSet.h"

// Friends

// External
#include "../../../tfw/collection/List.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SsKeywordSet::SsKeywordSet()
{
	// 予約語
	List<EsKeyword*> listWord;
	// 標準的予約語
	listWord.addTail(new EsKeyword("break",			TKN_BREAK,		OP_NOP));
	listWord.addTail(new EsKeyword("case",			TKN_CASE,		OP_NOP));
	listWord.addTail(new EsKeyword("continue",		TKN_CONTINUE,	OP_NOP));
	listWord.addTail(new EsKeyword("default",		TKN_DEFAULT,	OP_NOP));
	listWord.addTail(new EsKeyword("delete",		TKN_DELETE,		OP_NOP));
	listWord.addTail(new EsKeyword("do",			TKN_DO,			OP_NOP));
	listWord.addTail(new EsKeyword("else",			TKN_ELSE,		OP_NOP));
	listWord.addTail(new EsKeyword("export",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("false",			TKN_PRIMARY,	OP_FALSE));
	listWord.addTail(new EsKeyword("for",			TKN_FOR,		OP_NOP));
	listWord.addTail(new EsKeyword("function",		TKN_FUNCTION,	OP_NOP));
	listWord.addTail(new EsKeyword("if",			TKN_IF,			OP_NOP));
	listWord.addTail(new EsKeyword("in",			TKN_IN,			OP_IN));
	listWord.addTail(new EsKeyword("new",			TKN_NEW,		OP_NEW));
	listWord.addTail(new EsKeyword("null",			TKN_PRIMARY,	OP_NULL));
	listWord.addTail(new EsKeyword("return",		TKN_RETURN,		OP_NOP));
	listWord.addTail(new EsKeyword("switch",		TKN_SWITCH,		OP_NOP));
	listWord.addTail(new EsKeyword("this",			TKN_PRIMARY,	OP_THIS));
	listWord.addTail(new EsKeyword("true",			TKN_PRIMARY,	OP_TRUE));
	listWord.addTail(new EsKeyword("typeof",		TKN_UNARYOP,	OP_TYPEOF));
	listWord.addTail(new EsKeyword("var",			TKN_VAR,		OP_DEFVAR));
	listWord.addTail(new EsKeyword("void",			TKN_UNARYOP,	OP_VOID));
	listWord.addTail(new EsKeyword("while",			TKN_WHILE,		OP_NOP));
	listWord.addTail(new EsKeyword("with",			TKN_WITH,		OP_NOP));
	listWord.addTail(new EsKeyword("try",			TKN_TRY,		OP_NOP));
	listWord.addTail(new EsKeyword("catch",			TKN_CATCH,		OP_NOP));
	listWord.addTail(new EsKeyword("finally",		TKN_FINALLY,	OP_NOP));
	listWord.addTail(new EsKeyword("throw",			TKN_THROW,		OP_NOP));
	listWord.addTail(new EsKeyword("instanceof",	TKN_INSTANCEOF,	OP_INSTANCEOF));
#if 1 // Java のキーワード
	listWord.addTail(new EsKeyword("abstract",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("boolean",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("byte",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("char",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("class",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("double",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("extends",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("final",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("float",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("goto",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("implements",	TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("import",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("int",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("interface",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("long",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("native",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("package",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("private",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("protected",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("public",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("short",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("static",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("super",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("synchronized",	TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("throws",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("transient",		TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("volatile",		TKN_RESERVED,	OP_NOP));
#endif
	// 拡張的予約語
	listWord.addTail(new EsKeyword("const",			TKN_VAR,		OP_DEFCONST));	// 標準仕様では無いらしい
	listWord.addTail(new EsKeyword("enum",			TKN_RESERVED,	OP_NOP));
	listWord.addTail(new EsKeyword("debugger",		TKN_DEBUGGER,	OP_NOP));
	listWord.addTail(new EsKeyword("yield",			TKN_YIELD,		OP_NOP));
	listWord.addTail(new EsKeyword("let",			TKN_LET,		OP_NOP));
	// 配列化
	_numWord = listWord.getCount();
	_kwarrWord = listWord.toArray();

	// 論理的な型
	_kwarrLogicalType[LTYPE_NULL			] = getStatic(S_TYPE_NULL);
	_kwarrLogicalType[LTYPE_VOID			] = getStatic(S_TYPE_UNDEFINED);
	_kwarrLogicalType[LTYPE_OBJ				] = getStatic(S_TYPE_OBJECT);
	_kwarrLogicalType[LTYPE_FUNC			] = getStatic(S_TYPE_FUNCTION);
	_kwarrLogicalType[LTYPE_STRING			] = getStatic(S_TYPE_STRING);
	_kwarrLogicalType[LTYPE_NUMBER			] = getStatic(S_TYPE_NUMBER);
	_kwarrLogicalType[LTYPE_BOOLEAN			] = getStatic(S_TYPE_BOOLEAN);

	// 記号
	List<EsKeyword*> listSign;
	// 記号など（定義準が重要）
	listSign.addTail(new EsKeyword(".",				TKN_SEMI,		OP_NOP));		// ;
	listSign.addTail(new EsKeyword("ka",			TKN_LB,			OP_NOP));		// [
	listSign.addTail(new EsKeyword("ko",			TKN_RB,			OP_NOP));		// ]
	listSign.addTail(new EsKeyword("la",			TKN_LC,			OP_NOP));		// {
	listSign.addTail(new EsKeyword("lo",			TKN_RC,			OP_NOP));		// }
	listSign.addTail(new EsKeyword("a",				TKN_LP,			OP_NOP));		// (
	listSign.addTail(new EsKeyword("o",				TKN_RP,			OP_NOP));		// )
	listSign.addTail(new EsKeyword(",",				TKN_COMMA,		OP_NOP));		// ,
	listSign.addTail(new EsKeyword("hate",			TKN_HOOK,		OP_NOP));		// ?
	listSign.addTail(new EsKeyword("n",				TKN_DOT,		OP_NOP));		// .
	listSign.addTail(new EsKeyword("colon",			TKN_COLON,		OP_NOP));		// :
	listSign.addTail(new EsKeyword("loor",			TKN_OR,			OP_NOP));		// ||
	listSign.addTail(new EsKeyword("bioras",		TKN_ASSIGN,		OP_BITOR));		// |=
	listSign.addTail(new EsKeyword("bior",			TKN_BITOR,		OP_NOP));		// |
	listSign.addTail(new EsKeyword("bixoras",		TKN_ASSIGN,		OP_BITXOR));	// ^=
	listSign.addTail(new EsKeyword("bixor",			TKN_BITXOR,		OP_NOP));		// ^
	listSign.addTail(new EsKeyword("loand",			TKN_AND,		OP_NOP));		// &&
	listSign.addTail(new EsKeyword("biandas",		TKN_ASSIGN,		OP_BITAND));	// &=
	listSign.addTail(new EsKeyword("biand",			TKN_BITAND,		OP_NOP));		// &
	listSign.addTail(new EsKeyword("exeq",			TKN_EQOP,		OP_STRICTEQ));	// ===
	listSign.addTail(new EsKeyword("eq",			TKN_EQOP,		OP_EQ));		// ==
	listSign.addTail(new EsKeyword("as",			TKN_ASSIGN,		OP_NOP));		// =
	listSign.addTail(new EsKeyword("exne",			TKN_EQOP,		OP_STRICTNE));	// !==
	listSign.addTail(new EsKeyword("ne",			TKN_EQOP,		OP_NE));		// !=
	listSign.addTail(new EsKeyword("lonot",			TKN_UNARYOP,	OP_NOT));		// !
	listSign.addTail(new EsKeyword("<!--",			TKN_NULL,		OP_NULL));		// 要検討修正
	listSign.addTail(new EsKeyword("bileshiftas",	TKN_ASSIGN,		OP_LSFT));		// <<=
	listSign.addTail(new EsKeyword("bileshift",		TKN_SFTOP,		OP_LSFT));		// <<
	listSign.addTail(new EsKeyword("le",			TKN_INQOP,		OP_LE));		// <=
	listSign.addTail(new EsKeyword("lt",			TKN_INQOP,		OP_LT));		// <
	listSign.addTail(new EsKeyword("birishiftasaez",TKN_ASSIGN,		OP_URSFT));		// >>>=
	listSign.addTail(new EsKeyword("birishiftaez",	TKN_SFTOP,		OP_URSFT));		// >>>
	listSign.addTail(new EsKeyword("birishiftas",	TKN_ASSIGN,		OP_RSFT));		// >>=
	listSign.addTail(new EsKeyword("birishift",		TKN_SFTOP,		OP_RSFT));		// >>
	listSign.addTail(new EsKeyword("ge",			TKN_INQOP,		OP_GE));		// >=
	listSign.addTail(new EsKeyword("gt",			TKN_INQOP,		OP_GT));		// >
	listSign.addTail(new EsKeyword("multas",		TKN_ASSIGN,		OP_MUL));		// *=
	listSign.addTail(new EsKeyword("mult",			TKN_STAR,		OP_MUL));		// *
	listSign.addTail(new EsKeyword("//",			TKN_NULL,		OP_NULL));		// 要検討修正
	listSign.addTail(new EsKeyword("/*",			TKN_NULL,		OP_NULL));		// 要検討修正
	listSign.addTail(new EsKeyword("divas",			TKN_ASSIGN,		OP_DIV));		// /=
	listSign.addTail(new EsKeyword("div",			TKN_DIVOP,		OP_DIV));		// /
	listSign.addTail(new EsKeyword("modulas",		TKN_ASSIGN,		OP_MOD));		// %=
	listSign.addTail(new EsKeyword("modul",			TKN_DIVOP,		OP_MOD));		// %
	listSign.addTail(new EsKeyword("binot",			TKN_UNARYOP,	OP_BITNOT));	// ~
	listSign.addTail(new EsKeyword("addas",			TKN_ASSIGN,		OP_ADD));		// +=
	listSign.addTail(new EsKeyword("inc",			TKN_INC,		OP_NOP));		// ++
	listSign.addTail(new EsKeyword("plus",			TKN_PLUS,		OP_POS));		// +
	listSign.addTail(new EsKeyword("-->",			TKN_NULL,		OP_NULL));		// 要検討修正
	listSign.addTail(new EsKeyword("subas",			TKN_ASSIGN,		OP_SUB));		// -=
	listSign.addTail(new EsKeyword("dec",			TKN_DEC,		OP_NOP));		// --
	listSign.addTail(new EsKeyword("minus",			TKN_MINUS,		OP_NEG));		// -
	// 配列化
	_numSign = listSign.getCount();
	_kwarrSign = listSign.toArray();
	ASSERT(NUM_S_SIGN == _numSign);

	// 文字列引用符
	_numStringQuote = 4;
	_kwarrStringQuote = new EsKeyword*[_numStringQuote];
	_kwarrStringQuote[0] = new EsKeyword("\"", TKN_NULL, OP_NULL);					// "
	_kwarrStringQuote[1] = new EsKeyword("'", TKN_NULL, OP_NULL);					// '
	_kwarrStringQuote[2] = new EsKeyword(";", TKN_NULL, OP_NULL);					// "
	_kwarrStringQuote[3] = new EsKeyword(":", TKN_NULL, OP_NULL);					// '

	// ブロックコメント開始
	_numBlockCommentStart = 2;
	_kwarrBlockCommentStart = new EsKeyword*[_numBlockCommentStart];
	_kwarrBlockCommentStart[0] = new EsKeyword("/:", TKN_NULL, OP_NULL);
	_kwarrBlockCommentStart[1] = new EsKeyword("<!--", TKN_NULL, OP_NULL);

	// ブロックコメント終了
	_numBlockCommentEnd = 2;
	_kwarrBlockCommentEnd = new EsKeyword*[_numBlockCommentEnd];
	_kwarrBlockCommentEnd[0] = new EsKeyword(":/", TKN_NULL, OP_NULL);
	_kwarrBlockCommentEnd[1] = new EsKeyword("-->", TKN_NULL, OP_NULL);

	// 行コメント
	_numLineComment = 1;
	_kwarrLineComment = new EsKeyword*[_numLineComment];
	_kwarrLineComment[0] = new EsKeyword("//", TKN_NULL, OP_NULL);

	// 無限大値
	_kwInfinity = new EsKeyword("Infinity", TKN_NULL, OP_NULL);
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
