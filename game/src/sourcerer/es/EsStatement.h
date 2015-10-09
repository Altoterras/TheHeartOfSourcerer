/***********************************************************************//**
 *	EsStatement.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/31.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_STATEMENT_H_
#define _ES_ES_STATEMENT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsAtom;
class EsObject;

/*---------------------------------------------------------------------*//**
 *	ステートメント情報
 *
**//*---------------------------------------------------------------------*/
class EsStatement
{
	//======================================================================
	// 定数
public:
	// ステートメント タイプ
	enum StatementType
	{
		STMTT_NULL,
		STMTT_LABEL,		// ラベル
		STMTT_IF,			// if
		STMTT_ELSE,			// else
		STMTT_SEQ,			// ステートメントの合成シークエンス
		STMTT_BLOCK,		// { ～ } ⇒ STMT_BLOCK
		STMTT_SWITCH,		// switch
		STMTT_WITH,			// with
		STMTT_CATCH,		// catch
		STMTT_TRY,			// try
		STMTT_FINALLY,		// finally
		STMTT_SUBROUTINE,	// サブルーチン本体
		STMTT__LOOP_START_,
		STMTT_WHILE,		// while
		STMTT_FOR,			// for
		STMTT_FORIN,		// for( in ) ⇒ STMT_FOR_IN_LOOP
		STMTT_DO,			// do
		STMTT__LOOP_END_,
	};

	// ブロックフラグ
	static const u8			BLOCKF_SCOPE	= 0x01;		// ⇒ SIF_SCOPE
	static const u8			BLOCKF_BODY		= 0x02;		// ⇒ SIF_BODY_BLOCK
	static const u8			BLOCKF_FOR		= 0x04;		// ⇒ SIF_FOR_BLOCK

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// タイプを得る
	inline StatementType getType() const { return _type; }
	// 下位スコープのステートメント情報を得る
	inline EsStatement* getDownScopeStatement() const { return _stmtDownScope; }
	// 閉じたステートメントの情報を得る
	inline EsStatement* getDownEnclosingStatement() const { return _stmtDownEncl; }
	// ブロックフラグを得る
	inline u8 getBlockFlags() const { return _blockflags; }
	inline u8* blockFlags() { return &_blockflags; }
	// ブロック ID を得る
	inline u32 getBlockId() { return _blockid; }
	// ブロックオブジェクトを得る
	inline EsObject* getBlockObject() const { return _objBlock; }
	// ラベル名アトムを得る
	inline const EsAtom* getLabelAtom() const { return _atomLabel; }

	// バイトコードオフセット位置を得る
	inline s32 getUpdateBcOffset() const { return _bcofsUpdate; }
	inline s32 getLastBreakBcOffset() const { return _bcofsLastBreak; }
	inline s32 getLastContinueBcOffset() const { return _bcofsLastContinue; }

	// タイプを設定する
	inline void	setType(StatementType type) { _type = type; }
	// 下位スコープのステートメント情報を設定する
	inline void	setDownScopeStatement(EsStatement* stmt) { _stmtDownScope = stmt; }
	// 閉じたステートメントの情報を設定する
	inline void	setDownEnclosingStatement(EsStatement* stmt) { _stmtDownEncl = stmt; }
	// ブロック ID を設定する
	inline void setBlockId(u32 blockid) { _blockid = blockid; }
	// ブロックオブジェクトを設定する
	inline void setBlockObject(EsObject* objBlock) { _objBlock = objBlock; }
	// ラベル名アトムを設定する
	inline void setLabelAtom(const EsAtom* atom) { _atomLabel = atom; }

	// バイトコードオフセット位置を設定する
	inline void setUpdateBcOffset(s32 bcofs) { _bcofsUpdate = bcofs; }
	inline void setLastBreakBcOffset(s32 bcofs) { _bcofsLastBreak = bcofs; }
	inline void setLastContinueBcOffset(s32 bcofs) { _bcofsLastContinue = bcofs; }

	// .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .
	// タイプ判定
	inline bool isLoop() const			{	return (STMTT__LOOP_START_ < _type) && (_type < STMTT__LOOP_END_);								}	// ⇒ STMT_IS_LOOP
	inline bool isLinksScope() const	{	return (_type == STMTT_WITH) || (_type == STMTT_CATCH) || ((_blockflags & BLOCKF_SCOPE) != 0);	}	// ⇒ STMT_LINKS_SCOPE
	inline bool isPerhapsScope() const	{	return (_type != STMTT_WITH) && ((STMTT_BLOCK <= _type) && (_type <= STMTT_SUBROUTINE));		}	// ⇒ STMT_MAYBE_SCOPE, STMT_TYPE_MAYBE_SCOPE

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setTopBcOffset(s32 bcofsTop);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsStatement();
	EsStatement(StatementType type);
	~EsStatement();

private:
	void clear();

	//======================================================================
	// 変数
private:
	StatementType		_type;				// タイプ
	EsStatement*		_stmtDownScope;		// 下位スコープのステートメント情報 ⇒ downScope
	EsStatement*		_stmtDownEncl;		// 下位の閉じステートメント情報 ⇒ down
	u8					_blockflags;		// ブロック フラグ
	u32					_blockid;			// ブロック ID
	EsObject*			_objBlock;			// ブロックスコープオブジェクトに対する参照 ⇒ blockObj
	const EsAtom*		_atomLabel;			// ラベル名
	s32					_bcofsUpdate;		// 更新されるバイトコードオフセット値 ⇒ update
	s32					_bcofsLastBreak;	// ループ内の最後の break
	s32					_bcofsLastContinue;	// ループ内の最後の continue
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_STATEMENT_H_
