/***********************************************************************//**
 *	EsProgramCounter.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsProgramCounter.h"

// Friends
#include "EsAtomList.h"
#include "EsBcExInfo.h"
#include "EsBytecodeBuilder.h"
#include "EsContext.h"
#include "EsInterlang.h"
#include "EsKeywordSet.h"
#include "EsOpcode.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	プログラムカウンタ位置は変えず、次の命令コードを得る
**//*---------------------------------------------------------------------*/
bool EsProgramCounter::peekNextOpcode(EsOpcodeType* otNextOut, s32* pcofsNextOut, EsContext* ctx, const EsOpcodeType otCur) const
{
	// 次の命令コードまでのオフセットを得る
	s32 pcofsNext = EsOpcode::getSpec(otCur)->_length;
	if(ctx->getInterlang()->isBcExInfoAdd())	{	pcofsNext += EsBcExInfo::LEN_BC;	}	// 拡張情報分を加算

	// 安全性確認
	if(!isSafeOffset(pcofsNext))
	{
		return false;
	}

	if(otNextOut != 0L)		{	*otNextOut = (EsOpcodeType)get(pcofsNext);	}
	if(pcofsNextOut != 0L)	{	*pcofsNextOut = pcofsNext;					}
	return true;
}


/*---------------------------------------------------------------------*//**
	バイトコードから完全なインデックスを取得する
	※ 後続の OP_RESETBASE, OP_RESETBASE0 も先読みする
	⇒ js_GetIndexFromBytecode
**//*---------------------------------------------------------------------*/
s32 EsProgramCounter::getCompleteIndex(EsContext* ctx, s32 offsetPc) const
{
	s32 index = (s32)getU16(offsetPc + 1);	// +1 は命令コード（EsOpcodeType）分

	// 現在の命令コードを得る
	EsOpcodeType otCur = (EsOpcodeType)get(0);

	// 次の命令コードを取得する
	EsOpcodeType otNext;
	s32 pcofsNext;
	if(peekNextOpcode(&otNext, &pcofsNext, ctx, otCur))
	{
		// 大きいインデックスは OP_INDEXBASE, OP_xxx, OP_RESETBASE の順で来る
		// 次の命令コードが OP_RESETBASE, OP_RESETBASE0 ならば、その分も足す
		if(otNext == OP_RESETBASE)
		{
			// １つ前の命令コードの長さを得る
			s32 lenIndexbaseOpc = EsOpcode::getSpec(OP_INDEXBASE)->_length;
			if(ctx->getInterlang()->isBcExInfoAdd())	{	lenIndexbaseOpc += EsBcExInfo::LEN_BC;	}	// 拡張情報分を加算
			ASSERT(isSafeOffset(- lenIndexbaseOpc));
			ASSERT((EsOpcodeType)get(- lenIndexbaseOpc) == OP_INDEXBASE);				// １つ前の命令コードタイプは OP_INDEXBASE のはずである
			// 上位バイトインデックスを加算する ⇒ GET_INDEXBASE(pc - JSOP_INDEXBASE_LENGTH)
			index += ((s32)getU16(- lenIndexbaseOpc + 1)) << 16;
		}
	}

	/// ※※※ 実装なし ※※※ JSOP_RESETBASE0, JSOP_INDEXBASE1, JSOP_INDEXBASE2, JSOP_INDEXBASE3

	return index;
}

