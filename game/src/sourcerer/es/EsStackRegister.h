/***********************************************************************//**
 *	EsStackRegister.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_STACK_REGISTER_H_
#define _ES_ES_STACK_REGISTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsValue.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsContext;
class EsProgramCounter;

/*---------------------------------------------------------------------*//**
 *	スタックレジスタ
 *
**//*---------------------------------------------------------------------*/
class EsStackRegister
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// プログラムカウンタを得る
	inline EsProgramCounter* pc() const	{ return _pc; }
	// スタックポインタを得る
	inline EsValue* sp() const { ASSERTM((0 <= _idxSp) && (_idxSp < _numSp), "{EsStackRegister::sp} (0 <= _idxSp) && (_idxSp < _numSp)"); return _spTop + _idxSp; }
	inline EsValue* sp(s32 offset) const { ASSERTM((0 <= (_idxSp + offset)) && ((_idxSp + offset) < _numSp), "{EsStackRegister::sp} (0 <= (_idxSp + offset)) && ((_idxSp + offset) < _numSp)"); return _spTop + _idxSp + offset; }

	// スタックの数を得る
	inline s32 getSpNum() const { return _numSp; }
	// 現在のスタックポインタインデックスを得る
	inline s32 getCurSpIndex() const { return _idxSp; }
	// スタックポインタを絶対インデックス指定で得る
	inline EsValue* spFromAbsoluteIndex(s32 idxAbsolute) const { ASSERTM((0 <= idxAbsolute) && (idxAbsolute < _numSp), "{EsStackRegister::spFromAbsoluteIndex} (0 <= idxAbsolute) && (idxAbsolute < _numSp)"); return _spTop + idxAbsolute; }

	// 現在のスタックポインタインデックスを設定する
	inline void setCurSpIndex(s32 indexSp) { _idxSp = indexSp; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void advancePc(s32 count);
	void advanceSp(s32 count);

	void push(const EsValue* val);
	inline void pushOperand(const EsValue* val) { push(val); }	// ⇒ PUSH_OPND

	EsValue pop();
	inline EsValue popOperand() { return pop(); }	// ⇒ POP_OPND
	bool popBoolean(EsValue* val);

	EsValue fetchOperand(s32 spofs);
	s32 fetchS32(s32 spofs);
	u32 fetchU32(s32 spofs);
	f64 fetchF64(s32 spofs);
	bool fetchObject(EsObject** objOut, EsValue* valOut, s32 spofs);
	inline bool fetchObject(EsObject** objOut, s32 spofs) { return fetchObject(objOut, 0L, spofs); }
	bool fetchAtom(const EsAtom** atomOut, s32 spofs);
	bool fetchId(EsValue* id, s32 spofs);

	void storeOperand(s32 spofs, const EsValue* val);
	bool storeS32(s32 spofs, s32 ival);
	bool storeU32(s32 spofs, u32 ival);
	bool storeF64(s32 spofs, f64 dval);

	bool convValueToObject(EsObject** objOut, s32 offset, EsValue* val);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsStackRegister(EsContext* ctx, EsProgramCounter* pc, EsValue* sp, s32 numSp, s32 idxSpInit);
	EsStackRegister(const EsStackRegister& src);
	void copy(const EsStackRegister* src);

	//======================================================================
	// 変数
private:
	EsProgramCounter* _pc;		// プログラムカウンタ
	EsValue* _spTop;			// スタックポインタの先頭
	s32 _numSp;					// スタックの数
	s32 _idxSp;					// スタックポインタのインデックス

	EsContext* _ctxRef;			// コンテキストへの参照
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_STACK_REGISTER_H_
