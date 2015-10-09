/***********************************************************************//**
 *	MatQualt.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MatQualt.h"

// Friends
#include "Elene.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MatQualt メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void MatQualt::setup(const Elene* elene)
{
	s32 vw[NUM_QUALITY];

	#define ELE(_e_)	elene->get(Elene::EL_##_e_)
	vw[ WT ] = (ELE(SO) / 1) + (ELE(WA) / 2) - (ELE(FI) / 4) - (ELE(WI) / 1) - (ELE(AE) / 4);	// 重さ（weight）		｛軽い - 重い｝
	vw[ HT ] = (ELE(SO) / 4) + (ELE(WA) / 1) - (ELE(FI) / 1) + (ELE(WI) / 1) - (ELE(AE) / 8);	// 熱さ（heat）			｛熱い - 冷たい｝
	vw[ UC ] = (ELE(SO) / 4) + (ELE(WA) / 1) - (ELE(FI) / 4) - (ELE(WI) / 1) - (ELE(AE) / 8);	// 油性（unctuousness）	｛乾燥 - 油性｝
	vw[ SP ] = (ELE(SO) / 1) - (ELE(WA) / 8) - (ELE(FI) / 1) - (ELE(WI) / 4) - (ELE(AE) / 8);	// 鋭さ（sharpness）	｛鋭い - 鈍い｝
	vw[ HD ] = (ELE(SO) / 1) - (ELE(WA) / 1) - (ELE(FI) / 4) - (ELE(WI) / 4) - (ELE(AE) / 4);	// 硬さ（hardness）		｛柔らかい - 硬い｝
	vw[ MN ] = (ELE(SO) / 1) + (ELE(WA) / 8) - (ELE(FI) / 8) - (ELE(WI) / 4) - (ELE(AE) / 1);	// 精妙さ（minuteness）	｛精妙 - 荒い｝
	vw[ LQ ] = (ELE(SO) / 1) - (ELE(WA) / 1) - (ELE(FI) * 0) - (ELE(WI) * 0) - (ELE(AE) * 0);	// 液性（liquidness）	｛液性 - 固形｝
	vw[ RG ] = (ELE(SO) / 4) + (ELE(WA) * 0) + (ELE(FI) * 0) + (ELE(WI) / 8) - (ELE(AE) / 1);	// 粗さ（roughness）	｛滑らか - 粗い｝
	vw[ SC ] = (ELE(SO) / 8) + (ELE(WA) / 1) - (ELE(FI) / 8) - (ELE(WI) / 8) - (ELE(AE) / 1); 	// 粘性（stickiness）	｛澄んだ - 粘性｝
	vw[ MV ] = (ELE(SO) / 1) - (ELE(WA) / 8) - (ELE(FI) / 4) - (ELE(WI) / 1) - (ELE(AE) / 8);	// 動き（move）			｛動き - 安定｝

	for(int i = 0; i < NUM_QUALITY; i++)
	{
		if(vw[i] < 0)
		{
			vw[i] = 0;
		}
		else if(vw[i] > MAX_VALUE)
		{
			vw[i] = MAX_VALUE;
		}
		_val[i] = (u16)vw[i];
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