/*---------------------------------------------------------------------*//**
	オブジェクト・ディテクティングのチェック ⇒ Detecting
**//*---------------------------------------------------------------------*/
bool EsProgramCounter::checkDetecting(EsContext* ctx, s8 lengthCurOp) const
{
	EsProgramCounter pcWk(*this);

	while(true)
	{
		EsOpcodeType ot = (EsOpcodeType)pcWk.get(0);	// ⇒ op = js_GetOpcode(cx, script, pc);
		if(TFW_IS_FLAG(EsOpcode::getSpec(ot)->_format, EsOpcode::OPFMT_DETECTING))
		{
			return true;
		}

		switch(ot)
		{
		case OP_NULL:
			if(pcWk.isSafeOffset(1))	// ⇒ if (++pc < endpc)
			{
				ot = (EsOpcodeType)pcWk.get(1);
				return (ot == OP_EQ) || (ot == OP_NE);
			}
			return false;

		case OP_NAME:
			{
				// アトムを得る ⇒ JS_GET_SCRIPT_ATOM
				s32 index = pcWk.getCompleteIndex(ctx, 0);
				// ※※※ 実装なし ※※※ ⇒ imacpc
				ASSERT(index < ctx->getInterlang()->getAtomNum());						// ⇒ JS_ASSERT((uint32)(index) < atoms_->length);
				const EsAtom* atom = ctx->getInterlang()->getAtom(index);				// ⇒ (atom) = atoms_->vector[index];
				if(atom == ctx->getKeywordSet()->getLogicalType(LTYPE_VOID)->getAtom())	// ⇒ atom == cx->runtime->atomState.typeAtoms[JSTYPE_VOID]
				{
					s32 lengthOp = EsOpcode::getSpec(OP_NAME)->_length;
					if(pcWk.isSafeOffset(lengthOp))	// ⇒ pc += js_CodeSpec[op].length) < endpc
					{
						ot = (EsOpcodeType)pcWk.get(lengthOp);
						return (ot == OP_EQ) || (ot == OP_NE) || (ot == OP_STRICTEQ) || (ot == OP_STRICTNE);
					}
				}
			}
			return false;

		default:
			if(!TFW_IS_FLAG(EsOpcode::getSpec(ot)->_format, EsOpcode::OPFMT_INDEXBASE))
			{
				return false;
			}
			break;
		}

		pcWk.advance(lengthCurOp);	// ⇒ pc += js_CodeSpec[op].length / pc += cs->length;
		if(ctx->getInterlang()->isBcExInfoAdd())	{	pcWk.advance(EsBcExInfo::LEN_BC);	}	// 拡張情報分を計算
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	可変長バイトコードも含めたバイトコード長を計算する
	⇒ js_GetVariableBytecodeLength
**//*---------------------------------------------------------------------*/
s32 EsProgramCounter::getBytecodeLength(s32 offsetOpcodePos) const
{
	s32 bcofsWk = offsetOpcodePos;
	EsOpcodeType ot = (EsOpcodeType)get(bcofsWk);

	const EsOpcode::Spec* spec = EsOpcode::getSpec(ot);
	if(spec->_length >= 0)
	{
		return spec->_length;	// 固定長
	}

	switch(ot)
	{
	case OP_TABLESWITCHX:
	case OP_TABLESWITCH:
		{
			s32 lenJump = (ot == OP_TABLESWITCHX) ? EsBytecodeBuilder::SIZE_JUMPX_OFFSET_BC : EsBytecodeBuilder::SIZE_JUMP_OFFSET_BC;
			bcofsWk += lenJump;
			s32 ofsLow = getJumpOffset(bcofsWk);
			bcofsWk += EsBytecodeBuilder::SIZE_JUMP_OFFSET_BC;
			s32 ofsHigh = getJumpOffset(bcofsWk);
			return 1 + lenJump + EsBytecodeBuilder::SIZE_INDEX_BC + EsBytecodeBuilder::SIZE_INDEX_BC + ((ofsHigh - ofsLow + 1) * lenJump);
		}
		break;
	case OP_LOOKUPSWITCHX:
	case OP_LOOKUPSWITCH:
		{
			s32 lenJump = (ot == OP_LOOKUPSWITCHX) ? EsBytecodeBuilder::SIZE_JUMPX_OFFSET_BC : EsBytecodeBuilder::SIZE_JUMP_OFFSET_BC;
			bcofsWk += lenJump;
			s32 ofs = getJumpOffset(bcofsWk);
			return 1 + lenJump + EsBytecodeBuilder::SIZE_INDEX_BC + ((EsBytecodeBuilder::SIZE_INDEX_BC + lenJump) * ofs);
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	return -1;
}

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	デバッグ トレース
**//*---------------------------------------------------------------------*/
void EsProgramCounter::debug_trace(s32 offset, s32 length) const
{
	TRACE("{EsProgramCounter::debug_trace} >>>\n");
	for(int i = 0; i < length; i++)
	{
		s32 idx = _idx + i;
		if((idx < 0) || (_len <= idx))
		{
			break;
		}
		TRACE("%02x ", _bc[idx]);
	}
	TRACE("{EsProgramCounter::debug_trace} <<<\n");
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsProgramCounter::EsProgramCounter(const VcString* bytecode)
	: _bc((const u8*)bytecode->getRaw())
	, _len(bytecode->getLength())
	, _idx(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsProgramCounter::~EsProgramCounter()
{
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void EsProgramCounter::copy(const EsProgramCounter* src)
{
	_bc = src->_bc;
	_len = src->_len;
	_idx = src->_idx;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
