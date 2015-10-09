/***********************************************************************//**
 *	SsUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_UTILS_H_
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SoumaScriptUtils.h"

// Friends
#include "SoumaScriptKeywords.h"
#include "../../status/EleneStat.h"

// External
#include "../../../tfw/string/VcString.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SoumaScriptUtils メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	素種名を元素種別値に変換する
	※無効な文字列の場合は NUM_EL を返す
**//*---------------------------------------------------------------------*/
s32 SoumaScriptUtils::matchElementNameKeyword(const CStringBase* name, bool eqnoreCase)
{
	VcString s(*name);
	if(eqnoreCase)
	{
		s = s.toLower();
	}

	// ※本来は SoumaScriptKeywords の S_GENEW_xx_SOIL ～ S_GENEW_xx_AETHEL を見るべきであるが良く使われる単語のため最適化する
	// 　言語を追加した時はこの関数も変更しなければならない
	ASSERT(SoumaScriptKeywords::NUM_LANG == 2);

	// EL_SO (地) : "solo" or "soil"
	// EL_WA (水) : "neru" or "water"
	// EL_FI (火) : "fogo" or "fire"
	// EL WI (風) : "van" or "wind"
	// EL_AE (空) : "aeth" or "aethel"
	int len = s.getLength();
	if(len < 3) { return Elene::NUM_EL; }
	char c1 = s.getAt(0);
	char c2 = s.getAt(1);
	char c3 = s.getAt(2);
	switch(c1)
	{
	case 'a':
		if((len == 4) && (c2 == 'e') && (c3 == 't') && (s.getAt(3) == 'h'))
		{
			return Elene::EL_AE;
		}
		if((len == 6) && (c2 == 'e') && (c3 == 't') && (s.getAt(3) == 'h') && (s.getAt(4) == 'e') && (s.getAt(5) == 'l'))
		{
			return Elene::EL_AE;
		}
		break;
	case 'f':
		if(len == 4)
		{
			if((c2 == 'o') && (c3 == 'g') && (s.getAt(3) == 'o'))
			{
				return Elene::EL_FI;
			}
			if((c2 == 'i') && (c3 == 'r') && (s.getAt(3) == 'e'))
			{
				return Elene::EL_FI;
			}
		}
		break;
	case 'n':
		if((len == 4) && (c2 == 'e') && (c3 == 'r') && (s.getAt(3) == 'u'))
		{
			return Elene::EL_WA;
		}
		break;
	case 's':
		if(len == 4)
		{
			if(c2 == 'o')
			{
				if((c3 == 'l') && (s.getAt(3) == 'o'))
				{
					return Elene::EL_SO;
				}
				if((c3 == 'i') && (s.getAt(3) == 'l'))
				{
					return Elene::EL_SO;
				}
			}
		}
		break;
	case 'v':
		if((len == 3) && (c2 == 'a') && (c3 == 'n'))
		{
			return Elene::EL_WI;
		}
		break;
	case 'w':
		if((len == 4) && (c2 == 'i') && (c3 == 'n') && (s.getAt(3) == 'd'))
		{
			return Elene::EL_WI;
		}
		if((len == 5) && (c2 == 'a') && (c3 == 't') && (s.getAt(3) == 'e') && (s.getAt(4) == 'r'))
		{
			return Elene::EL_WA;
		}
		break;
	}
	return Elene::NUM_EL;
}

/*---------------------------------------------------------------------*//**
	英語での "point" 又は "points" に文字列一致したかを返す
	@retval 0 不一致
	@retval 1 point 相当に一致
	@retval 2 points 相当に一致
**//*---------------------------------------------------------------------*/
s32 SoumaScriptUtils::matchPointOrPointsKeyword(const CStringBase* str, bool eqnoreCase)
{
	VcString s(*str);
	if(eqnoreCase)
	{
		s = s.toLower();
	}

	// ※本来は SoumaScriptKeywords の S_GENEW_xx_POINT ～ S_GENEW_xx_POINTS を見るべきであるが良く使われる単語のため最適化する
	// 　言語を追加した時はこの関数も変更しなければならない
	ASSERT(SoumaScriptKeywords::NUM_LANG == 2);

	// S_GENEW_NS_POINT : "ment"
	// S_GENEW_EN_POINT : "point"
	// S_GENEW_NS_POINTS : "ments"
	// S_GENEW_EN_POINTS : "points"
	int len = s.getLength();
	if(len < 4) { return 0; }
	char c1 = s.getAt(0);
	switch(c1)
	{
	case 'm':
		if((s.getAt(1) == 'e') && (s.getAt(2) == 'n') && (s.getAt(3) == 't'))
		{
			if((len == 5) && (s.getAt(4) == 's'))
			{
				return 2;
			}
			return 1;
		}
		break;
	case 'p':
		if((len >= 5) && (s.getAt(1) == 'o') && (s.getAt(2) == 'i') && (s.getAt(3) == 'n') && (s.getAt(4) == 't'))
		{
			if((len == 6) && (s.getAt(5) == 's'))
			{
				return 2;
			}
			return 1;
		}
		break;
	}
	return 0;
}

