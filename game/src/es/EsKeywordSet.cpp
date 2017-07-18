/***********************************************************************//**
 *	EsKeywordSet.cpp
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
#include "EsKeywordSet.h"

// Friends
#include "EsAtom.h"
#include "EsString.h"

// External
#include "../tfw/collection/Array.h"
#include "../tfw/hash/HashTable.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsKeywordSet 定数

// 静的キーワード
const EsKeyword EsKeywordSet::_kwarrStatic[EsKeywordSet::NUM_S] =
{
	EsKeyword("Array"),					// S_CLASS_ARRAY
	EsKeyword("Block"),					// S_CLASS_BLOCK
	EsKeyword("Boolean"),				// S_CLASS_BOOLEAN
	EsKeyword("Call"),					// S_CLASS_CALL
	EsKeyword("Function"),				// S_CLASS_FUNCTION
	EsKeyword("global"),				// S_CLASS_GLOBAL
	EsKeyword("Iterator"),				// S_CLASS_ITERATOR
	EsKeyword("Math"),					// S_CLASS_MATH
	EsKeyword("Null"),					// S_CLASS_NULL
	EsKeyword("Number"),				// S_CLASS_NUMBER
	EsKeyword("Object"),				// S_CLASS_OBJECT
	EsKeyword("Script"),				// S_CLASS_SCRIPT
	EsKeyword("String"),				// S_CLASS_STRING
	EsKeyword("With"),					// S_CLASS_WITH
	EsKeyword("E"),						// S_CONST_E
	EsKeyword("Infinity"),				// S_CONST_INFINITY
	EsKeyword("LN10"),					// S_CONST_LN10
	EsKeyword("LN2"),					// S_CONST_LN2
	EsKeyword("LOG10E"),				// S_CONST_LOG10E
	EsKeyword("LOG2E"),					// S_CONST_LOG2E
	EsKeyword("MAX_VALUE"),				// S_CONST_MAX_VALUE
	EsKeyword("MIN_VALUE"),				// S_CONST_MIN_VALUE
	EsKeyword("NaN"),					// S_CONST_NAN
	EsKeyword("NEGATIVE_INFINITY"),		// S_CONST_NEGATIVE_INFINITY
	EsKeyword("PI"),					// S_CONST_PI
	EsKeyword("POSITIVE_INFINITY"),		// S_CONST_POSITIVE_INFINITY
	EsKeyword("SQRT1_2"),				// S_CONST_SQRT1_2
	EsKeyword("SQRT2"),					// S_CONST_SQRT2
	EsKeyword(""),						// S_EMPTY
	EsKeyword("abs"),					// S_PROP_ABS
	EsKeyword("acos"),					// S_PROP_ACOS
	EsKeyword("anonymous"),				// S_PROP_ANONYMOUS
	EsKeyword("apply"),					// S_PROP_APPLY
	EsKeyword("arguments"),				// S_PROP_ARGUMENTS
	EsKeyword("arity"),					// S_PROP_ARITY
	EsKeyword("asin"),					// S_PROP_ASIN
	EsKeyword("atan"),					// S_PROP_ATAN
	EsKeyword("atan2"),					// S_PROP_ATAN2
	EsKeyword("call"),					// S_PROP_CALL
	EsKeyword("callee"),				// S_PROP_CALLEE
	EsKeyword("caller"),				// S_PROP_CALLER
	EsKeyword("ceil"),					// S_PROP_CEIL
	EsKeyword("charAt"),				// S_PROP_CHARAT
	EsKeyword("charCodeAt"),			// S_PROP_CHARCODEAT
	EsKeyword("concat"),				// S_PROP_CONCAT
	EsKeyword("constructor"),			// S_PROP_CONSTRUCTOR
	EsKeyword("cos"),					// S_PROP_COS
	EsKeyword("__count__"),				// S_PROP_COUNT
	EsKeyword("eval"),					// S_PROP_EVAL
	EsKeyword("exp"),					// S_PROP_EXP
	EsKeyword("floor"),					// S_PROP_FLOOR
	EsKeyword("fromCharCode"),			// S_PROP_FROMCHARCODE
	EsKeyword("getPrototypeOf"),		// S_PROP_GETPROTOTYPEOF
	EsKeyword("hasOwnProperty"),		// S_PROP_HASOWNPROPERTY
	EsKeyword("indexOf"),				// S_PROP_INDEXOF
	EsKeyword("isPrototypeOf"),			// S_PROP_ISPROTOTYPEOF
	EsKeyword("isFinite"),				// S_PROP_ISFINITE
	EsKeyword("isNaN"),					// S_PROP_ISNAN
	EsKeyword("__iterator__"),			// S_PROP_ITERATOR
	EsKeyword("join"),					// S_PROP_JOIN
	EsKeyword("lastIndexOf"),			// S_PROP_LASTINDEXOF
	EsKeyword("length"),				// S_PROP_LENGTH
	EsKeyword("log"),					// S_PROP_LOG
	EsKeyword("match"),					// S_PROP_MATCH
	EsKeyword("max"),					// S_PROP_MAX
	EsKeyword("min"),					// S_PROP_MIN
	EsKeyword("name"),					// S_PROP_NAME
	EsKeyword("next"),					// S_PROP_NEXT
	EsKeyword("__parent__"),			// S_PROP_PARENT
	EsKeyword("parseFloat"),			// S_PROP_PARSEFLOAT
	EsKeyword("parseInt"),				// S_PROP_PARSEINT
	EsKeyword("pop"),					// S_PROP_POP
	EsKeyword("pow"),					// S_PROP_POW
	EsKeyword("propertyIsEnumerable"),	// S_PROP_PROPERTYISENUMERABLE
	EsKeyword("__proto__"),				// S_PROP_PROTO
	EsKeyword("prototype"),				// S_PROP_PROTOTYPE
	EsKeyword("push"),					// S_PROP_PUSH
	EsKeyword("random"),				// S_PROP_RANDOM
	EsKeyword("replace"),				// S_PROP_REPLACE
	EsKeyword("reverse"),				// S_PROP_REVERSE
	EsKeyword("round"),					// S_PROP_ROUND
	EsKeyword("search"),				// S_PROP_SEARCH
	EsKeyword("shift"),					// S_PROP_SHIFT
	EsKeyword("sin"),					// S_PROP_SIN
	EsKeyword("slice"),					// S_PROP_SLICE
	EsKeyword("sort"),					// S_PROP_SORT
	EsKeyword("splice"),				// S_PROP_SPLICE
	EsKeyword("split"),					// S_PROP_SPLIT
	EsKeyword("sqrt"),					// S_PROP_SQRT
	EsKeyword("substring"),				// S_PROP_SUBSTRING
	EsKeyword("tan"),					// S_PROP_TAN
	EsKeyword("toExponential"),			// S_PROP_TOEXPONENTIAL
	EsKeyword("toFixed"),				// S_PROP_TOFIXED
	EsKeyword("toLowerCase"),			// S_PROP_TOLOWERCASE
	EsKeyword("toPrecision"),			// S_PROP_TOPRECISION
	EsKeyword("toUpperCase"),			// S_PROP_TOUPPERCASE
	EsKeyword("toString"),				// S_PROP_TOSTRING
	EsKeyword("trim"),					// S_PROP_TRIM
	EsKeyword("trimLeft"),				// S_PROP_TRIMLEFT
	EsKeyword("trimRight"),				// S_PROP_TRIMRIGHT
	EsKeyword("unshift"),				// S_PROP_UNSHIFT
	EsKeyword("valueOf"),				// S_PROP_VALUEOF
	EsKeyword("null"),					// S_TYPE_NULL
	EsKeyword("undefined"),				// S_TYPE_UNDEFINED
	EsKeyword("object"),				// S_TYPE_OBJECT
	EsKeyword("function"),				// S_TYPE_FUNCTION
	EsKeyword("string"),				// S_TYPE_STRING
	EsKeyword("number"),				// S_TYPE_NUMBER
	EsKeyword("boolean"),				// S_TYPE_BOOLEAN
	EsKeyword("false"),					// S_VALUE_FALSE
	EsKeyword("true"),					// S_VALUE_TRUE
};

//==========================================================================
// EsKeywordSet メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	キーワード検索
**//*---------------------------------------------------------------------*/
const EsKeyword* EsKeywordSet::findWord(const VcString* name) const
{
	for(int i = 0; i < _numWord; i++)
	{
		if(name->equals(_kwarrWord[i]->getName()))
		{
			return _kwarrWord[i];
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	キーワード検索
**//*---------------------------------------------------------------------*/
const EsKeyword* EsKeywordSet::findWord(EsTokenType tt) const
{
	for(int i = 0; i < _numWord; i++)
	{
		if(_kwarrWord[i]->getTokenType() == tt)
		{
			return _kwarrWord[i];
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	キーワードのインデックス検索
**//*---------------------------------------------------------------------*/
s32 EsKeywordSet::findWordIndex(const VcString* name) const
{
	for(int i = 0; i < _numWord; i++)
	{
		if(name->equals(_kwarrWord[i]->getName()))
		{
			return i;
		}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	記号検索
**//*---------------------------------------------------------------------*/
const EsKeyword* EsKeywordSet::findSign(const VcString* name) const
{
	for(int i = 0; i < _numSign; i++)
	{
		if(name->equals(_kwarrSign[i]->getName()))
		{
			return _kwarrSign[i];
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	記号のインデックス検索
**//*---------------------------------------------------------------------*/
s32 EsKeywordSet::findSignIndex(const VcString* name) const
{
	for(int i = 0; i < _numSign; i++)
	{
		if(name->equals(_kwarrSign[i]->getName()))
		{
			return i;
		}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	文字列引用符検索
**//*---------------------------------------------------------------------*/
const EsKeyword* EsKeywordSet::findStringQuote(const VcString* name) const
{
	for(int i = 0; i < _numStringQuote; i++)
	{
		if(name->equals(_kwarrStringQuote[i]->getName()))
		{
			return _kwarrStringQuote[i];
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	アトムを検索する
**//*---------------------------------------------------------------------*/
const EsAtom* EsKeywordSet::findAtom(const VcString* str)
{
	// 静的キーワード検索
	for(int i = 0; i < NUM_S; i++)
	{
		if(_kwarrStatic[i].getAtom()->isSameString(str))
		{
			return _kwarrStatic[i].getAtom();
		}
	}

	// ハッシュテーブル検索
	HashTable* htbl = getHashTableInstance();
	EsAtom::EsAtomHashEntry* entity = (EsAtom::EsAtomHashEntry*)htbl->find(str);
	if(entity == 0L)
	{
		return 0L;
	}
	return entity->getAtom();
}

/*---------------------------------------------------------------------*//**
	アトムの登録
**//*---------------------------------------------------------------------*/
void EsKeywordSet::registerAtom(const EsAtom* atom)
{
	// ハッシュテーブルに追加する
	const VcString* str = atom->getString();
	HashTable* htbl = getHashTableInstance();
	EsAtom::EsAtomHashEntry* entity = (EsAtom::EsAtomHashEntry*)htbl->add(str);
	if(entity == 0L)
	{
		ASSERT(false);
		return;
	}
	entity->setAtom(atom);
}

/*---------------------------------------------------------------------*//**
	アトムの新規作成
**//*---------------------------------------------------------------------*/
const EsAtom* EsKeywordSet::newRegisterdAtom(const char* keyword, bool isSearchRegisterdAtom)
{
	// アトム静的配列を作成する
	static Array<const EsAtom*> s_arrAtoms(true, true);

	if(isSearchRegisterdAtom)
	{
		VcString str(keyword);
		const EsAtom* atom = findAtom(&str);
		if(atom != 0L)
		{
			return atom;
		}
	}

	const EsAtom* atom = new EsAtom(new VcString(keyword), EsAtom::F_DEL_DATA);
	registerAtom(atom);
	s_arrAtoms.add(atom);
	return atom;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsKeywordSet::EsKeywordSet()
	: _kwarrWord(0L)
	, _numWord(0)
	, _kwarrSign(0L)
	, _numSign(0)
	, _kwarrStringQuote(0L)
	, _numStringQuote(0)
	, _kwarrBlockCommentStart(0L)
	, _numBlockCommentStart(0)
	, _kwarrBlockCommentEnd(0L)
	, _numBlockCommentEnd(0)
	, _kwarrLineComment(0L)
	, _numLineComment(0)
	, _kwInfinity(0L)
{
	for(int i = 0; i < NUM_LTYPE; i++)	{	_kwarrLogicalType[i] = 0L;	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsKeywordSet::~EsKeywordSet()
{
	for(int i = 0; i < _numWord; i++)				{	delete _kwarrWord[i];				}
	delete[] _kwarrWord;
	for(int i = 0; i < _numSign; i++)				{	delete _kwarrSign[i];				}
	delete[] _kwarrSign;
	for(int i = 0; i < _numStringQuote; i++)		{	delete _kwarrStringQuote[i];		}
	delete[] _kwarrStringQuote;
	for(int i = 0; i < _numBlockCommentStart; i++)	{	delete _kwarrBlockCommentStart[i];	}
	delete[] _kwarrBlockCommentStart;
	for(int i = 0; i < _numBlockCommentEnd; i++)	{	delete _kwarrBlockCommentEnd[i];	}
	delete[] _kwarrBlockCommentEnd;
	for(int i = 0; i < _numLineComment; i++)		{	delete _kwarrLineComment[i];		}
	delete[] _kwarrLineComment;
	delete _kwInfinity;
}

/*---------------------------------------------------------------------*//**
	ハッシュテーブルを取得する
**//*---------------------------------------------------------------------*/
HashTable* EsKeywordSet::getHashTableInstance()
{
	// ハッシュ静的テーブルを作成する
	static HashTable s_htblAtoms(new EsString::EsStringRefAtomHashOp(), 256);

	// 静的キーワードをハッシュテーブルに登録する
	static bool isFirst = true;
	if(isFirst)
	{
		isFirst = false;
		for(int i = 0; i < NUM_S; i++)
		{
			registerAtom(_kwarrStatic[i].getAtom());
		}
	}

	return &s_htblAtoms;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
