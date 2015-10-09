/***********************************************************************//**
 *	StatusIfEx.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2015/02/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_STATUS_IF_EX_H_
#define _SRCR_STATUS_STATUS_IF_EX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "StatusChangeResult.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class StatusChangeResult;
class CharStat;
class Elene;
class EleneStat;
class TransStat;

/*---------------------------------------------------------------------*//**
 *	ステータス影響処理 StatusInfluencer の拡張ラッパークラス
 *
**//*---------------------------------------------------------------------*/
class StatusIfEx
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool influence(CharStat* cstat, const TransStat* tstat, bool showEffect, bool showMsg);
	static bool wasteSoumaMaterializeEnergy(CharStat* cstat, const Elene* input, bool isForce);
	static bool wasteSoumaFunctionEnergy(CharStat* cstat, s32 lfeneCost, bool isForce);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_STATUS_IF_EX_H_