/*---------------------------------------------------------------------*//**
	文字列を作用値に変換する
**//*---------------------------------------------------------------------*/
u32 SoumaScriptUtils::convOperationFlagFromString(const VcString* str)
{
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_PURIFY, str, true)) { return EleneStat::FF_OP_PURIFY; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_CURE, str, true)) { return EleneStat::FF_OP_CURE; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_CATHARSIS, str, true)) { return EleneStat::FF_OP_CATHARSIS; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_EXCEED, str, true)) { return EleneStat::FF_OP_EXCEED; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_WRAP, str, true)) { return EleneStat::FF_OP_WRAP; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_BARRIER, str, true)) { return EleneStat::FF_OP_BARRIER; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_SUFFUSE, str, true)) { return EleneStat::FF_OP_SUFFUSE; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_UNWRAP, str, true)) { return EleneStat::FF_OP_UNWRAP; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_UNBARRIER, str, true)) { return EleneStat::FF_OP_UNBARRIER; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_UNSUFFUSE, str, true)) { return EleneStat::FF_OP_UNSUFFUSE; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_ASCEN_S, str, true)) { return EleneStat::FF_OP_ASCEN_S; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_ASCEN_A, str, true)) { return EleneStat::FF_OP_ASCEN_A; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_DESCEN_S, str, true)) { return EleneStat::FF_OP_DESCEN_S; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_DESCEN_A, str, true)) { return EleneStat::FF_OP_DESCEN_A; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_ATTACK, str, true)) { return EleneStat::FF_OP_ATTACK; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_EXC_ATTACK, str, true)) { return EleneStat::FF_OP_EXC_ATTACK; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_UNW_ATTACK, str, true)) { return EleneStat::FF_OP_UNW_ATTACK; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_UNB_ATTACK, str, true)) { return EleneStat::FF_OP_UNB_ATTACK; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_UNS_ATTACK, str, true)) { return EleneStat::FF_OP_UNS_ATTACK; }
	if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_OP_NOP, str, true)) { return EleneStat::FF_OP_NOP; }
	return 0;
}

