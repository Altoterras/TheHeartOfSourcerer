/***********************************************************************//**
 *	EsToken.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_TOKEN_H_
#define _ES_ES_TOKEN_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsTokenPos.h"
#include "EsOpcode.h"

////////////////////////////////////////////////////////////////////////////
// 定数

ES_BEGIN_NS

enum EsTokenType
{
	TKN_ERROR = -1,
	TKN_NULL	= 0,
	TKN_EOF,					// ファイル終端
	TKN_EOL,					// 行終端
	TKN_SEMI,					// ;
	TKN_COMMA,					// ,
	TKN_ASSIGN,					// 代入演算子 = += -= 等
	TKN_UNARYOP,				// 単項演算子 ! ^ void typedef 等
	TKN_INC,					// 単項算術演算子 ++
	TKN_DEC,					// 単項算術演算子 --
	TKN_PLUS,					// 二項算術演算子 +
	TKN_MINUS,					// 二項算術演算子 -
	TKN_STAR,					// 二項算術演算子 *
	TKN_DIVOP,					// 二項算術演算子 / %
	TKN_EQOP,					// 二項論理演算子 == !=
	TKN_INQOP,					// 二項論理演算子 < <= > >=	⇒ TOK_RELOP
	TKN_OR,						// 二項論理演算子 ||
	TKN_AND,					// 二項論理演算子 &&
	TKN_BITOR,					// 二項ビット演算子 |
	TKN_BITXOR,					// 二項ビット演算子 ^
	TKN_BITAND,					// 二項ビット演算子 &
	TKN_SFTOP,					// 二項ビットシフト演算子 << >> >>>	⇒ TOK_SHOP
	TKN_HOOK,					// 三項論理演算子 ?
	TKN_COLON,					// 三項論理演算子 :
	TKN_DOT,					// ドット演算子 .
	TKN_LB,						// [
	TKN_RB,						// ]
	TKN_LC,						// {
	TKN_RC,						// }
	TKN_LP,						// (
	TKN_RP,						// )
	TKN_NAME,					// 識別子
	TKN_NUMBER,					// 定数
	TKN_STRING,					// 文字列
	TKN_REGEXP,					// 正規表現定数
	TKN_PRIMARY,				// true, false, null, this, super
	TKN_FUNCTION,				// function
	TKN_IF,						// if
	TKN_ELSE,					// else
	TKN_SWITCH,					// switch
	TKN_CASE,					// case
	TKN_DEFAULT,				// default
	TKN_WHILE,					// while
	TKN_DO,						// do
	TKN_FOR,					// for
	TKN_BREAK,					// break
	TKN_CONTINUE,				// continue
	TKN_IN,						// in
	TKN_VAR,					// var
	TKN_WITH,					// with
	TKN_RETURN,					// return
	TKN_NEW,					// new
	TKN_DELETE,					// delete
	TKN_DEFSHARP,				// オブジェクト／配列初期化構文 #n=
	TKN_USESHARP,				// オブジェクト／配列初期化構文 #n#
	TKN_TRY,					// try
	TKN_CATCH,					// catch
	TKN_FINALLY,				// finally
	TKN_THROW,					// throw
	TKN_INSTANCEOF,				// instanceof
	TKN_DEBUGGER,				// debugger
	TKN_DBLCOLON,				// ネームスペース指定 ::
	TKN_XMLSTAGO,				// XML 開始タグオープン <
	TKN_XMLETAGO,				// XML 終了タグクローズ </
	TKN_XMLPTAGC,				// XML 空要素タグクローズ />
	TKN_XMLTAGC,				// XML タグクローズ >
	TKN_XMLNAME,				// XML タグ名
	TKN_XMLATTR,				// XML 属性値
	TKN_XMLSPACE,				// XML 空白文字
	TKN_XMLTEXT,				// XML テキスト
	TKN_XMLCOMMENT,				// XML コメント
	TKN_XMLCDATA,				// XML CDATA セクション
	TKN_XMLPI,					// XML Processing-Instruction
	TKN_AT,						// XML 属性演算子 @
	TKN_ANYNAME,				// XML ワイルドカード *
	TKN_DBLDOT,					// XML 後続要素演算子 ..
	TKN_FILTER,					// XML フィルタリング演算子 .()
	TKN_XMLELEM,				// XML 要素ノードタイプ（トークンではない）
	TKN_XMLLIST,				// XML リストノードタイプ（トークンではない）
	TKN_YIELD,					// yield from generator function
	TKN_ARRAYCOMP,				// array comprehension initialiser
	TKN_ARRAYPUSH,				// array push within comprehension
	TKN_LEXICALSCOPE,			// block scope AST node label
	TKN_LET,					// let
	TKN_SEQ,					// synthetic sequence of statements, not a block
	TKN_FORHEAD,				// for(;;) ループ
	TKN_ARGSBODY,				// formal args in list + body at end
	TKN_UPVARS,					// lexical dependencies as JSAtomList of definitions paired with a parse tree full of uses of those names
	TKN_RESERVED,				// reserved
	NUM_TKN
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsAtom;

/*---------------------------------------------------------------------*//**
 *	トークン
 *
**//*---------------------------------------------------------------------*/
class EsToken
{
	//======================================================================
	// 定数
private:
	enum Format
	{
		FMT_NULL,
		FMT_NS,
		FMT_DOUBLE,
	};

	//======================================================================
	// 変数
private:
	EsTokenType	_type;
	EsTokenPos	_pos;
	u8			_fmt;
	union
	{
		struct NameOrString
		{
			EsOpcodeType	_ot;
			const EsAtom*	_atom;
		} _ns;
		u64				_u;
		double			_d;
	} _v;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	EsTokenType			getType() const							{	return _type;		}
	const EsTokenPos*	getPos() const							{	return &_pos;		}
	void				setType(EsTokenType type)				{	_type = type;		}
	void				setPos(const EsTokenPos* pos)			{	_pos = *pos;		}

	const double*		getDoubleValue() const;
	void				setDoubleValue(double d);

	const EsOpcodeType	getOpcodeTypeValue() const;
	void				setOpcodeTypeValue(EsOpcodeType ot);

	const EsAtom*		getAtomValue() const;
	void				setAtomValue(const EsAtom* atom);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	EsToken();
	void				copy(const EsToken* src);
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_TOKEN_H_
