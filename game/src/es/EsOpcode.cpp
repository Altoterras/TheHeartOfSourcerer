/***********************************************************************//**
 *	EsOpcode.cpp
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
#include "EsOpcode.h"

// Friends

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 静的変数

EsOpcode EsOpcode::_self;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	バイトコードのスタック使用量を得る ⇒ js_GetStackUses
**//*---------------------------------------------------------------------*/
s32 EsOpcode::getStackUses(EsOpcodeType ot, const VcString* bc, u32 bcofs)
{
	const EsOpcode::Spec* opspec = getSpec(ot);
	if(opspec->_numStackUses >= 0)
	{
		return opspec->_numStackUses;
	}

	#if defined(_DEBUG)
		u32 n1 = ot;
		const u8* pc = (const u8*)bc->getRaw(bcofs);
		ASSERT(n1 == *pc);
	#endif

	// 可変長サイズの演算子スタック使用量を出力されたバイトコードから取得する ⇒ js_GetVariableStackUses
#if 0
	u8 b1 = bc->getAt(bcofs + 1);
	u8 b2 = bc->getAt(bcofs + 2);
	FTRACE("=== {EsOpcode::getStackUses} 001. %02x %02x %02x\n", ot, b1, b2);
	switch(ot)
	{
	case OP_POPN:
	case OP_LEAVEBLOCK:
	case OP_NEWARRAY:
		return (s32)((b1 <<  8) | (b2 <<  0));
	case OP_LEAVEBLOCKEXPR:
		return (s32)((b1 <<  8) | (b2 <<  0)) + 1;
	default:
		return (s32)((b1 <<  8) | (b2 <<  0)) + 2;	// ⇒ 2 + GET_ARGC(pc);
	}
#else
	switch(ot)
	{
	case OP_POPN:
	case OP_LEAVEBLOCK:
	case OP_NEWARRAY:
		return (s32)((bc->getAt(bcofs + 1) <<  8) | (bc->getAt(bcofs + 2) <<  0));
	case OP_LEAVEBLOCKEXPR:
		return (s32)((bc->getAt(bcofs + 1) <<  8) | (bc->getAt(bcofs + 2) <<  0)) + 1;
	default:
		return (s32)((bc->getAt(bcofs + 1) <<  8) | (bc->getAt(bcofs + 2) <<  0)) + 2;	// ⇒ 2 + GET_ARGC(pc);
	}
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsOpcode::EsOpcode()
{
	memset(_spec, 0, sizeof(Spec) * NUM_OP);
	_spec[OP_NULL]				= Spec("NULL", 				1, 0, 1, 19,	OPFMT_BYTE);
	_spec[OP_NOP]				= Spec("NOP", 				1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_PUSH]				= Spec("PUSH",			 	1, 0, 1, 0,		OPFMT_BYTE);
	_spec[OP_POPV]				= Spec("POPV",				1, 1, 0, 2,		OPFMT_BYTE);
	_spec[OP_ENTERWITH]			= Spec("ENTERWITH", 		1, 1, 1, 0,		OPFMT_BYTE | OPFMT_PARENHEAD);
	_spec[OP_LEAVEWITH]			= Spec("LEAVEWITH", 		1, 1, 0, 0,		OPFMT_BYTE);
	_spec[OP_RETURN]			= Spec("RETURN",			1, 1, 0, 2,		OPFMT_BYTE);
	_spec[OP_GOTO]				= Spec("GOTO",				3, 0, 0, 0,		OPFMT_JUMP);
	_spec[OP_IFEQ]				= Spec("IFEQ",				3, 1, 0, 4,		OPFMT_JUMP | OPFMT_DETECTING);
	_spec[OP_IFNE]				= Spec("IFNE", 				3, 1, 0, 0,		OPFMT_JUMP | OPFMT_PARENHEAD);
	_spec[OP_ARGUMENTS]			= Spec("ARGS",				1, 0, 1, 18,	OPFMT_BYTE);
	_spec[OP_FORARG]			= Spec("FORARG",		 	3, 2, 2, 19,	OPFMT_QARG | OPFMT_NAME | OPFMT_FOR);
	_spec[OP_FORLOCAL]			= Spec("FORLOCAL", 			3, 2, 2, 19,	OPFMT_LOCAL | OPFMT_NAME | OPFMT_FOR);
	_spec[OP_DUP]				= Spec("DUP",				1, 1, 2, 0,		OPFMT_BYTE);
	_spec[OP_DUP2]				= Spec("DUP2",				1, 2, 4, 0,		OPFMT_BYTE);
	_spec[OP_SETCONST]			= Spec("SETCONST",			3, 1, 1, 3,		OPFMT_ATOM | OPFMT_NAME | OPFMT_SET);
	_spec[OP_BITOR]				= Spec("BITOR",				1, 2, 1, 7,		OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_BITXOR]			= Spec("BITXOR", 			1, 2, 1, 8,		OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_BITAND]			= Spec("BITAND",			1, 2, 1, 9,		OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_EQ]				= Spec("EQ",				1, 2, 1, 10,	OPFMT_BYTE | OPFMT_LEFTASSOC | OPFMT_DETECTING);
	_spec[OP_NE]				= Spec("NE",			 	1, 2, 1, 10,	OPFMT_BYTE | OPFMT_LEFTASSOC | OPFMT_DETECTING);
	_spec[OP_LT]				= Spec("LT",			 	1, 2, 1, 11,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_LE]				= Spec("LE", 				1, 2, 1, 11,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_GT]				= Spec("GT", 				1, 2, 1, 11,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_GE]				= Spec("GE", 				1, 2, 1, 11,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_LSFT]				= Spec("LSH",				1, 2, 1, 12,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_RSFT]				= Spec("RSH",				1, 2, 1, 12,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_URSFT]				= Spec("URSH",				1, 2, 1, 12,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_ADD]				= Spec("ADD",				1, 2, 1, 13,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_SUB]				= Spec("SUB",				1, 2, 1, 13,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_MUL]				= Spec("MUL",				1, 2, 1, 14,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_DIV]				= Spec("DIV",				1, 2, 1, 14,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_MOD]				= Spec("MOD", 				1, 2, 1, 14,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_NOT]				= Spec("NOT", 				1, 1, 1, 15,	OPFMT_BYTE | OPFMT_DETECTING);
	_spec[OP_BITNOT]			= Spec("BITNOT",			1, 1, 1, 15,	OPFMT_BYTE);
	_spec[OP_NEG]				= Spec("NEG",				1, 1, 1, 15,	OPFMT_BYTE);
	_spec[OP_POS]				= Spec("POS",				1, 1, 1, 15,	OPFMT_BYTE);
	_spec[OP_DELNAME]			= Spec("DELNAME",			3, 0, 1, 15,	OPFMT_ATOM | OPFMT_NAME | OPFMT_DEL);
	_spec[OP_DELPROP]			= Spec("DELPROP",			3, 1, 1, 15,	OPFMT_ATOM | OPFMT_PROP | OPFMT_DEL);
	_spec[OP_DELELEM]			= Spec("DELELEM",			1, 2, 1, 15,	OPFMT_BYTE  | OPFMT_ELEM | OPFMT_DEL);
	_spec[OP_TYPEOF]			= Spec("TYPEOF",			1, 1, 1, 15,	OPFMT_BYTE | OPFMT_DETECTING);
	_spec[OP_VOID]				= Spec("VOID",				1, 1, 1, 15,	OPFMT_BYTE);
	_spec[OP_INCNAME]			= Spec("INCNAME",			3, 0, 1, 15,	OPFMT_ATOM | OPFMT_NAME | OPFMT_INC | OPFMT_TMPSLOT2);
	_spec[OP_INCPROP]			= Spec("INCPROP",			3, 1, 1, 15,	OPFMT_ATOM | OPFMT_PROP | OPFMT_INC | OPFMT_TMPSLOT2);
	_spec[OP_INCELEM]			= Spec("INCELEM",			1, 2, 1, 15,	OPFMT_BYTE  | OPFMT_ELEM | OPFMT_INC | OPFMT_TMPSLOT2);
	_spec[OP_DECNAME]			= Spec("DECNAME",			3, 0, 1, 15,	OPFMT_ATOM | OPFMT_NAME | OPFMT_DEC | OPFMT_TMPSLOT2);
	_spec[OP_DECPROP]			= Spec("DECPROP",			3, 1, 1, 15,	OPFMT_ATOM | OPFMT_PROP | OPFMT_DEC | OPFMT_TMPSLOT2);
	_spec[OP_DECELEM]			= Spec("DECELEM",			1, 2, 1, 15,	OPFMT_BYTE  | OPFMT_ELEM | OPFMT_DEC | OPFMT_TMPSLOT2);
	_spec[OP_NAMEINC]			= Spec("NAMEINC",			3, 0, 1, 15,	OPFMT_ATOM | OPFMT_NAME | OPFMT_INC | OPFMT_POST | OPFMT_TMPSLOT2);
	_spec[OP_PROPINC]			= Spec("PROPINC",			3, 1, 1, 15,	OPFMT_ATOM | OPFMT_PROP | OPFMT_INC | OPFMT_POST | OPFMT_TMPSLOT2);
	_spec[OP_ELEMINC]			= Spec("ELEMINC",			1, 2, 1, 15,	OPFMT_BYTE  | OPFMT_ELEM | OPFMT_INC | OPFMT_POST | OPFMT_TMPSLOT2);
	_spec[OP_NAMEDEC]			= Spec("NAMEDEC",			3, 0, 1, 15,	OPFMT_ATOM | OPFMT_NAME | OPFMT_DEC | OPFMT_POST | OPFMT_TMPSLOT2);
	_spec[OP_PROPDEC]			= Spec("PROPDEC",			3, 1, 1, 15,	OPFMT_ATOM | OPFMT_PROP | OPFMT_DEC | OPFMT_POST | OPFMT_TMPSLOT2);
	_spec[OP_ELEMDEC]			= Spec("ELEMDEC",			1, 2, 1, 15,	OPFMT_BYTE  | OPFMT_ELEM | OPFMT_DEC | OPFMT_POST | OPFMT_TMPSLOT2);
	_spec[OP_GETPROP]			= Spec("GETPROP",			3, 1, 1, 18,	OPFMT_ATOM | OPFMT_PROP);
	_spec[OP_SETPROP]			= Spec("SETPROP",		 	3, 2, 1, 3,		OPFMT_ATOM | OPFMT_PROP | OPFMT_SET | OPFMT_DETECTING);
	_spec[OP_GETELEM]			= Spec("GETELEM",		 	1, 2, 1, 18,	OPFMT_BYTE  | OPFMT_ELEM | OPFMT_LEFTASSOC);
	_spec[OP_SETELEM]			= Spec("SETELEM",		 	1, 3, 1, 3,		OPFMT_BYTE  | OPFMT_ELEM | OPFMT_SET | OPFMT_DETECTING);
	_spec[OP_CALLNAME]			= Spec("CALLNAME",			3, 0, 2, 19,	OPFMT_ATOM | OPFMT_NAME | OPFMT_CALLOP);
	_spec[OP_CALL]				= Spec("CALL", 				3, -1, 1, 18,	OPFMT_UINT16 | OPFMT_INVOKE);
	_spec[OP_NAME]				= Spec("NAME",				3, 0, 1, 19,	OPFMT_ATOM | OPFMT_NAME);
	_spec[OP_DOUBLE]			= Spec("DOUBLE",			3, 0, 1, 16,	OPFMT_ATOM);
	_spec[OP_STRING]			= Spec("STRING",		 	3, 0, 1, 19,	OPFMT_ATOM);
	_spec[OP_ZERO]				= Spec("ZERO",				1, 0, 1, 16,	OPFMT_BYTE);
	_spec[OP_ONE]				= Spec("ONE",				1, 0, 1, 16,	OPFMT_BYTE);
	_spec[OP_NULLVALUE]			= Spec("NULL", 				1, 0, 1, 19,	OPFMT_BYTE);
	_spec[OP_THIS]				= Spec("THIS", 				1, 0, 1, 19,	OPFMT_BYTE);
	_spec[OP_FALSE]				= Spec("FALSE",				1, 0, 1, 19,	OPFMT_BYTE);
	_spec[OP_TRUE]				= Spec("TRUE",				1, 0, 1, 19,	OPFMT_BYTE);
	_spec[OP_OR]				= Spec("OR",				3, 1, 0, 5,		OPFMT_JUMP | OPFMT_DETECTING | OPFMT_LEFTASSOC);
	_spec[OP_AND]				= Spec("AND",				3, 1, 0, 6,		OPFMT_JUMP | OPFMT_DETECTING | OPFMT_LEFTASSOC);
	_spec[OP_TABLESWITCH]		= Spec("TABLESWITCH", 		-1, 1, 0, 0,	OPFMT_TABLESWITCH | OPFMT_DETECTING | OPFMT_PARENHEAD);
	_spec[OP_LOOKUPSWITCH]		= Spec("LOOKUPSWITCH", 		-1, 1, 0, 0,	OPFMT_LOOKUPSWITCH | OPFMT_DETECTING | OPFMT_PARENHEAD);
	_spec[OP_STRICTEQ]			= Spec("STRICTEQ",			1, 2, 1, 10,	OPFMT_BYTE | OPFMT_DETECTING | OPFMT_LEFTASSOC);
	_spec[OP_STRICTNE]			= Spec("STRICTNE",			1, 2, 1, 10,	OPFMT_BYTE | OPFMT_DETECTING | OPFMT_LEFTASSOC);
	_spec[OP_SETCALL]			= Spec("SETCALL",			3, -1, 2, 18,	OPFMT_UINT16 | OPFMT_SET);
	_spec[OP_ITER]				= Spec("ITER", 				2, 1, 2, 0,		OPFMT_UINT8);
	_spec[OP_NEXTITER]			= Spec("NEXTITER", 			1, 2, 3, 0,		OPFMT_BYTE);
	_spec[OP_ENDITER]			= Spec("ENDITER", 			1, 2, 0, 0,		OPFMT_BYTE);
	_spec[OP_APPLY]				= Spec("APPLY", 			3, -1, 1, 18,	OPFMT_UINT16 | OPFMT_INVOKE);
	_spec[OP_SWAP]				= Spec("SWAP",				1, 2, 2, 0,		OPFMT_BYTE);
	_spec[OP_OBJECT]			= Spec("OBJECT", 			3, 0, 1, 19,	OPFMT_OBJECT);
	_spec[OP_POP]				= Spec("POP", 				1, 1, 0, 2,		OPFMT_BYTE);
	_spec[OP_NEW]				= Spec("NEW", 				3, -1,  1, 17,  OPFMT_UINT16 | OPFMT_INVOKE);
	_spec[OP_TRAP]				= Spec("TRAP", 				1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_GETARG]			= Spec("GETARG", 			3, 0, 1, 19,	OPFMT_QARG  | OPFMT_NAME);
	_spec[OP_SETARG]			= Spec("SETARG", 			3, 1, 1, 3,		OPFMT_QARG  | OPFMT_NAME | OPFMT_SET);
	_spec[OP_GETLOCAL]			= Spec("GETLOCAL", 			3, 0, 1, 19,	OPFMT_LOCAL | OPFMT_NAME);
	_spec[OP_SETLOCAL]			= Spec("SETLOCAL", 			3, 1, 1, 3,		OPFMT_LOCAL | OPFMT_NAME | OPFMT_SET | OPFMT_DETECTING);
	_spec[OP_UINT16]			= Spec("UINT16", 			3, 0, 1, 16,	OPFMT_UINT16);
	_spec[OP_NEWINIT]			= Spec("NEWINIT",			2, 0, 1, 19,	OPFMT_INT8);
	_spec[OP_ENDINIT]			= Spec("ENDINIT",			1, 0, 0, 19,	OPFMT_BYTE);
	_spec[OP_INITPROP]			= Spec("INITPROP",			3, 1, 0, 3,		OPFMT_ATOM | OPFMT_PROP | OPFMT_SET | OPFMT_DETECTING);
	_spec[OP_INITELEM]			= Spec("INITELEM",		 	1, 2, 0, 3,		OPFMT_BYTE  | OPFMT_ELEM | OPFMT_SET | OPFMT_DETECTING);
	_spec[OP_DEFSHARP]			= Spec("DEFSHARP",			3, 0, 0, 0,		OPFMT_UINT16);
	_spec[OP_USESHARP]			= Spec("USESHARP",			3, 0, 1, 0,		OPFMT_UINT16);
	_spec[OP_INCARG]			= Spec("INCARG",			3, 0, 1, 15,	OPFMT_QARG  | OPFMT_NAME | OPFMT_INC);
	_spec[OP_DECARG]			= Spec("DECARG",			3, 0, 1, 15,	OPFMT_QARG  | OPFMT_NAME | OPFMT_DEC);
	_spec[OP_ARGINC]			= Spec("ARGINC",			3, 0, 1, 15,	OPFMT_QARG  | OPFMT_NAME | OPFMT_INC | OPFMT_POST);
	_spec[OP_ARGDEC]			= Spec("ARGDEC",			3, 0, 1, 15,	OPFMT_QARG  | OPFMT_NAME | OPFMT_DEC | OPFMT_POST);
	_spec[OP_INCLOCAL]			= Spec("INCLOCAL",			3, 0, 1, 15,	OPFMT_LOCAL | OPFMT_NAME | OPFMT_INC);
	_spec[OP_DECLOCAL]			= Spec("DECLOCAL",			3, 0, 1, 15,	OPFMT_LOCAL | OPFMT_NAME | OPFMT_DEC);
	_spec[OP_LOCALINC]			= Spec("LOCALINC",			3, 0, 1, 15,	OPFMT_LOCAL | OPFMT_NAME | OPFMT_INC | OPFMT_POST);
	_spec[OP_LOCALDEC]			= Spec("LOCALDEC",			3, 0, 1, 15,	OPFMT_LOCAL | OPFMT_NAME | OPFMT_DEC | OPFMT_POST);
	_spec[OP_IMACOP]			= Spec("IMACOP",			1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_FORNAME]			= Spec("FORNAME",		 	3, 2, 2, 19,	OPFMT_ATOM | OPFMT_NAME | OPFMT_FOR);
	_spec[OP_FORPROP]			= Spec("FORPROP", 			3, 3, 2, 18,	OPFMT_ATOM | OPFMT_PROP | OPFMT_FOR);
	_spec[OP_FORELEM]			= Spec("FORELEM",			1, 2, 3, 18,	OPFMT_BYTE  | OPFMT_ELEM | OPFMT_FOR);
	_spec[OP_POPN]				= Spec("POPN", 				3, -1, 0, 0,	OPFMT_UINT16);
	_spec[OP_BINDNAME]			= Spec("BINDNAME", 			3, 0, 1, 0,		OPFMT_ATOM | OPFMT_NAME | OPFMT_SET);
	_spec[OP_SETNAME]			= Spec("SETNAME", 			3, 2, 1, 3,		OPFMT_ATOM | OPFMT_NAME | OPFMT_SET | OPFMT_DETECTING);
	_spec[OP_THROW]				= Spec("THROW", 			1, 1, 0, 0,		OPFMT_BYTE);
	_spec[OP_IN]				= Spec("IN",				1, 2, 1, 11,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_INSTANCEOF]		= Spec("INSTANCEOF", 		1, 2, 1, 11,	OPFMT_BYTE | OPFMT_LEFTASSOC);
	_spec[OP_DEBUGGER]			= Spec("DEBUGGER", 			1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_GOSUB]				= Spec("GOSUB",				3, 0, 0, 0,		OPFMT_JUMP);
	_spec[OP_RETSUB]			= Spec("RETSUB", 			1, 2, 0, 0,		OPFMT_BYTE);
	_spec[OP_EXCEPTION]			= Spec("EXCEPTION", 		1, 0, 1, 0,		OPFMT_BYTE);
	_spec[OP_LINENO]			= Spec("LINENO",			3, 0, 0, 0,		OPFMT_UINT16);
	_spec[OP_CONDSWITCH]		= Spec("CONDSWITCH", 		1, 0, 0, 0,		OPFMT_BYTE | OPFMT_PARENHEAD);
	_spec[OP_CASE]				= Spec("CASE", 				3, 2, 1, 0,		OPFMT_JUMP);
	_spec[OP_DEFAULT]			= Spec("DEFAULT", 			3, 1, 0, 0,		OPFMT_JUMP);
	_spec[OP_EVAL]				= Spec("EVAL", 				3, -1, 1, 18,	OPFMT_UINT16 | OPFMT_INVOKE);
	_spec[OP_ENUMELEM]			= Spec("ENUMELEM", 			1, 3, 0, 3,		OPFMT_BYTE  | OPFMT_SET);
	_spec[OP_GETTER]			= Spec("GETTER", 			1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_SETTER]			= Spec("SETTER", 			1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_DEFFUN]			= Spec("DEFFUN", 			3, 0, 0, 0,		OPFMT_OBJECT | OPFMT_DECLARING);
	_spec[OP_DEFCONST]			= Spec("DEFCONST", 			3, 0, 0, 0,		OPFMT_ATOM | OPFMT_DECLARING);
	_spec[OP_DEFVAR]			= Spec("DEFVAR", 			3, 0, 0, 0,		OPFMT_ATOM | OPFMT_DECLARING);
	_spec[OP_LAMBDA]			= Spec("LAMBDA", 			3, 0, 1, 19,	OPFMT_OBJECT);
	_spec[OP_CALLEE]			= Spec("CALLEE", 			1, 0, 1, 19,	OPFMT_BYTE);
	_spec[OP_SETLOCALPOP]		= Spec("SETLOCALPOP", 		3, 1, 0, 3,		OPFMT_LOCAL | OPFMT_NAME | OPFMT_SET);
	_spec[OP_PICK]				= Spec("PICK", 				2, 0, 0, 0,		OPFMT_UINT8);
	_spec[OP_TRY]				= Spec("TRY",				1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_FINALLY]			= Spec("FINALLY",		 	1, 0, 2, 0,		OPFMT_BYTE);
	_spec[OP_GETDSLOT]			= Spec("GETDSLOT", 			3, 0, 1, 19,	OPFMT_UINT16 | OPFMT_NAME);
	_spec[OP_CALLDSLOT]			= Spec("CALLDSLOT", 		3, 0, 2, 19,	OPFMT_UINT16 | OPFMT_NAME | OPFMT_CALLOP);
	_spec[OP_ARGSUB]			= Spec("ARGSUB",			3, 0, 1, 18,	OPFMT_QARG  | OPFMT_NAME);
	_spec[OP_ARGCNT]			= Spec("ARGCNT",	 		1, 0, 1, 18,	OPFMT_BYTE);
	_spec[OP_DEFLOCALFUN]		= Spec("DEFLOCALFUN", 		5, 0, 0, 0,		OPFMT_SLOTOBJECT | OPFMT_DECLARING);
	_spec[OP_GOTOX]				= Spec("GOTOX",				5, 0, 0, 0,		OPFMT_JUMPX);
	_spec[OP_IFEQX]				= Spec("IFEQX",				5, 1, 0, 4,		OPFMT_JUMPX | OPFMT_DETECTING);
	_spec[OP_IFNEX]				= Spec("IFNEX",				5, 1, 0, 0,		OPFMT_JUMPX | OPFMT_PARENHEAD);
	_spec[OP_ORX]				= Spec("ORX",				5, 1, 0, 5,		OPFMT_JUMPX | OPFMT_DETECTING);
	_spec[OP_ANDX]				= Spec("ANDX",		 		5, 1, 0, 6,		OPFMT_JUMPX | OPFMT_DETECTING);
	_spec[OP_GOSUBX]			= Spec("GOSUBX",			5, 0, 0, 0,		OPFMT_JUMPX);
	_spec[OP_CASEX]				= Spec("CASEX",				5, 2, 1, 0,		OPFMT_JUMPX);
	_spec[OP_DEFAULTX]			= Spec("DEFAULTX",			5, 1, 0, 0,		OPFMT_JUMPX);
	_spec[OP_TABLESWITCHX]		= Spec("TABLESWITCHX",		-1, 1, 0, 0,	OPFMT_TABLESWITCHX | OPFMT_DETECTING | OPFMT_PARENHEAD);
	_spec[OP_LOOKUPSWITCHX]		= Spec("LOOKUPSWITCHX",		-1, 1, 0, 0,	OPFMT_LOOKUPSWITCHX | OPFMT_DETECTING | OPFMT_PARENHEAD);
	_spec[OP_BACKPATCH]			= Spec("BACKPATCH", 		3, 0, 0, 0,		OPFMT_JUMP | OPFMT_BACKPATCH);
	_spec[OP_BACKPATCH_POP]		= Spec("BACKPATCH_POP",		3, 1, 0, 0,		OPFMT_JUMP | OPFMT_BACKPATCH);
	_spec[OP_THROWING]			= Spec("THROWING",			1, 1, 0, 0,		OPFMT_BYTE);
	_spec[OP_SETRVAL]			= Spec("SETRVAL",			1, 1, 0, 2,		OPFMT_BYTE);
	_spec[OP_RETRVAL]			= Spec("RETRVAL",			1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_GETGVAR]			= Spec("GETGVAR",			3, 0, 1, 19,	OPFMT_ATOM | OPFMT_NAME);
	_spec[OP_SETGVAR]			= Spec("SETGVAR",			3, 1, 1, 3,		OPFMT_ATOM | OPFMT_NAME | OPFMT_SET | OPFMT_DETECTING);
	_spec[OP_INCGVAR]			= Spec("INCGVAR",			3, 0, 1, 15,	OPFMT_ATOM | OPFMT_NAME | OPFMT_INC | OPFMT_TMPSLOT2);
	_spec[OP_DECGVAR]			= Spec("DECGVAR",			3, 0, 1, 15,	OPFMT_ATOM | OPFMT_NAME | OPFMT_DEC | OPFMT_TMPSLOT2);
	_spec[OP_GVARINC]			= Spec("GVARINC",		 	3, 0, 1, 15,	OPFMT_ATOM | OPFMT_NAME | OPFMT_INC | OPFMT_POST | OPFMT_TMPSLOT2);
	_spec[OP_GVARDEC]			= Spec("GVARDEC",		 	3, 0, 1, 15,	OPFMT_ATOM | OPFMT_NAME | OPFMT_DEC | OPFMT_POST | OPFMT_TMPSLOT2);
	_spec[OP_REGEXP]			= Spec("REGEXP",			3, 0, 1, 19,	OPFMT_REGEXP);
	_spec[OP_DEFXMLNS]			= Spec("DEFXMLNS",			1, 1, 0, 0,		OPFMT_BYTE);
	_spec[OP_ANYNAME]			= Spec("ANYNAME", 			1, 0, 1, 19,	OPFMT_BYTE | OPFMT_XMLNAME);
	_spec[OP_QNAMEPART]			= Spec("QNAMEPART", 		3, 0, 1, 19,	OPFMT_ATOM | OPFMT_XMLNAME);
	_spec[OP_QNAMECONST]		= Spec("QNAMECONST", 		3, 1, 1, 19,	OPFMT_ATOM | OPFMT_XMLNAME);
	_spec[OP_QNAME]				= Spec("QNAME",				1, 2, 1, 0,		OPFMT_BYTE | OPFMT_XMLNAME);
	_spec[OP_TOATTRNAME]		= Spec("TOATTRNAME", 		1, 1, 1, 19,	OPFMT_BYTE | OPFMT_XMLNAME);
	_spec[OP_TOATTRVAL]			= Spec("TOATTRVAL", 		1, 1, 1, 19,	OPFMT_BYTE);
	_spec[OP_ADDATTRNAME]		= Spec("ADDATTRNAME", 		1, 2, 1, 13,	OPFMT_BYTE);
	_spec[OP_ADDATTRVAL]		= Spec("ADDATTRVAL", 		1, 2, 1, 13,	OPFMT_BYTE);
	_spec[OP_BINDXMLNAME]		= Spec("BINDXMLNAME", 		1, 1, 2, 3,		OPFMT_BYTE | OPFMT_SET);
	_spec[OP_SETXMLNAME]		= Spec("SETXMLNAME", 		1, 3, 1, 3,		OPFMT_BYTE | OPFMT_SET | OPFMT_DETECTING);
	_spec[OP_XMLNAME]			= Spec("XMLNAME",			1, 1, 1, 19,	OPFMT_BYTE);
	_spec[OP_DESCENDANTS]		= Spec("DESCENDANTS", 		1, 2, 1, 18,	OPFMT_BYTE);
	_spec[OP_FILTER]			= Spec("FILTER", 			3, 1, 1, 0,		OPFMT_JUMP);
	_spec[OP_ENDFILTER]			= Spec("ENDFILTER", 		3, 2, 1, 18,	OPFMT_JUMP);
	_spec[OP_TOXML]				= Spec("TOXML", 			1, 1, 1, 19,	OPFMT_BYTE);
	_spec[OP_TOXMLLIST]			= Spec("TOXMLLIST", 		1, 1, 1, 19,	OPFMT_BYTE);
	_spec[OP_XMLTAGEXPR]		= Spec("XMLTAGEXPR", 		1, 1, 1, 0,		OPFMT_BYTE);
	_spec[OP_XMLELTEXPR]		= Spec("XMLELTEXPR", 		1, 1, 1, 0,		OPFMT_BYTE);
	_spec[OP_XMLOBJECT]			= Spec("XMLOBJECT", 		3, 0, 1, 19,	OPFMT_OBJECT);
	_spec[OP_XMLCDATA]			= Spec("XMLCDATA",			3, 0, 1, 19,	OPFMT_ATOM);
	_spec[OP_XMLCOMMENT]		= Spec("XMLCOMMENT", 		3, 0, 1, 19,	OPFMT_ATOM);
	_spec[OP_XMLPI]				= Spec("XMLPI", 			3, 1, 1, 19,	OPFMT_ATOM);
	_spec[OP_CALLPROP]			= Spec("CALLPROP",			3, 1, 2, 18,	OPFMT_ATOM | OPFMT_PROP | OPFMT_CALLOP);
	_spec[OP_GETUPVAR]			= Spec("GETUPVAR", 			3, 0, 1, 19,	OPFMT_UINT16 | OPFMT_NAME);
	_spec[OP_CALLUPVAR]			= Spec("CALLUPVAR", 		3, 0, 2, 19,	OPFMT_UINT16 | OPFMT_NAME | OPFMT_CALLOP);
	_spec[OP_DELDESC]			= Spec("DELDESC", 			1, 2, 1, 15,	OPFMT_BYTE | OPFMT_ELEM | OPFMT_DEL);
	_spec[OP_UINT24]			= Spec("UINT24", 			4, 0, 1, 16,	OPFMT_UINT24);
	_spec[OP_INDEXBASE]			= Spec("ATOMBASE",			2, 0, 0, 0,		OPFMT_UINT8 | OPFMT_INDEXBASE);
	_spec[OP_RESETBASE]			= Spec("RESETBASE", 		1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_RESETBASE0]		= Spec("RESETBASE0", 		1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_STARTXML]			= Spec("STARTXML",			1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_STARTXMLEXPR]		= Spec("STARTXMLEXPR", 		1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_CALLELEM]			= Spec("CALLELEM",			1, 2, 2, 18,	OPFMT_BYTE  | OPFMT_ELEM | OPFMT_LEFTASSOC | OPFMT_CALLOP);
	_spec[OP_STOP]				= Spec("STOP",				1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_GETXPROP]			= Spec("GETXPROP",			3, 1, 1, 18,	OPFMT_ATOM | OPFMT_PROP);
	_spec[OP_CALLXMLNAME]		= Spec("CALLXMLNAME", 		1, 1, 2, 19,	OPFMT_BYTE | OPFMT_CALLOP);
	_spec[OP_TYPEOFEXPR]		= Spec("TYPEOFEXPR", 		1, 1, 1, 15,	OPFMT_BYTE | OPFMT_DETECTING);
	_spec[OP_ENTERBLOCK]		= Spec("ENTERBLOCK", 		3, 0, -1, 0,	OPFMT_OBJECT);
	_spec[OP_LEAVEBLOCK]		= Spec("LEAVEBLOCK", 		3, -1, 0, 0,	OPFMT_UINT16);
	_spec[OP_IFPRIMTOP]			= Spec("IFPRIMTOP", 		3, 1, 1, 0,		OPFMT_JUMP | OPFMT_DETECTING);
	_spec[OP_PRIMTOP]			= Spec("PRIMTOP",			2, 1, 1, 0,		OPFMT_INT8);
	_spec[OP_GENERATOR]			= Spec("GENERATOR", 		1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_YIELD]				= Spec("YIELD", 			1, 1, 1, 1,		OPFMT_BYTE);
	_spec[OP_ARRAYPUSH]			= Spec("ARRAYPUSH", 		3, 1, 0, 3,		OPFMT_LOCAL);
	_spec[OP_GETFUNNS]			= Spec("GETFUNNS",			1, 0, 1, 19,	OPFMT_BYTE);
	_spec[OP_ENUMCONSTELEM]		= Spec("ENUMCONSTELEM",		1, 3, 0, 3,		OPFMT_BYTE | OPFMT_SET);
	_spec[OP_LEAVEBLOCKEXPR]	= Spec("LEAVEBLOCKEXPR",	3, -1, 1, 3,	OPFMT_UINT16);
	_spec[OP_GETTHISPROP]		= Spec("GETTHISPROP", 		3, 0, 1, 18,	OPFMT_ATOM | OPFMT_VARPROP);
	_spec[OP_GETARGPROP]		= Spec("GETARGPROP", 		5, 0, 1, 18,	OPFMT_SLOTATOM | OPFMT_VARPROP);
	_spec[OP_GETLOCALPROP]		= Spec("GETLOCALPROP", 		5, 0, 1, 18,	OPFMT_SLOTATOM | OPFMT_VARPROP);
	_spec[OP_INDEXBASE1]		= Spec("INDEXBASE1", 		1, 0, 0, 0,		OPFMT_BYTE  | OPFMT_INDEXBASE);
	_spec[OP_INDEXBASE2]		= Spec("INDEXBASE2", 		1, 0, 0, 0,		OPFMT_BYTE  | OPFMT_INDEXBASE);
	_spec[OP_INDEXBASE3]		= Spec("INDEXBASE3", 		1, 0, 0, 0,		OPFMT_BYTE  | OPFMT_INDEXBASE);
	_spec[OP_CALLGVAR]			= Spec("CALLGVAR",			3, 0, 2, 19,	OPFMT_ATOM | OPFMT_NAME | OPFMT_CALLOP);
	_spec[OP_CALLLOCAL]			= Spec("CALLLOCAL", 		3, 0, 2, 19,	OPFMT_LOCAL | OPFMT_NAME | OPFMT_CALLOP);
	_spec[OP_CALLARG]			= Spec("CALLARG", 			3, 0, 2, 19,	OPFMT_QARG  | OPFMT_NAME | OPFMT_CALLOP);
	_spec[OP_CALLBUILTIN]		= Spec("CALLBUILTIN", 		3, 1, 2, 0,		OPFMT_UINT16);
	_spec[OP_INT8]				= Spec("INT8",				2, 0, 1, 16,	OPFMT_INT8);
	_spec[OP_INT32]				= Spec("INT32",				5, 0, 1, 16,	OPFMT_INT32);
	_spec[OP_LENGTH]			= Spec("LENGTH", 			1, 1, 1, 18,	OPFMT_BYTE | OPFMT_PROP);
	_spec[OP_NEWARRAY]			= Spec("NEWARRAY",	 		3, -1, 1, 19,	OPFMT_UINT16);
	_spec[OP_HOLE]				= Spec("HOLE",		 		1, 0, 1, 0,		OPFMT_BYTE);
	_spec[OP_DEFFUN_FC]			= Spec("DEFFUN_FC", 		3, 0, 0, 0,		OPFMT_OBJECT | OPFMT_DECLARING);
	_spec[OP_DEFLOCALFUN_FC]	= Spec("DEFLOCALFUN_FC",	5, 0, 0, 0,		OPFMT_SLOTOBJECT | OPFMT_DECLARING);
	_spec[OP_LAMBDA_FC]			= Spec("LAMBDA_FC", 		3, 0, 1, 19,	OPFMT_OBJECT);
	_spec[OP_OBJTOP]			= Spec("OBJTOP", 			3, 0, 0, 0,		OPFMT_UINT16);
	_spec[OP_LOOP]				= Spec("LOOP", 				1, 0, 0, 0,		OPFMT_BYTE);
	_spec[OP_GETUPVAR_DBG]		= Spec("GETUPVAR_DBG", 		3, 0, 1, 19,	OPFMT_UINT16 | OPFMT_NAME);
	_spec[OP_CALLUPVAR_DBG]		= Spec("CALLUPVAR_DBG",		3, 0, 2, 19,	OPFMT_UINT16 | OPFMT_NAME | OPFMT_CALLOP);
	_spec[OP_DEFFUN_DBGFC]		= Spec("DEFFUN_DBGFC", 		3, 0, 0, 0,		OPFMT_OBJECT | OPFMT_DECLARING);
	_spec[OP_DEFLOCALFUN_DBGFC]	= Spec("DEFLOCALFUN_DBGFC",	5, 0, 0, 0,		OPFMT_SLOTOBJECT | OPFMT_DECLARING);
	_spec[OP_LAMBDA_DBGFC]		= Spec("LAMBDA_DBGFC", 		3, 0, 1, 19,	OPFMT_OBJECT);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsOpcode::~EsOpcode()
{
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