/*---------------------------------------------------------------------*//**
	指定元素機能とエネルギー量から元素バランスを算出する
**//*---------------------------------------------------------------------*/
bool SoumaScriptUtils::computeEleneBalanceFromIntent(Elene* eleneNqlOut, Elene* eleneCqlOut, u32 flagsFunc, s32 lfeneTotalSpec)
{
#if 1
	// 元素テーブル
	//
	// 作成不能なものは 0 で埋められている。
	// この元素並びは、ソウマ元素量シミュレーション.ods に合わせて、空風火水地であることに注意。
	static const s32 NUM_FORM = 4;
	static const s32 NUM_OP = 20;
	static const s32 NUM_QL = 2;
	static const s32 ELMTBL[NUM_FORM][NUM_OP][NUM_QL][Elene::NUM_EL] =
	{
		// FF_FORM_SOIL
		{
			// FF_OP_PURIFY
			{
				{ 1000, 1000, 1000, 1000, 1000 },
				{ 1000, 1010, 1020, 1050, 1100 },
			},
			// FF_OP_CURE
			{
				{ 1000, 1000, 1000, 1000, 1000 },
				{ 1000, 1000, 1000, 1020, 1040 },
			},
			// FF_OP_CATHARSIS
			{
				{ 1000, 1000, 1000, 1000, 1000 },
				{ 1000, 1000, 1000, 1000, 1000 },
			},
			// FF_OP_EXCEED
			{
				{ 1000, 1000, 1000, 1000, 1000 },
				{ 1000, 1000, 1000, 2000, 2000 },
			},
			// FF_OP_WRAP
			{
				{ 1000, 600, 800, 1200, 1800 },
				{ 1000, 600, 900, 1400, 1800 },
			},
			// FF_OP_BARRIER
			{
				{ 1000, 600, 800, 1200, 1800 },
				{ 1000, 600, 840, 1200, 1800 },
			},
			// FF_OP_SUFFUSE
			{
				{ 1000, 600, 800, 1200, 1800 },
				{ 1000, 600, 800, 1200, 1800 },
			},
			// FF_OP_UNWRAP
			{
				{ 1000, 80, 120, 1100, 1200 },
				{ 1000, 80, 120, 1550, 1900 },
			},
			// FF_OP_UNBARRIER
			{
				{ 1000, 80, 120, 1100, 1200 },
				{ 1000, 80, 120, 1900, 2750 },
			},
			// FF_OP_UNSUFFUSE
			{
				{ 1000, 80, 120, 1100, 1200 },
				{ 1000, 80, 120, 2940, 2880 },
			},
			// FF_OP_ASCEN_S
			{
				{ 1000, 2500, 1400, 100, 1000 },
				{ 1000, 2500, 1400, 200, 1000 },
			},
			// FF_OP_ASCEN_A
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_DESCEN_S
			{
				{ 1000, 2500, 1400, 100, 1000 },
				{ 1000, 2500, 1400, 1000, 1300 },
			},
			// FF_OP_DESCEN_A
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_ATTACK
			{
				{ 1000, 1000, 1000, 1000, 1000 },
				{ 1000, 1100, 1750, 1100, 1000 },
			},
			// FF_OP_EXC_ATTACK
			{
				{ 1000, 1000, 1000, 1000, 1000 },
				{ 1000, 1100, 1750, 2000, 2000 },
			},
			// FF_OP_UNW_ATTACK
			{
				{ 1000, 100, 100, 1100, 1200 },
				{ 1000, 110, 175, 1900, 1900 },
			},
			// FF_OP_UNB_ATTACK
			{
				{ 1000, 100, 100, 1100, 1200 },
				{ 1000, 110, 175, 1900, 2750 },
			},
			// FF_OP_UNS_ATTACK
			{
				{ 1000, 100, 100, 1100, 1200 },
				{ 1000, 110, 175, 2900, 2880 },
			},
			// FF_OP_NOP
			{
				{ 1000, 1000, 1000, 1000, 1100 },
				{ 1000, 1000, 1000, 1100, 1200 },
			},
		},
		// FF_FORM_WATER
		{
			// FF_OP_PURIFY
			{
				{ 1000, 1000, 1000, 1000, 500 },
				{ 1000, 1000, 1000, 1000, 550 },
			},
			// FF_OP_CURE
			{
				{ 1000, 1000, 1000, 1000, 100 },
				{ 1000, 1000, 1000, 1000, 100 },
			},
			// FF_OP_CATHARSIS
			{
				{ 1000, 1000, 1000, 1000, 980 },
				{ 1000, 1000, 1000, 1000, 980 },
			},
			// FF_OP_EXCEED
			{
				{ 1000, 1000, 1000, 1000, 500 },
				{ 1000, 1000, 1000, 1250, 1000 },
			},
			// FF_OP_WRAP
			{
				{ 1000, 100, 120, 1710, 980 },
				{ 1000, 100, 120, 1800, 1100 },
			},
			// FF_OP_BARRIER
			{
				{ 1000, 100, 120, 1710, 980 },
				{ 1000, 100, 120, 1740, 990 },
			},
			// FF_OP_SUFFUSE
			{
				{ 1000, 100, 120, 1710, 980 },
				{ 1000, 100, 120, 1710, 980 },
			},
			// FF_OP_UNWRAP
			{
				{ 1000, 100, 120, 1710, 980 },
				{ 1000, 100, 120, 1740, 2220 },
			},
			// FF_OP_UNBARRIER
			{
				{ 1000, 100, 120, 1710, 980 },
				{ 1000, 100, 120, 2400, 2880 },
			},
			// FF_OP_UNSUFFUSE
			{
				{ 1000, 100, 120, 1710, 980 },
				{ 1000, 100, 120, 2940, 2880 },
			},
			// FF_OP_ASCEN_S
			{
				{ 1000, 1800, 1400, 1000, 200 },
				{ 1000, 1800, 1400, 1000, 250 },
			},
			// FF_OP_ASCEN_A
			{
				{ 1000, 1800, 1400, 1000, 200 },
				{ 1000, 1800, 1400, 1000, 200 },
			},
			// FF_OP_DESCEN_S
			{
				{ 1000, 1800, 1400, 1000, 200 },
				{ 1000, 1800, 1400, 1320, 1000 },
			},
			// FF_OP_DESCEN_A
			{
				{ 1000, 1800, 1400, 1000, 200 },
				{ 1000, 1800, 1400, 1500, 1400 },
			},
			// FF_OP_ATTACK
			{
				{ 1000, 1000, 1000, 1000, 500 },
				{ 1000, 1100, 1750, 1100, 500 },
			},
			// FF_OP_EXC_ATTACK
			{
				{ 1000, 1000, 1000, 1000, 500 },
				{ 1000, 1100, 1750, 2000, 1000 },
			},
			// FF_OP_UNW_ATTACK
			{
				{ 1000, 100, 120, 1700, 980 },
				{ 1000, 100, 320, 1700, 2220 },
			},
			// FF_OP_UNB_ATTACK
			{
				{ 1000, 100, 120, 1700, 980 },
				{ 1000, 100, 320, 2400, 2880 },
			},
			// FF_OP_UNS_ATTACK
			{
				{ 1000, 100, 120, 1700, 980 },
				{ 1000, 100, 320, 2940, 2880 },
			},
			// FF_OP_NOP
			{
				{ 1000, 1000, 1000, 1000, 980 },
				{ 1000, 1000, 1000, 1100, 1100 },
			},
		},
		// FF_FORM_FIRE
		{
			// FF_OP_PURIFY
			{
				{ 1000, 1000, 1000, 800, 400 },
				{ 1000, 1000, 1000, 800, 430 },
			},
			// FF_OP_CURE
			{
				{ 1000, 1000, 1000, 800, 250 },
				{ 1000, 1000, 1000, 800, 250 },
			},
			// FF_OP_CATHARSIS
			{
				{ 1000, 1000, 1000, 980, 980 },
				{ 1000, 1000, 1000, 980, 980 },
			},
			// FF_OP_EXCEED
			{
				{ 1000, 1000, 1000, 800, 400 },
				{ 1000, 1000, 1000, 1000, 780 },
			},
			// FF_OP_WRAP
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_BARRIER
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_SUFFUSE
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_UNWRAP
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_UNBARRIER
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_UNSUFFUSE
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_ASCEN_S
			{
				{ 1000, 1000, 1000, 700, 340 },
				{ 1000, 1050, 1000, 710, 360 },
			},
			// FF_OP_ASCEN_A
			{
				{ 1000, 1000, 1000, 700, 340 },
				{ 1000, 1000, 1000, 700, 340 },
			},
			// FF_OP_DESCEN_S
			{
				{ 1000, 1000, 1000, 700, 340 },
				{ 1000, 1000, 1000, 1000, 1050 },
			},
			// FF_OP_DESCEN_A
			{
				{ 1000, 1000, 1000, 700, 340 },
				{ 1000, 1000, 1000, 1100, 1450 },
			},
			// FF_OP_ATTACK
			{
				{ 1000, 1000, 1000, 800, 400 },
				{ 1000, 1000, 1800, 1000, 400 },
			},
			// FF_OP_EXC_ATTACK
			{
				{ 1000, 1000, 1000, 800, 400 },
				{ 1000, 1000, 1800, 1000, 780 },
			},
			// FF_OP_UNW_ATTACK
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_UNB_ATTACK
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_UNS_ATTACK
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_NOP
			{
				{ 1000, 1000, 1000, 800, 400 },
				{ 1000, 1000, 1000, 850, 500 },
			},
		},
		// FF_FORM_WIND
		{
			// FF_OP_PURIFY
			{
				{ 1000, 1000, 900, 700, 400 },
				{ 1000, 1000, 900, 710, 450 },
			},
			// FF_OP_CURE
			{
				{ 1000, 1000, 800, 600, 400 },
				{ 1000, 1000, 800, 600, 400 },
			},
			// FF_OP_CATHARSIS
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_EXCEED
			{
				{ 1000, 1000, 900, 700, 400 },
				{ 1000, 1000, 1000, 1000, 700 },
			},
			// FF_OP_WRAP
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_BARRIER
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_SUFFUSE
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_UNWRAP
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_UNBARRIER
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_UNSUFFUSE
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_ASCEN_S
			{
				{ 1000, 1000, 800, 600, 400 },
				{ 1000, 1050, 800, 600, 400 },
			},
			// FF_OP_ASCEN_A
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_DESCEN_S
			{
				{ 1000, 1000, 800, 600, 400 },
				{ 1000, 1000, 800, 1000, 1000 },
			},
			// FF_OP_DESCEN_A
			{
				{ 1000, 1000, 800, 600, 400 },
				{ 1000, 1000, 800, 1250, 1250 },
			},
			// FF_OP_ATTACK
			{
				{ 1000, 1000, 900, 700, 400 },
				{ 1000, 1000, 1660, 720, 400 },
			},
			// FF_OP_EXC_ATTACK
			{
				{ 1000, 1000, 900, 700, 400 },
				{ 1000, 1000, 1660, 1000, 700 },
			},
			// FF_OP_UNW_ATTACK
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_UNB_ATTACK
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_UNS_ATTACK
			{
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
			},
			// FF_OP_NOP
			{
				{ 1000, 1000, 900, 700, 400 },
				{ 1000, 1000, 1000, 800, 500 },
			},
		},
	};
	static const u32 ATRBTBL[Elene::NUM_EL] =
	{
		EleneStat::FF_ATRB_SOIL,
		EleneStat::FF_ATRB_WATER,
		EleneStat::FF_ATRB_FIRE,
		EleneStat::FF_ATRB_WIND,
		EleneStat::FF_ATRB_AETHEL,
	};

	s32 iop = flagsFunc & EleneStat::FF_MASK_OP;
	if(iop >= EleneStat::FF_OP_ATTACK)
	{
		switch(iop)
		{
		case EleneStat::FF_OP_ATTACK: iop = 14; break;
		case EleneStat::FF_OP_EXC_ATTACK: iop = 15; break;
		case EleneStat::FF_OP_UNW_ATTACK: iop = 16; break;
		case EleneStat::FF_OP_UNB_ATTACK: iop = 17; break;
		case EleneStat::FF_OP_UNS_ATTACK: iop = 18; break;
		case EleneStat::FF_OP_NOP: iop = 19; break;
		}
	}
	else
	{
		iop--;
	}
	ASSERT((0 <= iop) && (iop < NUM_OP));

	s32 iform = (flagsFunc & EleneStat::FF_MASK_FORM);
	switch(flagsFunc & EleneStat::FF_MASK_FORM)
	{
	case EleneStat::FF_FORM_SOIL: iform = 0; break;
	case EleneStat::FF_FORM_WATER: iform = 1; break;
	case EleneStat::FF_FORM_FIRE: iform = 2; break;
	case EleneStat::FF_FORM_WIND: iform = 3; break;
	}
	ASSERT((0 <= iform) && (iform < NUM_FORM));

	// まずは標準値で作成
	Elene eleneNqlWork(ELMTBL[iform][iop][0][4], ELMTBL[iform][iop][0][3], ELMTBL[iform][iop][0][2], ELMTBL[iform][iop][0][1], ELMTBL[iform][iop][0][0]);
	Elene eleneCqlWork(ELMTBL[iform][iop][1][4], ELMTBL[iform][iop][1][3], ELMTBL[iform][iop][1][2], ELMTBL[iform][iop][1][1], ELMTBL[iform][iop][1][0]);

	// 必要な質を求める
	u32 qfSpec = EleneStat::calcOperationFlagToQualityFlags(flagsFunc);

	if(eleneNqlWork.getAethel() <= 0)
	{
		return false;	// 作成不可
	}

	// 属性を反映
	for(int iel = 0; iel < Elene::NUM_EL; iel++)
	{
		if(flagsFunc & ATRBTBL[iel])
		{
			s32 target = eleneNqlWork.get(iel) * 2;

			// 元素過大を防ぐ
			s32 limit = (eleneNqlWork.getAethel() * (EleneStat::PERCENT_OVERBREAK - 1)) / 100;
			if(target > limit)
			{
				target = limit;
			}

			if(target > eleneCqlWork.get(iel))
			{
				eleneCqlWork.set(iel, target);
			}
		}
	}

	// 質を維持する（現状は純性のみ維持、質の等級（+1, +2）は問わない）
	if(TFW_IS_FLAG(qfSpec, EleneStat::QF_MASK_PURE))
	{
		// 複数回計算して、目標の質に近づける
		for(int j = 0; j < 10; j++)
		{
			u16 rateExct, rateDull;
			u32 qfWork = EleneStat::calcQualityFlags(0L, &rateExct, &rateDull, &eleneCqlWork, &eleneNqlWork);
			if(TFW_IS_FLAG(qfWork, EleneStat::QF_MASK_PURE))
			{
				break;	// 問題なし
			}
			if(TFW_IS_FLAG(qfWork, EleneStat::QF_MASK_EXCT))
			{
				f32 r = (rateExct + 1 - EleneStat::PERCENT_UNPURE) * 0.01f;	// 減算率
				s32 cwa = eleneCqlWork.getWater();
				s32 cfi = eleneCqlWork.getFire();
				s32 cwi = eleneCqlWork.getWind();
				// EleneStat::calcQualityFlags の s32 eC = (cwi / 2) + cfi + (cwa / 8); の計算式から想定される影響分を減算する
				cwa -= (s32)(cwa / 8 * r);
				cfi -= (s32)(cfi * r);
				cwi -= (s32)(cwi / 2 * r);
				// 未成熟にならないように範囲クリップ
				if(cwa < eleneNqlWork.getWater()) { cwa = eleneNqlWork.getWater(); }
				if(cfi < eleneNqlWork.getFire()) { cfi = eleneNqlWork.getFire(); }
				if(cwi < eleneNqlWork.getWind()) { cwi = eleneNqlWork.getWind(); }
				// 新しい値を設定
				eleneCqlWork.setWater(cwa);
				eleneCqlWork.setFire(cfi);
				eleneCqlWork.setWind(cwi);
			}
			if(TFW_IS_FLAG(qfWork, EleneStat::QF_MASK_DULL))
			{
				f32 r = (rateDull + 1 - EleneStat::PERCENT_UNPURE) * 0.01f;	// 減算率
				s32 cso = eleneCqlWork.getSoil();
				s32 cwa = eleneCqlWork.getWater();
				// EleneStat::calcQualityFlags の s32 dC = (cwa * 7 / 8) + cso; の計算式から想定される影響分を減算する
				cso -= (s32)(cso * r);
				cwa -= (s32)(cwa * 7 / 8 * r);
				// 未成熟にならないように範囲クリップ
				if(cso < eleneNqlWork.getSoil()) { cso = eleneNqlWork.getSoil(); }
				if(cwa < eleneNqlWork.getWater()) { cwa = eleneNqlWork.getWater(); }
				// 新しい値を設定
				eleneCqlWork.setSoil(cso);
				eleneCqlWork.setWater(cwa);
			}
		}
	}

	// 複数回計算して、目標消費エネルギーに近づける
	for(int j = 0; j < 10; j++)
	{
		s32 lfeneCost = eleneCqlWork.getSoumaMaterializeLifeEnergy();
		if(lfeneCost == 0) { break; }	// エラー
		f32 scale = lfeneTotalSpec / (f32)lfeneCost;
		TRACE("{SoumaScriptUtils::computeEleneBalanceFromIntent} [%d] cost=%d/%d, s=%f\n", j, lfeneCost, lfeneTotalSpec, scale);
		if((1.0f <= scale) && (scale <= 1.01f)) { break; }	// OK!
		for(int iel = 0; iel < Elene::NUM_EL; iel++)
		{
			eleneNqlWork.set(iel, (s32)(eleneNqlWork.get(iel) * scale));
			eleneCqlWork.set(iel, (s32)(eleneCqlWork.get(iel) * scale));
		}
	}
	TRACE("{SoumaScriptUtils::computeEleneBalanceFromIntent} -> cost=%d/%d\n", eleneCqlWork.getSoumaMaterializeLifeEnergy(), lfeneTotalSpec);

	eleneNqlOut->copy(&eleneNqlWork);
	eleneCqlOut->copy(&eleneCqlWork);
	return true;


#elif 1

	// まずは標準値で作成
	Elene eleneNqlWork;
	Elene eleneCqlWork;

	switch (flagFunc & EleneStat::FF_MASK_OP)
	{
	case EleneStat::FF_OP_PURIFY:		// 純化
		eleneNqlWork.set(1000, 1000, 1000, 1000, 1000);
		eleneCqlWork.set(1000, 1200, 1010, 1000, 1050);
		break;
	case EleneStat::FF_OP_CURE:			// 回復
		eleneNqlWork.set(1000, 1000, 1000, 1000, 1000);
		eleneCqlWork.set(1000, 1000, 1010, 1020, 1040);
		break;
	case EleneStat::FF_OP_CATHARSIS:	// 浄化（純化 + 回復）
		eleneNqlWork.set(1000, 1000, 1000, 1000, 1000);
		eleneCqlWork.set(1000, 1000, 1000, 1000, 1000);
		break;
	case EleneStat::FF_OP_EXCEED:		// 増悪
		eleneNqlWork.set(1000, 1000, 1000, 500, 500);
		eleneCqlWork.set(1000, 1000, 1000, 1000, 1000);
		break;
	case EleneStat::FF_OP_WRAP:			// 持続結界強化
		eleneNqlWork.set(1000,  600,  800, 1200, 1800);
		eleneCqlWork.set(1000,  700,  900, 1200, 1800);
		break;
	case EleneStat::FF_OP_BARRIER:		// 瞬間結界強化
		eleneNqlWork.set(1000, 600, 800, 1200, 1800);
		eleneCqlWork.set(1000, 600, 900, 1200, 1800);
		break;
	case EleneStat::FF_OP_SUFFUSE:		// 両結界強化
		eleneNqlWork.set(1000, 600, 800, 1200, 1800);
		eleneCqlWork.set(1000, 600, 800, 1200, 1800);
		break;
	case EleneStat::FF_OP_UNWRAP:		// 持続結界弱体
		eleneNqlWork.set(1000, 100, 100, 1100, 1200);
		eleneCqlWork.set(1000, 100, 100, 1550, 1900);
		break;
	case EleneStat::FF_OP_UNBARRIER:	// 瞬間結界弱体
		eleneNqlWork.set(1000, 100, 100, 1100, 1200);
		eleneCqlWork.set(1000, 100, 100, 1900, 2750);
		break;
	case EleneStat::FF_OP_UNSUFFUSE:	// 両結界弱体（持続結界弱体 + 瞬間結界弱体）
		eleneNqlWork.set(1000, 100, 100, 1100, 1200);
		eleneCqlWork.set(1000, 100, 100, 1900, 3750);
		break;
	case EleneStat::FF_OP_ASCEN_S:		// 単元素アセンション
	case EleneStat::FF_OP_ASCEN_A:		// 全体アセンション
	case EleneStat::FF_OP_DESCEN_S:		// 単元素ディセンション
	case EleneStat::FF_OP_DESCEN_A:		// 全体ディセンション
		break;
	case EleneStat::FF_OP_ATTACK:		// 攻撃
	case EleneStat::FF_OP_EXC_ATTACK:	// 増悪攻撃（増悪 + 攻撃）
	case EleneStat::FF_OP_UNW_ATTACK:	// 持続結界弱体攻撃（持続結界弱体 + 攻撃）
	case EleneStat::FF_OP_UNB_ATTACK:	// 瞬間結界弱体攻撃（瞬間結界弱体 + 攻撃）
	case EleneStat::FF_OP_UNS_ATTACK:	// 両結界弱体攻撃（両結界弱体 + 攻撃）
	case EleneStat::FF_OP_MISS_ATTACK:	// 攻撃ミス
	case EleneStat::FF_OP_NOP:			// エフェクト（対決時は特に効果なし）
	case EleneStat::FF_OP_IMMATURE:		// 元素過小（未成熟）
	case EleneStat::FF_OP_OVERBREAK:	// 元素過大（破裂）
		break;
	}

	eleneNqlOut->copy(&eleneNqlWork);
	eleneCqlOut->copy(&eleneCqlWork);
	return true;

#else
	bool isSucceeded = false;

	// 条件を得る
	u32 qfSpec = EleneStat::calcOperationFlagToQualityFlags(flagFunc);
	u32 qfExctSpec = qfSpec & EleneStat::QF_MASK_EXCT;
	u32 qfDullSpec = qfSpec & EleneStat::QF_MASK_DULL;
	f32 eneEqOneF = eneTotalSpec * 0.2f;	// ÷5
	s32 eneEqOneI = (s32)eneEqOneF;

	// 試し変化量を設定する
	s32 eeDelta = eneEqOneI / 100;
	if (eeDelta <= 0)	{ eeDelta = 1; }

	// 元素バランスを作成する
	eleneNql->set(eneEqOneI, eneEqOneI, eneEqOneI, eneEqOneI, eneEqOneI);	// まずは均等に
	for (int safecnt = 0; safecnt < 100; safecnt++)
	{
		eleneCql->copy(eleneNql);
		s32 eneRest = eneTotalSpec - eleneCql->sum();

		do
		{
			u32 qfWk = EleneStat::calcQualityFlags(0L, 0L, 0L, eleneCql, eleneNql);
			if (qfWk == qfSpec)	// 指定の質を作成できた
			{
				isSucceeded = true;
				break;
			}

			// 余りエネルギー（eneRest）分で調整
			if (eneRest <= 0)
			{
				break;	// 調整できず
			}
			if ((EleneStat::QF_PURE_1 <= qfSpec) && (qfSpec <= EleneStat::QF_PURE_3))
			{
				eleneCql->addSoil(eeDelta);	// 地を増悪させて純性度を落とす
			}
			else
			{
				u32 qfExctWk = qfWk & EleneStat::QF_MASK_EXCT;
				if (qfExctWk != qfExctSpec)
				{
					if ((EleneStat::QF_EXCT_1 <= qfExctSpec) && (qfExctSpec <= EleneStat::QF_EXCT_3))
					{
						eleneCql->addFire(eeDelta);		// 火を増悪させて激性度を上げる
					}
				}
				else
				{
					u32 qfDullWk = qfWk & EleneStat::QF_MASK_DULL;
					if (qfDullWk != qfDullSpec)
					{
						if ((EleneStat::QF_DULL_1 <= qfDullSpec) && (qfDullSpec <= EleneStat::QF_DULL_3))
						{
							eleneCql->addSoil(eeDelta);	// 地を増悪させて鈍性度を上げる
						}
					}
				}
			}
			eneRest--;
		} while (true);
		if (isSucceeded)	{ break; }	// バランス確定済み

		// 全体の原質を下げて調整しやすくする
		eleneNql->subSoil(eeDelta);
		eleneNql->subWater(eeDelta);
		eleneNql->subFire(eeDelta);
		eleneNql->subWind(eeDelta);
		eleneNql->subAethel(eeDelta);
		// 激性が求められている場合は、原質の火を落として調整しやすくする
		if ((EleneStat::QF_EXCT_1 <= qfExctSpec) && (qfExctSpec <= EleneStat::QF_EXCT_3))
		{
			eleneNql->subFire(eeDelta);
		}
		// 鈍性が求められている場合は、原質の地を落として調整しやすくする
		if ((EleneStat::QF_DULL_1 <= qfDullSpec) && (qfDullSpec <= EleneStat::QF_DULL_3))
		{
			eleneNql->subSoil(eeDelta);
		}
	}

	return isSucceeded;
#endif
}

/*---------------------------------------------------------------------*//**
	指定元素機能とエネルギー量から元素バランスを算出する
**//*---------------------------------------------------------------------*/
bool SoumaScriptUtils::computeEleneBalanceAttributeAdded(Elene* eleneNql, Elene* eleneCql, u32 flagsAtrb)
{
	flagsAtrb &= EleneStat::FF_MASK_ATRB;
	if(flagsAtrb == 0)
	{
		return false;
	}

	const u32 afs[Elene::NUM_EL] = { EleneStat::FF_ATRB_SOIL, EleneStat::FF_ATRB_WATER, EleneStat::FF_ATRB_FIRE, EleneStat::FF_ATRB_WIND, EleneStat::FF_ATRB_AETHEL };
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		if(!TFW_IS_FLAG(flagsAtrb, afs[i])) { continue; }
		s32 tc = eleneNql->get(i) * 2;
		if(tc > eleneCql->get(i))
		{
			eleneCql->set(i, tc);
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
