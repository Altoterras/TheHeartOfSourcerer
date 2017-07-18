/***********************************************************************//**
 *	EsError.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_ERROR_H_
#define _ES_ES_ERROR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsTokenPos.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class VcStringArray;

TFW_END_NS

ES_BEGIN_NS

class EsContext;

/*---------------------------------------------------------------------*//**
 *	エラー処理
 *
**//*---------------------------------------------------------------------*/
class EsError
{
	//======================================================================
	// 定数
public:
	enum Id
	{
		ID_NULL = 0,
		ID_OK = 1,

		// コンパイルエラー ID
		COMPILE_ERROR_ID__START_ = 10000,
		ID_RESERVED_NAME,					// 予約語を使用した
		ID_MISSING_EXPONENT,				// 不正な指数
		ID_UNTERMINATED_STRING,				// 文字列が終端されていない
		ID_UNTERMINATED_COMMENT,			// コメントが終端されていない
		ID_ILLEGAL_CHARACTER,				// 不正な文字
		ID_BAD_LABEL,						// 不正なラベル
		ID_STMT_AFTER_SEMICOLON,			// ステートメントの後にセミコロンが無い
		ID_BAD_LEFTSIDE_OF_ASSIGN,			// 不正な左辺代入
		ID_BAD_OPERAND,						// 不正な演算子
		ID_NAME_AFTER_DOT,					// ドット演算子の後に名前が無い
		ID_PARENTHESIS_FUNC_LEFT,			// function( ... ) の左括弧が無い ⇒ JSMSG_PAREN_BEFORE_FORMAL
		ID_PARENTHESIS_FUNC_RIGHT,			// function( ... ) の右括弧が無い ⇒ JSMSG_PAREN_AFTER_FORMAL
		ID_PARENTHESIS_ARGS_LEFT,			// ( 引数 ) の左括弧が無い
		ID_PARENTHESIS_ARGS_RIGHT,			// ( 引数 ) の右括弧が無い ⇒ JSMSG_PAREN_AFTER_ARGS
		ID_PARENTHESIS_CONDITION_LEFT,		// ( 条件文 ) の左括弧が無い
		ID_PARENTHESIS_CONDITION_RIGHT,		// ( 条件文 ) の右括弧が無い
		ID_PARENTHESIS_SWITCH_LEFT,			// switch の後に ( が無い ⇒ JSMSG_PAREN_BEFORE_SWITCH
		ID_PARENTHESIS_SWITCH_RIGHT,		// switch( ... ) の右括弧が無い ⇒ JSMSG_PAREN_AFTER_SWITCH
		ID_PARENTHESIS_IN_PARENTHESIS,		// ( ) の右括弧が無い ⇒ JSMSG_PAREN_IN_PAREN
		ID_PARENTHESIS_FOR_LEFT,			// for の後に ( が無い ⇒ JSMSG_PAREN_AFTER_FOR
		ID_PARENTHESIS_FOR_RIGHT,			// for( ... ) の右括弧が無い ⇒ JSMSG_PAREN_AFTER_FOR_CTRL
		ID_PARENTHESIS_WITH_LEFT,			// with の後に ( が無い ⇒ JSMSG_PAREN_BEFORE_WITH
		ID_PARENTHESIS_WITH_RIGHT,			// with( ... ) の右括弧が無い ⇒ JSMSG_PAREN_AFTER_WITH
		ID_PARENTHESIS_LET_LEFT,			// let( ... ) の左括弧が無い
		ID_PARENTHESIS_LET_RIGHT,			// let( ... ) の右括弧が無い
		ID_BRANCKET_BLOCK_RIGHT,			// { ブロック } の右大括弧が無い
		ID_BRANCKET_LIST_RIGHT,				// [ リスト ] の右カギ括弧が無い
		ID_CURLY_BEFORE_BODY,				// 本体定義の前に { が来なかった
		ID_CURLY_AFTER_BODY,				// 本体定義の後に } が来なかった
		ID_CURLY_AFTER_LET,					// let の後に } が来なかった
		ID_CURLY_SWITCH_LEFT,				// switch の後に { が来なかった ⇒ JSMSG_CURLY_BEFORE_SWITCH
		ID_NO_VARIABLE_NAME,				// 変数名が無い
		ID_NO_FUNCTION_NAME,				// 関数名が無い
		ID_REDECLARED_VAR,					// 変数が再定義された ⇒ JSMSG_REDECLARED_VAR
		ID_REDECLARED_PARAM,				// パラメータが再定義された
		ID_VAR_HIDES_ARG,					// 定義した変数が引数を隠してしまう
		ID_DUPLICATE_LOCAL_VAR,				// 局所変数が再定義された
		ID_MISSING_FORMAL,					// 不明な引数リスト
		ID_TOO_MANY_LITERALS,				// リテラルが多すぎる
		ID_TOO_MANY_SCORE_ENTRIES,			// スコープにプロパティエントリーが多すぎる
		ID_BAD_VAR_INIT,					// 変数の初期化に失敗した
		ID_TRAILING_COMMA,					// 末尾のカンマが不正（カンマが最後で終わった）
		ID_BAD_PROP_ID,						// プロパティの ID が不正
		ID_COLON_AFTER_ID,					// ID の後はコロンである必要がある
		ID_CURLY_AFTER_LIST,				// リスト内の構文が不正
		ID_BAD_RETURN,						// 不正な return ⇒ JSMSG_BAD_RETURN_OR_YIELD
		ID_BAD_YIELD,						// 不正な yield
		ID_WHILE_AFTER_DO,					// do の後に while がくるはずが来なかった
		ID_BAD_FORIN_LEFTSIDE,				// for in の左辺が不正 ⇒ JSMSG_BAD_FOR_LEFTSIDE
		ID_SEMI_AFTER_FOR_INIT,				// for の変数初期式の後にセミコロンが無い
		ID_LET_DECL_NOT_IN_BLOCK,			// let 宣言がブロック内ではない場所で行われた ⇒ JSMSG_LET_DECL_NOT_IN_BLOCK
		ID_MULTIPLE_SWITCH_DEFAULT,			// switch に default が複数あった ⇒ JSMSG_TOO_MANY_DEFAULTS
		ID_BAD_SWITCH_SYNTAX,				// 不正な switch 構文
		ID_COLON_AFTER_CASE,				// case の後に : が来ない ⇒ JSMSG_COLON_AFTER_CASE
		ID_LABEL_NOT_FOUND,					// ラベルが見つからなかった ⇒ JSMSG_LABEL_NOT_FOUND
		ID_BAD_BREAK,						// break できないところで break した ⇒ JSMSG_TOUGH_BREAK
		ID_BAD_CONTINUE,					// continue できないところで continue した ⇒ JSMSG_BAD_CONTINUE
		ID_DUPLICATE_LABEL,					// ラベルが重複している ⇒ JSMSG_DUPLICATE_LABEL
		ID_STACK_UNDERFLOW,					// スタックアンダーフロー ⇒ JSMSG_STACK_UNDERFLOW
		ID_STACK_OVERFLOW,					// スタックオーバーフロー ⇒ JSMSG_STACK_OVERFLOW
		ID_TOO_MANY_LOCALS,					// ローカル変数が多すぎる ⇒ JSMSG_TOO_MANY_LOCALS
		ID_TOO_MANY_ARGC,					// 関数引数が多すぎる ⇒ JSMSG_TOO_MANY_CON_ARGS
		ID_TOO_MANY_ARRAY_INIT,				// 配列初期要素数が多すぎる ⇒ JSMSG_ARRAY_INIT_TOO_BIG
		ID_UNDEFINED_PROP,					// プロパティが定義されていない ⇒ JSMSG_UNDEFINED_PROP
		ID_USELESS_EXPRESSION,				// 使用されない無駄な式 ⇒ JSMSG_USELESS_EXPR
		ID_BAD_BRACKET_IN_INDEX,			// 不正なインデックスブランケット[] ⇒ JSMSG_BRACKET_IN_INDEX
		ID_BAD_COLON_IN_HOOKCOND,			// 三項条件式（xxx ? yyy : zzz）のコロンが不正 ⇒ JSMSG_COLON_IN_COND
		ID_BAD_GENERATOR_RETURN,			// 生成返答値が不正 ⇒ JSMSG_BAD_GENERATOR_RETURN
		ID_BAD_FORMAL,						// 不正ない形式 ⇒ JSMSG_BAD_FORMAL
		ID_SYNTAX_ERROR,					// 構文エラー ⇒ JSMSG_SYNTAX_ERROR
		COMPILE_ERROR_ID__END_,

