/***********************************************************************//**
 *	EsOpcode.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_OPCODE_H_
#define _ES_ES_OPCODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsOpcodeType.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スクリプト演算コード
 *
**//*---------------------------------------------------------------------*/
class EsOpcode
{
	//======================================================================
	// 定数
public:
	static const u32 OPFMT_BYTE				= 0;			// ⇒ JOF_BYTE single bytecode, no immediates
	static const u32 OPFMT_JUMP				= 1;			// ⇒ JOF_JUMP signed 16-bit jump offset immediate
	static const u32 OPFMT_ATOM				= 2;			// ⇒ JOF_ATOM unsigned 16-bit constant index
	static const u32 OPFMT_UINT16			= 3;			// ⇒ JOF_UINT unsigned 16-bit immediate operand
	static const u32 OPFMT_TABLESWITCH		= 4;			// ⇒ JOF_TABLESWITCH table switch
	static const u32 OPFMT_LOOKUPSWITCH		= 5;			// ⇒ JOF_LOOKUPSWITCH lookup switch
	static const u32 OPFMT_QARG				= 6;			// ⇒ JOF_QARG quickened get/set function argument ops
	static const u32 OPFMT_LOCAL			= 7;			// ⇒ JOF_LOCAL var or block-local variable
	static const u32 OPFMT_SLOTATOM			= 8;			// ⇒ JOF_SLOTATOM uint16 slot + constant index
	static const u32 OPFMT_JUMPX			= 9;			// ⇒ JOF_JUMPX signed 32-bit jump offset immediate
	static const u32 OPFMT_TABLESWITCHX		= 10;			// ⇒ JOF_TABLESWITCHX extended (32-bit offset) table switch
	static const u32 OPFMT_LOOKUPSWITCHX	= 11;			// ⇒ JOF_LOOKUPSWITCHX extended (32-bit offset) lookup switch
	static const u32 OPFMT_UINT24			= 12;			// ⇒ JOF_UINT extended unsigned 24-bit literal (index)
	static const u32 OPFMT_UINT8			= 13;			// ⇒ JOF_UINT uint8 immediate, e.g. top 8 bits of 24-bit atom index
	static const u32 OPFMT_INT32			= 14;			// ⇒ JOF_INT int32 immediate operand
	static const u32 OPFMT_OBJECT			= 15;			// ⇒ JOF_OBJECT unsigned 16-bit object index
	static const u32 OPFMT_SLOTOBJECT		= 16;			// ⇒ JOF_SLOTOBJECT uint16 slot index + object index
	static const u32 OPFMT_REGEXP			= 17;			// ⇒ JOF_REGEXP unsigned 16-bit regexp index
	static const u32 OPFMT_INT8				= 18;			// ⇒ JOF_INT int8 immediate operand
	static const u32 OPFMT_ATOMOBJECT		= 19;			// ⇒ JOF_ATOMOBJECT uint16 constant index + object index
	static const u32 OPFMT_TYPEMASK			= 0x001f;		// ⇒ JOF_TYPEMASK mask for above immediate types
	static const u32 OPFMT_NAME				= (1U<<5);		// ⇒ JOF_NAME name operation
	static const u32 OPFMT_PROP				= (2U<<5);		// ⇒ JOF_PROP obj.prop operation
	static const u32 OPFMT_ELEM				= (3U<<5);		// ⇒ JOF_ELEM obj[index] operation
	static const u32 OPFMT_XMLNAME			= (4U<<5);		// ⇒ JOF_XMLNAME XML name: *, a::b, @a, @a::b, etc.
	static const u32 OPFMT_VARPROP			= (5U<<5);		// ⇒ JOF_VARPROP x.prop for this, arg, var, or local x
	static const u32 OPFMT_MODEMASK			= (7U<<5);		// ⇒ JOF_MODEMASK mask for above addressing modes
	static const u32 OPFMT_SET				= (1U<<8);		// ⇒ JOF_SET set (i.e., assignment) operation
	static const u32 OPFMT_DEL				= (1U<<9);		// ⇒ JOF_DEL delete operation
	static const u32 OPFMT_DEC				= (1U<<10);		// ⇒ JOF_DEC decrement (--, not ++) opcode
	static const u32 OPFMT_INC				= (2U<<10);		// ⇒ JOF_INC increment (++, not --) opcode
	static const u32 OPFMT_INCDEC			= (3U<<10);		// ⇒ JOF_INCDEC increment or decrement opcode
	static const u32 OPFMT_POST				= (1U<<12);		// ⇒ JOF_POST postorder increment or decrement
	static const u32 OPFMT_FOR				= (1U<<13);		// ⇒ JOF_FOR for-in property op (akin to OPFMT_SET)
	static const u32 OPFMT_ASSIGNING		= OPFMT_SET;	// ⇒ JOF_ASSIGNING hint for JSClass.resolve, used for ops that do simplex assignment
	static const u32 OPFMT_DETECTING		= (1U<<14);		// ⇒ JOF_DETECTING object detection for JSNewResolveOp
	static const u32 OPFMT_BACKPATCH		= (1U<<15);		// ⇒ JOF_BACKPATCH backpatch placeholder during codegen
	static const u32 OPFMT_LEFTASSOC		= (1U<<16);		// ⇒ JOF_LEFTASSOC left-associative operator
	static const u32 OPFMT_DECLARING		= (1U<<17);		// ⇒ JOF_DECLARING var, const, or function declaration op
	static const u32 OPFMT_INDEXBASE		= (1U<<18);		// ⇒ JOF_INDEXBASE atom segment base setting prefix op
	static const u32 OPFMT_CALLOP			= (1U<<19);		// ⇒ JOF_CALLOP call operation that pushes function and this
	static const u32 OPFMT_PARENHEAD		= (1U<<20);		// ⇒ JOF_PARENHEAD opcode consumes value of expression in parenthesized statement head
	static const u32 OPFMT_INVOKE			= (1U<<21);		// ⇒ JOF_INVOKE JSOP_CALL,	JSOP_NEW,	JSOP_EVAL
	static const u32 OPFMT_TMPSLOT			= (1U<<22);		// ⇒ JOF_TMPSLOT interpreter uses extra temporary slot to root intermediate objects besides the slots opcode uses
	static const u32 OPFMT_TMPSLOT2			= (2U<<22);		// ⇒ JOF_TMPSLOT interpreter uses extra 2 temporary slot besides the slots opcode uses
	static const u32 OPFMT_TMPSLOT_SHIFT	= 22;

