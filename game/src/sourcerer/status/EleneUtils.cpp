/***********************************************************************//**
 *	Elene.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2014/09/08.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EleneUtils.h"

// Friends
#include "EleneStat.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EleneUtils メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

#if defined(_DEBUG)

/*---------------------------------------------------------------------*//**
	機能フラグの作用文字列を得る
**//*---------------------------------------------------------------------*/
void EleneUtils::_debug_toQualityStringFromQualityFlags(VcString* str, u32 flags)
{
	s32 ipn = -1;
	switch(flags & EleneStat::QF_MASK_PN)
	{
	case EleneStat::QF_POS_1:	ipn = 0;	break;	// 陽性
	case EleneStat::QF_POS_2:	ipn = 1;	break;	// 極陽性
	case EleneStat::QF_MOD_1:	ipn = 2;	break;	// 中性
	case EleneStat::QF_MOD_2:	ipn = 3;	break;	// 極中性
	case EleneStat::QF_NEG_1:	ipn = 4;	break;	// 陰性
	case EleneStat::QF_NEG_2:	ipn = 5;	break;	// 極陰性
	}
	s32 ip = -1;
	switch(flags & EleneStat::QF_MASK_PURE)
	{
	case EleneStat::QF_PURE_1:	ip = 0;	break;	// 純性
	case EleneStat::QF_PURE_2:	ip = 1;	break;	// 純性+1
	case EleneStat::QF_PURE_3:	ip = 2;	break;	// 純性+1
	}
	s32 ie = -1;
	switch(flags & EleneStat::QF_MASK_EXCT)
	{
	case EleneStat::QF_EXCT_1:	ie = 0;	break;	// 激性
	case EleneStat::QF_EXCT_2:	ie = 1;	break;	// 激性+1
	case EleneStat::QF_EXCT_3:	ie = 2;	break;	// 激性+1
	}
	s32 id = -1;
	switch(flags & EleneStat::QF_MASK_DULL)
	{
	case EleneStat::QF_DULL_1:	id = 0;	break;	// 鈍性
	case EleneStat::QF_DULL_2:	id = 1;	break;	// 鈍性+1
	case EleneStat::QF_DULL_3:	id = 2;	break;	// 鈍性+1
	}
	if(ipn != -1)
	{
		const char* NAMES[] =
		{
			"+", "++", "*", "**", "-", "--"
		};
		str->set(NAMES[ipn]);
	}
	if(ip != -1)
	{
		const char* NAMES[] =
		{
			"P1", "P2", "P3"
		};
		str->add(NAMES[ip]);
	}
	if(ie != -1)
	{
		const char* NAMES[] =
		{
			"E1", "E2", "E3"
		};
		str->add(NAMES[ie]);
	}
	if(id != -1)
	{
		const char* NAMES[] =
		{
			"D1", "D2", "D3"
		};
		str->add(NAMES[id]);
	}
}

/*---------------------------------------------------------------------*//**
	機能フラグの作用文字列を得る
**//*---------------------------------------------------------------------*/
void EleneUtils::_debug_toOpStringFromFuncFlags(VcString* str, u32 flags)
{
	const u32 FLAGS[] =
	{
		EleneStat::FF_OP_PURIFY,
		EleneStat::FF_OP_CURE,
		EleneStat::FF_OP_CATHARSIS,
		EleneStat::FF_OP_EXCEED,
		EleneStat::FF_OP_WRAP,
		EleneStat::FF_OP_BARRIER,
		EleneStat::FF_OP_SUFFUSE,
		EleneStat::FF_OP_UNWRAP,
		EleneStat::FF_OP_UNBARRIER,
		EleneStat::FF_OP_UNSUFFUSE,
		EleneStat::FF_OP_ASCEN_S,
		EleneStat::FF_OP_ASCEN_A,
		EleneStat::FF_OP_DESCEN_S,
		EleneStat::FF_OP_DESCEN_A,
		EleneStat::FF_OP_ATTACK,
		EleneStat::FF_OP_EXC_ATTACK,
		EleneStat::FF_OP_UNW_ATTACK,
		EleneStat::FF_OP_UNB_ATTACK,
		EleneStat::FF_OP_UNS_ATTACK,
		EleneStat::FF_OP_NOP,
		EleneStat::FF_OP_IMMATURE,
		EleneStat::FF_OP_OVERBREAK,
	};
	const char* NAMES[] =
	{
		"PUR",
		"CUR",
		"CAT",
		"EXC",
		"WRP",
		"BAR",
		"SUF",
		"UNW",
		"UNB",
		"UNS",
		"ACS",
		"ACA",
		"DCS",
		"DCA",
		"ATT",
		"EXA",
		"UWA",
		"UBA",
		"USA",
		"NOP",
		"IMM",
		"OVR",
	};
	u32 f = flags & EleneStat::FF_MASK_OP;
	for(int i = 0; i < (sizeof(FLAGS) / sizeof(u32)); i++)
	{
		if(FLAGS[i] == f)
		{
			str->set(NAMES[i]);
			return;
		}
	}
	str->set("---");
}

/*---------------------------------------------------------------------*//**
	機能フラグの属性文字列を得る
**//*---------------------------------------------------------------------*/
void EleneUtils::_debug_toAtrbStringFromFuncFlags(VcString* str, u32 flags)
{
	const u32 FLAGS[] =
	{
		EleneStat::FF_ATRB_SOIL,
		EleneStat::FF_ATRB_WATER,
		EleneStat::FF_ATRB_FIRE,
		EleneStat::FF_ATRB_WIND,
		EleneStat::FF_ATRB_AETHEL,
	};
	const char* NAMES[] =
	{
		"SO",
		"WA",
		"FI",
		"WI",
		"AE",
	};
	u32 f = flags & EleneStat::FF_MASK_ATRB;
	str->emptyEasy();
	for(int i = 0; i < (sizeof(FLAGS) / sizeof(u32)); i++)
	{
		if(TFW_IS_FLAG(f, FLAGS[i]))
		{
			str->add(NAMES[i]);
		}
	}
}

/*---------------------------------------------------------------------*//**
	形態フラグの属性文字列を得る
**//*---------------------------------------------------------------------*/
void EleneUtils::_debug_toFormStringFromFuncFlags(VcString* str, u32 flags)
{
	const u32 FLAGS[] =
	{
		EleneStat::FF_FORM_SOIL,
		EleneStat::FF_FORM_WATER,
		EleneStat::FF_FORM_FIRE,
		EleneStat::FF_FORM_WIND,
	};
	const char* NAMES[] =
	{
		"SO",
		"WA",
		"FI",
		"WI",
	};
	u32 f = flags & EleneStat::FF_MASK_FORM;
	for(int i = 0; i < (sizeof(FLAGS) / sizeof(u32)); i++)
	{
		if(FLAGS[i] == f)
		{
			str->set(NAMES[i]);
			return;
		}
	}
	str->set("--");
}
#endif

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
