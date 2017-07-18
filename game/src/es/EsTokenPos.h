/***********************************************************************//**
 *	EsTokenPos.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/28.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_TOKEN_POS_H_
#define _ES_ES_TOKEN_POS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsContext;

/*---------------------------------------------------------------------*//**
 *	トークン位置
 *
**//*---------------------------------------------------------------------*/
class EsTokenPos
{
	//======================================================================
	// クラス
public:
	class EsTokenPoint
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// インデックスを得る
		inline s32 getIndex() const { return _idx; }
		// 行を得
		inline u32 getLineNo() const { return _lno; }

		// インデックスを設定する
		inline void setIndex(s32 idx) { _idx = idx; }
		// 行を設定する
		inline void setLineNo(u32 lineno) { _lno = lineno; }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:
		// コンストラクタ
		EsTokenPoint() : _idx(-1), _lno(0) {}
		EsTokenPoint(s32 idx, u32 lineno) : _idx(idx), _lno(lineno) {}
		EsTokenPoint(const EsTokenPoint& s) { copy(&s); }
		// コピー
		inline EsTokenPoint& operator = (const EsTokenPoint& s) { copy(&s); return *this; }
		inline void copy(const EsTokenPoint* s) { _idx = s->_idx; _lno = s->_lno; }
		// 複製
		inline EsTokenPoint clone() const { return EsTokenPoint(*this); }
		// 設定
		inline void set(s32 idx, u32 lineno) { _idx = idx; _lno = lineno; }

		// 開始位置についての <
		inline bool lt(const EsTokenPoint* s) const { return (_lno < s->_lno) || ((_lno == s->_lno) && (_idx < s->_idx)); }
		// 開始位置についての <=
		inline bool le(const EsTokenPoint* s) const { return (_lno < s->_lno) || ((_lno == s->_lno) && (_idx <= s->_idx)); }
		// 開始位置についての >
		inline bool gt(const EsTokenPoint* s) const { return !le(s); }
		// 開始位置についての >=
		inline bool ge(const EsTokenPoint* s) const { return !lt(s); }

		//==================================================================
		// 変数
	private:
		s32 _idx;		// インデックス
		s32 _lno;		// 行
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 開始インデックスを得る
	inline s32 getBeginIndex() const { return _begin.getIndex(); }
	// 開始行を得る
	inline u32 getBeginLineNo() const { return _begin.getLineNo(); }
	// 終了インデックスを得る（このインデックスを含まない）
	inline s32 getEndIndex() const { return _end.getIndex(); }
	// 終了行を得る
	inline u32 getEndLineNo() const { return _end.getLineNo(); }

	// 開始インデックスを設定する
	inline void setBeginIndex(s32 idx) { _begin.setIndex(idx); }
	// 開始行を設定する
	inline void setBeginLineNo(u32 lineno) { _begin.setLineNo(lineno); }
	// 終了インデックスを設定する
	inline void setEndIndex(s32 idx) { _end.setIndex(idx); }
	// 終了行を設定する
	inline void setEndLineNo(u32 lineno) { _end.setLineNo(lineno); }

	// 開始点を得る
	inline const EsTokenPoint* getBegin() const { return &_begin; }
	inline EsTokenPoint* begin() { return &_begin; }
	// 終了点を得る（この位置を含まない）
	inline const EsTokenPoint* getEnd() const { return &_end; }
	inline EsTokenPoint* end() { return &_end; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// コンストラクタ
	EsTokenPos() {}
	EsTokenPos(s32 idxBegin, u32 linenoBegin, s32 idxEnd, u32 linenoEnd) { _begin.set(idxBegin, linenoBegin), _end.set(idxEnd, linenoEnd); }
	EsTokenPos(const EsTokenPos& s) { copy(&s); }
	EsTokenPos(const EsTokenPos* begin, const EsTokenPos* end) { _begin.copy(&begin->_begin); _end.copy(&end->_end); }
	// コピー
	inline EsTokenPos& operator = (const EsTokenPos& s) { copy(&s); return *this; }
	inline void copy(const EsTokenPos* s) { _begin.copy(&s->_begin); _end.copy(&s->_end); }
	inline void copyBegin(const EsTokenPos* s) { _begin.copy(&s->_begin); }
	inline void copyEnd(const EsTokenPos* s) { _end.copy(&s->_end); }
	// 複製
	inline EsTokenPos clone() const { return EsTokenPos(*this); }
	// 設定
	inline void set(s32 idxBegin, u32 linenoBegin, s32 idxEnd, u32 linenoEnd) { _begin.set(idxBegin, linenoBegin), _end.set(idxEnd, linenoEnd); }

	bool calcBackwardByIndex(const EsContext* ctx, s32 index);

	//======================================================================
	// 変数
private:
	EsTokenPoint	_begin;		// 開始点
	EsTokenPoint	_end;		// 終了点
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_TOKEN_POS_H_