	//======================================================================
	// クラス
public:
	class Spec
	{
	public:
		s8			_length;			// ⇒ JOF_length
		s8			_numStackUses;		// ⇒ JOF_nuses
		s8			_numStackDefs;		// ⇒ JOF_ndefs
		u8			_precedence;		// 優先度 ⇒ prec
		u32			_format;			// ⇒ JOF_format
#if defined(_DEBUG)
		const char*	_name;
#endif
		
#if defined(_DEBUG)
		const char* getName() const { return _name; }
#else
		const char* getName() const { return 0L; }
#endif

		Spec()
			: _length(0)
			, _numStackUses(0)
			, _numStackDefs(0)
			, _precedence(0)
			, _format(0)
#if defined(_DEBUG)
			, _name("")
#endif
		{
		}
		Spec(const char* name, s8 length, s8 numStackUses, s8 numStackDefs, u8 precedence, u32 format)
			: _length(length)
			, _numStackUses(numStackUses)
			, _numStackDefs(numStackDefs)
			, _precedence(precedence)
			, _format(format)
#if defined(_DEBUG)
			, _name(name)
#endif
		{
		}
		Spec& operator = (const Spec& src)
		{
			_length = src._length;
			_numStackUses = src._numStackUses;
			_numStackDefs = src._numStackDefs;
			_precedence = src._precedence;
			_format = src._format;
#if defined(_DEBUG)
			_name = src._name;
#endif
			return *this;
		}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// スペックを得る
	static inline const Spec* getSpec(EsOpcodeType ot)	{ return &_self._spec[ot]; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static s32 getStackUses(EsOpcodeType ot, const VcString* bc, u32 bcofs);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
private:
	EsOpcode();
	~EsOpcode();

	//======================================================================
	// 変数
private:
	static EsOpcode		_self;
	Spec				_spec[NUM_OP];
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_OPCODE_H_
