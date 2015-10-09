/***********************************************************************//**
 *	EleneStat.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EleneStat.h"

// Friends
#include "EleneVeil.h"
#include "Nature.h"

// External
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////
// マクロ

#define _ENLIGHT_V01_3_ABOVE	1

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 質フラグと機能フラグの変換テーブル
const u32 EleneStat::s_foptbl[2][3][4] =
{
	//		極陽性,				極中性,				極陰性				その他
	// 純性
	{
		{	FF_OP_WRAP,			FF_OP_PURIFY,		FF_OP_NOP,			FF_OP_NOP,		},	// 純正
		{	FF_OP_BARRIER,		FF_OP_CURE,			FF_OP_ASCEN_S,		FF_OP_PURIFY,	},	// 純正+1
		{	FF_OP_SUFFUSE,		FF_OP_CATHARSIS,	FF_OP_CURE,			FF_OP_CURE,		},	// 純正+2
	},
	// 鈍性
	{
		{	FF_OP_UNWRAP,		FF_OP_NOP,			FF_OP_NOP,			FF_OP_EXCEED,	},	// 鈍性
		{	FF_OP_UNBARRIER,	FF_OP_EXCEED,		FF_OP_DESCEN_S,		FF_OP_EXCEED,	},	// 鈍性+1
		{	FF_OP_UNSUFFUSE,	FF_OP_EXCEED,		FF_OP_EXCEED,		FF_OP_EXCEED,	},	// 鈍性+2
	}
};

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EleneStat メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	初期値（全てゼロ）かどうかを得る
**//*---------------------------------------------------------------------*/
bool EleneStat::isInitialValue() const
{
	if(!_eleneOql.isInitialValue())	{	return false;	}
	if(!_eleneNql.isInitialValue())	{	return false;	}
	if(!_eleneCql.isInitialValue())	{	return false;	}
	if(_shift != 0)	{	return false;	}
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	質の計算
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
u32 EleneStat::calcQualityFlags(u16* ratePure, u16* rateExct, u16* rateDull, const Elene* eleneCql, const Elene* eleneNql)
{
	u32 qf = 0;
	s32 nso = eleneNql->getSoil();
	s32 nwa = eleneNql->getWater();
	s32 nfi = eleneNql->getFire();
	s32 nwi = eleneNql->getWind();
	s32 nae = eleneNql->getAethel();
	s32 cso = eleneCql->getSoil();
	s32 cwa = eleneCql->getWater();
	s32 cfi = eleneCql->getFire();
	s32 cwi = eleneCql->getWind();
	//s32 cae = eleneCql->getAethel();

	// 純性値
	s32 pN = nwi + (nfi * 2) + nwa + (nso * 2);
	s32 pC = cwi + (cfi * 2) + cwa + (cso * 2);
	if(pN <= 0)	{	return 0;	}
	s32 pR = pC * 100 / pN;

	// 激性値
	s32 eN = (nwi / 2) + nfi + (nwa / 8); 
	s32 eC = (cwi / 2) + cfi + (cwa / 8);
	s32 eR = eC * 100 / eN;

	// 鈍性値
	s32 dN = (nwa * 7 / 8) + nso;
	s32 dC = (cwa * 7 / 8) + cso;
	s32 dR = dC * 100 / dN;

	// 質判定
	if((eR >= PERCENT_UNPURE) || (dR >= PERCENT_UNPURE))
	{
		if(eR >= 250)		{	qf = QF_EXCT_3;		}
		else if(eR >= 200)	{	qf = QF_EXCT_2;		}
		else if(eR >= 150)	{	qf = QF_EXCT_1;		}
		if(dR >= 250)		{	qf |= QF_DULL_3;	}
		else if(dR >= 200)	{	qf |= QF_DULL_2;	}
		else if(dR >= 150)	{	qf |= QF_DULL_1;	}
	}
	else
	{
		if(pR == 100)						{	qf = QF_PURE_3;		}
		else if((96 <= pR) && (pR <= 104))	{	qf = QF_PURE_2;		}
		else								{	qf = QF_PURE_1;		}
	}

	// 陰陽判定
	f32 pn =
		(f32)(
			(  ( (nso - nwa) + (nwa - nfi) + (nfi - nwi) + (nwi - nae) ) * 4  ) +
			(  ( (nso - nfi) + (nwa - nwi) + (nfi - nae) ) * 3  ) +
			(  ( (nso - nwi) + (nwa - nae) ) * 2  ) +
			(  ( (nso - nae) )  )
		) / (f32)(nso + nwa + nfi + nwi + nae);
	if(pn >= 2.0f)								{	qf |= QF_POS_2;	}
	else if(pn >= 1.0f)							{	qf |= QF_POS_1;	}
	else if(pn <= -2.0f)						{	qf |= QF_NEG_2;	}
	else if(pn <= -1.0f)						{	qf |= QF_NEG_1;	}
	else if(TFW_IS_RANGE(pn, -0.001f, 0.001f))	{	qf |= QF_MOD_2;	}
	else										{	qf |= QF_MOD_1;	}

	if(ratePure != 0L)	{	*ratePure = (u16)pR;	}
	if(rateExct != 0L)	{	*rateExct = (u16)eR;	}
	if(rateDull != 0L)	{	*rateDull = (u16)dR;	}

	return qf;
}

/*---------------------------------------------------------------------*//**
	五元素バランスによる機能フラグを算出

	@param qulityfCalced 計算済みの質（計算済みでない場合は 0 を指定）
	@param isSouma ソウマかどうか
**//*---------------------------------------------------------------------*/
u32 EleneStat::calcFuncFlags(u32 qulityfCalced, const Elene* eleneCql, const Elene* eleneNql, bool isSouma)
{
	// 空がゼロ以下の場合
	if(eleneCql->getAethel() <= 0)
	{
		return FF_OP_IMMATURE;		// ● 未成熟である
	}

	// 原質より小さい場合
	if(	(eleneCql->getSoil() < eleneNql->getSoil()) ||
		(eleneCql->getWater() < eleneNql->getWater()) ||
		(eleneCql->getFire() < eleneNql->getFire()) ||
		(eleneCql->getWind() < eleneNql->getWind()) ||
		(eleneCql->getAethel() < eleneNql->getAethel())	)
	{
		return FF_OP_IMMATURE;		// ● 未成熟である
	}

	// 元素過大判定
	if(eleneNql->getAethel() <= 0)
	{
		return FF_OP_OVERBREAK;		// ● 元素過大である
	}
	s32 ratioSo = eleneCql->getSoil() * 100 / eleneNql->getAethel();
	s32 ratioWa = eleneCql->getWater() * 100 / eleneNql->getAethel();
	s32 ratioFi = eleneCql->getFire() * 100 / eleneNql->getAethel();
	s32 ratioWi = eleneCql->getWind() * 100 / eleneNql->getAethel();
	//s32 ratioAe = eleneCql->getAethel() * 100 / eleneNql->getAethel();
	if(isSouma)
	{
		if((ratioSo >= PERCENT_OVERBREAK) || (ratioWa >= PERCENT_OVERBREAK) || (ratioFi >= PERCENT_OVERBREAK) || (ratioWi >= PERCENT_OVERBREAK))
		{
			return FF_OP_OVERBREAK;		// ● 元素過大である
		}
	}

	// 機能判定
	u32 fop = calcQualityFlagsToOperationFlag(qulityfCalced);

	// 形態
	u32 fform = 0;
	if(eleneNql->getSoil() >= eleneNql->getAethel())
	{
		fform = FF_FORM_SOIL;
	}
	else if(eleneNql->getWater() >= eleneNql->getAethel())
	{
		fform = FF_FORM_WATER;
	}
	else if(eleneNql->getFire() >= eleneNql->getAethel())
	{
		fform = FF_FORM_FIRE;
	}
	else if(eleneNql->getWind() >= eleneNql->getAethel())
	{
		fform = FF_FORM_WIND;
	}

	// 属性
	u32 fatrb = 0;
	if((fop == FF_OP_ASCEN_S) || (fop == FF_OP_DESCEN_S))	// 単元素アセンション／ディセンション
	{
		if(ratioSo >= 200)
		{
			fatrb = FF_ATRB_SOIL;
		}
		else if(ratioWa >= 200)
		{
			fatrb = FF_ATRB_WATER;
		}
		else if(ratioFi >= 200)
		{
			fatrb = FF_ATRB_FIRE;
		}
		else if(ratioWi >= 200)
		{
			fatrb = FF_ATRB_WIND;
		}
		else if((ratioSo < 50) && (ratioWa < 50) && (ratioFi < 50) && (ratioWi < 50))
		{
			fatrb = FF_ATRB_AETHEL;
		}
		// 無属性の場合は単元素シフトを無効に
		if(fatrb == 0)
		{
			fop = FF_OP_NOP;
		}
	}
	else
	{
		if(ratioSo >= 200)
		{
			fatrb = FF_ATRB_SOIL;
		}
		if(ratioWa >= 200)
		{
			fatrb |= FF_ATRB_WATER;
		}
		if(ratioFi >= 200)
		{
			fatrb |= FF_ATRB_FIRE;
		}
		if(ratioWi >= 200)
		{
			fatrb |= FF_ATRB_WIND;
		}
		if((ratioSo < 50) && (ratioWa < 50) && (ratioFi < 50) && (ratioWi < 50))
		{
			fatrb |= FF_ATRB_AETHEL;
		}
	}

	// エフェクト種別
	u32 fef = 0;
	// 極端な属性時のエフェクト（他の属性よりも２倍以上ある）
	if((ratioWi >= 150) && (ratioWi >= (ratioFi << 1)) && (ratioWi >= (ratioWa << 1)) && (ratioWi >= (ratioSo << 1)))
	{
		fef = FF_EFF_WIND;
	}
	else if((ratioFi >= 150) && (ratioFi >= (ratioWi << 1)) && (ratioFi >= (ratioWa << 1)) && (ratioFi >= (ratioSo << 1)))
	{
		fef = FF_EFF_FIRE;
	}
	else if((ratioWa >= 150) && (ratioWa >= (ratioWi << 1)) && (ratioWa >= (ratioFi << 1)) && (ratioWa >= (ratioSo << 1)))
	{
		fef = (ratioFi < 50) ? FF_EFF_ICE : FF_EFF_WATER;
	}
	else if((ratioSo >= 150) && (ratioSo >= (ratioWi << 1)) && (ratioSo >= (ratioFi << 1)) && (ratioSo >= (ratioWa << 1)))
	{
		fef = FF_EFF_SOIL;
	}
	// それ以外の複合エフェクト
	else
	{
		const u32 feftbl[8][3] =
		{
			{	FF_EFF_SAND,		FF_EFF_SAND_L,		FF_EFF_SAND_H,		},
			{	FF_EFF_SMOKE,		FF_EFF_SMOKE_L,		FF_EFF_SMOKE_H,		},
			{	FF_EFF_SPRAY,		FF_EFF_SPRAY_L,		FF_EFF_SPRAY_H,		},
			{	FF_EFF_FOG,			FF_EFF_FOG_L,		FF_EFF_FOG_H,		},
			{	FF_EFF_MIST,		FF_EFF_MIST_L,		FF_EFF_MIST_H,		},
			{	FF_EFF_WGRAIN,		FF_EFF_WGRAIN_L,	FF_EFF_WGRAIN_H,	},
			{	0,					FF_EFF_SHINE,		FF_EFF_SHINE,		},
			{	0,					0,					FF_EFF_SHINE,		},
		};
		int i, j;
		i = (ratioSo >= 200) ? 0 : (ratioSo >= 150) ? 3 : 6;
		i += (ratioWa >= 200) ? 2 : (ratioWa >= 150) ? 1 : 0;
		j = (ratioFi >= 200) ? 2 : (ratioFi >= 150) ? 1 : 0;
		if((i < 8) && (j < 3))
		{
			fef = feftbl[i][j];
		}
	}

	return fop | fform | fatrb | fef;
}

/*---------------------------------------------------------------------*//**
	質フラグから作用フラグを得る
**//*---------------------------------------------------------------------*/
u32 EleneStat::calcQualityFlagsToOperationFlag(u32 qulityfCalced)
{
	u32 fop = 0;
	int ipn;
	switch(qulityfCalced & QF_MASK_PN)
	{
	case QF_POS_2:	ipn = 0;	break;	// 極陽性
	case QF_MOD_2:	ipn = 1;	break;	// 極中性
	case QF_NEG_2:	ipn = 2;	break;	// 極陰性
	default:		ipn = 3;	break;	// その他
	}
	u32 qfPure = qulityfCalced & QF_MASK_PURE;
	if(qfPure != 0)	// 純性
	{
		switch(qfPure)
		{
		case QF_PURE_1:	fop = s_foptbl[0][0][ipn];	break;	// 純性
		case QF_PURE_2:	fop = s_foptbl[0][1][ipn];	break;	// 純性+1
		default:		fop = s_foptbl[0][2][ipn];	break;	// 純性+2
		}
	}
	else	// 激性、もしくは鈍性
	{
		u32 fe = ((qulityfCalced & QF_MASK_EXCT) != 0) ? FF_OP_ATTACK : 0;
		u32 fd = 0;
		switch(qulityfCalced & QF_MASK_DULL)
		{
		case QF_DULL_1:	fd = s_foptbl[1][0][ipn];	break;	// 鈍性
		case QF_DULL_2:	fd = s_foptbl[1][1][ipn];	break;	// 鈍性+1
		case QF_DULL_3:	fd = s_foptbl[1][2][ipn];	break;	// 鈍性+2
		}
		if(fe != 0)
		{
			switch(fd)
			{
			case FF_OP_NOP:	case FF_OP_DESCEN_S:	case FF_OP_DESCEN_A:
				fop = FF_OP_ATTACK;
				break;
			default:
				fop = FF_OP_ATTACK + fd;	// 足し算で攻撃追加できるように値を振っている
				break;
			}
		}
		else
		{
			fop = fd;
		}
	}
	return fop;
}

/*---------------------------------------------------------------------*//**
	作用フラグから質フラグを得る
**//*---------------------------------------------------------------------*/
u32 EleneStat::calcOperationFlagToQualityFlags(u32 flagFunc)
{
	u32 flagOp = flagFunc & FF_MASK_OP;
	u32 qfExct = 0;
	if (flagOp == FF_OP_ATTACK)
	{
		return QF_EXCT_1;
	}
	else if ((FF_OP_ATTACK < flagOp) && (flagOp < FF_OP_MISS_ATTACK))
	{
		qfExct = QF_EXCT_1;
		flagOp -= FF_OP_ATTACK;	// 足し算で攻撃追加できるように値を振っている
	}

	switch (flagOp)
	{
	case FF_OP_PURIFY:		// 純化
	case FF_OP_CURE:		// 回復
	case FF_OP_CATHARSIS:	// 浄化（純化 + 回復）
	case FF_OP_WRAP:		// 持続結界強化
	case FF_OP_BARRIER:		// 瞬間結界強化
	case FF_OP_SUFFUSE:		// 両結界強化
	case FF_OP_ASCEN_S:		// 単元素アセンション
	case FF_OP_ASCEN_A:		// 全体アセンション
		for(int i = 0; i <= 2; i++)
		{
			for(int j = 0; j <= 3; j++)
			{
				if (s_foptbl[0][i][j] == flagOp)
				{
					switch(i)
					{
					case 0:	return QF_PURE_1;
					case 1:	return QF_PURE_2;
					case 2:	return QF_PURE_3;
					}
				}
			}
		}
		break;
	case FF_OP_EXCEED:	// 増悪
	case FF_OP_UNWRAP:	// 持続結界弱体
	case FF_OP_UNBARRIER:	// 瞬間結界弱体
	case FF_OP_UNSUFFUSE:		// 両結界弱体（持続結界弱体 + 瞬間結界弱体）
	case FF_OP_DESCEN_S:	// 単元素ディセンション
	case FF_OP_DESCEN_A:	// 全体ディセンション
		for(int i = 0; i <= 2; i++)
		{
			for(int j = 0; j <= 3; j++)
			{
				if (s_foptbl[1][i][j] == flagOp)
				{
					switch(i)
					{
					case 3:	return qfExct | QF_DULL_1;
					case 4:	return qfExct | QF_DULL_2;
					case 5:	return qfExct | QF_DULL_3;
					}
				}
			}
		}
		break;
	}

	return 0;	// 不明
}

/*---------------------------------------------------------------------*//**
	否定的な作用かを判定する
**//*---------------------------------------------------------------------*/
bool EleneStat::isNegativeOperation(u32 flagFunc)
{
	switch(flagFunc & FF_MASK_OP)
	{
	case FF_OP_PURIFY:		// 純化
	case FF_OP_CURE:		// 回復
	case FF_OP_CATHARSIS:	// 浄化（純化 + 回復）
	case FF_OP_WRAP:		// 持続結界強化
	case FF_OP_BARRIER:		// 瞬間結界強化
	case FF_OP_SUFFUSE:		// 両結界強化
	case FF_OP_ASCEN_S:		// 単元素アセンション
	case FF_OP_ASCEN_A:		// 全体アセンション
		return false;
	case FF_OP_EXCEED:		// 増悪
	case FF_OP_UNWRAP:		// 持続結界弱体
	case FF_OP_UNBARRIER:	// 瞬間結界弱体
	case FF_OP_UNSUFFUSE:	// 両結界弱体（持続結界弱体 + 瞬間結界弱体）
	case FF_OP_DESCEN_S:	// 単元素ディセンション
	case FF_OP_DESCEN_A:	// 全体ディセンション
	case FF_OP_ATTACK:		// 攻撃
	case FF_OP_EXC_ATTACK:	// 増悪攻撃（増悪 + 攻撃）
	case FF_OP_UNW_ATTACK:	// 持続結界弱体攻撃（持続結界弱体 + 攻撃）
	case FF_OP_UNB_ATTACK:	// 瞬間結界弱体攻撃（瞬間結界弱体 + 攻撃）
	case FF_OP_UNS_ATTACK:	// 両結界弱体攻撃（両結界弱体 + 攻撃）
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	現在元素量と持続結界元素量の合計値を得る
**//*---------------------------------------------------------------------*/
Elene EleneStat::calcCurrentAndWrapperElene() const
{
	Elene elene(_eleneCql);
	elene.add(_veils[VEIL_WP].getElene());
	return elene;
}

/*---------------------------------------------------------------------*//**
	ベールの各元素小計を得る
**//*---------------------------------------------------------------------*/
s32 EleneStat::calcVeilSubtotalElene(s32 el) const
{
	s32 ene = 0;
	for(int i = 0; i < NUM_VEIL; i++)
	{
		ene += _veils[i].getElene()->get(el);
	}
	return ene;
}

/*---------------------------------------------------------------------*//**
	ベールがゼロ値かを得る
**//*---------------------------------------------------------------------*/
bool EleneStat::isCleardVeil() const
{
	for(int i = 0; i < NUM_VEIL; i++)
	{
		for(int j = 0; j < Elene::NUM_EL; j++)
		{
			if(_veils[i].getElene()->get(j) != 0)
			{
				return false;
			}
		}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	エネルギーシフト（全体アセンション／全体ディセンション）
**//*---------------------------------------------------------------------*/
void EleneStat::shift(s32 shiftval)
{
	_eleneCql.shift(&_eleneCql, shiftval);

	s32 shiftNew = _shift + shiftval;
	_eleneOql.shift(&_eleneNql, shiftNew);
	_shift = shiftNew;
}

/*---------------------------------------------------------------------*//**
	単元素シフト（単元素アセンション／単元素ディセンション）
	※	単元素シフトは、原質値を変更できず、
		また、原質崩れを修正することしか出来ない
**//*---------------------------------------------------------------------*/
void EleneStat::shiftSingle(s32 el, s32 shiftval)
{
	if(shiftval > 0)
	{
		do
		{
			s32 valMv = 0;
			for(int i = 0; i < Elene::NUM_EL; i++)
			{
				if(el == i)
				{
					s32 diff = _eleneCql.get(i) - _eleneNql.get(i);
					if(diff > 0)
					{
						valMv = (diff >= shiftval) ? shiftval : diff;
						_eleneCql.set(i, _eleneCql.get(i) - valMv);
					}
				}
				else if((el + 1) == i)
				{
					s32 valNw = _eleneCql.get(i) + valMv;
					_eleneCql.set(i, valNw);
				}
			}
			shiftval -= Elene::SHIFT_STDVAL;
		}
		while(shiftval > 0);
	}
	else if(shiftval < 0)
	{
		shiftval = - shiftval;
		do
		{
			s32 valMv = 0;
			for(int i = Elene::NUM_EL - 1; i >= 0; i--)
			{
				if(el == i)
				{
					s32 diff = _eleneCql.get(i) - _eleneNql.get(i);
					if(diff > 0)
					{
						valMv = (diff >= shiftval) ? shiftval : diff;
						_eleneCql.set(i, _eleneCql.get(i) - valMv);
					}
				}
				else if((el - 1) == i)
				{
					s32 valNw = _eleneCql.get(i) + valMv;
					_eleneCql.set(i, valNw);
				}
			}
			shiftval -= Elene::SHIFT_STDVAL;
		}
		while(shiftval > 0);
	}
}

/*---------------------------------------------------------------------*//**
	質の純性化
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
void EleneStat::purification(Elene* rest, const Elene* input)
{
	s32 bad, red;

	s32 soI = input->getSoil();
	s32 waI = input->getWater();
	s32 fiI = input->getFire();
	s32 wiI = input->getWind();

	// 地の純性化
	bad = _eleneCql.getSoil() - _eleneOql.getSoil();	// 余剰分
	// 水による軽減
	if((bad > 0) && (waI > 0))
	{
		red = waI / 4;	// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subSoil(red);
		bad -= red;
		waI -= red;// * 4;
	}
	// 火による軽減
	if((bad > 0) && (fiI > 0))
	{
		red = fiI / 2;	// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subSoil(red);
		bad -= red;
		fiI -= red;// * 2;
	}
	// 風による軽減
	if((bad > 0) && (wiI > 0))
	{
		red = wiI;		// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subSoil(red);
		bad -= red;
		wiI -= red;
	}

	// 水の純性化
	bad = _eleneCql.getWater() - _eleneOql.getWater();	// 余剰分
	// 地による軽減
	if((bad > 0) && (soI > 0))
	{
		red = soI / 4;	// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subWater(red);
		bad -= red;
		soI -= red;// * 4;
	}
	// 火による軽減
	if((bad > 0) && (fiI > 0))
	{
		red = fiI;		// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subWater(red);
		bad -= red;
		fiI -= red;
	}
	// 風による軽減
	if((bad > 0) && (wiI > 0))
	{
		red = wiI / 2;	// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subWater(red);
		bad -= red;
		wiI -= red;// * 2;
	}

	// 火の純性化
	bad = _eleneCql.getFire() - _eleneOql.getFire();	// 余剰分
	// 地による軽減
	if((bad > 0) && (soI > 0))
	{
		red = soI / 2;	// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subFire(red);
		bad -= red;
		soI -= red;// * 2;
	}
	// 水による軽減
	if((bad > 0) && (waI > 0))
	{
		red = waI;		// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subFire(red);
		bad -= red;
		waI -= red;
	}
	// 風による軽減
	if((bad > 0) && (wiI > 0))
	{
		red = wiI / 4;	// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subFire(red);
		bad -= red;
		wiI -= red;// * 4;
	}

	// 風の純性化
	bad = _eleneCql.getWind() - _eleneOql.getWind();	// 余剰分
	// 地による軽減
	if((bad > 0) && (soI > 0))
	{
		red = soI;		// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subWind(red);
		bad -= red;
		soI -= red;
	}
	// 水による軽減
	if((bad > 0) && (waI > 0))
	{
		red = waI / 2;	// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subWind(red);
		bad -= red;
		waI -= red;// * 2;
	}
	// 火による軽減
	if((bad > 0) && (fiI > 0))
	{
		red = fiI / 4;	// 軽減量
		red = TFW_MIN(bad, red);
		_eleneCql.subWind(red);
		bad -= red;
		fiI -= red;// * 4;
	}

	if(rest != 0L)
	{
		rest->set(Elene::EL_SO, soI);
		rest->set(Elene::EL_WA, waI);
		rest->set(Elene::EL_FI, fiI);
		rest->set(Elene::EL_WI, wiI);
		rest->set(Elene::EL_AE, input->getAethel());
	}
}

/*---------------------------------------------------------------------*//**
	質の増悪化
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
void EleneStat::impurification(const Elene* input)
{
	// 基本影響値
	s32 eeSoIe = input->getSoil() / 2;
	s32 eeWaIe = input->getWater() / 2;
	s32 eeFiIe = input->getFire() / 2;
	s32 eeWiIe = input->getWind() / 2;

	s32 bad;
	s32 res;
	s32 limit;
	// 地の増悪
	bad = eeSoIe + (eeWaIe / 4);	// 増悪量
	res = _eleneCql.getSoil() + bad;
	limit = _eleneNql.getSoil() * 3;
	if(res > limit)	{	res = limit;	}
	_eleneCql.setSoil(res);
	// 水の増悪
	bad = eeWaIe + (eeFiIe / 4);	// 増悪量
	res = _eleneCql.getWater() + bad;
	limit = _eleneNql.getWater() * 3;
	if(res > limit)	{	res = limit;	}
	_eleneCql.setWater(res);
	// 火の増悪
	bad = eeFiIe + (eeWiIe / 4);	// 増悪量
	res = _eleneCql.getFire() + bad;
	limit = _eleneNql.getFire() * 3;
	if(res > limit)	{	res = limit;	}
	_eleneCql.setFire(res);
	// 風の増悪
	bad = eeWiIe;					// 増悪量
	res = _eleneCql.getWind() + bad;
	limit = _eleneNql.getWind() * 3;
	if(res > limit)	{	res = limit;	}
	_eleneCql.setWind(res);
}

/*---------------------------------------------------------------------*//**
	ベールをクリア
**//*---------------------------------------------------------------------*/
void EleneStat::clearVeil()
{
	for(int i = 0; i < NUM_VEIL; i++) { _veils[i].clear(); }
}

/*---------------------------------------------------------------------*//**
	実効元素エネルギー値を更新
**//*---------------------------------------------------------------------*/
void EleneStat::updateResultValue(bool isSyncAnimatedValue)
{
#if 0	// ベールは _eleneDfv に含めないように変更
	for(int i = 0; i < NUM_VEIL; i++)
	{
		if(!_veils[i].isValid())	{	continue;	}

		for(int j = 0; j < Elene::NUM_EL; j++)
		{
			_eleneFv.add(j, _veils[i].getElene()->get(j));
		}
	}

	if(isSyncAnimatedValue)
	{
		_eleneDfv.copy(&_eleneFv);
	}
#else
	if(isSyncAnimatedValue)
	{	
		_eleneDfv.copy(&_eleneCql);
	}
#endif
}

/*---------------------------------------------------------------------*//**
	アニメーション適用後の結果値を更新
**//*---------------------------------------------------------------------*/
void EleneStat::updateAnimatedValue(f32 framerate)
{
	// 表示用結果値更新
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		if(_eleneDfv.get(i) != _eleneCql.get(i))
		{
			s32 n = (s32)((_eleneCql.get(i) - _eleneDfv.get(i)) * 0.0625f * framerate);
			n += _eleneDfv.get(i);
			_eleneDfv.set(i, n);
		}
	}
}

/*---------------------------------------------------------------------*//**
	スケーリング - 100 を等倍とした整数演算
**//*---------------------------------------------------------------------*/
void EleneStat::scale(s32 ratio)
{
	_eleneOql.scale(ratio);
	_eleneNql.scale(ratio);
	_eleneCql.scale(ratio);
	_eleneDfv.scale(ratio);
	for(int i = 0; i < NUM_VEIL; i++) { _veils[i].scale(ratio); }
}

/*---------------------------------------------------------------------*//**
	スケーリング - 1.0f を等倍とした浮動小数点演算
**//*---------------------------------------------------------------------*/
void EleneStat::scale(f32 ratio)
{
	_eleneOql.scale(ratio);
	_eleneNql.scale(ratio);
	_eleneCql.scale(ratio);
	_eleneDfv.scale(ratio);
	for(int i = 0; i < NUM_VEIL; i++) { _veils[i].scale(ratio); }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void EleneStat::copy(const EleneStat* src)
{
	_eleneOql.copy(&src->_eleneOql);
	_eleneNql.copy(&src->_eleneNql);
	_eleneCql.copy(&src->_eleneCql);
	_eleneDfv.copy(&src->_eleneDfv);
	_shift = src->_shift;
	for(int i = 0; i < NUM_VEIL; i++) { _veils[i].copy(&src->_veils[i]); }
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void EleneStat::clear()
{
	ASSERT(sizeof(Elene) == 20);
	ASSERT(sizeof(EleneStat) == 132);

	_eleneOql.clear();
	_eleneNql.clear();
	_eleneCql.clear();
	_eleneDfv.clear();
	_shift = 0;
	for(int i = 0; i < NUM_VEIL; i++) { _veils[i].clear(); }
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void EleneStat::reset()
{
	setup(&_eleneOql, &_eleneOql);
}

/*---------------------------------------------------------------------*//**
	初期設定
**//*---------------------------------------------------------------------*/
void EleneStat::setup(const Elene* eleneOql, const Elene* eleneCql, bool isNoClearVeil)
{
	_eleneOql = *eleneOql;
	_eleneNql = *eleneOql;
	_eleneCql = *eleneCql;
	_shift = 0;
	if(!isNoClearVeil)
	{
		for(int i = 0; i < NUM_VEIL; i++)
		{
			_veils[i].clear();
		}
	}

	updateResultValue(true);
}

/*---------------------------------------------------------------------*//**
	正規化
**//*---------------------------------------------------------------------*/
void EleneStat::normalize()
{
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		if(_eleneCql.get(i) < _eleneNql.get(i))
		{
			_eleneCql.set(i, _eleneNql.get(i));
		}
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void EleneStat::exec(const ExecCtx* ec)
{
	// ベール更新
#if 0
	bool isVeilValid = false;
#endif
	for(int i = 0; i < NUM_VEIL; i++)
	{
		if(_veils[i].isValid())
		{
			_veils[i].exec(ec);
#if 0
			isVeilValid = true;
#endif
		}
	}
#if 0	// ベールは _eleneDfv に含めないように変更
	if(isVeilValid)
	{
		updateResultValue(false);	// 結果値更新
	}
#endif

	// 表示用結果値更新
	updateAnimatedValue(ec->getDeltaFrame());
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool EleneStat::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!storeToStructure(&svst)) { return false; }
		if(!ser->store(&svst, sizeof(SaveStructure))) { return false; }
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	デシリアイズ
**//*---------------------------------------------------------------------*/
bool EleneStat::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!ser->restore(&svst, sizeof(SaveStructure))) { return false; }
		return restoreFromStructure(&svst);
	}
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャに保存する
**//*---------------------------------------------------------------------*/
bool EleneStat::storeToStructure(SaveStructure* svst) const
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	TFW_COPY_MEMORY(&svst->_eleneNql, &_eleneNql, sizeof(Elene));
	TFW_COPY_MEMORY(&svst->_eleneCql, &_eleneCql, sizeof(Elene));
	svst->_shift = _shift;
	TFW_COPY_MEMORY(svst->_veils, _veils, sizeof(EleneVeil) * NUM_VEIL);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool EleneStat::restoreFromStructure(const SaveStructure* svst)
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);
	
	TFW_COPY_MEMORY(&_eleneNql, &svst->_eleneNql, sizeof(Elene));
	TFW_COPY_MEMORY(&_eleneCql, &svst->_eleneCql, sizeof(Elene));
	_shift = 0;
	s32 shiftval = svst->_shift;
	TFW_COPY_MEMORY(_veils, svst->_veils, sizeof(EleneVeil) * NUM_VEIL);

	// 根本原質がリストア前に設定されていな場合は、今世原質をコピーする
	if(_eleneOql.isInitialValue())
	{
		TFW_COPY_MEMORY(&_eleneOql, &svst->_eleneNql, sizeof(Elene));
	}

	// 元素バランスの正規化
	#if _ENLIGHT_V01_3_ABOVE	// Ver1.3 以上
		normalize();
	#endif

	#if 0
		// ベールの最大値強制クリップ（仕様変更時の対応）
		for(int i = 0; i < Elene::NUM_EL; i++)
		{
			s32 limit = _eleneNql.get(i) * RATIO_WP_VEIL_MAX;
			s32 cur = _veils[VEIL_WP].getElene()->get(i);
			if(cur > limit)	{	_veils[VEIL_WP].elene()->set(i, limit);	}
			limit = _eleneNql.get(i) * RATIO_EF_VEIL_MAX;
			cur = _veils[VEIL_EF].getElene()->get(i);
			if(cur > limit)	{	_veils[VEIL_EF].elene()->set(i, limit);	}
		}
	#endif

	shift(shiftval);
	updateResultValue(true);

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
