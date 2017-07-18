/***********************************************************************//**
 *	EsProgramCounter.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_PROGRAM_COUNTER_H_
#define _ES_ES_PROGRAM_COUNTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsOpcodeType.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

TFW_END_NS

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	プログラム カウンター
 *
**//*---------------------------------------------------------------------*/
class EsProgramCounter
{
	//======================================================================
	// 定数
protected:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// バイトコードから値を得る
	inline u8 get(s32 offset) const { ASSERTM((0 <= (_idx + offset)) && ((_idx + offset) < _len), "{EsProgramCounter::get} (0 <= (_idx + offset)) && ((_idx + offset) < _len)"); return _bc[_idx + offset]; }
	inline u32 getU16(s32 offset) const { return (u32)((get(offset + 0) <<  8) | (get(offset + 1) <<  0)); }
	inline u32 getU24(s32 offset) const { return (u32)((get(offset + 0) << 16) | (get(offset + 1) <<  8) | (get(offset + 2) <<  0)); }
	inline u32 getS32(s32 offset) const { return (s32)((get(offset + 0) << 24) | (get(offset + 1) << 16) | (get(offset + 2) <<  8) | (get(offset + 3) <<  0)); }
	// 現在のオフセットを得る
	inline s32 getCurrentOffset() const { return _idx; }
	// バイトコードの長さを得る
	inline s32 getBytecodeLength() const { return _len; }

	// 安全なオフセットかを得る
	inline bool isSafeOffset(s32 offset) const { return (0 <= (_idx + offset)) && ((_idx + offset) < _len); }
	// バイトコードからジャンプオフセット値を得る（EsBytecodeBuilder::SIZE_JUMP_OFFSET_BC と同サイズ） ⇒ GET_JUMP_OFFSET
	inline s16 getJumpOffset() const { return (s16)getU16(1); }
	inline s16 getJumpOffset(s32 offset) const { return (s16)getU16(offset + 1); }
	// バイトコードから拡張ジャンプオフセット値を得る（EsBytecodeBuilder::SIZE_JUMPX_OFFSET_BC と同サイズ） ⇒ GET_JUMPX_OFFSET
	inline s32 getJumpXOffset() const { return (s32)getU24(1); }
	inline s32 getJumpXOffset(s32 offset) const { return (s32)getU24(offset + 1); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool peekNextOpcode(EsOpcodeType* otNextOut, s32* pcofsNextOut, EsContext* ctx, const EsOpcodeType otCur) const;
	s32 getCompleteIndex(EsContext* ctx, s32 offsetPc) const;
	bool checkDetecting(EsContext* ctx, s8 lengthCurOp) const;
	s32 getBytecodeLength(s32 offsetOpcodePos) const;

	#if defined(_DEBUG)
		void debug_trace(s32 offset, s32 length) const;
	#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsProgramCounter(const VcString* bytecode);
	EsProgramCounter(const EsProgramCounter& s) : _bc(s._bc), _len(s._len), _idx(s._idx) {}
	~EsProgramCounter();

	void advance(s32 count) { _idx += count; ASSERT((0 <= _idx) && (_idx < _len)); }
	void reset() { _idx = 0; }
	void copy(const EsProgramCounter* src);

	//======================================================================
	// 変数
private:
	const u8* _bc;
	s32 _len;
	s32 _idx;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_PROGRAM_COUNTER_H_