		// 警告 ID
		COMPILE_WARNING_ID__START_ = 20000,
		ID_BAD_OCTAL,						// 不正な８進数 ⇒ JSMSG_BAD_OCTAL
		ID_DUPLICATE_PARAMETER,				// 仮引数が二重定義されている ⇒ JSMSG_DUPLICATE_FORMAL
		COMPILE_WARNING_ID__END_,

		// 実行エラー ID
		RUN_ERROR_ID__START_ = 30000,
		ID_SYSTEM_ERROR,					// システムエラー
		ID_NO_PROPERTY,						// プロパティが見つからない
		ID_OUT_OF_MEMORY,					// メモリオーバー
		ID_BAD_MEMORY_ACCESS,				// 不正なメモリアクセス
		ID_ERROR_READ_ONLY,					// 読み込み専用に対して書き込みをした ⇒ JSMSG_READ_ONLY
		ID_NULL_OR_UNDEFINED,				// null もしくは undefined ⇒ JSMSG_NO_PROPERTIES
		ID_NOT_CONSTRUCTOR,					// コンストラクタがない ⇒ JSMSG_NO_CONSTRUCTOR
		ID_NOT_FUNCTION,					// 関数がない ⇒ JSMSG_NOT_FUNCTION
		ID_BAD_ITERATOR,					// 不正なイテレータ ⇒ JSMSG_BAD_ITERATOR
		ID_BAD_PRIM_TYPE,					// プリミティブ型が不正
		ID_BAD_OBJ_TYPE,					// オブジェクト型が不正
		ID_IN_NOT_OBJECT,					// in の先がオブジェクトでない ⇒ JSMSG_IN_NOT_OBJECT
		ID_BAD_ITERATOR_RETURN,				// 不正なイテレータ返答値 ⇒ JSMSG_BAD_ITERATOR_RETURN
		ID_BAD_ARRAY_LENGTH,				// 不正な配列長さ ⇒ JSMSG_BAD_ARRAY_LENGTH
		ID_CANT_CONVERT_TO,					// 変換できない ⇒ JSMSG_CANT_CONVERT_TO
		ID_BAD_NEW_RESULT,					// 不正な新しい返答 ⇒ JSMSG_BAD_NEW_RESULT
		ID_BAD_INSTANCEOF_RHS,				// 不正な instanceof 右辺値 ⇒ JSMSG_BAD_INSTANCEOF_RHS
		ID_INCOMPATIBLE_PROTO,				// 非互換のプロトタイプ ⇒ JSMSG_INCOMPATIBLE_PROTO
		ID_DEPRECATED_USAGE,				// 非推奨の使用方法 ⇒ JSMSG_DEPRECATED_USAGE
		ID_BAD_APPLY_ARGS,					// 不正な apply メソッドの引数 ⇒ JSMSG_BAD_APPLY_ARGS
		ID_MISSING_FUN_ARG,					// 関数引数が欠落している ⇒ JSMSG_MISSING_FUN_ARG
		ID_BAD_RADIX,						// 不正な基数 ⇒ JSMSG_BAD_RADIX
		ID_INVALID_PRECISION_RANGE,			// 不正な精度範囲 ⇒ JSMSG_PRECISION_RANGE
		ID_BAD_SORT_ARG,					// 不正なソート引数 ⇒ JSMSG_BAD_SORT_ARG
		ID_BAD_INDIRECT_CALL,				// 不正な間接コール ⇒ JSMSG_BAD_INDIRECT_CALL
		ID_LEAK_OPTIMIZED_CLOSURE,			// 不正な最適化クロージャ ⇒ JSMSG_OPTIMIZED_CLOSURE_LEAK
		RUN_ERROR_ID__END_,

		// アプリケーションエラー ID
		APP_ERROR_ID__START_ = 30000,
	};

	//======================================================================
	// クラス
public:
	class Report
	{
	public:
		static const u8	F_WARNING	= 0x1;
		static const u8	F_STRICT	= 0x2;

	public:
		Id				_id;
		EsTokenPos		_pos;
		VcStringArray*	_saParam;
		u8				_flags;

	public:
		Report(Id id, EsTokenPos pos, const VcStringArray* saParam, u8 flags);
		~Report();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// コンパイルエラーリストを得る
	const List<Report*>* getCompileErrorList() const { return _replistCompile; }
	// 実行時エラーリストを得る
	const List<Report*>* getRunErrorList() const { return _replistRun; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	EsError();
	~EsError();
	void clear();
	void addCompileError(Id id, EsTokenPos pos, const VcStringArray* saParam);
	void addRunError(EsContext* ctx, Id id, bool isWarning, bool isStrict);

private:

	//======================================================================
	// 変数
private:
	List<Report*>* _replistCompile;
	List<Report*>* _replistRun;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_ERROR_H_
