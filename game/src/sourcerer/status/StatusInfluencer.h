/***********************************************************************//**
 *	StatusInfluencer.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/16.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_STATUS_INFLUENCER_H_
#define _SRCR_STATUS_STATUS_INFLUENCER_H_

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
 *	ステータス影響処理
 *
**//*---------------------------------------------------------------------*/
class StatusInfluencer
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static void influence(StatusChangeResult* scres, CharStat* cstat, const TransStat* tstat);
	static bool wasteSoumaMaterializeEnergy(StatusChangeResult* scres, CharStat* cstat, const Elene* input, bool isForce);
	static bool wasteSoumaFunctionEnergy(StatusChangeResult* scres, CharStat* cstat, s32 lfeneCost, bool isForce);

private:
	static bool guardByVeil(Elene* rest, CharStat* cstat, const Elene* input);
	static void purification(Elene* rest, CharStat* cstat, const Elene* input);
	static void digest(StatusChangeResult* scres, Elene* rest, CharStat* cstat, const Elene* input, u32 eeqf);
	static void impurification(CharStat* cstat, const Elene* input);
	static bool strengthenVeil(Elene* rest, CharStat* cstat, const Elene* input, s32 iveil, u32 eeff, Unit* unitMaster);
	static bool weakenVeil(Elene* rest, CharStat* cstat, const Elene* input, s32 iveil);
	static bool shift(CharStat* cstat, const Elene* input, u32 eeqf, u32 eeff);
	static bool shiftSingle(CharStat* cstat, const Elene* input, u32 eeqf, u32 eeff);
	static void shockEnergy(StatusChangeResult* scres, Elene* rest, CharStat* cstat, const Elene* input);
	static void shockPhysics(StatusChangeResult* scres, CharStat* cstat, const Elene* input);
	static void recoverEnergy(StatusChangeResult* scres, CharStat* cstat, const Elene* elene);
	static bool wasteEnergyBySouma(StatusChangeResult* scres, CharStat* cstat, s32 lfeneCost, bool isForce);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_STATUS_INFLUENCER_H_
