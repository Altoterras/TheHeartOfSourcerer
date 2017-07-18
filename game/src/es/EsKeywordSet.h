/***********************************************************************//**
 *	EsKeywordSet.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_KEYWORD_SET_H_
#define _ES_ES_KEYWORD_SET_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsKeyword.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class HashTable;

TFW_END_NS

ES_BEGIN_NS

class EsAtom;
class EsKeywordSetStaticInstance;

/*---------------------------------------------------------------------*//**
 *	スクリプト キーワード セット
 *
**//*---------------------------------------------------------------------*/
class EsKeywordSet
{
	//======================================================================
	// 定数
public:
	// 静的キーワード
	enum StaticKeyword
	{
		S_CLASS_ARRAY,
		S_CLASS_BLOCK,
		S_CLASS_BOOLEAN,
		S_CLASS_CALL,
		S_CLASS_FUNCTION,
		S_CLASS_GLOBAL,
		S_CLASS_ITERATOR,
		S_CLASS_MATH,
		S_CLASS_NULL,
		S_CLASS_NUMBER,
		S_CLASS_OBJECT,
		S_CLASS_SCRIPT,
		S_CLASS_STRING,
		S_CLASS_WITH,
		S_CONST_E,
		S_CONST_INFINITY,
		S_CONST_LN10,
		S_CONST_LN2,
		S_CONST_LOG10E,
		S_CONST_LOG2E,
		S_CONST_MAX_VALUE,
		S_CONST_MIN_VALUE,
		S_CONST_NAN,
		S_CONST_NEGATIVE_INFINITY,
		S_CONST_PI,
		S_CONST_POSITIVE_INFINITY,
		S_CONST_SQRT1_2,
		S_CONST_SQRT2,
		S_EMPTY,						// 空文字 ⇒ cx->runtime->atomState.emptyAtom
		S_PROP_ABS,
		S_PROP_ACOS,
		S_PROP_ANONYMOUS,				// anonymous ⇒ cx->runtime->atomState.anonymousAtom
		S_PROP_APPLY,					// apply ⇒ cx->runtime->atomState.applyAtom
		S_PROP_ARGUMENTS,				// arguments ⇒ cx->runtime->atomState.argumentsAto
		S_PROP_ARITY,					// arity ⇒ cx->runtime->atomState.arityAtom
		S_PROP_ASIN,
		S_PROP_ATAN,
		S_PROP_ATAN2,
		S_PROP_CALL,					// call ⇒ cx->runtime->atomState.callAtom
		S_PROP_CALLEE,					// callee ⇒ cx->runtime->atomState.calleeAtom
		S_PROP_CALLER,					// callee ⇒ cx->runtime->atomState.callerAtom
		S_PROP_CEIL,
		S_PROP_CHARAT,
		S_PROP_CHARCODEAT,
		S_PROP_CONCAT,
		S_PROP_CONSTRUCTOR,				// constructor ⇒ cx->runtime->atomState.constructorAtom
		S_PROP_COS,
		S_PROP_COUNT,					// __count__ ※MozillaJs の特殊プロパティのため基本的にサポートしない（ただし、内部処理用に使用する） ⇒ cx->runtime->atomState.countAtom
		S_PROP_EVAL,					// eval ⇒ cx->runtime->atomState.evalAtom
		S_PROP_EXP,
		S_PROP_FLOOR,
		S_PROP_FROMCHARCODE,
		S_PROP_GETPROTOTYPEOF,
		S_PROP_HASOWNPROPERTY,
		S_PROP_INDEXOF,
		S_PROP_ISPROTOTYPEOF,
		S_PROP_ISFINITE,
		S_PROP_ISNAN,
		S_PROP_ITERATOR,				// __iterator__ ※MozillaJs の特殊プロパティのためサポートしない（ただし、内部処理用に使用する） ⇒ cx->runtime->atomState.iteratorAtom
		S_PROP_JOIN,
		S_PROP_LASTINDEXOF,
		S_PROP_LENGTH,					// length ⇒ cx->runtime->atomState.lengthAtom
		S_PROP_LOG,
		S_PROP_MATCH,
		S_PROP_MAX,
		S_PROP_MIN,
		S_PROP_NAME,					// next ⇒ cx->runtime->atomState.nameAtom
		S_PROP_NEXT,					// next ⇒ cx->runtime->atomState.nextAtom
		S_PROP_PARENT,					// __parent__ ※MozillaJs の特殊プロパティのためサポートしない（ただし、内部処理用に使用する） ⇒ cx->runtime->atomState.parentAtom
		S_PROP_PARSEFLOAT,
		S_PROP_PARSEINT,
		S_PROP_POP,
		S_PROP_POW,
		S_PROP_PROPERTYISENUMERABLE,
		S_PROP_PROTO,					// __proto__ ※MozillaJs の特殊プロパティのためサポートしない（ただし、内部処理用に使用する） ⇒ cx->runtime->atomState.protoAtom
		S_PROP_PROTOTYPE,				// prototype ⇒ cx->runtime->atomState.classPrototypeAtom
		S_PROP_PUSH,
		S_PROP_RANDOM,
		S_PROP_REPLACE,
		S_PROP_REVERSE,
		S_PROP_ROUND,
		S_PROP_SEARCH,
		S_PROP_SHIFT,
		S_PROP_SIN,
		S_PROP_SLICE,
		S_PROP_SORT,
		S_PROP_SPLICE,
		S_PROP_SPLIT,
		S_PROP_SQRT,
		S_PROP_SUBSTRING,
		S_PROP_TAN,
		S_PROP_TOEXPONENTIAL,
		S_PROP_TOFIXED,
		S_PROP_TOLOWERCASE,
		S_PROP_TOPRECISION,
		S_PROP_TOUPPERCASE,
		S_PROP_TOSTRING,				// toString ⇒ cx->runtime->atomState.toStringAtom
		S_PROP_TRIM,
		S_PROP_TRIMLEFT,
		S_PROP_TRIMRIGHT,
		S_PROP_UNSHIFT,
		S_PROP_VALUEOF,					// valueOf ⇒ js_valueOf_str
		S_TYPE_NULL,
		S_TYPE_UNDEFINED,
		S_TYPE_OBJECT,
		S_TYPE_FUNCTION,
		S_TYPE_STRING,
		S_TYPE_NUMBER,
		S_TYPE_BOOLEAN,
		S_VALUE_FALSE,
		S_VALUE_TRUE,
		NUM_S
	};

	// 静的単語キーワード
	// ※各言語でこれ以外の（超える個数の）独自単語キーワードを定義してもよいとする
	enum StaticWordKeyword
	{
		S_WORD_BREAK,
		S_WORD_CASE,
		S_WORD_CONTINUE,
		S_WORD_DEFAULT,
		S_WORD_DELETE,
		S_WORD_DO,
		S_WORD_ELSE,
		S_WORD_RESERVED,
		S_WORD_FALSE,
		S_WORD_FOR,
		S_WORD_FUNCTION,
		S_WORD_IF,
		S_WORD_IN,
		S_WORD_NEW,
		S_WORD_NULL,
		S_WORD_RETURN,
		S_WORD_SWITCH,
		S_WORD_THIS,
		S_WORD_TRUE,
		S_WORD_TYPEOF,
		S_WORD_VAR,
		S_WORD_VOID,
		S_WORD_WHILE,
		S_WORD_WITH,
		S_WORD_TRY,
		S_WORD_CATCH,
		S_WORD_FINALLY,
		S_WORD_THROW,
		S_WORD_INSTANCEOF,
		NUM_S_WORD
	};

	// 静的記号キーワード
	// ※各言語でこれ以外の（超える個数の）独自記号キーワードを定義してもよいとする
	enum StaticSignKeyword
	{
		S_SIGN_SEMI,		// ;
		S_SIGN_LB,			// [
		S_SIGN_RB,			// ]
		S_SIGN_LC,			// {
		S_SIGN_RC,			// }
		S_SIGN_LP,			// (
		S_SIGN_RP,			// )
		S_SIGN_COMMA,		// ,
		S_SIGN_HOOK,		// ?
		S_SIGN_DOT,			// .
		S_SIGN_COLON,		// :
		S_SIGN_OR,			// ||
		S_SIGN_BITOREQ,		// |=
		S_SIGN_BITOR,		// |
		S_SIGN_BITXOREQ,	// ^=
		S_SIGN_BITXOR,		// ^
		S_SIGN_AND,			// &&
		S_SIGN_BITANDEQ,	// &=
		S_SIGN_BITAND,		// &
		S_SIGN_STRICTEQ,	// ===
		S_SIGN_EQ,			// ==
		S_SIGN_ASSIGN,		// =
		S_SIGN_STRICTNE,	// !==
		S_SIGN_NE,			// !=
		S_SIGN_UNARYOP,		// !
		S_SIGN_RESERVED_01,
		S_SIGN_LSFT,		// <<=
		S_SIGN_SFTOP,		// <<
		S_SIGN_LE,			// <=
		S_SIGN_LT,			// <
		S_SIGN_URSFTEQ,		// >>>=
		S_SIGN_URSFT,		// >>>
		S_SIGN_RSFTEQ,		// >>=
		S_SIGN_RSFT,		// >>
		S_SIGN_GE,			// >=
		S_SIGN_GT,			// >
		S_SIGN_MULEQ,		// *=
		S_SIGN_STAR,		// *
		S_SIGN_RESERVED_02,
		S_SIGN_RESERVED_03,
		S_SIGN_DIVEQ,		// /=
		S_SIGN_DIV,			// /
		S_SIGN_MODEQ,		// %=
		S_SIGN_MOD,			// %
		S_SIGN_BITNOT,		// ~
		S_SIGN_ADDEQ,		// +=
		S_SIGN_INC,			// ++
		S_SIGN_PLUS,		// +
		S_SIGN_RESERVED_04,
		S_SIGN_SUBEQ,		// -=
		S_SIGN_DEC,			// --
		S_SIGN_MINUS,		// -
		NUM_S_SIGN
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline s32 getWordNum() const										{	return _numWord;						}
	inline const EsKeyword* getWord(s32 idx) const						{	return _kwarrWord[idx];					}
	inline const EsKeyword* getLogicalType(EsLogicalType ltype) const	{	return _kwarrLogicalType[ltype];		}
	inline s32 getSignNum() const										{	return _numSign;						}
	inline const EsKeyword* getSign(s32 idx) const						{	return _kwarrSign[idx];					}
	inline s32 getStringQuoteNum() const								{	return _numStringQuote;					}
	inline const EsKeyword* getStringQuote(s32 idx) const				{	return _kwarrStringQuote[idx];			}
	inline s32 getBlockCommentStartNum() const							{	return _numBlockCommentStart;			}
	inline const EsKeyword* getBlockCommentStart(s32 idx) const			{	return _kwarrBlockCommentStart[idx];	}
	inline s32 getBlockCommentEndNum() const							{	return _numBlockCommentEnd;				}
	inline const EsKeyword* getBlockCommentEnd(s32 idx) const			{	return _kwarrBlockCommentEnd[idx];		}
	inline s32 getLineCommentNum() const								{	return _numLineComment;					}
	inline const EsKeyword* getLineComment(s32 idx) const				{	return _kwarrLineComment[idx];			}
	inline const EsKeyword* getInfinity() const							{	return _kwInfinity;						}
	inline static const EsKeyword* getStatic(StaticKeyword sk)			{	return &_kwarrStatic[sk];				}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual const EsKeyword* findWord(const VcString* name) const;
	virtual const EsKeyword* findWord(EsTokenType tt) const;
	virtual s32 findWordIndex(const VcString* name) const;
	virtual const EsKeyword* findSign(const VcString* name) const;
	virtual s32 findSignIndex(const VcString* name) const;
	virtual const EsKeyword* findStringQuote(const VcString* name) const;
	static const EsAtom* findAtom(const VcString* str);
	static void registerAtom(const EsAtom* atom);
	static const EsAtom* newRegisterdAtom(const char* keyword, bool isSearchRegisterdAtom);
	static inline const EsAtom* newRegisterdAtom(const char* keyword) { return newRegisterdAtom(keyword, false); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsKeywordSet();
	virtual ~EsKeywordSet();

private:
	static HashTable* getHashTableInstance();

	//======================================================================
	// 変数
protected:
	EsKeyword**					_kwarrWord;
	s32							_numWord;
	const EsKeyword*			_kwarrLogicalType[NUM_LTYPE];
	EsKeyword**					_kwarrSign;
	s32							_numSign;
	EsKeyword**					_kwarrStringQuote;
	s32							_numStringQuote;
	EsKeyword**					_kwarrBlockCommentStart;
	s32							_numBlockCommentStart;
	EsKeyword**					_kwarrBlockCommentEnd;
	s32							_numBlockCommentEnd;
	EsKeyword**					_kwarrLineComment;
	s32							_numLineComment;
	EsKeyword*					_kwInfinity;
	static const EsKeyword		_kwarrStatic[NUM_S];
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_KEYWORD_SET_H_
